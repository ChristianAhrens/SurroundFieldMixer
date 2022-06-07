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

#include "AbstractAudioVisualizer.h"

#include <Image_utils.h>

namespace SurroundFieldMixer
{


//==============================================================================
AbstractAudioVisualizer::AbstractAudioVisualizer()
{
    m_changesPending = false;
}

AbstractAudioVisualizer::~AbstractAudioVisualizer()
{
}

void AbstractAudioVisualizer::paint (Graphics& g)
{
    Component::paint(g);
}

void AbstractAudioVisualizer::resized()
{

}

void AbstractAudioVisualizer::mouseDown(const MouseEvent& event)
{
    Component::mouseDown(event);
}

void AbstractAudioVisualizer::setUsesValuesInDB(bool useValuesInDB)
{
    m_usesValuesInDB = useValuesInDB;
}

bool AbstractAudioVisualizer::getUsesValuesInDB()
{
    return m_usesValuesInDB;
}

//std::unique_ptr<XmlElement> AbstractAudioVisualizer::createStateXml()
//{
//    XmlElement visualizerElement(String(AbstractAudioVisualizer::VisuTypeToString(getType())));
//
//    auto mappingStateXml = std::make_unique<XmlElement>(AppConfiguration::getTagName(AppConfiguration::TagID::VISUMAP));
//    for (auto mapping : m_channelMapping)
//    {
//        XmlElement* mappingElement = mappingStateXml->createNewChildElement("M" + String(static_cast<int>(mapping.first)));
//        if (mappingElement)
//        {
//            mappingElement->setAttribute("name", AudioVisualizerConfigBase::getMappingString(mapping.first));
//            mappingElement->addTextElement(String(mapping.second));
//        }
//    }
//
//    auto usesValuesInDBStateXml = std::make_unique<XmlElement>(AppConfiguration::getTagName(AppConfiguration::TagID::VISUUSEDB));
//    usesValuesInDBStateXml->addTextElement(m_usesValuesInDB ? "true" : "false");
//
//    visualizerElement.addChildElement(usesValuesInDBStateXml.release());
//    visualizerElement.addChildElement(mappingStateXml.release());
//
//    return std::make_unique<XmlElement>(visualizerElement);
//}
//
//bool AbstractAudioVisualizer::setStateXml(XmlElement* stateXml)
//{
//    if (!stateXml)
//        return false;
//
//    if (stateXml->getTagName() != String(AbstractAudioVisualizer::VisuTypeToString(getType())))
//        return false;
//
//    for (auto stateXmlElement : stateXml->getChildIterator())
//    {
//        if (stateXmlElement->getTagName() == AppConfiguration::getTagName(AppConfiguration::TagID::VISUMAP))
//        {
//            for (auto mappingElement : stateXmlElement->getChildIterator())
//            {
//                if (m_channelMapping.count(static_cast<AudioVisualizerConfigBase::MappingKey>(mappingElement->getTagName().getTrailingIntValue())) > 0)
//                    m_channelMapping.at(static_cast<AudioVisualizerConfigBase::MappingKey>(mappingElement->getTagName().getTrailingIntValue())) = mappingElement->getAllSubText().getIntValue();
//            }
//        }
//        else if (stateXmlElement->getTagName() == AppConfiguration::getTagName(AppConfiguration::TagID::VISUUSEDB))
//        {
//            auto valueString = stateXmlElement->getAllSubText();
//            m_usesValuesInDB = (stateXmlElement->getAllSubText() == "true") ? true : false;
//        }
//    }
//
//    processChangedChannelMapping();
//
//    return true;
//}

void AbstractAudioVisualizer::notifyChanges()
{
    m_changesPending = true;
    processChanges();
}

void AbstractAudioVisualizer::processChanges()
{
    if (m_changesPending)
    {
        m_changesPending = false;
        repaint();
    }
}

}
