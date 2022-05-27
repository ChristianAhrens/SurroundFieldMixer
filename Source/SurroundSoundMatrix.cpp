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

#include "SurroundSoundMatrix.h"

namespace SurroundSoundMatrix
{

//==============================================================================
SurroundSoundMatrix::SurroundSoundMatrix() :
    Component(),
    JUCEAppBasics::AppConfigurationBase::XmlConfigurableElement()
{
    m_SurroundSoundMatrixProcessor = std::make_unique<SurroundSoundMatrixProcessor>();
    m_audioDeviceSelectComponent = std::make_unique<AudioSelectComponent>(m_SurroundSoundMatrixProcessor->getDeviceManager(), 1, 64, 1, 8, false, false, false, false);
}

SurroundSoundMatrix::~SurroundSoundMatrix()
{
    if (m_SurroundSoundMatrixProcessor)
        m_SurroundSoundMatrixProcessor->editorBeingDeleted(m_SurroundSoundMatrixProcessor->getActiveEditor());
}

juce::Component* SurroundSoundMatrix::getUIComponent()
{
    if (m_SurroundSoundMatrixProcessor)
    {
        if (nullptr == m_SurroundSoundMatrixProcessor->getActiveEditor())
            m_SurroundSoundMatrixProcessor->createEditorIfNeeded();
        return m_SurroundSoundMatrixProcessor->getActiveEditor();
    }
    else
        return nullptr;
}

juce::Component* SurroundSoundMatrix::getDeviceSetupComponent()
{
    if (m_audioDeviceSelectComponent)
        return m_audioDeviceSelectComponent.get();
    else
        return nullptr;
}

std::unique_ptr<XmlElement> SurroundSoundMatrix::createStateXml()
{
    jassertfalse;
    return nullptr;
}

bool SurroundSoundMatrix::setStateXml(XmlElement* stateXml)
{
    jassertfalse;
    return false;
}

}