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

class SurroundFieldMixerRemoteWrapper :
	public ProcessingEngineNode::NodeListener,
	public juce::Timer
{
public:
	/**
	 * Abstract embedded interface class for message data handling
	 */
	class Listener
	{
	public:
		Listener() {};
		virtual ~Listener() {};

		virtual void OnRemoteMuteChange(unsigned int channel, int muteVal) = 0;
		virtual void OnRemoteXPosChange(unsigned int channel, float xPosVal) = 0;
		virtual void OnRemoteYPosChange(unsigned int channel, float yPosVal) = 0;
		virtual void OnRemoteXYPosChange(unsigned int channel, float xPosVal, float yPosVal) = 0;
		virtual void OnRemoteSpreadFactorChange(unsigned int channel, float spreadFactorVal) = 0;
		virtual void OnRemoteReverbSendGainChange(unsigned int channel, float reverbSendGainVal) = 0;
	};
public:
	SurroundFieldMixerRemoteWrapper();
	~SurroundFieldMixerRemoteWrapper();

	//==========================================================================
	void AddListener(SurroundFieldMixerRemoteWrapper::Listener* listener);
	
	//==========================================================================
	void setMute(unsigned int channel, bool muteState);
	void setPosition(unsigned int channel, juce::Point<float> position);
	void setSpreadFactor(unsigned int channel, float spreadFactor);
	void setReverbSendGain(unsigned int channel, float reverbSendGain);

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

	std::vector<SurroundFieldMixerRemoteWrapper::Listener*>	m_listeners;		/**< The listner objects, for message data handling callback. */

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SurroundFieldMixerRemoteWrapper)
};

}
