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

#include "AbstractAudioVisualizer.h"

#include "../SurroundFieldMixerProcessor/SurroundFieldMixerProcessor.h"

#include "PositionEditorComponent.h"

namespace SurroundFieldMixer
{

//==============================================================================
/*
*/
class MultiMeterInputComponent :    public AbstractAudioVisualizer, 
                                    public SurroundFieldMixerProcessor::InputCommander
{
public:
    MultiMeterInputComponent();
    ~MultiMeterInputComponent();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    //==============================================================================
    void processingDataChanged(AbstractProcessorData *data) override;

    //==============================================================================
    virtual void processChanges() override;

    //==============================================================================
    void setInputMute(unsigned int channel, bool muteState) override;
    void setInputGain(unsigned int channel, float gainValue) override;
    void setPosition(unsigned int channel, juce::Point<float> position) override;
    void setSpreadFactor(unsigned int channel, float spreadFactor) override;
    void setReverbSendGain(unsigned int channel, float reverbSendGain) override;

private:
    ProcessorLevelData                                      m_levelData;
    std::vector<std::unique_ptr<juce::Slider>>              m_inputGains;
    std::vector<std::unique_ptr<TextButton>>                m_inputMutes;
    std::vector<std::unique_ptr<PositionEditorComponent>>   m_inputPositions;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultiMeterInputComponent)
};

}