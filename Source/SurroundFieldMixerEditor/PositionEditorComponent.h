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


namespace SurroundFieldMixer
{


class PositionEditorPopupComponent : public Component
{
public:
    PositionEditorPopupComponent();
    ~PositionEditorPopupComponent();

    //==============================================================================
    void paint(Graphics&) override;
    void resized() override;

    //==============================================================================
    void mouseUp(const MouseEvent& event) override;
    void mouseDrag(const MouseEvent& event) override;

    //==============================================================================
    std::function<juce::Point<float>(void)> getCurrentPositionCallback;
    std::function<void(juce::Point<float>)> setCurrentPositionCallback;

private:
    void updatePosition(const juce::Point<int>& mousePosition);
};


class PositionEditorComponent : public Component
{
public:
    PositionEditorComponent();
    ~PositionEditorComponent();
	
    //==============================================================================
    void paint(Graphics&) override;
    void resized() override;

    //==============================================================================
    void mouseDown(const MouseEvent& event) override;
    void mouseUp(const MouseEvent& event) override;

    //==============================================================================
    void lookAndFeelChanged() override;

    //==============================================================================
    std::function<void(Component*, const juce::Point<float>&)> setPositionCallback;
    
    //==============================================================================
    void setCurrentPosition(const juce::Point<float>& currentPosition);
    const juce::Point<float> getCurrentPosition();

    //==============================================================================
    void closePositioningPopup();
    
private:
    void triggerPositioningPopup(const juce::Point<int>& popupStartPosition);

    std::unique_ptr<PositionEditorPopupComponent>   m_positioningPopup;
    
    juce::Point<float>                 m_currentPosition;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PositionEditorComponent)
};

};
