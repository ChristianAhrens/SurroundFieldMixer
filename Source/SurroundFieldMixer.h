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

#include "AppConfigurationBase.h"

#include "SurroundFieldMixerEditor/SurroundFieldMixerEditor.h"
#include "SurroundFieldMixerProcessor/SurroundFieldMixerProcessor.h"
#include "SurroundFieldMixerRemote/SurroundFieldMixerRemoteWrapper.h"


namespace SurroundFieldMixer
{

//==============================================================================
/*
 *
 */
class SurroundFieldMixer   :    public Component,
                                public JUCEAppBasics::AppConfigurationBase::XmlConfigurableElement,
                                public SurroundFieldMixerRemoteWrapper::Listener
{
public:
    SurroundFieldMixer();
    ~SurroundFieldMixer() override;
    
    //==========================================================================
    juce::Component* getUIComponent();
    juce::Component* getDeviceSetupComponent();

    //==========================================================================
    std::unique_ptr<XmlElement> createStateXml() override;
    bool setStateXml(XmlElement* stateXml) override;

    //==========================================================================
    void OnRemoteMuteChange(unsigned int channel, int muteVal) override;
    void OnRemoteXPosChange(unsigned int channel, float xPosVal) override;
    void OnRemoteYPosChange(unsigned int channel, float yPosVal) override;
    void OnRemoteXYPosChange(unsigned int channel, float xPosVal, float yPosVal) override;
    void OnRemoteSpreadChange(unsigned int channel, float spreadVal) override;
    void OnRemoteReverbSendGainChange(unsigned int channel, float reverbSendGainVal) override;

private:
    std::unique_ptr<SurroundFieldMixerProcessor>        m_SurroundFieldMixerProcessor;

    std::unique_ptr<SurroundFieldMixerRemoteWrapper>    m_SurroundFieldMixerRemote;

    std::unique_ptr<SurroundFieldMixerEditor>           m_audioVisuComponent;
    std::unique_ptr<AudioSelectComponent>               m_audioDeviceSelectComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SurroundFieldMixer)
};

};
