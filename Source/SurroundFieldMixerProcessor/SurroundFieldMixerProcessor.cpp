/* Copyright (c) 2022, Christian Ahrens
 *
 * This file is part of SurroundFieldMixer <https://github.com/ChristianAhrens/SurroundFieldMixer>
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

#include "SurroundFieldMixerProcessor.h"

namespace SurroundFieldMixer
{


SurroundFieldMixerProcessor::ChannelCommander::ChannelCommander()
{
}

SurroundFieldMixerProcessor::ChannelCommander::~ChannelCommander()
{
}

SurroundFieldMixerProcessor::InputCommander::InputCommander()
{
}

SurroundFieldMixerProcessor::InputCommander::~InputCommander()
{
}

void SurroundFieldMixerProcessor::InputCommander::setInputMuteChangeCallback(const std::function<void(InputCommander* sender, int, bool)>& callback)
{
	m_inputMuteChangeCallback = callback;
}

void SurroundFieldMixerProcessor::InputCommander::setPositionChangeCallback(const std::function<void(InputCommander* sender, int, juce::Point<float>)>& callback)
{
	m_positionChangeCallback = callback;
}

void SurroundFieldMixerProcessor::InputCommander::setSpreadFactorChangeCallback(const std::function<void(InputCommander* sender, int, float spreadFactor)>& callback)
{
	m_spreadFactorChangeCallback = callback;
}

void SurroundFieldMixerProcessor::InputCommander::setReverbSendGainChangeCallback(const std::function<void(InputCommander* sender, int, float reverbSendGain)>& callback)
{
	m_reverbSendGainChangeCallback = callback;
}

void SurroundFieldMixerProcessor::InputCommander::inputMuteChange(int channel, bool muteState)
{
	if (m_inputMuteChangeCallback)
		m_inputMuteChangeCallback(this, channel, muteState);
}

void SurroundFieldMixerProcessor::InputCommander::positionChange(int channel, const juce::Point<float>& position)
{
	if (m_positionChangeCallback)
		m_positionChangeCallback(this, channel, position);
}

void SurroundFieldMixerProcessor::InputCommander::spreadFactorChange(int channel, const float spreadFactor)
{
	if (m_spreadFactorChangeCallback)
		m_spreadFactorChangeCallback(this, channel, spreadFactor);
}

void SurroundFieldMixerProcessor::InputCommander::reverbSendGainChange(int channel, const float reverbSendGain)
{
	if (m_reverbSendGainChangeCallback)
		m_reverbSendGainChangeCallback(this, channel, reverbSendGain);
}

SurroundFieldMixerProcessor::OutputCommander::OutputCommander()
{
}

SurroundFieldMixerProcessor::OutputCommander::~OutputCommander()
{
}

void SurroundFieldMixerProcessor::OutputCommander::setOutputMuteChangeCallback(const std::function<void(OutputCommander* sender, int, bool)>& callback)
{
	m_outputMuteChangeCallback = callback;
}

void SurroundFieldMixerProcessor::OutputCommander::setOutputSchemeChangeCallback(const std::function<void(OutputCommander* sender, unsigned int)>& callback)
{
	m_outputSchemeChangeCallback = callback;
}

void SurroundFieldMixerProcessor::OutputCommander::outputMuteChange(int channel, bool muteState)
{
	if (m_outputMuteChangeCallback)
		m_outputMuteChangeCallback(nullptr, channel, muteState);
}

void SurroundFieldMixerProcessor::OutputCommander::outputSchemeChange(unsigned int outputScheme)
{
	if (m_outputSchemeChangeCallback)
		m_outputSchemeChangeCallback(nullptr, outputScheme);
}


//==============================================================================
SurroundFieldMixerProcessor::SurroundFieldMixerProcessor() :
	AudioProcessor()
{
	// setup class member variable default values before audio device init, etc., since they might be required then already!

	auto orig = s_defaultPos();
	auto a = 0.5f * sinf(juce::MathConstants<float>::pi / 6); // pi * (30/180)
	auto b = 0.5f * sinf(juce::MathConstants<float>::pi / 3); // pi * (60/180)
	m_leftPos = orig + juce::Point<float>(-a, b);
	m_rightPos = orig + juce::Point<float>(a, b);
	m_centerPos = orig + juce::Point<float>(0.f, 0.5f);
	m_leftSurroundPos = orig + juce::Point<float>(-b, -a);
	m_rightSurroundPos = orig + juce::Point<float>(b, -a);

	m_useCodecOutput = false;

#if JUCE_IOS || JUCE_MAC
	m_encodeAudioFormat = std::make_unique<juce::CoreAudioFormat>();
#elif JUCE_WINDOWS
	m_encodeAudioFormat = std::make_unique<juce::WindowsMediaAudioFormat>();
#endif

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

SurroundFieldMixerProcessor::~SurroundFieldMixerProcessor()
{
	m_deviceManager->removeAudioCallback(this);

	// cleanup processing data buffer (do this elsewhere in productive code to avoid excessive mem alloc/free)
	for (auto i = 0; i < s_maxChannelCount; i++)
		delete[] m_processorChannels[i];
	delete[] m_processorChannels;
}

void SurroundFieldMixerProcessor::addInputListener(ProcessorDataAnalyzer::Listener* listener)
{
	if (m_inputDataAnalyzer)
		m_inputDataAnalyzer->addListener(listener);
}

void SurroundFieldMixerProcessor::removeInputListener(ProcessorDataAnalyzer::Listener* listener)
{
	if (m_inputDataAnalyzer)
		m_inputDataAnalyzer->removeListener(listener);
}

void SurroundFieldMixerProcessor::addOutputListener(ProcessorDataAnalyzer::Listener* listener)
{
	if (m_outputDataAnalyzer)
		m_outputDataAnalyzer->addListener(listener);
}

void SurroundFieldMixerProcessor::removeOutputListener(ProcessorDataAnalyzer::Listener* listener)
{
	if (m_outputDataAnalyzer)
		m_outputDataAnalyzer->removeListener(listener);
}

void SurroundFieldMixerProcessor::addInputCommander(InputCommander* commander)
{
	if (commander == nullptr)
		return;

	if (std::find(m_inputCommanders.begin(), m_inputCommanders.end(), commander) == m_inputCommanders.end())
	{
		m_inputCommanders.push_back(commander);
		commander->setInputMuteChangeCallback([=](ChannelCommander* sender, int channel, bool state) { return setInputMuteState(sender, channel, state); } );
		commander->setPositionChangeCallback([=](InputCommander* sender, int channel, const juce::Point<float>& position) { return setInputPositionValue(sender, channel, position); });
	}
}

void SurroundFieldMixerProcessor::removeInputCommander(InputCommander* commander)
{
	if (commander == nullptr)
		return;

	auto existingInputCommander = std::find(m_inputCommanders.begin(), m_inputCommanders.end(), commander);
	if (existingInputCommander != m_inputCommanders.end())
		m_inputCommanders.erase(existingInputCommander);
}

void SurroundFieldMixerProcessor::addOutputCommander(OutputCommander* commander)
{
	if (commander == nullptr)
		return;

	if (std::find(m_outputCommanders.begin(), m_outputCommanders.end(), commander) == m_outputCommanders.end())
	{
		m_outputCommanders.push_back(commander);
		commander->setOutputMuteChangeCallback([=](ChannelCommander* sender, int channel, bool state) { return setOutputMuteState(sender, channel, state); });
	}
}

void SurroundFieldMixerProcessor::removeOutputCommander(OutputCommander* commander)
{
	if (commander == nullptr)
		return;

	auto existingOutputCommander = std::find(m_outputCommanders.begin(), m_outputCommanders.end(), commander);
	if (existingOutputCommander != m_outputCommanders.end())
		m_outputCommanders.erase(existingOutputCommander);
}

bool SurroundFieldMixerProcessor::getInputMuteState(int inputChannelNumber)
{
	jassert(inputChannelNumber > 0);
	const ScopedLock sl(m_readLock);
	return m_inputMuteStates[inputChannelNumber];
}

void SurroundFieldMixerProcessor::setInputMuteState(ChannelCommander* sender, int inputChannelNumber, bool muted)
{
	jassert(inputChannelNumber > 0);
	const ScopedLock sl(m_readLock);
	m_inputMuteStates[inputChannelNumber] = muted;

	for (auto const& inputCommander : m_inputCommanders)
	{
		if (inputCommander != reinterpret_cast<InputCommander*>(sender))
			inputCommander->setInputMute(inputChannelNumber, muted);
	}
}

bool SurroundFieldMixerProcessor::getOutputMuteState(int outputChannelNumber)
{
	jassert(outputChannelNumber > 0);
	const ScopedLock sl(m_readLock);
	return m_outputMuteStates[outputChannelNumber];
}

void SurroundFieldMixerProcessor::setOutputMuteState(ChannelCommander* sender, int outputChannelNumber, bool muted)
{
	jassert(outputChannelNumber > 0);
	const ScopedLock sl(m_readLock);
	m_outputMuteStates[outputChannelNumber] = muted;

	for (auto const& outputCommander : m_outputCommanders)
	{
		if (outputCommander != reinterpret_cast<OutputCommander*>(sender))
			outputCommander->setOutputMute(outputChannelNumber, muted);
	}
}

const juce::Point<float>& SurroundFieldMixerProcessor::getInputPositionValue(int inputChannelNumber)
{
	jassert(inputChannelNumber > 0);
	const ScopedLock sl(m_readLock);
	return m_inputPositionValues[inputChannelNumber];
}

void SurroundFieldMixerProcessor::setInputPositionValue(InputCommander* sender, int inputChannelNumber, const juce::Point<float>& position)
{
	jassert(inputChannelNumber > 0);
	const ScopedLock sl(m_readLock);
	m_inputPositionValues[inputChannelNumber] = position;

	for (auto const& inputCommander : m_inputCommanders)
	{
		if (inputCommander != reinterpret_cast<InputCommander*>(sender))
			inputCommander->setPosition(inputChannelNumber, position);
	}
}

AudioDeviceManager* SurroundFieldMixerProcessor::getDeviceManager()
{
	if (m_deviceManager)
		return m_deviceManager.get();
	else
		return nullptr;
}

//==============================================================================
const String SurroundFieldMixerProcessor::getName() const
{
	return m_Name;
}

void SurroundFieldMixerProcessor::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{
	if (m_inputDataAnalyzer)
		m_inputDataAnalyzer->initializeParameters(sampleRate, maximumExpectedSamplesPerBlock);
	if (m_outputDataAnalyzer)
		m_outputDataAnalyzer->initializeParameters(sampleRate, maximumExpectedSamplesPerBlock);

#if JUCE_IOS || JUCE_MAC
	m_encodeAudioFormatWriter = std::unique_ptr<juce::AudioFormatWriter>(m_encodeAudioFormat->createWriterFor(new juce::MemoryOutputStream, sampleRate, s_testingOutputChannels, /*dbg*/16, juce::StringPairArray(), 0));
