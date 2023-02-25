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

#include <JuceHeader.h>
#include <variant>

#include "../SurroundFieldMixerProcessor/SurroundFieldMixerProcessor.h"


namespace SurroundFieldMixer
{


class SurroundFieldMixerSnapshotCommander :	public SurroundFieldMixerProcessor::InputCommander,
											public SurroundFieldMixerProcessor::OutputCommander
{
public:
	enum ValueId
	{
		InputMute,
		InputGain,
		InputPosition,
		InputSpread,
		InputReverb,
		OutputMute,
		OutputGain,
		OutputScheme
	};
	typedef std::uint32_t	ChannelNo;
	static constexpr ChannelNo INVALID_CHANNEL = 0;

public:
	SurroundFieldMixerSnapshotCommander();
	~SurroundFieldMixerSnapshotCommander();
	
	//==========================================================================
	void setInputMute(unsigned int channel, bool muteState) override;
	void setInputGain(unsigned int channel, float gainValue) override;
	void setPosition(unsigned int channel, juce::Point<float> position) override;
	void setSpreadFactor(unsigned int channel, float spreadFactor) override;
	void setReverbSendGain(unsigned int channel, float reverbSendGain) override;

	//==========================================================================
	void setOutputMute(unsigned int channel, bool muteState) override;
	void setOutputGain(unsigned int channel, float gainValue) override;
	void setOutputScheme(unsigned int outputScheme) override;

protected:
	//==========================================================================

private:
	//==========================================================================
	std::map<ValueId, std::map<ChannelNo, std::variant<bool, unsigned int, int, float, juce::Point<float>>>>	m_currentValues;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SurroundFieldMixerSnapshotCommander)
};

}
