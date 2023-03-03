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

#include "SurroundFieldMixerRemoteWrapper.h"


namespace SurroundFieldMixer
{

SurroundFieldMixerRemoteWrapper::SurroundFieldMixerRemoteWrapper()
	:	m_servus("_osc._udp"), 
		m_bridgingXml("SurroundFieldMixerRemoteWrapper")
{
	SetupBridgingNode();
    
    m_processingNode.AddListener(this);

	m_servus.announce(LISTENING_PORT, JUCEApplication::getInstance()->getApplicationName().toStdString());

	startTimer(1500);
}

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

void SurroundFieldMixerRemoteWrapper::setInputGain(unsigned int channel, float gainValue)
{
	m_inputGains[channel] = gainValue;
}

void SurroundFieldMixerRemoteWrapper::setInputLevel(unsigned int channel, float levelValue)
{
	m_inputLevels[channel] = levelValue;
}

void SurroundFieldMixerRemoteWrapper::setInputPosition(unsigned int channel, juce::Point<float> position)
{
	m_inputPositions[channel] = position;
}

void SurroundFieldMixerRemoteWrapper::setInputSpread(unsigned int channel, float spreadValue)
{
	m_inputSpreads[channel] = spreadValue;
}

void SurroundFieldMixerRemoteWrapper::setInputReverb(unsigned int channel, float reverbValue)
{
	m_inputReverbs[channel] = reverbValue;
}

void SurroundFieldMixerRemoteWrapper::setOutputMute(unsigned int channel, bool muteState)
{
	m_outputMutes[channel] = muteState;
}

void SurroundFieldMixerRemoteWrapper::setOutputGain(unsigned int channel, float gainValue)
{
	m_outputGains[channel] = gainValue;
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

float SurroundFieldMixerRemoteWrapper::getInputGain(unsigned int channel)
{
	return m_inputGains[channel];
}

float SurroundFieldMixerRemoteWrapper::getInputLevel(unsigned int channel)
{
	return m_inputLevels[channel];
}

juce::Point<float> SurroundFieldMixerRemoteWrapper::getPosition(unsigned int channel)
{
	return m_inputPositions[channel];
}

float SurroundFieldMixerRemoteWrapper::getSpread(unsigned int channel)
{
	return m_inputSpreads[channel];
}

float SurroundFieldMixerRemoteWrapper::getReverb(unsigned int channel)
{
	return m_inputReverbs[channel];
}

bool SurroundFieldMixerRemoteWrapper::getOutputMute(unsigned int channel)
{
	return m_outputMutes[channel];
}

float SurroundFieldMixerRemoteWrapper::getOutputGain(unsigned int channel)
{
	return m_outputGains[channel];
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

void SurroundFieldMixerRemoteWrapper::sendInputGain(unsigned int channel)
{
	auto range = ProcessingEngineConfig::GetRemoteObjectRange(ROI_MatrixInput_Gain);
	auto inputGain = ProtocolProcessorBase::MapNormalizedValueToRange(m_inputGains[channel], range);

	RemoteObjectMessageData msgData;
	msgData._addrVal._first = channel;
	msgData._addrVal._second = 0;
	msgData._valCount = 1;
	msgData._valType = ROVT_FLOAT;
	msgData._payloadSize = sizeof(float);
	msgData._payloadOwned = false;
	msgData._payload = &inputGain;

	SendMessage(ROI_MatrixInput_Gain, msgData);
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

void SurroundFieldMixerRemoteWrapper::sendInputPosition(unsigned int channel)
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

void SurroundFieldMixerRemoteWrapper::sendInputSpread(unsigned int channel)
{
	auto range = ProcessingEngineConfig::GetRemoteObjectRange(ROI_Positioning_SourceSpread);
	auto inputSpread = ProtocolProcessorBase::MapNormalizedValueToRange(m_inputSpreads[channel], range);

	RemoteObjectMessageData msgData;
	msgData._addrVal._first = channel;
	msgData._addrVal._second = 0;
	msgData._valCount = 1;
	msgData._valType = ROVT_FLOAT;
	msgData._payloadSize = sizeof(float);
	msgData._payloadOwned = false;
	msgData._payload = &inputSpread;

	SendMessage(ROI_Positioning_SourceSpread, msgData);
}

void SurroundFieldMixerRemoteWrapper::sendInputReverb(unsigned int channel)
{
	auto range = ProcessingEngineConfig::GetRemoteObjectRange(ROI_MatrixInput_ReverbSendGain);
	auto inputReverb = ProtocolProcessorBase::MapNormalizedValueToRange(m_inputReverbs[channel], range);

	RemoteObjectMessageData msgData;
	msgData._addrVal._first = channel;
	msgData._addrVal._second = 0;
	msgData._valCount = 1;
	msgData._valType = ROVT_FLOAT;
	msgData._payloadSize = sizeof(float);
	msgData._payloadOwned = false;
	msgData._payload = &inputReverb;

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

void SurroundFieldMixerRemoteWrapper::sendOutputGain(unsigned int channel)
{
	auto range = ProcessingEngineConfig::GetRemoteObjectRange(ROI_MatrixOutput_Gain);
	auto outputGain = ProtocolProcessorBase::MapNormalizedValueToRange(m_outputGains[channel], range);

	RemoteObjectMessageData msgData;
	msgData._addrVal._first = channel;
	msgData._addrVal._second = 0;
	msgData._valCount = 1;
	msgData._valType = ROVT_FLOAT;
	msgData._payloadSize = sizeof(float);
	msgData._payloadOwned = false;
	msgData._payload = &outputGain;

	SendMessage(ROI_MatrixOutput_Gain, msgData);
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
	case RemoteObjectIdentifier::ROI_MatrixInput_Gain:
		{
			if (valuePoll)
			{
				inputGainPoll(channel);
				sendInputGain(channel);
			}
			else
			{
				auto valTypeMatch = messageDataValType == RemoteObjectValueType::ROVT_FLOAT;
				auto valCountMatch = 1 == messageDataValCount;
				auto gainValPtr = reinterpret_cast<const float*>(messageDataPayload);
				if (valTypeMatch && valCountMatch && gainValPtr)
				{
					auto range = ProcessingEngineConfig::GetRemoteObjectRange(ROI_MatrixInput_Gain);
					auto inputGainValue = ProtocolProcessorBase::NormalizeValueByRange(*gainValPtr, range);

					inputGainChange(channel, inputGainValue);
					setInputGain(channel, inputGainValue);
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
				inputPositionPoll(channel);
				sendInputPosition(channel);
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
					inputPositionChange(channel, pos);
					setInputPosition(channel, pos);
					jassertfalse;
				}
			}
		}
		break;
	case RemoteObjectIdentifier::ROI_CoordinateMapping_SourcePosition_Y:
		{
			if (valuePoll)
			{
				inputPositionPoll(channel);
				sendInputPosition(channel);
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
					inputPositionChange(channel, pos);
					setInputPosition(channel, pos);
					jassertfalse;
				}
			}
		}
		break;
	case RemoteObjectIdentifier::ROI_CoordinateMapping_SourcePosition_XY:
		{
			if (valuePoll)
			{
				inputPositionPoll(channel);
				sendInputPosition(channel);
			}
			else
			{
				auto valTypeMatch = messageDataValType == RemoteObjectValueType::ROVT_FLOAT;
				auto valCountMatch = 2 == messageDataValCount;
				auto xyPosValPtr = reinterpret_cast<const float*>(messageDataPayload);
				if (valTypeMatch && valCountMatch && xyPosValPtr)
				{
					auto xyPos = juce::Point<float>(xyPosValPtr[0], xyPosValPtr[1]);
					inputPositionChange(channel, xyPos);
					setInputPosition(channel, xyPos);
				}
			}
		}
		break;
	case RemoteObjectIdentifier::ROI_Positioning_SourceSpread:
		{
			if (valuePoll)
			{
				inputSpreadPoll(channel);
				sendInputSpread(channel);
			}
			else
			{
				auto valTypeMatch = messageDataValType == RemoteObjectValueType::ROVT_FLOAT;
				auto valCountMatch = 1 == messageDataValCount;
				auto spreadValPtr = reinterpret_cast<const float*>(messageDataPayload);
				if (valTypeMatch && valCountMatch && spreadValPtr)
				{
					auto range = ProcessingEngineConfig::GetRemoteObjectRange(ROI_Positioning_SourceSpread);
					auto inputSpread = ProtocolProcessorBase::NormalizeValueByRange(*spreadValPtr, range);

					inputSpreadChange(channel, inputSpread);
					setInputSpread(channel, inputSpread);
				}
			}
		}
		break;
	case RemoteObjectIdentifier::ROI_MatrixInput_ReverbSendGain:
		{
			if (valuePoll)
			{
				inputReverbPoll(channel);
				sendInputReverb(channel);
			}
			else
			{
				auto valTypeMatch = messageDataValType == RemoteObjectValueType::ROVT_FLOAT;
				auto valCountMatch = 1 == messageDataValCount;
				auto reverbValPtr = reinterpret_cast<const float*>(messageDataPayload);
				if (valTypeMatch && valCountMatch && reverbValPtr)
				{
					auto range = ProcessingEngineConfig::GetRemoteObjectRange(ROI_MatrixInput_ReverbSendGain);
					auto inputReverb = ProtocolProcessorBase::NormalizeValueByRange(*reverbValPtr, range);

					inputReverbChange(channel, inputReverb);
					setInputReverb(channel, inputReverb);
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
	case RemoteObjectIdentifier::ROI_MatrixOutput_Gain:
		{
			if (valuePoll)
			{
				outputGainPoll(channel);
				sendOutputGain(channel);
			}
			else
			{
				auto valTypeMatch = messageDataValType == RemoteObjectValueType::ROVT_FLOAT;
				auto valCountMatch = 1 == messageDataValCount;
				auto gainValPtr = reinterpret_cast<const float*>(messageDataPayload);
				if (valTypeMatch && valCountMatch && gainValPtr)
				{
					auto range = ProcessingEngineConfig::GetRemoteObjectRange(ROI_MatrixOutput_Gain);
					auto outputGainValue = ProtocolProcessorBase::NormalizeValueByRange(*gainValPtr, range);

					outputGainChange(channel, outputGainValue);
					setOutputGain(channel, outputGainValue);
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
			clientPortXmlElement->setAttribute(ProcessingEngineConfig::getAttributeName(ProcessingEngineConfig::AttributeID::PORT), REPLY_TO_PORT);

		auto hostPortXmlElement = protocolAXmlElement->createNewChildElement(ProcessingEngineConfig::getTagName(ProcessingEngineConfig::TagID::HOSTPORT));
		if (hostPortXmlElement)
			hostPortXmlElement->setAttribute(ProcessingEngineConfig::getAttributeName(ProcessingEngineConfig::AttributeID::PORT), LISTENING_PORT);
	}

	m_processingNode.setStateXml(nodeXmlElement.get());

	m_bridgingXml.addChildElement(nodeXmlElement.release());
}


}
