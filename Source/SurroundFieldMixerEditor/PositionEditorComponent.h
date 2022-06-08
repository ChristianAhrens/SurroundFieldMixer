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

class PositionEditorComponent :
    public Component,
    private MessageListener
{

public:
    PositionEditorComponent();
    ~PositionEditorComponent();
	
    //==============================================================================
    void paint(Graphics&) override;
    void resized() override;
    
    //==============================================================================
    void handleMessage(const Message& msg) override;

    //==============================================================================
    void lookAndFeelChanged() override;

    //==============================================================================
    std::function<void(Component*, std::tuple<float, float, float>)> onPositionSet;
    
    //==============================================================================
    void setCurrentPosition(const std::tuple<float, float, float>& currentPosition);
    const std::tuple<float, float, float>& getCurrentPosition();
    
private:
    //void triggerPositioningPopup();
    //void updatePopupMenu();
    //void handlePopupResult();
    //
    //PopupMenu                       m_popup;
    
    std::tuple<float, float, float>     m_currentPosition;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PositionEditorComponent)
};

};