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
	int muteValue = muteState ? 1 : 0;

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

void SurroundFieldMixerRemoteWrapper::setPosition(unsigned int channel, juce::Point<float> position)
{
	float positionValues[2];
	positionValues[0] = position.getX();
	positionValues[1] = position.getY();

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

void SurroundFieldMixerRemoteWrapper::setSpreadFactor(unsigned int channel, float spreadFactor)
{
	RemoteObjectMessageData msgData;
	msgData._addrVal._first = channel;
	msgData._addrVal._second = 0;
	msgData._valCount = 1;
	msgData._valType = ROVT_FLOAT;
	msgData._payloadSize = sizeof(float);
	msgData._payloadOwned = false;
	msgData._payload = &spreadFactor;

	SendMessage(ROI_Positioning_SourceSpread, msgData);
}

void SurroundFieldMixerRemoteWrapper::setReverbSendGain(unsigned int channel, float reverbSendGain)
{
	RemoteObjectMessageData msgData;
	msgData._addrVal._first = channel;
	msgData._addrVal._second = 0;
	msgData._valCount = 1;
	msgData._valType = ROVT_FLOAT;
	msgData._payloadSize = sizeof(float);
	msgData._payloadOwned = false;
	msgData._payload = &reverbSendGain;

	SendMessage(ROI_MatrixInput_ReverbSendGain, msgData);
}

void SurroundFieldMixerRemoteWrapper::setOutputMute(unsigned int channel, bool muteState)
{
	int muteValue = muteState ? 1 : 0;

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

void SurroundFieldMixerRemoteWrapper::setOutputScheme(unsigned int /*outputScheme*/)
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
	auto& messageDataPayload = callbackMessage->_protocolMessage._msgData._payload;
	auto& channel = callbackMessage->_protocolMessage._msgData._addrVal._first;
	//auto& record = callbackMessage->_protocolMessage._msgData._addrVal._second;

	switch (remoteObjectId)
	{
	case RemoteObjectIdentifier::ROI_MatrixInput_Mute:
		{
			auto valTypeMatch = messageDataValType == RemoteObjectValueType::ROVT_INT;
			auto valCountMatch = 1 == messageDataValCount;
			auto muteValPtr = reinterpret_cast<const int*>(messageDataPayload);
			if (valTypeMatch && valCountMatch && muteValPtr)
			{
				inputMuteChange(channel, *muteValPtr);
			}
		}
		break;
	case RemoteObjectIdentifier::ROI_CoordinateMapping_SourcePosition_X:
		{
			auto valTypeMatch = messageDataValType == RemoteObjectValueType::ROVT_FLOAT;
			auto valCountMatch = 1 == messageDataValCount;
			auto xPosValPtr = reinterpret_cast<const float*>(messageDataPayload);
			if (valTypeMatch && valCountMatch && xPosValPtr)
			{
				positionChange(channel, juce::Point<float>(*xPosValPtr, 0.0f));	// TODO
				jassertfalse;
			}
		}
		break;
	case RemoteObjectIdentifier::ROI_CoordinateMapping_SourcePosition_Y:
		{
			auto valTypeMatch = messageDataValType == RemoteObjectValueType::ROVT_FLOAT;
			auto valCountMatch = 1 == messageDataValCount;
			auto yPosValPtr = reinterpret_cast<const float*>(messageDataPayload);
			if (valTypeMatch && valCountMatch && yPosValPtr)
			{
				positionChange(channel, juce::Point<float>(0.0f, *yPosValPtr));	// TODO
				jassertfalse;
			}
		}
		break;
	case RemoteObjectIdentifier::ROI_CoordinateMapping_SourcePosition_XY:
		{
			auto valTypeMatch = messageDataValType == RemoteObjectValueType::ROVT_FLOAT;
			auto valCountMatch = 2 == messageDataValCount;
			auto xyPosValPtr = reinterpret_cast<const float*>(messageDataPayload);
			if (valTypeMatch && valCountMatch && xyPosValPtr)
			{
				auto xyPos = juce::Point<float>(xyPosValPtr[0], xyPosValPtr[1]);
				positionChange(channel, xyPos);
			}
		}
		break;
	case RemoteObjectIdentifier::ROI_Positioning_SourceSpread:
		{
			auto valTypeMatch = messageDataValType == RemoteObjectValueType::ROVT_FLOAT;
			auto valCountMatch = 1 == messageDataValCount;
			auto spreadFactorValPtr = reinterpret_cast<const float*>(messageDataPayload);
			if (valTypeMatch && valCountMatch && spreadFactorValPtr)
			{
				spreadFactorChange(channel, *spreadFactorValPtr);
			}
		}
		break;
	case RemoteObjectIdentifier::ROI_MatrixInput_ReverbSendGain:
		{
			auto valTypeMatch = messageDataValType == RemoteObjectValueType::ROVT_FLOAT;
			auto valCountMatch = 1 == messageDataValCount;
			auto reverbSendGainValPtr = reinterpret_cast<const float*>(messageDataPayload);
			if (valTypeMatch && valCountMatch && reverbSendGainValPtr)
			{
				reverbSendGainChange(channel, *reverbSendGainValPtr);
			}
		}
		break;
	case RemoteObjectIdentifier::ROI_MatrixOutput_Mute:
		{
			auto valTypeMatch = messageDataValType == RemoteObjectValueType::ROVT_INT;
			auto valCountMatch = 1 == messageDataValCount;
			auto muteValPtr = reinterpret_cast<const int*>(messageDataPayload);
			if (valTypeMatch && valCountMatch && muteValPtr)
			{
				outputMuteChange(channel, *muteValPtr);
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
	{
		objectHandlingXmlElement->setAttribute(ProcessingEngineConfig::getAttributeName(ProcessingEngineConfig::AttributeID::MODE), ProcessingEngineConfig::ObjectHandlingModeToString(OHM_DS100_DeviceSimulation));
        
		auto simChCountXmlElement = objectHandlingXmlElement->createNewChildElement(ProcessingEngineConfig::getTagName(ProcessingEngineConfig::TagID::SIMCHCNT));
		if (simChCountXmlElement)
			simChCountXmlElement->setAttribute(ProcessingEngineConfig::getAttributeName(ProcessingEngineConfig::AttributeID::COUNT), 0);

		auto simMappingsCountXmlElement = objectHandlingXmlElement->createNewChildElement(ProcessingEngineConfig::getTagName(ProcessingEngineConfig::TagID::SIMMAPCNT));
		if (simMappingsCountXmlElement)
			simMappingsCountXmlElement->setAttribute(ProcessingEngineConfig::getAttributeName(ProcessingEngineConfig::AttributeID::COUNT), 0);

		auto simRefreshIntervalXmlElement = objectHandlingXmlElement->createNewChildElement(ProcessingEngineConfig::getTagName(ProcessingEngineConfig::TagID::REFRESHINTERVAL));
		if (simRefreshIntervalXmlElement)
			simRefreshIntervalXmlElement->setAttribute(ProcessingEngineConfig::getAttributeName(ProcessingEngineConfig::AttributeID::INTERVAL), 0);
	}

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
