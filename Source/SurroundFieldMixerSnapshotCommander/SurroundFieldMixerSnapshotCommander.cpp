/* Copyright (c) 2023, Christian Ahrens
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

#include "SurroundFieldMixerSnapshotCommander.h"


namespace SurroundFieldMixer
{


SurroundFieldMixerSnapshotCommander::SurroundFieldMixerSnapshotCommander()
{

}

SurroundFieldMixerSnapshotCommander::~SurroundFieldMixerSnapshotCommander()
{
	
}

void SurroundFieldMixerSnapshotCommander::setInputMute(unsigned int channel, bool muteState)
{
	m_currentValues[ValueId::InputMute][channel] = muteState;
}

void SurroundFieldMixerSnapshotCommander::setInputGain(unsigned int channel, float gainValue)
{
	m_currentValues[ValueId::InputGain][channel] = gainValue;
}

void SurroundFieldMixerSnapshotCommander::setPosition(unsigned int channel, juce::Point<float> position)
{
	m_currentValues[ValueId::InputPosition][channel] = position;
}

void SurroundFieldMixerSnapshotCommander::setSpreadFactor(unsigned int channel, float spreadFactor)
{
	m_currentValues[ValueId::InputSpread][channel] = spreadFactor;
}

void SurroundFieldMixerSnapshotCommander::setReverbSendGain(unsigned int channel, float reverbSendGain)
{
	m_currentValues[ValueId::InputReverb][channel] = reverbSendGain;
}

void SurroundFieldMixerSnapshotCommander::setOutputMute(unsigned int channel, bool muteState)
{
	m_currentValues[ValueId::OutputMute][channel] = muteState;
}

void SurroundFieldMixerSnapshotCommander::setOutputGain(unsigned int channel, float gainValue)
{
	m_currentValues[ValueId::OutputGain][channel] = gainValue;
}

void SurroundFieldMixerSnapshotCommander::setOutputScheme(unsigned int outputScheme)
{
	m_currentValues[ValueId::OutputScheme][INVALID_CHANNEL] = outputScheme;
}


}
