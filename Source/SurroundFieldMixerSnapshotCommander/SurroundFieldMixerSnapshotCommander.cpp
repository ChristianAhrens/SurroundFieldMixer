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
	m_inputMutes[channel] = muteState;
}

void SurroundFieldMixerSnapshotCommander::setInputGain(unsigned int channel, float gainValue)
{
	m_inputGains[channel] = gainValue;
}

void SurroundFieldMixerSnapshotCommander::setInputLevel(unsigned int channel, float levelValue)
{
	m_inputLevels[channel] = levelValue;
}

void SurroundFieldMixerSnapshotCommander::setPosition(unsigned int channel, juce::Point<float> position)
{
	m_inputPositions[channel] = position;
}

void SurroundFieldMixerSnapshotCommander::setSpreadFactor(unsigned int channel, float spreadFactor)
{
	m_inputSpreadFactors[channel] = spreadFactor;
}

void SurroundFieldMixerSnapshotCommander::setReverbSendGain(unsigned int channel, float reverbSendGain)
{
	m_inputReverbSendGains[channel] = reverbSendGain;
}

void SurroundFieldMixerSnapshotCommander::setOutputMute(unsigned int channel, bool muteState)
{
	m_outputMutes[channel] = muteState;
}

void SurroundFieldMixerSnapshotCommander::setOutputGain(unsigned int channel, float gainValue)
{
	m_outputGains[channel] = gainValue;
}

void SurroundFieldMixerSnapshotCommander::setOutputLevel(unsigned int channel, float levelValue)
{
	m_outputLevels[channel] = levelValue;
}

void SurroundFieldMixerSnapshotCommander::setOutputScheme(unsigned int /*outputScheme*/)
{
	/*t.b.d*/
}


}
