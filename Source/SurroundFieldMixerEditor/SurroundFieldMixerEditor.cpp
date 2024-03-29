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

#include "SurroundFieldMixerEditor.h"

#include <JuceHeader.h>

#include "../SurroundFieldMixerEditor/InputMixerComponent.h"
#include "../SurroundFieldMixerEditor/TwoDFieldOutputComponent.h"


namespace SurroundFieldMixer
{

//==============================================================================
SurroundFieldMixerEditor::SurroundFieldMixerEditor(AudioProcessor& processor)
    : AudioProcessorEditor(processor)
{
    m_mixer = std::make_unique<InputMixerComponent>();
    addAndMakeVisible(m_mixer.get());

    m_surroundField = std::make_unique<TwoDFieldOutputComponent>();
    addAndMakeVisible(m_surroundField.get());

    auto SurroundFieldMixerProc = dynamic_cast<SurroundFieldMixerProcessor*>(&processor);
    if (SurroundFieldMixerProc)
    {
        SurroundFieldMixerProc->addInputListener(m_mixer.get());
        SurroundFieldMixerProc->addInputCommander(m_mixer.get());
        SurroundFieldMixerProc->addInputCommander(m_surroundField.get());
        SurroundFieldMixerProc->addOutputListener(m_surroundField.get());
        SurroundFieldMixerProc->addOutputCommander(m_surroundField.get());
    }

    setSize(800, 700);
}

SurroundFieldMixerEditor::SurroundFieldMixerEditor(AudioProcessor* processor)
    : SurroundFieldMixerEditor(*processor)
{
}

SurroundFieldMixerEditor::~SurroundFieldMixerEditor()
{
}

void SurroundFieldMixerEditor::paint (Graphics& g)
{
    auto bounds = getLocalBounds();

    // Background
    g.setColour(getLookAndFeel().findColour(AlertWindow::backgroundColourId).darker());
    g.fillRect(bounds.toFloat());
}

void SurroundFieldMixerEditor::resized()
{
    auto bounds = getLocalBounds();
    auto mixerBounds = bounds;
    auto surroundFieldBounds = bounds;

    // horizontal layout
    if (m_editorLayouting == EL_Horizontal || (m_editorLayouting == EL_Dynamic && bounds.getWidth() > bounds.getHeight()))
    {
        mixerBounds = bounds.removeFromLeft(static_cast<int>(bounds.getWidth() * 0.3f));
        mixerBounds.reduce(0, 5);
        mixerBounds.removeFromRight(5);
        mixerBounds.removeFromLeft(5);

        surroundFieldBounds = bounds;
        surroundFieldBounds.reduce(0, 5);
        surroundFieldBounds.removeFromRight(5);
    }
    // vertical layout
    else if (m_editorLayouting == EL_Vertical || (m_editorLayouting == EL_Dynamic && bounds.getWidth() <= bounds.getHeight()))
    {
        mixerBounds = bounds.removeFromBottom(static_cast<int>(bounds.getHeight() * 0.3f));
        mixerBounds.reduce(5, 0);
        mixerBounds.removeFromTop(5);
        mixerBounds.removeFromBottom(5);

        surroundFieldBounds = bounds;
        surroundFieldBounds.reduce(5, 0);
        surroundFieldBounds.removeFromTop(5);
    }

    m_mixer->setBounds(mixerBounds);
    m_surroundField->setBounds(surroundFieldBounds);
}

void SurroundFieldMixerEditor::lookAndFeelChanged()
{
    AudioProcessorEditor::lookAndFeelChanged();
}

std::unique_ptr<XmlElement> SurroundFieldMixerEditor::createStateXml()
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

bool SurroundFieldMixerEditor::setStateXml(XmlElement* /*stateXml*/)
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

void SurroundFieldMixerEditor::lockCurrentLayout(bool doLock)
{
    if (doLock)
        m_editorLayouting = (getLocalBounds().getWidth() > getLocalBounds().getHeight()) ? EL_Horizontal : EL_Vertical;
    else
        m_editorLayouting = EL_Dynamic;

    resized();
}

}
