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

#include "PositionEditorComponent.h"


namespace SurroundFieldMixer
{


//==============================================================================
PositionEditorPopupComponent::PositionEditorPopupComponent()
{
    lookAndFeelChanged();

    setSize(50, 50);
}

PositionEditorPopupComponent::~PositionEditorPopupComponent()
{

}

void PositionEditorPopupComponent::paint(Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId).darker());

    auto bounds = getLocalBounds();

    // draw 1px border
    g.setColour(getLookAndFeel().findColour(TextButton::buttonColourId));
    g.drawRect(bounds);

    auto positioningNormalArea = bounds.reduced(10);

    g.setColour(getLookAndFeel().findColour(TextButton::buttonColourId).darker());
    g.drawRect(positioningNormalArea);

    if (getCurrentPositionCallback)
    {
        auto relPos = positioningNormalArea.getTopLeft().toFloat() + (juce::Point<float>(positioningNormalArea.getWidth(), positioningNormalArea.getHeight()) * juce::Point<float>(std::get<0>(getCurrentPositionCallback()), std::get<1>(getCurrentPositionCallback())));
        auto verticalPosLine = juce::Line<float>(juce::Point<float>(relPos.getX(), positioningNormalArea.getY()), juce::Point<float>(relPos.getX(), positioningNormalArea.getBottom()));
        g.drawLine(verticalPosLine, 1.0f);
        auto horizontalPosLine = juce::Line<float>(juce::Point<float>(positioningNormalArea.getX(), relPos.getY()), juce::Point<float>(positioningNormalArea.getRight(), relPos.getY()));
        g.drawLine(horizontalPosLine, 1.0f);

        g.setColour(Colours::forestgreen);
        g.fillEllipse(juce::Rectangle<float>(relPos - juce::Point<float>(5.0f, 5.0f), relPos + juce::Point<float>(5.0f, 5.0f)));
    }

    Component::paint(g);
}

void PositionEditorPopupComponent::resized()
{
    auto bounds = getLocalBounds();

    Component::resized();
}

//==============================================================================
PositionEditorComponent::PositionEditorComponent()
{
    lookAndFeelChanged();
}

PositionEditorComponent::~PositionEditorComponent()
{

}

void PositionEditorComponent::paint(Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId).darker());

    auto bounds = getLocalBounds();

    // draw 1px border
    g.setColour(getLookAndFeel().findColour(TextButton::buttonColourId));
    g.drawRect(bounds);

    auto positioningNormalArea = bounds.reduced(3);

    g.setColour(getLookAndFeel().findColour(TextButton::buttonColourId).darker());
    g.drawRect(positioningNormalArea);
    
    auto relPos = positioningNormalArea.getTopLeft().toFloat() + (juce::Point<float>(positioningNormalArea.getWidth(), positioningNormalArea.getHeight()) * juce::Point<float>(std::get<0>(getCurrentPosition()), std::get<1>(getCurrentPosition())));
    auto verticalPosLine = juce::Line<float>(juce::Point<float>(relPos.getX(), positioningNormalArea.getY()), juce::Point<float>(relPos.getX(), positioningNormalArea.getBottom()));
    g.drawLine(verticalPosLine, 1.0f);
    auto horizontalPosLine = juce::Line<float>(juce::Point<float>(positioningNormalArea.getX(), relPos.getY()), juce::Point<float>(positioningNormalArea.getRight(), relPos.getY()));
    g.drawLine(horizontalPosLine, 1.0f);

    g.setColour(Colours::forestgreen);
    g.fillEllipse(juce::Rectangle<float>(relPos - juce::Point<float>(2.0f, 2.0f), relPos + juce::Point<float>(2.0f, 2.0f)));

    Component::paint(g);
}

void PositionEditorComponent::resized()
{
	auto bounds = getLocalBounds();

    Component::resized();
}

void PositionEditorComponent::mouseUp(const MouseEvent& event)
{
    auto mouseUpPosition = event.getMouseDownPosition() + event.getOffsetFromDragStart();
    auto mouseUpWithinComponent = getLocalBounds().contains(mouseUpPosition);
    if (mouseUpWithinComponent)
        triggerPositioningPopup(mouseUpPosition);

    Component::mouseUp(event);
}

