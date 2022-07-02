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

#include "SurroundFieldMixer.h"

namespace SurroundFieldMixer
{

//==============================================================================
SurroundFieldMixer::SurroundFieldMixer() :
    Component(),
    JUCEAppBasics::AppConfigurationBase::XmlConfigurableElement()
{
    m_SurroundFieldMixerProcessor = std::make_unique<SurroundFieldMixerProcessor>();
    m_audioDeviceSelectComponent = std::make_unique<AudioSelectComponent>(m_SurroundFieldMixerProcessor->getDeviceManager(), 1, 64, 1, 8, false, false, false, false);
    m_SurroundFieldMixerRemote = std::make_unique<SurroundFieldMixerRemoteWrapper>();

    m_SurroundFieldMixerRemote->AddListener(this);
}

SurroundFieldMixer::~SurroundFieldMixer()
{
    if (m_SurroundFieldMixerProcessor)
        m_SurroundFieldMixerProcessor->editorBeingDeleted(m_SurroundFieldMixerProcessor->getActiveEditor());
}

juce::Component* SurroundFieldMixer::getUIComponent()
{
    if (m_SurroundFieldMixerProcessor)
    {
        if (nullptr == m_SurroundFieldMixerProcessor->getActiveEditor())
            m_SurroundFieldMixerProcessor->createEditorIfNeeded();
        return m_SurroundFieldMixerProcessor->getActiveEditor();
    }
    else
        return nullptr;
}

juce::Component* SurroundFieldMixer::getDeviceSetupComponent()
{
    if (m_audioDeviceSelectComponent)
        return m_audioDeviceSelectComponent.get();
    else
        return nullptr;
}

std::unique_ptr<XmlElement> SurroundFieldMixer::createStateXml()
{
    jassertfalse;
    return nullptr;
}

bool SurroundFieldMixer::setStateXml(XmlElement* stateXml)
{
    jassertfalse;
    return false;
}

void SurroundFieldMixer::OnRemoteMuteChange(unsigned int channel, int muteVal)
{

}

void SurroundFieldMixer::OnRemoteXPosChange(unsigned int channel, float xPosVal)
{

}

void SurroundFieldMixer::OnRemoteYPosChange(unsigned int channel, float yPosVal)
{

}

void SurroundFieldMixer::OnRemoteXYPosChange(unsigned int channel, float xPosVal, float yPosVal)
{

}

void SurroundFieldMixer::OnRemoteSpreadFactorChange(unsigned int channel, float spreadFactorVal)
{

}

void SurroundFieldMixer::OnRemoteReverbSendGainChange(unsigned int channel, float reverbSendGainVal)
{

}


}