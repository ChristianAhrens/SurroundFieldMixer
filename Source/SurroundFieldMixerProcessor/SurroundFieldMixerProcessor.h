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
    class InputCommander
    {
    public:
        virtual void setMuteCallback(const std::function<void(int, bool)>& callback) = 0;
    };

    class OutputCommander
    {
    public:
        virtual void setMuteCallback(const std::function<void(int, bool)>& callback) = 0;
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

    void toggleInputMute(int channelNumber, bool muted);
    void toggleOutputMute(int channelNumber, bool muted);

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

private:
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
    std::unique_ptr<SurroundFieldMixerEditor>  m_processorEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SurroundFieldMixerProcessor)
};

} // namespace SurroundFieldMixer
