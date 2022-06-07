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

namespace SurroundFieldMixer
{

//==============================================================================
/*
*/
class MultiMeterInputComponent :   public AbstractAudioVisualizer, 
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
    void setMuteChangeCallback(const std::function<void(int, bool)>& callback) override;
    void setMute(int channel, bool muteState) override;
    void setPositionChangeCallback(const std::function<void(int, std::tuple<float, float, float>)>& callback) override;
    void setPosition(int channel, std::tuple<float, float, float> position) override;

private:
    ProcessorLevelData                          m_levelData;
    std::vector<std::unique_ptr<TextButton>>    m_inputMutes;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultiMeterInputComponent)
};

}