#elif JUCE_WINDOWS
	// WindowsMediaAudoFormat has no writer implementation currently :(
	//m_encodeAudioFormatWriter = std::unique_ptr<juce::AudioFormatWriter>(m_encodeAudioFormat->createWriterFor(new juce::MemoryOutputStream, sampleRate, s_testingOutputChannels, /*dbg*/16, juce::StringPairArray(), 0));
#endif
}

void SurroundFieldMixerProcessor::releaseResources()
{
	if (m_inputDataAnalyzer)
		m_inputDataAnalyzer->clearParameters();
	if (m_outputDataAnalyzer)
		m_outputDataAnalyzer->clearParameters();
}

void SurroundFieldMixerProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
	ignoreUnused(midiMessages);

	// the lock is currently gloablly taken in audioDeviceIOCallback which is calling this method
	//const ScopedLock sl(m_readLock);

	for (auto const& inputMuteStateKV : m_inputMuteStates)
	{
		if (inputMuteStateKV.second)
		{
			auto channel = inputMuteStateKV.first;
			auto channelIdx = channel - 1;
			buffer.clear(channelIdx, 0, buffer.getNumSamples());
		}
	}

	postMessage(new AudioInputBufferMessage(buffer));

	// process data in buffer to be what shall be used as output
	auto inputChannels = buffer.getNumChannels();
	auto outputChannels = s_testingOutputChannels;
	AudioBuffer<float> processedBuffer;
	processedBuffer.setSize(outputChannels, buffer.getNumSamples(), false, true, true);
	for (auto inputIdx = 0; inputIdx < inputChannels; inputIdx++)
	{
		for (auto outputIdx = 0; outputIdx < outputChannels; outputIdx++)
		{
			auto gain = getInputToOutputGain(inputIdx + 1, outputIdx + 1);
			processedBuffer.addFrom(outputIdx, 0, buffer.getReadPointer(inputIdx), buffer.getNumSamples(), gain);
		}
	}
	buffer.makeCopyOf(processedBuffer, true);

	for (auto const& outputMuteStateKV : m_outputMuteStates)
	{
		if (outputMuteStateKV.second)
		{
			auto channel = outputMuteStateKV.first;
			auto channelIdx = channel - 1;
			buffer.clear(channelIdx, 0, buffer.getNumSamples());
		}
	}

	postMessage(new AudioOutputBufferMessage(buffer));

	if (m_useCodecOutput)
	{
		encodeBuffer(buffer);
		buffer.clear();
	}
}

