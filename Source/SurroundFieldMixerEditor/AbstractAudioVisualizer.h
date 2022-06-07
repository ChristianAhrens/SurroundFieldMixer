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

#include "../SurroundFieldMixerProcessor/SurroundFieldMixerProcessor.h"
#include "../submodules/JUCE-AppBasics/Source/SplitButtonComponent.h"

namespace SurroundFieldMixer
{

//==============================================================================
/*
*/
class AbstractAudioVisualizer : public Component, 
                                public ProcessorDataAnalyzer::Listener
{
public:
    AbstractAudioVisualizer();
    virtual ~AbstractAudioVisualizer();
    
    //==============================================================================
    void notifyChanges();
    virtual void processChanges();
    
    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    void mouseDown(const MouseEvent& event) override;

protected:
    //==============================================================================
    void setUsesValuesInDB(bool useValuesInDB);
    bool getUsesValuesInDB();

private:
    bool    m_changesPending{ false };
    bool    m_usesValuesInDB{ 0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AbstractAudioVisualizer)
};

}
