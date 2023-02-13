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

#include "SurroundFieldMixerRemoteWrapper.h"


namespace SurroundFieldMixer
{

/**
 *
 */
SurroundFieldMixerRemoteWrapper::SurroundFieldMixerRemoteWrapper()
	:   m_bridgingXml("SurroundFieldMixerRemoteWrapper")
{
	SetupBridgingNode();
    
    m_processingNode.AddListener(this);

	startTimer(1500);
}

/**
 *
 */
SurroundFieldMixerRemoteWrapper::~SurroundFieldMixerRemoteWrapper()
{
	stopTimer();
}

void SurroundFieldMixerRemoteWrapper::timerCallback()
{
	if (m_processingNode.IsRunning())
		return;

	auto nodeXmlElement = m_bridgingXml.getChildByName(ProcessingEngineConfig::getTagName(ProcessingEngineConfig::TagID::NODE));
	if (nodeXmlElement)
		m_processingNode.setStateXml(nodeXmlElement);

	auto oh = m_processingNode.GetObjectDataHandling();
	if (oh)
		oh->AddStateListener(this);

	m_processingNode.Start();
}

void SurroundFieldMixerRemoteWrapper::setInputMute(unsigned int channel, bool muteState)
{
	m_inputMutes[channel] = muteState;
}

void SurroundFieldMixerRemoteWrapper::setInputLevel(unsigned int channel, float levelValue)
{
	m_inputLevels[channel] = levelValue;
}

void SurroundFieldMixerRemoteWrapper::setPosition(unsigned int channel, juce::Point<float> position)
{
	m_inputPositions[channel] = position;
}

void SurroundFieldMixerRemoteWrapper::setSpreadFactor(unsigned int channel, float spreadFactor)
{
	m_inputSpreadFactors[channel] = spreadFactor;
}

void SurroundFieldMixerRemoteWrapper::setReverbSendGain(unsigned int channel, float reverbSendGain)
{
	m_inputReverbSendGains[channel] = reverbSendGain;
}

void SurroundFieldMixerRemoteWrapper::setOutputMute(unsigned int channel, bool muteState)
{
	m_outputMutes[channel] = muteState;
}

void SurroundFieldMixerRemoteWrapper::setOutputLevel(unsigned int channel, float levelValue)
{
	m_outputLevels[channel] = levelValue;
}

void SurroundFieldMixerRemoteWrapper::setOutputScheme(unsigned int /*outputScheme*/)
{
	/*t.b.d*/
}

bool SurroundFieldMixerRemoteWrapper::getInputMute(unsigned int channel)
{
	return m_inputMutes[channel];
}

float SurroundFieldMixerRemoteWrapper::getInputLevel(unsigned int channel)
{
	return m_inputLevels[channel];
}

juce::Point<float> SurroundFieldMixerRemoteWrapper::getPosition(unsigned int channel)
{
	return m_inputPositions[channel];
}

float SurroundFieldMixerRemoteWrapper::getSpreadFactor(unsigned int channel)
{
	return m_inputSpreadFactors[channel];
}

float SurroundFieldMixerRemoteWrapper::getReverbSendGain(unsigned int channel)
{
	return m_inputReverbSendGains[channel];
}

bool SurroundFieldMixerRemoteWrapper::getOutputMute(unsigned int channel)
{
	return m_outputMutes[channel];
}

float SurroundFieldMixerRemoteWrapper::getOutputLevel(unsigned int channel)
{
	return m_outputLevels[channel];
}

unsigned int SurroundFieldMixerRemoteWrapper::getOutputScheme()
{
	return m_outputScheme;
}

void SurroundFieldMixerRemoteWrapper::sendInputMute(unsigned int channel)
{
	int muteValue = m_inputMutes[channel] ? 1 : 0;

	RemoteObjectMessageData msgData;
	msgData._addrVal._first = channel;
	msgData._addrVal._second = 0;
	msgData._valCount = 1;
	msgData._valType = ROVT_INT;
	msgData._payloadSize = sizeof(int);
	msgData._payloadOwned = false;
	msgData._payload = &muteValue;

	SendMessage(ROI_MatrixInput_Mute, msgData);
}

void SurroundFieldMixerRemoteWrapper::sendInputLevel(unsigned int channel)
{
	auto range = ProcessingEngineConfig::GetRemoteObjectRange(ROI_MatrixInput_LevelMeterPreMute);
	auto inputLevel = ProtocolProcessorBase::MapNormalizedValueToRange(m_inputLevels[channel], range);

	RemoteObjectMessageData msgData;
	msgData._addrVal._first = channel;
	msgData._addrVal._second = 0;
	msgData._valCount = 1;
	msgData._valType = ROVT_FLOAT;
	msgData._payloadSize = sizeof(float);
	msgData._payloadOwned = false;
	msgData._payload = &inputLevel;

	SendMessage(ROI_MatrixInput_LevelMeterPreMute, msgData);
}

void SurroundFieldMixerRemoteWrapper::sendPosition(unsigned int channel)
{
	float positionValues[2];
	positionValues[0] = m_inputPositions[channel].getX();
	positionValues[1] = m_inputPositions[channel].getY();

	RemoteObjectMessageData msgData;
	msgData._addrVal._first = channel;
	msgData._addrVal._second = 1; // mapping 1 while developing
	msgData._valCount = 2;
	msgData._valType = ROVT_FLOAT;
	msgData._payloadSize = 2 * sizeof(float);
	msgData._payloadOwned = false;
	msgData._payload = &positionValues;

	SendMessage(ROI_CoordinateMapping_SourcePosition_XY, msgData);
}

void SurroundFieldMixerRemoteWrapper::sendSpreadFactor(unsigned int channel)
{
	auto range = ProcessingEngineConfig::GetRemoteObjectRange(ROI_Positioning_SourceSpread);
	auto inputSpreadFactor = ProtocolProcessorBase::MapNormalizedValueToRange(m_inputSpreadFactors[channel], range);

	RemoteObjectMessageData msgData;
	msgData._addrVal._first = channel;
	msgData._addrVal._second = 0;
	msgData._valCount = 1;
	msgData._valType = ROVT_FLOAT;
	msgData._payloadSize = sizeof(float);
	msgData._payloadOwned = false;
	msgData._payload = &inputSpreadFactor;

	SendMessage(ROI_Positioning_SourceSpread, msgData);
}

void SurroundFieldMixerRemoteWrapper::sendReverbSendGain(unsigned int channel)
{
	auto range = ProcessingEngineConfig::GetRemoteObjectRange(ROI_MatrixInput_ReverbSendGain);
	auto inputReverbSendGain = ProtocolProcessorBase::MapNormalizedValueToRange(m_inputReverbSendGains[channel], range);

	RemoteObjectMessageData msgData;
	msgData._addrVal._first = channel;
	msgData._addrVal._second = 0;
	msgData._valCount = 1;
	msgData._valType = ROVT_FLOAT;
	msgData._payloadSize = sizeof(float);
	msgData._payloadOwned = false;
	msgData._payload = &inputReverbSendGain;

	SendMessage(ROI_MatrixInput_ReverbSendGain, msgData);
}

void SurroundFieldMixerRemoteWrapper::sendOutputMute(unsigned int channel)
{
	int muteValue = m_outputMutes[channel] ? 1 : 0;

	RemoteObjectMessageData msgData;
	msgData._addrVal._first = channel;
	msgData._addrVal._second = 0;
	msgData._valCount = 1;
	msgData._valType = ROVT_INT;
	msgData._payloadSize = sizeof(int);
	msgData._payloadOwned = false;
	msgData._payload = &muteValue;

	SendMessage(ROI_MatrixOutput_Mute, msgData);
}

void SurroundFieldMixerRemoteWrapper::sendOutputLevel(unsigned int channel)
{
	auto range = ProcessingEngineConfig::GetRemoteObjectRange(ROI_MatrixOutput_LevelMeterPostMute);
	auto outputLevel = ProtocolProcessorBase::MapNormalizedValueToRange(m_outputLevels[channel], range);

	RemoteObjectMessageData msgData;
	msgData._addrVal._first = channel;
	msgData._addrVal._second = 0;
	msgData._valCount = 1;
	msgData._valType = ROVT_FLOAT;
	msgData._payloadSize = sizeof(float);
	msgData._payloadOwned = false;
	msgData._payload = &outputLevel;

	SendMessage(ROI_MatrixOutput_LevelMeterPostMute, msgData);
}

void SurroundFieldMixerRemoteWrapper::sendOutputScheme(unsigned int /*outputScheme*/)
{
	/*t.b.d*/
}

/**
 * Send a Message out via the active bridging node.
 * @param Id	The id of the remote object to be sent.
 * @param msgData	The message data to be sent.
 * @return True on success, false on failure
 */
bool SurroundFieldMixerRemoteWrapper::SendMessage(RemoteObjectIdentifier Id, RemoteObjectMessageData& msgData)
{
	return m_processingNode.SendMessageTo(DS100_1_PROCESSINGPROTOCOL_ID, Id, msgData);
}

/**
 * Called when the OSCReceiver receives a new OSC message, since Controller inherits from OSCReceiver::Listener.
 * It forwards the message to all registered Processor objects.
 * @param callbackMessage	The node data to handle encapsulated in a calbackmsg struct..
 */
void SurroundFieldMixerRemoteWrapper::HandleNodeData(const ProcessingEngineNode::NodeCallbackMessage* callbackMessage)
{
    if (!callbackMessage)
        return;
    
	auto& remoteObjectId = callbackMessage->_protocolMessage._Id;
	auto& messageDataValType = callbackMessage->_protocolMessage._msgData._valType;
	auto& messageDataValCount = callbackMessage->_protocolMessage._msgData._valCount;
	auto& messageDataPayloadSize = callbackMessage->_protocolMessage._msgData._payloadSize;
	auto& messageDataPayload = callbackMessage->_protocolMessage._msgData._payload;
	auto& channel = callbackMessage->_protocolMessage._msgData._addrVal._first;
	//auto& record = callbackMessage->_protocolMessage._msgData._addrVal._second;
	auto valuePoll = 0 == messageDataValCount && 0 == messageDataPayloadSize;
	auto emptyRemoteObjectData = RemoteObjectMessageData();

	switch (remoteObjectId)
	{
	case RemoteObjectIdentifier::ROI_HeartbeatPing:
		SendMessage(RemoteObjectIdentifier::ROI_HeartbeatPong, emptyRemoteObjectData);
		break;
	case RemoteObjectIdentifier::ROI_HeartbeatPong:
		SendMessage(RemoteObjectIdentifier::ROI_HeartbeatPing, emptyRemoteObjectData);
		break;
	case RemoteObjectIdentifier::ROI_MatrixInput_Mute:
		{
			if (valuePoll)
			{
				inputMutePoll(channel);
				sendInputMute(channel);
			}
			else
			{
				auto valTypeMatch = messageDataValType == RemoteObjectValueType::ROVT_INT;
				auto valCountMatch = 1 == messageDataValCount;
				auto muteValPtr = reinterpret_cast<const int*>(messageDataPayload);
				if (valTypeMatch && valCountMatch && muteValPtr)
				{
					inputMuteChange(channel, *muteValPtr);
					setInputMute(channel, *muteValPtr);
				}
			}
		}
		break;
	case RemoteObjectIdentifier::ROI_MatrixInput_LevelMeterPreMute:
		{
			if (valuePoll)
			{
				inputLevelPoll(channel);
				sendInputLevel(channel);
			}
			else
			{
				ignoreUnused(channel);
			}
		}
		break;
	case RemoteObjectIdentifier::ROI_CoordinateMapping_SourcePosition_X:
		{
			if (valuePoll)
			{
				positionPoll(channel);
				sendPosition(channel);
			}
			else
			{
				auto valTypeMatch = messageDataValType == RemoteObjectValueType::ROVT_FLOAT;
				auto valCountMatch = 1 == messageDataValCount;
				auto xPosValPtr = reinterpret_cast<const float*>(messageDataPayload);
				if (valTypeMatch && valCountMatch && xPosValPtr)
				{
					auto pos = getPosition(channel);
					pos.setX(*xPosValPtr);
					positionChange(channel, pos);
					setPosition(channel, pos);
					jassertfalse;
				}
			}
		}
		break;
	case RemoteObjectIdentifier::ROI_CoordinateMapping_SourcePosition_Y:
		{
			if (valuePoll)
			{
				positionPoll(channel);
				sendPosition(channel);
			}
			else
			{
				auto valTypeMatch = messageDataValType == RemoteObjectValueType::ROVT_FLOAT;
				auto valCountMatch = 1 == messageDataValCount;
				auto yPosValPtr = reinterpret_cast<const float*>(messageDataPayload);
				if (valTypeMatch && valCountMatch && yPosValPtr)
				{
					auto pos = getPosition(channel);
					pos.setY(*yPosValPtr);
					positionChange(channel, pos);
					setPosition(channel, pos);
					jassertfalse;
				}
			}
		}
		break;
	case RemoteObjectIdentifier::ROI_CoordinateMapping_SourcePosition_XY:
		{
			if (valuePoll)
			{
				positionPoll(channel);
				sendPosition(channel);
			}
			else
			{
				auto valTypeMatch = messageDataValType == RemoteObjectValueType::ROVT_FLOAT;
				auto valCountMatch = 2 == messageDataValCount;
				auto xyPosValPtr = reinterpret_cast<const float*>(messageDataPayload);
				if (valTypeMatch && valCountMatch && xyPosValPtr)
				{
					auto xyPos = juce::Point<float>(xyPosValPtr[0], xyPosValPtr[1]);
					positionChange(channel, xyPos);
					setPosition(channel, xyPos);
				}
			}
		}
		break;
	case RemoteObjectIdentifier::ROI_Positioning_SourceSpread:
		{
			if (valuePoll)
			{
				spreadFactorPoll(channel);
				sendSpreadFactor(channel);
			}
			else
			{
				auto valTypeMatch = messageDataValType == RemoteObjectValueType::ROVT_FLOAT;
				auto valCountMatch = 1 == messageDataValCount;
				auto spreadFactorValPtr = reinterpret_cast<const float*>(messageDataPayload);
				if (valTypeMatch && valCountMatch && spreadFactorValPtr)
				{
					auto range = ProcessingEngineConfig::GetRemoteObjectRange(ROI_Positioning_SourceSpread);
					auto inputSpreadFactor = ProtocolProcessorBase::NormalizeValueByRange(*spreadFactorValPtr, range);

					spreadFactorChange(channel, inputSpreadFactor);
					setSpreadFactor(channel, inputSpreadFactor);
				}
			}
		}
		break;
	case RemoteObjectIdentifier::ROI_MatrixInput_ReverbSendGain:
		{
			if (valuePoll)
			{
				reverbSendGainPoll(channel);
				sendReverbSendGain(channel);
			}
			else
			{
				auto valTypeMatch = messageDataValType == RemoteObjectValueType::ROVT_FLOAT;
				auto valCountMatch = 1 == messageDataValCount;
				auto reverbSendGainValPtr = reinterpret_cast<const float*>(messageDataPayload);
				if (valTypeMatch && valCountMatch && reverbSendGainValPtr)
				{
					auto range = ProcessingEngineConfig::GetRemoteObjectRange(ROI_MatrixInput_ReverbSendGain);
					auto inputReverbSendGain = ProtocolProcessorBase::NormalizeValueByRange(*reverbSendGainValPtr, range);

					reverbSendGainChange(channel, inputReverbSendGain);
					setReverbSendGain(channel, inputReverbSendGain);
				}
			}
		}
		break;
	case RemoteObjectIdentifier::ROI_MatrixOutput_Mute:
		{
			if (valuePoll)
			{
				outputMutePoll(channel);
				sendOutputMute(channel);
			}
			else
			{
				auto valTypeMatch = messageDataValType == RemoteObjectValueType::ROVT_INT;
				auto valCountMatch = 1 == messageDataValCount;
				auto muteValPtr = reinterpret_cast<const int*>(messageDataPayload);
				if (valTypeMatch && valCountMatch && muteValPtr)
				{
					outputMuteChange(channel, *muteValPtr);
					setOutputMute(channel, *muteValPtr);
				}
			}
		}
		break;
	case RemoteObjectIdentifier::ROI_MatrixOutput_LevelMeterPostMute:
		{
			if (valuePoll)
			{
				outputLevelPoll(channel);
				sendOutputLevel(channel);
			}
			else
			{
				ignoreUnused(channel);
			}
		}
		break;
	default:
		break;
	}
}

/**
 * Reimplentation of ObjectDataHandling_Abstract::StateListener method to be able
 * to react on osc online state changes
 */
void SurroundFieldMixerRemoteWrapper::protocolStateChanged(ProtocolId id, ObjectHandlingState state)
{
	if (onlineStateChangeCallback && DS100_1_PROCESSINGPROTOCOL_ID == id)
	{
		switch (state)
		{
		case OHS_Protocol_Up:
			onlineStateChangeCallback(true);
			break;
		case OHS_Protocol_Down:
			onlineStateChangeCallback(false);
			break;
		default:
			break;
		}
	}
}

/**
 * Disconnect the active bridging nodes' protocols.
 */
void SurroundFieldMixerRemoteWrapper::Disconnect()
{
	m_processingNode.Stop();
}

/**
 * Disconnect and re-connect the OSCSender to a host specified by the current ip settings.
 */
void SurroundFieldMixerRemoteWrapper::Reconnect()
{
	Disconnect();

	auto nodeXmlElement = m_bridgingXml.getChildByName(ProcessingEngineConfig::getTagName(ProcessingEngineConfig::TagID::NODE));
	if (nodeXmlElement)
		m_processingNode.setStateXml(nodeXmlElement);

	m_processingNode.Start();
}

/**
 * Method to create a basic configuration to use to setup the single supported
 * bridging node.
 * @param bridgingProtocolsToActivate	Bitmask definition of what bridging protocols to activate in the bridging node about to be created.
 */
void SurroundFieldMixerRemoteWrapper::SetupBridgingNode()
{
	auto nodeXmlElement = std::make_unique<XmlElement>(ProcessingEngineConfig::getTagName(ProcessingEngineConfig::TagID::NODE));

	nodeXmlElement->setAttribute(ProcessingEngineConfig::getAttributeName(ProcessingEngineConfig::AttributeID::ID), DEFAULT_PROCNODE_ID);

	auto objectHandlingXmlElement = nodeXmlElement->createNewChildElement(ProcessingEngineConfig::getTagName(ProcessingEngineConfig::TagID::OBJECTHANDLING));
	if (objectHandlingXmlElement)
		objectHandlingXmlElement->setAttribute(ProcessingEngineConfig::getAttributeName(ProcessingEngineConfig::AttributeID::MODE), ProcessingEngineConfig::ObjectHandlingModeToString(OHM_Bypass));

	// DS100 protocol - RoleA
	auto protocolAXmlElement = nodeXmlElement->createNewChildElement(ProcessingEngineConfig::getTagName(ProcessingEngineConfig::TagID::PROTOCOLA));
	if (protocolAXmlElement)
	{
		protocolAXmlElement->setAttribute(ProcessingEngineConfig::getAttributeName(ProcessingEngineConfig::AttributeID::ID), DS100_1_PROCESSINGPROTOCOL_ID);

		protocolAXmlElement->setAttribute(ProcessingEngineConfig::getAttributeName(ProcessingEngineConfig::AttributeID::TYPE), ProcessingEngineConfig::ProtocolTypeToString(PT_OSCProtocol));
		protocolAXmlElement->setAttribute(ProcessingEngineConfig::getAttributeName(ProcessingEngineConfig::AttributeID::USESACTIVEOBJ), 0);
        
        auto ipAdressXmlElement = protocolAXmlElement->createNewChildElement(ProcessingEngineConfig::getTagName(ProcessingEngineConfig::TagID::IPADDRESS));
        if (ipAdressXmlElement)
            ipAdressXmlElement->setAttribute(ProcessingEngineConfig::getAttributeName(ProcessingEngineConfig::AttributeID::ADRESS), "");

		auto clientPortXmlElement = protocolAXmlElement->createNewChildElement(ProcessingEngineConfig::getTagName(ProcessingEngineConfig::TagID::CLIENTPORT));
		if (clientPortXmlElement)
			clientPortXmlElement->setAttribute(ProcessingEngineConfig::getAttributeName(ProcessingEngineConfig::AttributeID::PORT), RX_PORT_DS100_HOST);

		auto hostPortXmlElement = protocolAXmlElement->createNewChildElement(ProcessingEngineConfig::getTagName(ProcessingEngineConfig::TagID::HOSTPORT));
		if (hostPortXmlElement)
			hostPortXmlElement->setAttribute(ProcessingEngineConfig::getAttributeName(ProcessingEngineConfig::AttributeID::PORT), RX_PORT_DS100_DEVICE);
	}

	m_processingNode.setStateXml(nodeXmlElement.get());

	m_bridgingXml.addChildElement(nodeXmlElement.release());
}


}
