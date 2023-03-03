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

#include "TwoDFieldOutputComponent.h"

#include <ProcessingEngine/ProcessingEngineNode.h>
#include <ProcessingEngine/ProcessingEngineConfig.h>

namespace SurroundFieldMixer
{

//#define PAINTINGHELPER

//==============================================================================
TwoDFieldOutputComponent::TwoDFieldOutputComponent()
    :   AbstractAudioVisualizer(),
        SurroundFieldMixerProcessor::InputCommander(),
        SurroundFieldMixerProcessor::OutputCommander()
{
    setUsesValuesInDB(true);

    m_outputMuteState[m_channelL] = false;
    m_outputMuteState[m_channelC] = false;
    m_outputMuteState[m_channelR] = false;
    m_outputMuteState[m_channelLS] = false;
    m_outputMuteState[m_channelRS] = false;

    auto range = ProcessingEngineConfig::GetRemoteObjectRange(ROI_MatrixOutput_Gain);
    auto defaultGainStrVal = juce::String(ProtocolProcessorBase::MapNormalizedValueToRange(1.0f, range)) + " dB";
    m_outputGainValue[m_channelL] = defaultGainStrVal;
    m_outputGainValue[m_channelC] = defaultGainStrVal;
    m_outputGainValue[m_channelR] = defaultGainStrVal;
    m_outputGainValue[m_channelLS] = defaultGainStrVal;
    m_outputGainValue[m_channelRS] = defaultGainStrVal;
}

TwoDFieldOutputComponent::~TwoDFieldOutputComponent()
{
}

void TwoDFieldOutputComponent::paint (Graphics& g)
{
    AbstractAudioVisualizer::paint(g);

    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

    // fill circle background
    g.setColour(getLookAndFeel().findColour(ResizableWindow::backgroundColourId).darker());
    g.fillEllipse(m_visuArea);

#if defined DEBUG && defined PAINTINGHELPER
    g.setColour(juce::Colours::red);
    g.drawRect(m_visuArea);
    g.setColour(juce::Colours::blue);
    g.drawRect(getLocalBounds());
#endif

    // draw level indication lines
    juce::Point<float> leftMax = m_levelOrig - m_leftMaxPoint;
    juce::Point<float> centerMax = m_levelOrig - m_centerMaxPoint;
    juce::Point<float> rightMax = m_levelOrig - m_rightMaxPoint;
    juce::Point<float> rightSurroundMax = m_levelOrig - m_rightSurroundMaxPoint;
    juce::Point<float> leftSurroundMax = m_levelOrig - m_leftSurroundMaxPoint;

    // hold values
    float holdLevelL{ 0 };
    float holdLevelC{ 0 };
    float holdLevelR{ 0 };
    float holdLevelLS{ 0 };
    float holdLevelRS{ 0 };
    if (getUsesValuesInDB())
    {
        holdLevelL = m_levelData.GetLevel(m_channelL).GetFactorHOLDdB();
        holdLevelC = m_levelData.GetLevel(m_channelC).GetFactorHOLDdB();
        holdLevelR = m_levelData.GetLevel(m_channelR).GetFactorHOLDdB();
        holdLevelLS = m_levelData.GetLevel(m_channelLS).GetFactorHOLDdB();
        holdLevelRS = m_levelData.GetLevel(m_channelRS).GetFactorHOLDdB();
    }
    else
    {
        holdLevelL = m_levelData.GetLevel(m_channelL).hold;
        holdLevelC = m_levelData.GetLevel(m_channelC).hold;
        holdLevelR = m_levelData.GetLevel(m_channelR).hold;
        holdLevelLS = m_levelData.GetLevel(m_channelLS).hold;
        holdLevelRS = m_levelData.GetLevel(m_channelRS).hold;
    }

    g.setColour(Colours::grey);
    Path holdPath;
    holdPath.startNewSubPath(m_levelOrig - leftMax * holdLevelL);
    holdPath.lineTo(m_levelOrig - centerMax * holdLevelC);
    holdPath.lineTo(m_levelOrig - rightMax * holdLevelR);
    holdPath.lineTo(m_levelOrig - rightSurroundMax * holdLevelRS);
    holdPath.lineTo(m_levelOrig - leftSurroundMax * holdLevelLS);
    holdPath.lineTo(m_levelOrig - leftMax * holdLevelL);
    g.strokePath(holdPath, PathStrokeType(1));
#if defined DEBUG && defined PAINTINGHELPER
    g.setColour(juce::Colours::yellow);
    g.drawRect(holdPath.getBounds());
#endif

    // peak values
    float peakLevelL{ 0 };
    float peakLevelC{ 0 };
    float peakLevelR{ 0 };
    float peakLevelLS{ 0 };
    float peakLevelRS{ 0 };
    if (getUsesValuesInDB())
    {
        peakLevelL = m_levelData.GetLevel(m_channelL).GetFactorPEAKdB();
        peakLevelC = m_levelData.GetLevel(m_channelC).GetFactorPEAKdB();
        peakLevelR = m_levelData.GetLevel(m_channelR).GetFactorPEAKdB();
        peakLevelLS = m_levelData.GetLevel(m_channelLS).GetFactorPEAKdB();
        peakLevelRS = m_levelData.GetLevel(m_channelRS).GetFactorPEAKdB();
    }
    else
    {
        peakLevelL = m_levelData.GetLevel(m_channelL).peak;
        peakLevelC = m_levelData.GetLevel(m_channelC).peak;
        peakLevelR = m_levelData.GetLevel(m_channelR).peak;
        peakLevelLS = m_levelData.GetLevel(m_channelLS).peak;
        peakLevelRS = m_levelData.GetLevel(m_channelRS).peak;
    }

    g.setColour(Colours::forestgreen.darker());
    Path peakPath;
    peakPath.startNewSubPath(m_levelOrig - leftMax * peakLevelL);
    peakPath.lineTo(m_levelOrig - centerMax * peakLevelC);
    peakPath.lineTo(m_levelOrig - rightMax * peakLevelR);
    peakPath.lineTo(m_levelOrig - rightSurroundMax * peakLevelRS);
    peakPath.lineTo(m_levelOrig - leftSurroundMax * peakLevelLS);
    peakPath.lineTo(m_levelOrig - leftMax * peakLevelL);
    g.fillPath(peakPath);
#if defined DEBUG && defined PAINTINGHELPER
    g.setColour(juce::Colours::orange);
    g.drawRect(peakPath.getBounds());
#endif

    // rms values
    float rmsLevelL{ 0 };
    float rmsLevelC{ 0 };
    float rmsLevelR{ 0 };
    float rmsLevelLS{ 0 };
    float rmsLevelRS{ 0 };
    if (getUsesValuesInDB())
    {
        rmsLevelL = m_levelData.GetLevel(m_channelL).GetFactorRMSdB();
        rmsLevelC = m_levelData.GetLevel(m_channelC).GetFactorRMSdB();
        rmsLevelR = m_levelData.GetLevel(m_channelR).GetFactorRMSdB();
        rmsLevelLS = m_levelData.GetLevel(m_channelLS).GetFactorRMSdB();
        rmsLevelRS = m_levelData.GetLevel(m_channelRS).GetFactorRMSdB();
    }
    else
    {
        rmsLevelL = m_levelData.GetLevel(m_channelL).rms;
        rmsLevelC = m_levelData.GetLevel(m_channelC).rms;
        rmsLevelR = m_levelData.GetLevel(m_channelR).rms;
        rmsLevelLS = m_levelData.GetLevel(m_channelLS).rms;
        rmsLevelRS = m_levelData.GetLevel(m_channelRS).rms;
    }

    g.setColour(Colours::forestgreen);
    Path rmsPath;
    rmsPath.startNewSubPath(m_levelOrig - leftMax * rmsLevelL);
    rmsPath.lineTo(m_levelOrig - centerMax * rmsLevelC);
    rmsPath.lineTo(m_levelOrig - rightMax * rmsLevelR);
    rmsPath.lineTo(m_levelOrig - rightSurroundMax * rmsLevelRS);
    rmsPath.lineTo(m_levelOrig - leftSurroundMax * rmsLevelLS);
    rmsPath.lineTo(m_levelOrig - leftMax * rmsLevelL);
    g.fillPath(rmsPath);
#if defined DEBUG && defined PAINTINGHELPER
    g.setColour(juce::Colours::turquoise);
    g.drawRect(rmsPath.getBounds());
#endif

    // draw a simple circle surrounding
    g.setColour(Colours::white);
    g.drawEllipse(m_visuArea, 1);

    // draw dashed field dimension indication lines
    float dparam[]{ 4.0f, 5.0f };
    g.drawDashedLine(juce::Line<float>(m_leftMaxPoint, m_levelOrig), dparam, 2);
    g.drawDashedLine(juce::Line<float>(m_leftSurroundMaxPoint, m_levelOrig), dparam, 2);
    g.drawDashedLine(juce::Line<float>(m_rightMaxPoint, m_levelOrig), dparam, 2);
    g.drawDashedLine(juce::Line<float>(m_rightSurroundMaxPoint, m_levelOrig), dparam, 2);
    g.drawDashedLine(juce::Line<float>(m_centerMaxPoint, m_levelOrig), dparam, 2);

    // draw L C R LS RS legend
    auto textRectSize = juce::Point<float>(20.0f, 20.0f);
    auto gainTextRectWidth = juce::Point<float>(40.0f, 0.0f);
    auto textRectSizeInv = juce::Point<float>(20.0f, -20.0f);

    auto textLRect = m_outputMuteState[m_channelL] 
        ? juce::Rectangle<float>(m_leftMaxPoint, m_leftMaxPoint - textRectSize) 
        : juce::Rectangle<float>(m_leftMaxPoint, m_leftMaxPoint - textRectSize - gainTextRectWidth);
    g.setColour(m_outputMuteState[m_channelL] ? Colours::red : Colours::white);
    auto textL = m_outputMuteState[m_channelL] ? "L" : "L " + m_outputGainValue[m_channelL];
    g.drawText(textL, textLRect, Justification::centred, true);

    auto textCRect = m_outputMuteState[m_channelC] 
        ? juce::Rectangle<float>(m_centerMaxPoint, m_centerMaxPoint - textRectSize) + juce::Point<float>(0.5f * textRectSize.getX(), 0.0f) 
        : juce::Rectangle<float>(m_centerMaxPoint + 0.5f * gainTextRectWidth, m_centerMaxPoint - textRectSize - 0.5f * gainTextRectWidth) + juce::Point<float>(0.5f * textRectSize.getX(), 0.0f);
    g.setColour(m_outputMuteState[m_channelC] ? Colours::red : Colours::white);
    auto textC = m_outputMuteState[m_channelC] ? "C" : "C " + m_outputGainValue[m_channelC];
    g.drawText(textC, textCRect, Justification::centred, true);

    auto textRRect = m_outputMuteState[m_channelR] 
        ? juce::Rectangle<float>(m_rightMaxPoint, m_rightMaxPoint + textRectSizeInv) 
        : juce::Rectangle<float>(m_rightMaxPoint, m_rightMaxPoint + textRectSizeInv + gainTextRectWidth);
    g.setColour(m_outputMuteState[m_channelR] ? Colours::red : Colours::white);
    auto textR = m_outputMuteState[m_channelR] ? "R" : "R " + m_outputGainValue[m_channelR];
    g.drawText(textR, textRRect, Justification::centred, true);

    auto textLSRect = m_outputMuteState[m_channelLS] 
        ? juce::Rectangle<float>(m_leftSurroundMaxPoint, m_leftSurroundMaxPoint - textRectSizeInv) 
        : juce::Rectangle<float>(m_leftSurroundMaxPoint, m_leftSurroundMaxPoint - textRectSizeInv - gainTextRectWidth);
    g.setColour(m_outputMuteState[m_channelLS] ? Colours::red : Colours::white);
    auto textLS = m_outputMuteState[m_channelLS] ? "LS" : "LS " + m_outputGainValue[m_channelLS];
    g.drawText(textLS, textLSRect, Justification::centred, true);

    auto textRSRect = m_outputMuteState[m_channelRS] 
        ? juce::Rectangle<float>(m_rightSurroundMaxPoint, m_rightSurroundMaxPoint + textRectSize) 
        : juce::Rectangle<float>(m_rightSurroundMaxPoint, m_rightSurroundMaxPoint + textRectSize + gainTextRectWidth);
    g.setColour(m_outputMuteState[m_channelRS] ? Colours::red : Colours::white);
    auto textRS = m_outputMuteState[m_channelRS] ? "RS" : "RS " + m_outputGainValue[m_channelRS];
    g.drawText(textRS, textRSRect, Justification::centred, true);

#if defined DEBUG && defined PAINTINGHELPER
    g.setColour(juce::Colours::lightblue);
    g.drawRect(textLRect);
    g.drawRect(textCRect);
    g.drawRect(textRRect);
    g.drawRect(textLSRect);
    g.drawRect(textRSRect);
#endif

    // draw dBFS
    g.setFont(12.0f);
    g.setColour(Colours::grey);
    String rangeText;
    if (getUsesValuesInDB())
        rangeText = String(SurroundFieldMixerProcessor::getGlobalMindB()) + " ... " + String(SurroundFieldMixerProcessor::getGlobalMaxdB()) + " dBFS";
    else
        rangeText = "0 ... 1";
    g.drawText(rangeText, Rectangle<float>(m_visuAreaOrigX + m_visuAreaWidth - 110.0f, m_visuAreaOrigY - m_visuAreaHeight - 5.0f, 110.0f, float(m_outerMargin)), Justification::centred, true);

    // draw inputs position indicators
    g.setFont(12.0f);
    g.setColour(Colours::white);
    for (auto const& position : m_inputPositions)
    {
        auto radius = juce::Point<float>(7.0f, 7.0f);
        auto mappedPos = juce::Point<float>(m_visuAreaOrigX + m_visuAreaWidth * position.second.getX(), m_visuAreaOrigY - m_visuAreaHeight * position.second.getY());
        auto indicationRect = juce::Rectangle<float>(mappedPos + radius, mappedPos - radius);
        g.drawText(String(position.first), indicationRect, juce::Justification::centred);
        g.drawEllipse(indicationRect, 1);
    }

}

void TwoDFieldOutputComponent::resized()
{
    // calculate what we need for our center circle
    auto width = getWidth();
    auto height = getHeight();

    m_visuAreaWidth = static_cast<float>(width < height ? width : height) - 2 * m_outerMargin;
    m_visuAreaHeight = static_cast<float>(width < height ? width : height) - 2 * m_outerMargin;

    m_visuAreaOrigX = float(0.5f * (width - m_visuAreaWidth));
    m_visuAreaOrigY = height - float(0.5f * (height - m_visuAreaHeight));

    m_visuArea = juce::Rectangle<float>(m_visuAreaOrigX, m_visuAreaOrigY - m_visuAreaHeight, m_visuAreaWidth, m_visuAreaHeight);

    auto visuAreaHalfHeight = m_visuAreaHeight * 0.5f;
    auto visuAreaHalfWidth = m_visuAreaWidth * 0.5f;

    m_levelOrig = juce::Point<float>(m_visuAreaOrigX + 0.5f * m_visuAreaWidth, m_visuAreaOrigY - 0.5f * m_visuAreaHeight);
    m_centerMaxPoint = juce::Point<float>(m_visuAreaOrigX + 0.5f * m_visuAreaWidth, m_visuAreaOrigY - m_visuAreaHeight);

    auto leftXLength = cosf(juce::MathConstants<float>::pi / 180.0f * 60.0f) * visuAreaHalfWidth;
    auto leftYLength = sinf(juce::MathConstants<float>::pi / 180.0f * 60.0f) * visuAreaHalfHeight;
    m_leftMaxPoint = m_levelOrig + juce::Point<float>(-leftXLength, -leftYLength);

    auto rightXLength = cosf(juce::MathConstants<float>::pi / 180.0f * 60.0f) * visuAreaHalfWidth;
    auto rightYLength = sinf(juce::MathConstants<float>::pi / 180.0f * 60.0f) * visuAreaHalfHeight;
    m_rightMaxPoint = m_levelOrig + juce::Point<float>(rightXLength, -rightYLength);

    auto leftSurroundXLength = cosf(juce::MathConstants<float>::pi / 180.0f * 20.0f) * visuAreaHalfWidth;
    auto leftSurroundYLength = sinf(juce::MathConstants<float>::pi / 180.0f * 20.0f) * visuAreaHalfHeight;
    m_leftSurroundMaxPoint = m_levelOrig + juce::Point<float>(-leftSurroundXLength, leftSurroundYLength);

    auto rightSurroundXLength = cosf(juce::MathConstants<float>::pi / 180.0f * 20.0f) * visuAreaHalfWidth;
    auto rightSurroundYLength = sinf(juce::MathConstants<float>::pi / 180.0f * 20.0f) * visuAreaHalfHeight;
    m_rightSurroundMaxPoint = m_levelOrig + juce::Point<float>(rightSurroundXLength, rightSurroundYLength);

    AbstractAudioVisualizer::resized();
}

void TwoDFieldOutputComponent::setInputPosition(unsigned int channel, juce::Point<float> positionValue)
{
    m_inputPositions[channel] = positionValue;
    repaint();
}

void TwoDFieldOutputComponent::setOutputMute(unsigned int channel, bool muteState)
{
    m_outputMuteState[channel] = muteState;
    repaint();
}

void TwoDFieldOutputComponent::setOutputGain(unsigned int channel, float gainValue)
{
    auto range = ProcessingEngineConfig::GetRemoteObjectRange(ROI_MatrixOutput_Gain);
    auto inputGain = ProtocolProcessorBase::MapNormalizedValueToRange(gainValue, range);
    m_outputGainValue[channel] = juce::String(inputGain) + " dB";
    repaint();
}

void TwoDFieldOutputComponent::setOutputScheme(unsigned int /*dummyschemetobechanged*/)
{

}

void TwoDFieldOutputComponent::processingDataChanged(AbstractProcessorData *data)
{
    if(!data)
        return;
    
    switch(data->GetDataType())
    {
        case AbstractProcessorData::Level:
            m_levelData = *(static_cast<ProcessorLevelData*>(data));
            notifyChanges();
            break;
        case AbstractProcessorData::AudioSignal:
        case AbstractProcessorData::Spectrum:
        case AbstractProcessorData::Invalid:
        default:
            break;
    }
}

}
