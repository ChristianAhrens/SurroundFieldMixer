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
class TwoDFieldOutputComponent  :   public AbstractAudioVisualizer,
                                    public SurroundFieldMixerProcessor::OutputCommander
{
public:
    TwoDFieldOutputComponent();
    ~TwoDFieldOutputComponent();
    
    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    //==============================================================================
    void processingDataChanged(AbstractProcessorData *data) override;

    //==============================================================================
    void setOutputMute(unsigned int channel, bool muteState) override;
    void setOutputGain(unsigned int channel, float gainValue) override;
    void setOutputScheme(unsigned int dummyschemetobechanged) override;

private:
    unsigned long   m_channelL{ 1 };
    unsigned long   m_channelC{ 2 };
    unsigned long   m_channelR{ 3 };
    unsigned long   m_channelLS{ 5 };
    unsigned long   m_channelRS{ 4 };

    float m_outerMargin{ 20.0f };
    float m_visuAreaWidth{ 0.0f };
    float m_visuAreaHeight{ 0.0f };
    float                   m_visuAreaOrigX{ 0.0f };
    float                   m_visuAreaOrigY{ 0.0f };
    juce::Rectangle<float>  m_visuArea;
    juce::Point<float>      m_levelOrig;
    juce::Point<float>      m_centerMaxPoint;
    juce::Point<float>      m_leftMaxPoint;
    juce::Point<float>      m_rightMaxPoint;
    juce::Point<float>      m_leftSurroundMaxPoint;
    juce::Point<float>      m_rightSurroundMaxPoint;

    ProcessorLevelData  m_levelData;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TwoDFieldOutputComponent)
};

}