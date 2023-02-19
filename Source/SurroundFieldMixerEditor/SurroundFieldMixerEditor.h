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
#include "AudioSelectComponent.h"


namespace SurroundFieldMixer
{

class MultiMeterInputComponent;
class TwoDFieldOutputComponent;


//==============================================================================
/*
*/
class SurroundFieldMixerEditor :    public AudioProcessorEditor,
                                    public JUCEAppBasics::AppConfigurationBase::XmlConfigurableElement
{
public:
    enum EditorLayout
    {
        EL_Dynamic,
        EL_Horizontal,
        EL_Vertical
    };

public:
    SurroundFieldMixerEditor(AudioProcessor& processor);
    SurroundFieldMixerEditor(AudioProcessor* processor);
    ~SurroundFieldMixerEditor();

    //==============================================================================
    void lockCurrentLayout(bool doLock);

    //==============================================================================
    void paint(Graphics&) override;
    void resized() override;

    //==========================================================================
    void lookAndFeelChanged() override;

    //==========================================================================
    std::unique_ptr<XmlElement> createStateXml() override;
    bool setStateXml(XmlElement* stateXml) override;

private:
    std::unique_ptr<MultiMeterInputComponent>   m_meterBank;
    std::unique_ptr<TwoDFieldOutputComponent>   m_surroundField;

    EditorLayout    m_editorLayouting{ EL_Dynamic };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SurroundFieldMixerEditor)
};

}