void PositionEditorComponent::handleMessage(const Message& msg)
{
    //if (auto* callbackMessage = dynamic_cast<const CallbackMidiMessage*> (&msg))
    //{
    //    auto& midiMessage = callbackMessage->_message;
    //
    //    DBG(String(__FUNCTION__) + " MIDI received: " + midiMessage.getDescription());
    //
    //    // sanity check if the incoming message comes from the device we want to listen to
    //    if (m_deviceIdentifier.isEmpty() || (m_deviceIdentifier != callbackMessage->_source->getDeviceInfo().identifier))
    //        return;
    //
    //    // start handling of the incoming data
    //    auto commandRangeAssi = JUCEAppBasics::MidiCommandRangeAssignment(midiMessage);
    //    auto commandType = commandRangeAssi.getCommandType();
    //    auto& commandData = commandRangeAssi.getCommandData();
    //
    //    // direct command assignments without value or command range
    //    {
    //        // iterate through already known command data/type to find if existing entries can be extended
    //        auto directAssisKeyIndex = -1;
    //        if (m_learnedDirectAssis.count(commandType) > 0)
    //            for (auto& learnedAssiKV : m_learnedDirectAssis.at(commandType))
    //                if (learnedAssiKV.second.getCommandData() == commandData && learnedAssiKV.second.isMatchingValueRange(midiMessage))
    //                    directAssisKeyIndex = learnedAssiKV.first;
    //
    //        // if the command data/type is not yet represented, create new entries for it
    //        if (directAssisKeyIndex == -1)
    //        {
    //            auto nextKey = ++m_popupItemIndexCounter;
    //            m_learnedDirectAssis[commandType][nextKey] = commandRangeAssi;
    //        }
    //    }
    //
    //    // command assignments with value range
    //    {
    //        // iterate through already known command data/type to find if existing entries can be extended
    //        auto commandAssisKeyIndex = -1;
    //        if (m_learnedValueRangeAssis.count(commandType) > 0)
    //        {
    //            for (auto& learnedAssiKV : m_learnedValueRangeAssis.at(commandType))
    //            {
    //                if (learnedAssiKV.second.getCommandData() == commandData)
    //                {
    //                    learnedAssiKV.second.extendValueRange(midiMessage);
    //                    commandAssisKeyIndex = learnedAssiKV.first;
    //                }
    //            }
    //        }
    //
    //        // if the command data/type is not yet represented, create new entries for it
    //        if (commandAssisKeyIndex == -1)
    //        {
    //            auto nextKey = ++m_popupItemIndexCounter;
    //            m_learnedValueRangeAssis[commandType][nextKey] = commandRangeAssi;
    //        }
    //    }
    //
    //    // assignment of a range of command and range of values
    //    {
    //        // iterate through already known command data/type to find if existing entries can be extended
    //        auto commandAndValueAssisKeyIndex = -1;
    //        if (m_learnedCommandAndValueRangeAssis.count(commandType) > 0)
    //        {
    //            jassert(m_learnedCommandAndValueRangeAssis.at(commandType).size() == 1);
    //            auto learnedAssiKV = m_learnedCommandAndValueRangeAssis.at(commandType).begin();
    //            
    //            learnedAssiKV->second.extendCommandRange(midiMessage);
    //            learnedAssiKV->second.extendValueRange(midiMessage);
    //            commandAndValueAssisKeyIndex = learnedAssiKV->first;
    //        }
    //
    //        // if the command data/type is not yet represented, create new entries for it
    //        if (commandAndValueAssisKeyIndex == -1)
    //        {
    //            auto nextKey = ++m_popupItemIndexCounter;
    //            m_learnedCommandAndValueRangeAssis[commandType][nextKey] = commandRangeAssi;
    //        }
    //    }
    //
    //    // if the cyclical updating of the popup menu contents is not active, start now to display the new available assignments
    //    if (!isTimerUpdatingPopup())
    //        startTimerUpdatingPopup();
    //}
}

void PositionEditorComponent::lookAndFeelChanged()
{
    Component::lookAndFeelChanged();
}

void PositionEditorComponent::triggerPositioningPopup(const juce::Point<int>& popupStartPosition)
{
    if (!m_positioningPopup)
        m_positioningPopup = std::make_unique<PositionEditorPopupComponent>();

    if (m_positioningPopup)
    {
        auto callback = std::make_unique<PositioningPopupCallback>();

        m_positioningPopup->getCurrentPositionCallback = [this]() { return getCurrentPosition(); };
        m_positioningPopup->setCurrentPositionCallback = [this](const std::tuple<float, float, float>& position) { setCurrentPosition(position); };

        m_positioningPopup->setVisible(true);
        m_positioningPopup->enterModalState(false, nullptr, true);
        ModalComponentManager::getInstance()->attachCallback(m_positioningPopup.get(), callback.release());

        m_positioningPopup->setTopLeftPosition(popupStartPosition);
        m_positioningPopup->toFront(false);  // need to do this after making it modal, or it could
                                  // be stuck behind other comps that are already modal..

    }
}

void PositionEditorComponent::setCurrentPosition(const std::tuple<float, float, float>& currentPosition)
{
    m_currentPosition = currentPosition;
}

const std::tuple<float, float, float>& PositionEditorComponent::getCurrentPosition()
{
    return m_currentPosition;
}

}
