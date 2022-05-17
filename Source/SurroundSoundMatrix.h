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

#include "AppConfigurationBase.h"

#include "SurroundSoundMatrixEditor/SurroundSoundMatrixEditor.h"
#include "SurroundSoundMatrixProcessor/SurroundSoundMatrixProcessor.h"


namespace SurroundSoundMatrix
{

//==============================================================================
/*
 *
 */
class SurroundSoundMatrix   :   public Component,
                                public JUCEAppBasics::AppConfigurationBase::XmlConfigurableElement
{
public:
    SurroundSoundMatrix();
    ~SurroundSoundMatrix() override;
    
    //==========================================================================
    juce::Component* getUIComponent();
    juce::Component* getDeviceSetupComponent();

    //==========================================================================
    std::unique_ptr<XmlElement> createStateXml() override;
    bool setStateXml(XmlElement* stateXml) override;

private:
    std::unique_ptr<SurroundSoundMatrixProcessor>   m_SurroundSoundMatrixProcessor;
    std::unique_ptr<SurroundSoundMatrixEditor>      m_audioVisuComponent;
    std::unique_ptr<AudioSelectComponent>           m_audioDeviceSelectComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SurroundSoundMatrix)
};

};
