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

#pragma once

#include <JuceHeader.h>

#include "ProcessorAudioSignalData.h"
#include "ProcessorLevelData.h"
#include "ProcessorSpectrumData.h"

#include "../SurroundSoundMatrixEditor/SurroundSoundMatrixEditor.h"


namespace SurroundSoundMatrix
{

//==============================================================================
/*
*/
class AudioBufferMessage : public Message
{
public:
    AudioBufferMessage(AudioBuffer<float>& buffer);
    ~AudioBufferMessage();

    const AudioBuffer<float>& getAudioBuffer() const;

private:
    AudioBuffer<float> m_buffer;
};

//==============================================================================
/*
*/
class SurroundSoundMatrixProcessor :    public AudioProcessor,
					                    public AudioIODeviceCallback,
                                        public MessageListener,
                                        public Timer
{
public:
    class Listener
    {
    public:
        virtual ~Listener() {};

        virtual void processingDataChanged(AbstractProcessorData* data) = 0;
    };

public:
    SurroundSoundMatrixProcessor();
    ~SurroundSoundMatrixProcessor();

    //==============================================================================
    void setHoldTime(int holdTimeMs);

    //==============================================================================
    void addListener(Listener* listener);
    void removeListener(Listener* listener);

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
    void timerCallback() override;

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
    void BroadcastData(AbstractProcessorData* data);
    void FlushHold();

    ProcessorAudioSignalData    m_centiSecondBuffer;
    ProcessorLevelData          m_level;
    ProcessorSpectrumData       m_spectrum;

    String                      m_Name;
    Array<Listener*>            m_callbackListeners;

    //==============================================================================
    CriticalSection     m_readLock;

    float**             m_processorChannels;

    unsigned long       m_sampleRate = 0;
    int                 m_samplesPerCentiSecond = 0;
    int                 m_bufferSize = 0;

    int                 m_missingSamplesForCentiSecond;

    //==============================================================================
    std::unique_ptr<AudioDeviceManager> m_deviceManager;

    //==============================================================================
    enum
    {
        fftOrder = 12,
        fftSize = 1 << fftOrder
    };
    dsp::FFT                                    m_fwdFFT;
    dsp::WindowingFunction<float>               m_windowF;
    float                                       m_FFTdata[2 * fftSize];
    int                                         m_FFTdataPos;

    int                                         m_holdTimeMs;

    std::unique_ptr<SurroundSoundMatrixEditor>  m_processorEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SurroundSoundMatrixProcessor)
};

} // namespace SurroundSoundMatrix