float SurroundFieldMixerProcessor::getInputToOutputGain(int input, int output)
{
	auto inputPos = getInputPosition(input);
	auto outputPos = getOutputPosition(output);

	//if (input == 1)
	//{
	//	DBG(String(__FUNCTION__) << " i:o=" << input << ":" << output << "(inputPos:" << inputPos.toString() << "; outputPos:" << outputPos.toString() << ")");
	//	DBG(String(__FUNCTION__) << " i:o=" << input << ":" << output << " resulting in dist " << inputPos.getDistanceFrom(outputPos));
	//}

	return 1.0f - inputPos.getDistanceFrom(outputPos);
}

bool SurroundFieldMixerProcessor::encodeBuffer(AudioBuffer<float>& buffer)
{
#if JUCE_IOS || JUCE_MAC
	if (m_encodeAudioFormatWriter)
		return m_encodeAudioFormatWriter->writeFromAudioSampleBuffer(buffer, 0, buffer.getNumSamples());
	return false;
#elif JUCE_WINDOWS
	// WindowsMediaAudoFormat has no writer implementation currently :(
	//if (m_encodeAudioFormatWriter)
	//	return m_encodeAudioFormatWriter->writeFromAudioSampleBuffer(buffer, 0, buffer.getNumSamples());
	ignoreUnused(buffer);
	return false;
#else
	ignoreUnused(buffer);
	return false;
#endif
}

