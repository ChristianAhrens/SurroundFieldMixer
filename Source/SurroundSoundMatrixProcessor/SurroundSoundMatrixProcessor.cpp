/* Copyright (c) 2022, Christian Ahrens
 *
 * This file is part of SpaConBridge <https://github.com/ChristianAhrens/SurroundSoundMatrix>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "SurroundSoundMatrixProcessor.h"

namespace SurroundSoundMatrix
{


//==============================================================================
SurroundSoundMatrixProcessor::SurroundSoundMatrixProcessor() :
	AudioProcessor()
{
	// prepare max sized processing data buffer
	m_processorChannels = new float* [s_maxChannelCount];
	for (auto i = 0; i < s_maxChannelCount; i++)
	{
		m_processorChannels[i] = new float[s_maxNumSamples];
		for (auto j = 0; j < s_maxNumSamples; j++)
		{
			m_processorChannels[i][j] = 0.0f;
		}
	}

	m_inputDataAnalyzer = std::make_unique<ProcessorDataAnalyzer>();
	m_outputDataAnalyzer = std::make_unique<ProcessorDataAnalyzer>();

	m_deviceManager = std::make_unique<AudioDeviceManager>();
	m_deviceManager->addAudioCallback(this);

	// Hacky bit of device manager initialization:
	// We first intialize it to be able to get a valid device setup,
	// then initialize with a dummy xml config to trigger the internal xml structure being reset
	// and finally apply the original initialized device setup again to have the audio running correctly.
	// If we did not do so, either the internal xml would not be present as long as the first configuration change was made
	// and therefor no valid config file could be written by Auvi or the audio would not be running
	// on first start and manual config would be required.
	m_deviceManager->initialiseWithDefaultDevices(64, 8/*7.1 setup as temp dev default*/);
	auto audioDeviceSetup = m_deviceManager->getAudioDeviceSetup();
	m_deviceManager->initialise(64, 8, nullptr/*std::make_unique<XmlElement>(AppConfiguration::getTagName(AppConfiguration::TagID::DEVCFG)).get()*/, true, {}, &audioDeviceSetup);
#if JUCE_IOS
	if (audioDeviceSetup.bufferSize < 512)
		audioDeviceSetup.bufferSize = 512; // temp. workaround for iOS where buffersizes <512 lead to no sample data being delivered?
#endif
	m_deviceManager->setAudioDeviceSetup(audioDeviceSetup, true);
}

SurroundSoundMatrixProcessor::~SurroundSoundMatrixProcessor()
{
	m_deviceManager->removeAudioCallback(this);

	// cleanup processing data buffer (do this elsewhere in productive code to avoid excessive mem alloc/free)
	for (auto i = 0; i < s_maxChannelCount; i++)
		delete[] m_processorChannels[i];
	delete[] m_processorChannels;
}

void SurroundSoundMatrixProcessor::addInputListener(ProcessorDataAnalyzer::Listener* listener)
{
	if (m_inputDataAnalyzer)
		m_inputDataAnalyzer->addListener(listener);
}

void SurroundSoundMatrixProcessor::removeInputListener(ProcessorDataAnalyzer::Listener* listener)
{
	if (m_inputDataAnalyzer)
		m_inputDataAnalyzer->removeListener(listener);
}

void SurroundSoundMatrixProcessor::addOutputListener(ProcessorDataAnalyzer::Listener* listener)
{
	if (m_outputDataAnalyzer)
		m_outputDataAnalyzer->addListener(listener);
}

void SurroundSoundMatrixProcessor::removeOutputListener(ProcessorDataAnalyzer::Listener* listener)
{
	if (m_outputDataAnalyzer)
		m_outputDataAnalyzer->removeListener(listener);
}

AudioDeviceManager* SurroundSoundMatrixProcessor::getDeviceManager()
{
	if (m_deviceManager)
		return m_deviceManager.get();
	else
		return nullptr;
}

//==============================================================================
const String SurroundSoundMatrixProcessor::getName() const
{
	return m_Name;
}

void SurroundSoundMatrixProcessor::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{
	if (m_inputDataAnalyzer)
		m_inputDataAnalyzer->initializeParameters(sampleRate, maximumExpectedSamplesPerBlock);
	if (m_outputDataAnalyzer)
		m_outputDataAnalyzer->initializeParameters(sampleRate, maximumExpectedSamplesPerBlock);
}

void SurroundSoundMatrixProcessor::releaseResources()
{
	if (m_inputDataAnalyzer)
		m_inputDataAnalyzer->clearParameters();
	if (m_outputDataAnalyzer)
		m_outputDataAnalyzer->clearParameters();
}

void SurroundSoundMatrixProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
	ignoreUnused(midiMessages);

	postMessage(new AudioInputBufferMessage(buffer));

	// process data in buffer to be what shall be used as output
	AudioBuffer<float> processedBuffer;
	processedBuffer.setSize(5, buffer.getNumSamples(), false, false, true);
	processedBuffer.copyFrom(0, 0, buffer.getReadPointer(0), buffer.getNumSamples());
	processedBuffer.copyFrom(1, 0, buffer.getReadPointer(1), buffer.getNumSamples());
	processedBuffer.copyFrom(2, 0, buffer.getReadPointer(0), buffer.getNumSamples());
	processedBuffer.copyFrom(3, 0, buffer.getReadPointer(0), buffer.getNumSamples());
	processedBuffer.copyFrom(4, 0, buffer.getReadPointer(1), buffer.getNumSamples());
	buffer.makeCopyOf(processedBuffer, true);

	postMessage(new AudioOutputBufferMessage(buffer));
}

void SurroundSoundMatrixProcessor::handleMessage(const Message& message)
{
	if (auto m = dynamic_cast<const AudioBufferMessage*> (&message))
	{
		if (m->getFlowDirection() == AudioBufferMessage::FlowDirection::Input && m_inputDataAnalyzer)
			m_inputDataAnalyzer->analyzeData(m->getAudioBuffer());
		else if (m->getFlowDirection() == AudioBufferMessage::FlowDirection::Output && m_outputDataAnalyzer)
			m_outputDataAnalyzer->analyzeData(m->getAudioBuffer());
	}
}

double SurroundSoundMatrixProcessor::getTailLengthSeconds() const
{
	/*dbg*/return 0.0;
}

bool SurroundSoundMatrixProcessor::acceptsMidi() const
{
	return false;
}

bool SurroundSoundMatrixProcessor::producesMidi() const
{
	return false;
}

AudioProcessorEditor* SurroundSoundMatrixProcessor::createEditor()
{
	if (!m_processorEditor)
		m_processorEditor = std::make_unique<SurroundSoundMatrixEditor>(this);

	return m_processorEditor.get();
}

bool SurroundSoundMatrixProcessor::hasEditor() const
{
	return !!m_processorEditor;
}

int SurroundSoundMatrixProcessor::getNumPrograms()
{
	/*dbg*/return 0;
}

int SurroundSoundMatrixProcessor::getCurrentProgram()
{
	/*dbg*/return 0;
}

void SurroundSoundMatrixProcessor::setCurrentProgram(int index)
{
	/*dbg*/ignoreUnused(index);
}

const String SurroundSoundMatrixProcessor::getProgramName(int index)
{
	/*dbg*/ignoreUnused(index);
	/*dbg*/return String();
}

void SurroundSoundMatrixProcessor::changeProgramName(int index, const String& newName)
{
	/*dbg*/ignoreUnused(index);
	/*dbg*/ignoreUnused(newName);
}

void SurroundSoundMatrixProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	/*dbg*/ignoreUnused(destData);
}

void SurroundSoundMatrixProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	/*dbg*/ignoreUnused(data);
	/*dbg*/ignoreUnused(sizeInBytes);
}

void SurroundSoundMatrixProcessor::audioDeviceIOCallback(const float** inputChannelData, int numInputChannels,
	float** outputChannelData, int numOutputChannels, int numSamples)
{
	const ScopedLock sl(m_readLock);

	auto maxActiveChannels = std::max(numInputChannels, numOutputChannels);

	// copy incoming data to processing data buffer
	for (auto i = 0; i < numInputChannels && i < maxActiveChannels; i++)
	{
		memcpy(m_processorChannels[i], inputChannelData[i], (size_t)numSamples * sizeof(float));
	}

	// from juce doxygen: buffer must be the size of max(inCh, outCh) and feeds the input data into the method and is returned with output data
	AudioBuffer<float> audioBufferToProcess(m_processorChannels, maxActiveChannels, numSamples);
	MidiBuffer midiBufferToProcess;
	processBlock(audioBufferToProcess, midiBufferToProcess);

	// copy the processed data buffer data to outgoing data
	auto processedChannelCount = audioBufferToProcess.getNumChannels();
	auto processedSampleCount = audioBufferToProcess.getNumSamples();
	auto processedData = audioBufferToProcess.getArrayOfReadPointers();
	jassert(processedChannelCount >= numOutputChannels);
	jassert(processedSampleCount == numSamples);
	for (auto i = 0; i < numOutputChannels && i < maxActiveChannels; i++)
	{
		memcpy(outputChannelData[i], processedData[i], (size_t)processedSampleCount * sizeof(float));
	}

}

void SurroundSoundMatrixProcessor::audioDeviceAboutToStart(AudioIODevice* device)
{
	if (device)
	{
		prepareToPlay(device->getCurrentSampleRate(), device->getCurrentBufferSizeSamples());
	}
}

void SurroundSoundMatrixProcessor::audioDeviceStopped()
{
	releaseResources();
}

} // namespace SurroundSoundMatrix