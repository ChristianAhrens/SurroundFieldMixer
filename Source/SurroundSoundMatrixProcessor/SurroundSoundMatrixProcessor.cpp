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
AudioBufferMessage::AudioBufferMessage(AudioBuffer<float>& buffer)
{
	m_buffer = buffer;
}

AudioBufferMessage::~AudioBufferMessage()
{

}

const AudioBuffer<float>& AudioBufferMessage::getAudioBuffer() const
{
	return m_buffer;
}

//==============================================================================
SurroundSoundMatrixProcessor::SurroundSoundMatrixProcessor() :
	AudioProcessor(),
	m_fwdFFT(fftOrder),
	m_windowF(fftSize, dsp::WindowingFunction<float>::hann)
{
	m_FFTdataPos = 0;
	zeromem(m_FFTdata, sizeof(m_FFTdata));

	setHoldTime(500);

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
}

void SurroundSoundMatrixProcessor::setHoldTime(int holdTimeMs)
{
	m_holdTimeMs = holdTimeMs;

	startTimer(m_holdTimeMs);
}

void SurroundSoundMatrixProcessor::addListener(Listener* listener)
{
	m_callbackListeners.add(listener);
}

void SurroundSoundMatrixProcessor::removeListener(Listener* listener)
{
	m_callbackListeners.remove(m_callbackListeners.indexOf(listener));
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
	m_sampleRate = static_cast<unsigned long>(sampleRate);
	m_samplesPerCentiSecond = static_cast<int>(sampleRate * 0.01f);
	m_bufferSize = maximumExpectedSamplesPerBlock;
	m_missingSamplesForCentiSecond = static_cast<int>(m_samplesPerCentiSecond + 0.5f);
	m_centiSecondBuffer.setSize(2, m_missingSamplesForCentiSecond, false, true, false);
}

void SurroundSoundMatrixProcessor::releaseResources()
{
	m_sampleRate = 0;
	m_samplesPerCentiSecond = 0;
	m_bufferSize = 0;
	m_centiSecondBuffer.clear();
	m_missingSamplesForCentiSecond = 0;
}

void SurroundSoundMatrixProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
	ignoreUnused(midiMessages);

	postMessage(new AudioBufferMessage(buffer));
}