const juce::Point<float> SurroundFieldMixerProcessor::getInputPosition(int channelNumber)
{
	return getInputPositionValue(channelNumber);
}

const juce::Point<float> SurroundFieldMixerProcessor::getOutputPosition(int channelNumber)
{
	jassert(channelNumber > 0);
	switch (channelNumber)
	{
	case 1: // L
		return getNormalizedDefaultPosition(juce::AudioChannelSet::ChannelType::left);
	case 2: // C
		return getNormalizedDefaultPosition(juce::AudioChannelSet::ChannelType::centre);
	case 3: // R
		return getNormalizedDefaultPosition(juce::AudioChannelSet::ChannelType::right);
	case 4: // RS
		return getNormalizedDefaultPosition(juce::AudioChannelSet::ChannelType::rightSurround);
	case 5: // LS
		return getNormalizedDefaultPosition(juce::AudioChannelSet::ChannelType::leftSurround);
	case 6: // LFE
		return getNormalizedDefaultPosition(juce::AudioChannelSet::ChannelType::LFE);
	default:
		return getNormalizedDefaultPosition(juce::AudioChannelSet::ChannelType::unknown);
	}
}

const juce::Point<float> SurroundFieldMixerProcessor::getNormalizedDefaultPosition(juce::AudioChannelSet::ChannelType channelIdent)
{
	switch (channelIdent)
	{
	case juce::AudioChannelSet::ChannelType::left:
		return m_leftPos;
	case juce::AudioChannelSet::ChannelType::right:
		return m_rightPos;
	case juce::AudioChannelSet::ChannelType::centre:
		return m_centerPos;
	case juce::AudioChannelSet::ChannelType::LFE:
		return s_defaultPos();
	case juce::AudioChannelSet::ChannelType::leftSurround:
		return m_leftSurroundPos;
	case juce::AudioChannelSet::ChannelType::rightSurround:
		return m_rightSurroundPos;
	case juce::AudioChannelSet::ChannelType::leftCentre:
	case juce::AudioChannelSet::ChannelType::rightCentre:
	case juce::AudioChannelSet::ChannelType::centreSurround:
	case juce::AudioChannelSet::ChannelType::leftSurroundSide:
	case juce::AudioChannelSet::ChannelType::rightSurroundSide:
	case juce::AudioChannelSet::ChannelType::topMiddle:
	case juce::AudioChannelSet::ChannelType::topFrontLeft:
	case juce::AudioChannelSet::ChannelType::topFrontCentre:
	case juce::AudioChannelSet::ChannelType::topFrontRight:
	case juce::AudioChannelSet::ChannelType::topRearLeft:
	case juce::AudioChannelSet::ChannelType::topRearCentre:
	case juce::AudioChannelSet::ChannelType::topRearRight:
	case juce::AudioChannelSet::ChannelType::LFE2:
	case juce::AudioChannelSet::ChannelType::leftSurroundRear:
	case juce::AudioChannelSet::ChannelType::rightSurroundRear:
	case juce::AudioChannelSet::ChannelType::wideLeft:
	case juce::AudioChannelSet::ChannelType::wideRight:
	case juce::AudioChannelSet::ChannelType::topSideLeft:
	case juce::AudioChannelSet::ChannelType::topSideRight:
	case juce::AudioChannelSet::ChannelType::ambisonicACN0:
	case juce::AudioChannelSet::ChannelType::ambisonicACN1: 
	case juce::AudioChannelSet::ChannelType::ambisonicACN2:
	case juce::AudioChannelSet::ChannelType::ambisonicACN3: 
	case juce::AudioChannelSet::ChannelType::ambisonicACN4:
	case juce::AudioChannelSet::ChannelType::ambisonicACN5:
	case juce::AudioChannelSet::ChannelType::ambisonicACN6: 
	case juce::AudioChannelSet::ChannelType::ambisonicACN7: 
	case juce::AudioChannelSet::ChannelType::ambisonicACN8:
	case juce::AudioChannelSet::ChannelType::ambisonicACN9:
	case juce::AudioChannelSet::ChannelType::ambisonicACN10:
	case juce::AudioChannelSet::ChannelType::ambisonicACN11:
	case juce::AudioChannelSet::ChannelType::ambisonicACN12:
	case juce::AudioChannelSet::ChannelType::ambisonicACN13:
	case juce::AudioChannelSet::ChannelType::ambisonicACN14:
	case juce::AudioChannelSet::ChannelType::ambisonicACN15:
	case juce::AudioChannelSet::ChannelType::ambisonicACN16:
	case juce::AudioChannelSet::ChannelType::ambisonicACN17:
	case juce::AudioChannelSet::ChannelType::ambisonicACN18:
	case juce::AudioChannelSet::ChannelType::ambisonicACN19:
	case juce::AudioChannelSet::ChannelType::ambisonicACN20:
	case juce::AudioChannelSet::ChannelType::ambisonicACN21:
	case juce::AudioChannelSet::ChannelType::ambisonicACN22:
	case juce::AudioChannelSet::ChannelType::ambisonicACN23:
	case juce::AudioChannelSet::ChannelType::ambisonicACN24:
	case juce::AudioChannelSet::ChannelType::ambisonicACN25:
	case juce::AudioChannelSet::ChannelType::ambisonicACN26:
	case juce::AudioChannelSet::ChannelType::ambisonicACN27:
	case juce::AudioChannelSet::ChannelType::ambisonicACN28:
	case juce::AudioChannelSet::ChannelType::ambisonicACN29:
	case juce::AudioChannelSet::ChannelType::ambisonicACN30:
	case juce::AudioChannelSet::ChannelType::ambisonicACN31:
	case juce::AudioChannelSet::ChannelType::ambisonicACN32:
	case juce::AudioChannelSet::ChannelType::ambisonicACN33:
	case juce::AudioChannelSet::ChannelType::ambisonicACN34:
	case juce::AudioChannelSet::ChannelType::ambisonicACN35:
	case juce::AudioChannelSet::ChannelType::bottomFrontLeft:
	case juce::AudioChannelSet::ChannelType::bottomFrontCentre:
	case juce::AudioChannelSet::ChannelType::bottomFrontRight:
	case juce::AudioChannelSet::ChannelType::proximityLeft:
	case juce::AudioChannelSet::ChannelType::proximityRight:
	case juce::AudioChannelSet::ChannelType::bottomSideLeft:
	case juce::AudioChannelSet::ChannelType::bottomSideRight:
	case juce::AudioChannelSet::ChannelType::bottomRearLeft:
	case juce::AudioChannelSet::ChannelType::bottomRearCentre:
	case juce::AudioChannelSet::ChannelType::bottomRearRight:
	case juce::AudioChannelSet::ChannelType::discreteChannel0:
	case juce::AudioChannelSet::ChannelType::unknown:
	default:
		return s_defaultPos();
	}
}

