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

#include "TwoDFieldAudioVisualizer.h"

namespace SurroundFieldMixer
{

#define PAINTINGHELPER

//==============================================================================
TwoDFieldAudioVisualizer::TwoDFieldAudioVisualizer()
    : AbstractAudioVisualizer()
{
    showConfigButton(true);
    setConfigFeatures(AudioVisualizerConfigBase::ConfigFeatures::ChannelMapping | AudioVisualizerConfigBase::ConfigFeatures::UseValuesInDBToogle);
    setChannelMapping({
        {AudioVisualizerConfigBase::MappingKey::L, m_channelL},
        {AudioVisualizerConfigBase::MappingKey::C, m_channelC},
        {AudioVisualizerConfigBase::MappingKey::R, m_channelR},
        {AudioVisualizerConfigBase::MappingKey::LS, m_channelLS},
        {AudioVisualizerConfigBase::MappingKey::RS, m_channelRS},
        });
    setUsesValuesInDB(true);
}

TwoDFieldAudioVisualizer::~TwoDFieldAudioVisualizer()
{
}

void TwoDFieldAudioVisualizer::paint (Graphics& g)
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
    auto textRectSizeInv = juce::Point<float>(20.0f, -20.0f);
    auto textLRect = juce::Rectangle<float>(m_leftMaxPoint, m_leftMaxPoint - textRectSize);
    g.drawText("L", textLRect, Justification::centred, true);
    auto textCRect = juce::Rectangle<float>(m_centerMaxPoint, m_centerMaxPoint - textRectSize) + juce::Point<float>(0.5f * textRectSize.getX(), 0.0f);
    g.drawText("C", textCRect, Justification::centred, true);
    auto textRRect = juce::Rectangle<float>(m_rightMaxPoint, m_rightMaxPoint + textRectSizeInv);
    g.drawText("R", textRRect, Justification::centred, true);
    auto textLSRect = juce::Rectangle<float>(m_leftSurroundMaxPoint, m_leftSurroundMaxPoint - textRectSizeInv);
    g.drawText("LS", textLSRect, Justification::centred, true);
    auto textRSRect = juce::Rectangle<float>(m_rightSurroundMaxPoint, m_rightSurroundMaxPoint + textRectSize);
    g.drawText("RS", textRSRect, Justification::centred, true);
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

}

void TwoDFieldAudioVisualizer::resized()
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

AbstractAudioVisualizer::VisuType TwoDFieldAudioVisualizer::getType()
{
    return AbstractAudioVisualizer::VisuType::TwoDField;
}

void TwoDFieldAudioVisualizer::processChangedChannelMapping()
{
    if(getChannelMapping().count(AudioVisualizerConfigBase::MappingKey::L))
        m_channelL = getChannelMapping().at(AudioVisualizerConfigBase::MappingKey::L);
    if(getChannelMapping().count(AudioVisualizerConfigBase::MappingKey::C))
        m_channelC = getChannelMapping().at(AudioVisualizerConfigBase::MappingKey::C);
    if(getChannelMapping().count(AudioVisualizerConfigBase::MappingKey::R))
        m_channelR = getChannelMapping().at(AudioVisualizerConfigBase::MappingKey::R);
    if(getChannelMapping().count(AudioVisualizerConfigBase::MappingKey::LS))
        m_channelLS = getChannelMapping().at(AudioVisualizerConfigBase::MappingKey::LS);
    if(getChannelMapping().count(AudioVisualizerConfigBase::MappingKey::RS))
        m_channelRS = getChannelMapping().at(AudioVisualizerConfigBase::MappingKey::RS);
}

void TwoDFieldAudioVisualizer::processingDataChanged(AbstractProcessorData *data)
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