void SurroundSoundMatrixProcessor::handleMessage(const Message& message)
{
	if (auto m = dynamic_cast<const AudioBufferMessage*> (&message))
	{
		auto buffer = m->getAudioBuffer();

		int numChannels = buffer.getNumChannels();

		if (numChannels != m_centiSecondBuffer.getNumChannels())
			m_centiSecondBuffer.setSize(numChannels, m_samplesPerCentiSecond, false, true, true);
		if (m_sampleRate != m_centiSecondBuffer.GetSampleRate())
			m_centiSecondBuffer.SetSampleRate(m_sampleRate);

		int availableSamples = buffer.getNumSamples();

		int readPos = 0;
		int writePos = m_samplesPerCentiSecond - m_missingSamplesForCentiSecond;
		while (availableSamples >= m_missingSamplesForCentiSecond)
		{
			for (int i = 0; i < numChannels; ++i)
			{
				// generate signal buffer data
				m_centiSecondBuffer.copyFrom(i, writePos, buffer.getReadPointer(i) + readPos, m_missingSamplesForCentiSecond);

				// generate level data
				auto peak = m_centiSecondBuffer.getMagnitude(i, 0, m_samplesPerCentiSecond);
				auto rms = m_centiSecondBuffer.getRMSLevel(i, 0, m_samplesPerCentiSecond);
				auto hold = std::max(peak, m_level.GetLevel(i + 1).hold);
				m_level.SetLevel(i + 1, ProcessorLevelData::LevelVal(peak, rms, hold, static_cast<float>(getGlobalMindB())));

				// generate spectrum data
				{
					int unprocessedSamples = 0;
					if (m_FFTdataPos < fftSize)
					{
						int missingSamples = fftSize - m_FFTdataPos;
						if (missingSamples < m_samplesPerCentiSecond)
						{
							memcpy(m_FFTdata, m_centiSecondBuffer.getReadPointer(i), missingSamples);
							m_FFTdataPos += missingSamples;
							unprocessedSamples = m_samplesPerCentiSecond - missingSamples;
						}
						else
						{
							memcpy(m_FFTdata, m_centiSecondBuffer.getReadPointer(i), m_samplesPerCentiSecond);
							m_FFTdataPos += m_samplesPerCentiSecond;
						}
					}

					if (m_FFTdataPos >= fftSize)
					{
						m_windowF.multiplyWithWindowingTable(m_FFTdata, fftSize);
						m_fwdFFT.performFrequencyOnlyForwardTransform(m_FFTdata);
						ProcessorSpectrumData::SpectrumBands spectrumBands = m_spectrum.GetSpectrum(i);

						spectrumBands.mindB = static_cast<float>(getGlobalMindB());
						spectrumBands.maxdB = static_cast<float>(getGlobalMaxdB());

						spectrumBands.minFreq = static_cast<float>(m_sampleRate / ProcessorSpectrumData::SpectrumBands::count);
						spectrumBands.maxFreq = static_cast<float>(m_sampleRate / 2);
						spectrumBands.freqRes = static_cast<float>((spectrumBands.maxFreq - spectrumBands.minFreq) / ProcessorSpectrumData::SpectrumBands::count);

						auto spectrumStepWidth = static_cast<int>(0.5f * (fftSize / ProcessorSpectrumData::SpectrumBands::count));
						auto spectrumPos = 0;
						for (int j = 0; j < ProcessorSpectrumData::SpectrumBands::count && spectrumPos < fftSize; ++j)
						{
							float spectrumVal = 0;

							for (int k = 0; k < spectrumStepWidth; ++k, ++spectrumPos)
								spectrumVal += m_FFTdata[spectrumPos];
							spectrumVal = spectrumVal / spectrumStepWidth;

							auto leveldB = jlimit(spectrumBands.mindB, spectrumBands.maxdB, Decibels::gainToDecibels(spectrumVal));
							auto level = jmap(leveldB, spectrumBands.mindB, spectrumBands.maxdB, 0.0f, 1.0f);

							spectrumBands.bandsPeak[j] = level;
							spectrumBands.bandsHold[j] = std::max(level, spectrumBands.bandsHold[j]);
						}

						m_spectrum.SetSpectrum(i, spectrumBands);

						zeromem(m_FFTdata, sizeof(m_FFTdata));
						m_FFTdataPos = 0;
					}

					if (unprocessedSamples != 0)
					{
						memcpy(m_FFTdata, m_centiSecondBuffer.getReadPointer(i, m_samplesPerCentiSecond - unprocessedSamples), unprocessedSamples);
						m_FFTdataPos += unprocessedSamples;
					}
				}
			}

			BroadcastData(&m_level);
			BroadcastData(&m_centiSecondBuffer);
			BroadcastData(&m_spectrum);

			readPos += m_missingSamplesForCentiSecond;
			availableSamples -= m_missingSamplesForCentiSecond;

			m_missingSamplesForCentiSecond = m_samplesPerCentiSecond;

			writePos = m_samplesPerCentiSecond - m_missingSamplesForCentiSecond;

			if (availableSamples <= 0)
				break;
		}

		if (availableSamples > 0)
		{
			for (int i = 0; i < numChannels; ++i)
			{
				m_centiSecondBuffer.copyFrom(i, writePos, buffer.getReadPointer(i) + readPos, availableSamples);
			}
		}
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
	ignoreUnused(outputChannelData);
	ignoreUnused(numOutputChannels);

	// these should have been prepared by audioDeviceAboutToStart()...
	jassert(m_sampleRate > 0 && m_bufferSize > 0);

	const ScopedLock sl(m_readLock);

	int numActiveChans = 0;
	int numInputs = 0;

	// messy stuff needed to compact the channels down into an array
	// of non-zero pointers..
	for (int i = 0; i < numInputChannels; ++i)
	{
		if (inputChannelData[i] != nullptr)
		{
			inputChans[numInputs++] = inputChannelData[i];
			if (numInputs >= numElementsInArray(inputChans))
				break;
		}
	}

	if (numInputs > m_buffer.getNumChannels())
	{
		numActiveChans = m_buffer.getNumChannels();

		// if there aren't enough output channels for the number of
		// inputs, we need to create some temporary extra ones (can't
		// use the input data in case it gets written to)
		m_buffer.setSize(numInputs, numSamples,
			false, false, true);

		for (int i = numActiveChans; i < numInputs; ++i)
		{
			m_processorChannels[numActiveChans] = m_buffer.getWritePointer(i);
			memcpy(m_processorChannels[numActiveChans], inputChans[i], (size_t)numSamples * sizeof(float));
			++numActiveChans;
		}
	}
	else
	{
		for (int i = 0; i < numInputs; ++i)
		{
			memcpy(m_processorChannels[numActiveChans], inputChans[i], (size_t)numSamples * sizeof(float));
			++numActiveChans;
		}
	}

	AudioBuffer<float> newAudioBuffer(m_processorChannels, numActiveChans, numSamples);
	MidiBuffer newMidiBuffer;

	processBlock(newAudioBuffer, newMidiBuffer);
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

void SurroundSoundMatrixProcessor::BroadcastData(AbstractProcessorData* data)
{
	for (Listener* l : m_callbackListeners)
		l->processingDataChanged(data);
}

void SurroundSoundMatrixProcessor::timerCallback()
{
	FlushHold();
}

void SurroundSoundMatrixProcessor::FlushHold()
{
	// clear level hold values
	auto channelCount = static_cast<int>(m_level.GetChannelCount());
	for (auto i = 0; i < channelCount; ++i)
	{
		m_level.SetLevel(i + 1, ProcessorLevelData::LevelVal(0.0f, 0.0f, 0.0f));
	}

	// clear spectrum hold values	auto channelCount = m_level.GetChannelCount();
	channelCount = static_cast<int>(m_spectrum.GetChannelCount());
	for (auto i = 0; i < channelCount; ++i)
	{
		ProcessorSpectrumData::SpectrumBands spectrumBands = m_spectrum.GetSpectrum(i);
		for (auto j = 0; j < ProcessorSpectrumData::SpectrumBands::count; ++j)
		{
			spectrumBands.bandsPeak[j] = 0.0f;
			spectrumBands.bandsHold[j] = 0.0f;
		}

		m_spectrum.SetSpectrum(i, spectrumBands);
	}
}

} // namespace SurroundSoundMatrix