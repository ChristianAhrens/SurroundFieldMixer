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
	:	SurroundFieldMixerProcessor::InputCommander(),
		SurroundFieldMixerProcessor::OutputCommander()
{
}

SurroundFieldMixerSnapshotCommander::~SurroundFieldMixerSnapshotCommander()
{
}

void SurroundFieldMixerSnapshotCommander::initialize()
{
	setDefaultValues();
	publishCurrentValues();
}

void SurroundFieldMixerSnapshotCommander::setDefaultValues()
{
	for (auto channel = 1; channel <= SurroundFieldMixerProcessor::s_maxChannelCount; channel++)
	{
		setInputMute(channel, false);
		setInputGain(channel, 0.8f);
		setPosition(channel, juce::Point<float>(0.5f, 0.5f));
		setSpreadFactor(channel, 0.5f);
		setReverbSendGain(channel, 0.0f);
		setOutputMute(channel, false);
		setOutputGain(channel, 0.8f);
	}
	setOutputScheme(1);
}

void SurroundFieldMixerSnapshotCommander::publishCurrentValues()
{
	for (auto channel = 1; channel <= SurroundFieldMixerProcessor::s_maxChannelCount; channel++)
	{
		inputMuteChange(channel, getInputMute(channel));
		inputGainChange(channel, getInputGain(channel));
		positionChange(channel, getPosition(channel));
		spreadFactorChange(channel, getSpreadFactor(channel));
		reverbSendGainChange(channel, getReverbSendGain(channel));
		outputMuteChange(channel, getOutputMute(channel));
		outputGainChange(channel, getOutputGain(channel));
	}
	outputSchemeChange(getOutputScheme());
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

bool SurroundFieldMixerSnapshotCommander::getInputMute(unsigned int channel)
{
	return std::get<bool>(m_currentValues[ValueId::InputMute][channel]);
}

float SurroundFieldMixerSnapshotCommander::getInputGain(unsigned int channel)
{
	return std::get<float>(m_currentValues[ValueId::InputGain][channel]);
}

const juce::Point<float> SurroundFieldMixerSnapshotCommander::getPosition(unsigned int channel)
{
	return std::get<juce::Point<float>>(m_currentValues[ValueId::InputPosition][channel]);
}

float SurroundFieldMixerSnapshotCommander::getSpreadFactor(unsigned int channel)
{
	return std::get<float>(m_currentValues[ValueId::InputSpread][channel]);
}

float SurroundFieldMixerSnapshotCommander::getReverbSendGain(unsigned int channel)
{
	return std::get<float>(m_currentValues[ValueId::InputReverb][channel]);
}

bool SurroundFieldMixerSnapshotCommander::getOutputMute(unsigned int channel)
{
	return std::get<bool>(m_currentValues[ValueId::OutputMute][channel]);
}

float SurroundFieldMixerSnapshotCommander::getOutputGain(unsigned int channel)
{
	return std::get<float>(m_currentValues[ValueId::OutputGain][channel]);
}

unsigned int SurroundFieldMixerSnapshotCommander::getOutputScheme()
{
	return std::get<unsigned int>(m_currentValues[ValueId::OutputScheme][INVALID_CHANNEL]);
}


}
