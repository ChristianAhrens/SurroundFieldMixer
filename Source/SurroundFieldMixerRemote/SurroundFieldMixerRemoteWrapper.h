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

#include <ProcessingEngine/ProcessingEngineNode.h>

#include "../SurroundFieldMixerProcessor/SurroundFieldMixerProcessor.h"

#include <JuceHeader.h>

namespace SurroundFieldMixer
{

static constexpr int RX_PORT_DS100_DEVICE = 50010;		//< UDP port which the DS100 is listening to for OSC
static constexpr int RX_PORT_DS100_HOST = 50011;		//< UDP port to which the DS100 will send OSC replies


/**
 * Pre-define processing bridge config values
 */
static constexpr int DEFAULT_PROCNODE_ID = 1;
static constexpr int DS100_1_PROCESSINGPROTOCOL_ID = 2;

class SurroundFieldMixerRemoteWrapper :	public ProcessingEngineNode::NodeListener,
										public SurroundFieldMixerProcessor::InputCommander,
										public SurroundFieldMixerProcessor::OutputCommander,
										public juce::Timer
{
public:
	SurroundFieldMixerRemoteWrapper();
	~SurroundFieldMixerRemoteWrapper();
	
	//==========================================================================
	void setInputMute(unsigned int channel, bool muteState) override;
	void setPosition(unsigned int channel, juce::Point<float> position) override;
	void setSpreadFactor(unsigned int channel, float spreadFactor) override;
	void setReverbSendGain(unsigned int channel, float reverbSendGain) override;

	//==========================================================================
	void setOutputMute(unsigned int channel, bool muteState) override;
	void setOutputScheme(unsigned int outputScheme) override;

	//==========================================================================
	void Disconnect();
	void Reconnect();

	//==========================================================================
	void timerCallback() override;

private:
	//==========================================================================
	void HandleNodeData(const ProcessingEngineNode::NodeCallbackMessage* callbackMessage) override;
	bool SendMessage(RemoteObjectIdentifier Id, RemoteObjectMessageData& msgData);

	//==========================================================================
	void SetupBridgingNode();

	//==========================================================================
	void OnRemoteHeartbeatReceived();

	ProcessingEngineNode							m_processingNode;	/**< The node that encapsulates the protocols that are used to send, receive and bridge data. */
	XmlElement										m_bridgingXml;		/**< The current xml config for bridging (contains node xml). */

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SurroundFieldMixerRemoteWrapper)
};

}
