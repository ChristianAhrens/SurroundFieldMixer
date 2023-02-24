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

#pragma once

#include "../SurroundFieldMixerProcessor/SurroundFieldMixerProcessor.h"

#include <JuceHeader.h>

namespace SurroundFieldMixer
{


class SurroundFieldMixerSnapshotCommander :	public SurroundFieldMixerProcessor::InputCommander,
											public SurroundFieldMixerProcessor::OutputCommander
{
public:
	SurroundFieldMixerSnapshotCommander();
	~SurroundFieldMixerSnapshotCommander();
	
	//==========================================================================
	void setInputMute(unsigned int channel, bool muteState) override;
	void setInputGain(unsigned int channel, float gainValue) override;
	void setInputLevel(unsigned int channel, float levelValue) override;
	void setPosition(unsigned int channel, juce::Point<float> position) override;
	void setSpreadFactor(unsigned int channel, float spreadFactor) override;
	void setReverbSendGain(unsigned int channel, float reverbSendGain) override;

	//==========================================================================
	void setOutputMute(unsigned int channel, bool muteState) override;
	void setOutputGain(unsigned int channel, float gainValue) override;
	void setOutputLevel(unsigned int channel, float levelValue) override;
	void setOutputScheme(unsigned int outputScheme) override;

protected:
	//==========================================================================

private:
	//==========================================================================
	std::map<unsigned int, bool>				m_inputMutes;
	std::map<unsigned int, float>				m_inputGains;
	std::map<unsigned int, float>				m_inputLevels;
	std::map<unsigned int, juce::Point<float>>	m_inputPositions;
	std::map<unsigned int, float>				m_inputSpreadFactors;
	std::map<unsigned int, float>				m_inputReverbSendGains;

	//==========================================================================
	std::map<unsigned int, bool>	m_outputMutes;
	std::map<unsigned int, float>	m_outputGains;
	std::map<unsigned int, float>	m_outputLevels;
	unsigned int					m_outputScheme;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SurroundFieldMixerSnapshotCommander)
};

}
