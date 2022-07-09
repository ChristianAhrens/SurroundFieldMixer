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

#pragma once

#include <JuceHeader.h>

#include "ProcessorDataAnalyzer.h"
#include "../SurroundFieldMixerEditor/SurroundFieldMixerEditor.h"


namespace SurroundFieldMixer
{


//==============================================================================
/*
*/
class SurroundFieldMixerProcessor : public AudioProcessor,
					                public AudioIODeviceCallback,
                                    public MessageListener
{
public:
    class ChannelCommander
    {
    public:
        ChannelCommander();
        virtual ~ChannelCommander();

    protected:

    private:
    };

    class InputCommander : public ChannelCommander
    {
    public:
        InputCommander();
        virtual ~InputCommander() override;

        void setInputMuteChangeCallback(const std::function<void(InputCommander* sender, int, bool)>& callback);
        void setPositionChangeCallback(const std::function<void(InputCommander* sender, int, juce::Point<float>)>& callback);
        void setSpreadFactorChangeCallback(const std::function<void(InputCommander* sender, int, float spreadFactor)>& callback);
        void setReverbSendGainChangeCallback(const std::function<void(InputCommander* sender, int, float reverbSendGain)>& callback);

        virtual void setInputMute(unsigned int channel, bool muteState) = 0;
        virtual void setPosition(unsigned int channel, juce::Point<float> position) = 0;
        virtual void setSpreadFactor(unsigned int channel, float spreadFactor) = 0;
        virtual void setReverbSendGain(unsigned int channel, float reverbSendGain) = 0;

    protected:
        void inputMuteChange(int channel, bool muteState);
        void positionChange(int channel, const juce::Point<float>& position);
        void spreadFactorChange(int channel, const float spreadFactor);
        void reverbSendGainChange(int channel, const float reverbSendGain);

    private:
        std::function<void(InputCommander* sender, int, bool)>                  m_inputMuteChangeCallback{ nullptr };
        std::function<void(InputCommander* sender, int, juce::Point<float>)>    m_positionChangeCallback{ nullptr };
        std::function<void(InputCommander* sender, int, float spreadFactor)>    m_spreadFactorChangeCallback{ nullptr };
        std::function<void(InputCommander* sender, int, float reverbSendGain)>  m_reverbSendGainChangeCallback{ nullptr };
    };

    class OutputCommander : public ChannelCommander
    {
    public:
        OutputCommander();
        virtual ~OutputCommander() override;

        void setOutputMuteChangeCallback(const std::function<void(OutputCommander* sender, int, bool)>& callback);
        void setOutputSchemeChangeCallback(const std::function<void(OutputCommander* sender, unsigned int)>& callback);

        virtual void setOutputMute(unsigned int channel, bool muteState) = 0;
        virtual void setOutputScheme(unsigned int outputScheme) = 0;

    protected:
        void outputMuteChange(int channel, bool muteState);
        void outputSchemeChange(unsigned int outputScheme);

    private:
        std::function<void(OutputCommander* sender, int, bool)>     m_outputMuteChangeCallback{ nullptr };
        std::function<void(OutputCommander* sender, unsigned int)>  m_outputSchemeChangeCallback{ nullptr };
    };

public:
    SurroundFieldMixerProcessor();
    ~SurroundFieldMixerProcessor();

    //==============================================================================
    void addInputListener(ProcessorDataAnalyzer::Listener* listener);
    void removeInputListener(ProcessorDataAnalyzer::Listener* listener);
    void addOutputListener(ProcessorDataAnalyzer::Listener* listener);
    void removeOutputListener(ProcessorDataAnalyzer::Listener* listener);

    //==============================================================================
    void addInputCommander(InputCommander* commander);
    void removeInputCommander(InputCommander* commander);
    void addOutputCommander(OutputCommander* commander);
    void removeOutputCommander(OutputCommander* comander);

    bool getInputMuteState(int channelNumber);
    void setInputMuteState(ChannelCommander* sender, int channelNumber, bool muted);
    bool getOutputMuteState(int channelNumber);
    void setOutputMuteState(ChannelCommander* sender, int channelNumber, bool muted);

    const juce::Point<float>& getInputPositionValue(int channelNumber);
    void setInputPositionValue(InputCommander* sender, int channelNumber, const juce::Point<float>& position);

    //==============================================================================
    AudioDeviceManager* getDeviceManager();

    //==============================================================================
    const String getName() const override;
    void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;
    void releaseResources() override;
    void processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override;

    double getTailLengthSeconds() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;

    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const String getProgramName(int index) override;
    void changeProgramName(int index, const String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================
    void audioDeviceIOCallback(const float** inputChannelData,
        int numInputChannels,
        float** outputChannelData,
        int numOutputChannels,
        int numSamples) override;
    void audioDeviceAboutToStart(AudioIODevice* device) override;
    void audioDeviceStopped() override;

    //==============================================================================
    float getInputToOutputGain(int input, int output);

    //==============================================================================
    void handleMessage(const Message& message) override;

    //==============================================================================
    enum dBRange
    {
        min = -90,
        max = 0,
    };

    static int getGlobalMindB()
    {
        return dBRange::min;
    }

    static int getGlobalMaxdB()
    {
        return dBRange::max;
    }

    static constexpr int s_maxChannelCount = 64;
    static constexpr int s_maxNumSamples = 1024;

    static constexpr juce::Point<float> s_defaultPos(){return juce::Point<float>(0.5f, 0.5f);};
    juce::Point<float> m_leftPos;
    juce::Point<float> m_rightPos;
    juce::Point<float> m_centerPos;
    juce::Point<float> m_leftSurroundPos;
    juce::Point<float> m_rightSurroundPos;

private:
    const juce::Point<float> getInputPosition(int channelNumber);
    const juce::Point<float> getOutputPosition(int channelNumber);

    //==============================================================================
    const juce::Point<float> getNormalizedDefaultPosition(juce::AudioChannelSet::ChannelType channelIdent);

    //==============================================================================
    String                      m_Name;

    //==============================================================================
    CriticalSection     m_readLock;

    float**             m_processorChannels;

    //==============================================================================
    std::unique_ptr<AudioDeviceManager> m_deviceManager;

    //==============================================================================
    std::unique_ptr<ProcessorDataAnalyzer>  m_inputDataAnalyzer;
    std::unique_ptr<ProcessorDataAnalyzer>  m_outputDataAnalyzer;

    //==============================================================================
    std::vector<InputCommander*>    m_inputCommanders;
    std::vector<OutputCommander*>   m_outputCommanders;

    //==============================================================================
    std::map<int, bool> m_inputMuteStates;
    std::map<int, bool> m_outputMuteStates;

    //==============================================================================
    std::map<int, juce::Point<float>>  m_inputPositionValues;

    //==============================================================================
    std::unique_ptr<SurroundFieldMixerEditor>  m_processorEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SurroundFieldMixerProcessor)
};

} // namespace SurroundFieldMixer
