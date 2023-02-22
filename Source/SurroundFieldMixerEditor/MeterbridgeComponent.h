/* Copyright (c) 2022 - 2023, Christian Ahrens
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

#include "AbstractAudioVisualizer.h"


namespace SurroundFieldMixer
{

//==============================================================================
/*
*/
class MeterbridgeComponent : public AbstractAudioVisualizer
{
public:
    MeterbridgeComponent();
    ~MeterbridgeComponent();

    //==============================================================================
    void paint (Graphics&) override;

    //==============================================================================
    void processingDataChanged(AbstractProcessorData* data) override;

private:
    ProcessorLevelData  m_levelData;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MeterbridgeComponent)
};

}