void SurroundFieldMixerProcessor::handleMessage(const Message& message)
{
	if (auto m = dynamic_cast<const AudioBufferMessage*> (&message))
	{
		if (m->getFlowDirection() == AudioBufferMessage::FlowDirection::Input && m_inputDataAnalyzer)
			m_inputDataAnalyzer->analyzeData(m->getAudioBuffer());
		else if (m->getFlowDirection() == AudioBufferMessage::FlowDirection::Output && m_outputDataAnalyzer)
			m_outputDataAnalyzer->analyzeData(m->getAudioBuffer());
	}
}

double SurroundFieldMixerProcessor::getTailLengthSeconds() const
{
	/*dbg*/return 0.0;
}

bool SurroundFieldMixerProcessor::acceptsMidi() const
{
	return false;
}

bool SurroundFieldMixerProcessor::producesMidi() const
{
	return false;
}

AudioProcessorEditor* SurroundFieldMixerProcessor::createEditor()
{
	if (!m_processorEditor)
		m_processorEditor = std::make_unique<SurroundFieldMixerEditor>(this);

	return m_processorEditor.get();
}

bool SurroundFieldMixerProcessor::hasEditor() const
{
	return !!m_processorEditor;
}

int SurroundFieldMixerProcessor::getNumPrograms()
{
	/*dbg*/return 0;
}

