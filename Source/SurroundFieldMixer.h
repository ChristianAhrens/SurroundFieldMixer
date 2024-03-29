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


namespace SurroundFieldMixer
{

/**
 * Fwd. decls
 */
class AudioSelectComponent;
class SurroundFieldMixerEditor;
class SurroundFieldMixerProcessor;
class SurroundFieldMixerRemoteWrapper;

//==============================================================================
/*
 *
 */
class SurroundFieldMixer   :    public Component,
                                public JUCEAppBasics::AppConfigurationBase::XmlConfigurableElement
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
    bool isControlOnline();
    std::function<void()>   onRemoteOnlineCallback;

    //==========================================================================
    void lockCurrentLayout(bool doLock);

private:
    void setControlOnlineState(bool online);


    std::unique_ptr<SurroundFieldMixerProcessor>        m_SurroundFieldMixerProcessor;

    std::unique_ptr<SurroundFieldMixerRemoteWrapper>    m_SurroundFieldMixerRemote;
    bool                                                m_SurroundFieldMixerRemoteOnline;

    std::unique_ptr<SurroundFieldMixerEditor>           m_audioVisuComponent;
    std::unique_ptr<AudioSelectComponent>               m_audioDeviceSelectComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SurroundFieldMixer)
};

};
