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

#include "SurroundSoundMatrixEditor.h"

#include <JuceHeader.h>

#include "../SurroundSoundMatrixEditor/MultiMeterAudioVisualizer.h"
#include "../SurroundSoundMatrixEditor/TwoDFieldAudioVisualizer.h"


namespace SurroundSoundMatrix
{

//==============================================================================
SurroundSoundMatrixEditor::SurroundSoundMatrixEditor(AudioProcessor& processor)
    : AudioProcessorEditor(processor)
{
    m_meterBank = std::make_unique<MultiMeterAudioVisualizer>();
    addAndMakeVisible(m_meterBank.get());
    m_surroundField = std::make_unique<TwoDFieldAudioVisualizer>();
    addAndMakeVisible(m_surroundField.get());

    setSize(800, 600);
}

SurroundSoundMatrixEditor::SurroundSoundMatrixEditor(AudioProcessor* processor)
    : SurroundSoundMatrixEditor(*processor)
{
}

SurroundSoundMatrixEditor::~SurroundSoundMatrixEditor()
{
}

void SurroundSoundMatrixEditor::paint (Graphics& g)
{
    auto bounds = getLocalBounds();

    // Background
    g.setColour(getLookAndFeel().findColour(AlertWindow::backgroundColourId).darker());
    g.fillRect(bounds.toFloat());
}

void SurroundSoundMatrixEditor::resized()
{
    auto bounds = getLocalBounds();

    m_meterBank->setBounds(bounds.removeFromBottom(bounds.getHeight() * 0.25f).reduced(5, 5));
    m_surroundField->setBounds(bounds.reduced(5, 5));
}

void SurroundSoundMatrixEditor::lookAndFeelChanged()
{
    AudioProcessorEditor::lookAndFeelChanged();
}

std::unique_ptr<XmlElement> SurroundSoundMatrixEditor::createStateXml()
{
    //auto activeVisuTypes = getActiveVisuTypes();
    //
    //auto activeVisualizersElement = std::make_unique<XmlElement>(AppConfiguration::getTagName(AppConfiguration::TagID::VISU));
    //for (auto i = AbstractAudioVisualizer::InvalidFirst + 1; i < AbstractAudioVisualizer::InvalidLast; ++i)
    //{
    //    AbstractAudioVisualizer::VisuType visuType = static_cast<AbstractAudioVisualizer::VisuType>(i);
    //    auto visualizerElement = static_cast<XmlElement*>(0);
    //
    //    if (m_AudioVisualizers.count(visuType) == 0)
    //    {
    //        visualizerElement = activeVisualizersElement->createNewChildElement(AbstractAudioVisualizer::VisuTypeToString(visuType));
    //    }
    //    else
    //    {
    //        visualizerElement = m_AudioVisualizers.at(visuType)->createStateXml().release();
    //        activeVisualizersElement->addChildElement(visualizerElement);
    //    }
    //
    //    if(visualizerElement)
    //    {
    //        visualizerElement->setAttribute("isActive", (activeVisuTypes.count(visuType) > 0) ? 1 : 0);
    //    }
    //}
    //
    //return std::make_unique<XmlElement>(*activeVisualizersElement);

    return nullptr;
}

bool SurroundSoundMatrixEditor::setStateXml(XmlElement* stateXml)
{
    //if (!stateXml || (stateXml->getTagName() != AppConfiguration::getTagName(AppConfiguration::TagID::VISU)))
    //    return false;
    //
    //std::set<AbstractAudioVisualizer::VisuType> visualizerTypes = {};
    //std::map< AbstractAudioVisualizer::VisuType, XmlElement*> visualizerXmlElements;
    //for (auto visualizerChildElement : stateXml->getChildIterator())
    //{
    //    auto visuType = AbstractAudioVisualizer::StringToVisuType(visualizerChildElement->getTagName().toStdString());
    //    auto isActiveAttributeValue = visualizerChildElement->getBoolAttribute("isActive");
    //    if (visuType < AbstractAudioVisualizer::InvalidLast && isActiveAttributeValue)
    //    {
    //        visualizerTypes.insert(visuType);
    //        visualizerXmlElements.insert(std::make_pair(visuType, visualizerChildElement));
    //    }
    //}
    //
    //onUpdateVisuTypes(visualizerTypes);
    //
    //for (auto mappingStateXml : visualizerXmlElements)
    //{
    //    if (m_AudioVisualizers.count(mappingStateXml.first) > 0)
    //        m_AudioVisualizers.at(mappingStateXml.first)->setStateXml(mappingStateXml.second);
    //}

    return true;
}

}