int SurroundFieldMixerProcessor::getCurrentProgram()
{
	/*dbg*/return 0;
}

void SurroundFieldMixerProcessor::setCurrentProgram(int index)
{
	/*dbg*/ignoreUnused(index);
}

const String SurroundFieldMixerProcessor::getProgramName(int index)
{
	/*dbg*/ignoreUnused(index);
	/*dbg*/return String();
}

void SurroundFieldMixerProcessor::changeProgramName(int index, const String& newName)
{
	/*dbg*/ignoreUnused(index);
	/*dbg*/ignoreUnused(newName);
}

void SurroundFieldMixerProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	/*dbg*/ignoreUnused(destData);
}

void SurroundFieldMixerProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	/*dbg*/ignoreUnused(data);
	/*dbg*/ignoreUnused(sizeInBytes);
}

void SurroundFieldMixerProcessor::audioDeviceIOCallbackWithContext(const float* const* inputChannelData, int numInputChannels,
	float* const* outputChannelData, int numOutputChannels, int numSamples, const AudioIODeviceCallbackContext& context)
{
    ignoreUnused(context);
    
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
	jassert(processedSampleCount == numSamples);
	for (auto i = 0; i < numOutputChannels && i < processedChannelCount; i++)
	{
		memcpy(outputChannelData[i], processedData[i], (size_t)processedSampleCount * sizeof(float));
	}

}

void SurroundFieldMixerProcessor::audioDeviceAboutToStart(AudioIODevice* device)
{
	if (device)
	{
		prepareToPlay(device->getCurrentSampleRate(), device->getCurrentBufferSizeSamples());

		auto inputChannels = device->getActiveInputChannels().toInteger();
		for (auto i = 1; i <= inputChannels; i++)
			setInputPositionValue(nullptr, i, s_defaultPos());

		//auto outputChannels = device->getActiveOutputChannels();
	}
}

void SurroundFieldMixerProcessor::audioDeviceStopped()
{
	releaseResources();
}

} // namespace SurroundFieldMixer
