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

#include "MultiMeterAudioVisualizer.h"

namespace SurroundFieldMixer
{

//==============================================================================
MultiMeterAudioVisualizer::MultiMeterAudioVisualizer()
    : AbstractAudioVisualizer()
{
    showConfigButton(true);
    setConfigFeatures(AudioVisualizerConfigBase::ConfigFeatures::UseValuesInDBToogle);
    setUsesValuesInDB(true);
}

MultiMeterAudioVisualizer::~MultiMeterAudioVisualizer()
{
}

void MultiMeterAudioVisualizer::paint(Graphics& g)
{
    AbstractAudioVisualizer::paint(g);

	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

	// calculate what we need for our center circle
	auto width = getWidth();
	auto height = getHeight();
	auto outerMargin = 20;
    auto maxMeterWidth = 30;
	auto visuAreaWidth = width - 2 * outerMargin;
	auto visuAreaHeight = height - 2 * outerMargin;

	Rectangle<int> visuArea(outerMargin, outerMargin, visuAreaWidth, visuAreaHeight);

	// fill our visualization area background
	g.setColour(getLookAndFeel().findColour(ResizableWindow::backgroundColourId).darker());
	g.fillRect(visuArea);

	auto visuAreaOrigX = float(outerMargin);
	auto visuAreaOrigY = float(outerMargin + visuAreaHeight);

	// draw a simple baseline
    g.setColour(Colours::grey);
	g.drawLine(Line<float>(visuAreaOrigX, visuAreaOrigY, visuAreaOrigX + visuAreaWidth, visuAreaOrigY));
    // draw dBFS
    g.setFont(12.0f);
    g.setColour(Colours::grey);
    String rangeText;
    if (getUsesValuesInDB())
        rangeText = String(SurroundFieldMixerProcessor::getGlobalMindB()) + " ... " + String(SurroundFieldMixerProcessor::getGlobalMaxdB()) + " dBFS";
    else
        rangeText = "0 ... 1";
    g.drawText(rangeText, Rectangle<float>(visuAreaOrigX + visuAreaWidth - 100.0f, float(outerMargin), 110.0f, float(outerMargin)), Justification::centred, true);

	// draw meters
    auto meterSpacing = outerMargin * 0.5f;
    auto meterWidth = (visuArea.getWidth() - (m_levelData.GetChannelCount() + 1) * meterSpacing) / m_levelData.GetChannelCount();
    meterWidth = meterWidth > maxMeterWidth ? maxMeterWidth : meterWidth;
    auto meterMaxHeight = visuArea.getHeight() - 2 * meterSpacing;
    auto meterLeft = visuAreaOrigX + meterSpacing;

    g.setFont(14.0f);
    for(unsigned long i=1; i<=m_levelData.GetChannelCount(); ++i)
    {
        auto level = m_levelData.GetLevel(i);
        float peakMeterHeight {0};
        float rmsMeterHeight  {0};
        float holdMeterHeight {0};
        if (getUsesValuesInDB())
        {
            peakMeterHeight = meterMaxHeight * level.GetFactorPEAKdB();
            rmsMeterHeight = meterMaxHeight * level.GetFactorRMSdB();
            holdMeterHeight = meterMaxHeight * level.GetFactorHOLDdB();
        }
        else
        {
            peakMeterHeight = meterMaxHeight * level.peak;
            rmsMeterHeight = meterMaxHeight * level.rms;
            holdMeterHeight = meterMaxHeight * level.hold;
        }

        // peak bar
        g.setColour(Colours::forestgreen.darker());
        g.fillRect(Rectangle<float>(meterLeft, visuAreaOrigY - peakMeterHeight, meterWidth, peakMeterHeight));
        // rms bar
        g.setColour(Colours::forestgreen);
        g.fillRect(Rectangle<float>(meterLeft, visuAreaOrigY - rmsMeterHeight, meterWidth, rmsMeterHeight));
        // hold strip
        g.setColour(Colours::grey);
        g.drawLine(Line<float>(meterLeft, visuAreaOrigY - holdMeterHeight, meterLeft + meterWidth, visuAreaOrigY - holdMeterHeight));
        // channel # label
		g.setColour(Colours::white);
        g.drawText(String(i), Rectangle<float>(meterLeft, visuAreaOrigY, meterWidth, float(outerMargin)), Justification::centred, true);

        meterLeft += meterWidth + meterSpacing;
    }
}

void MultiMeterAudioVisualizer::resized()
{
    AbstractAudioVisualizer::resized();
}

AbstractAudioVisualizer::VisuType MultiMeterAudioVisualizer::getType()
{
    return AbstractAudioVisualizer::VisuType::MultiMeter;
}

void MultiMeterAudioVisualizer::processChangedChannelMapping()
{

}

void MultiMeterAudioVisualizer::processingDataChanged(AbstractProcessorData *data)
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
