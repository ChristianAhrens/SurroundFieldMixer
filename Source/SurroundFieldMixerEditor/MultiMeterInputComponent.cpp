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

#include "MultiMeterInputComponent.h"

namespace SurroundFieldMixer
{

//==============================================================================
MultiMeterInputComponent::MultiMeterInputComponent()
    : AbstractAudioVisualizer()
{
    setUsesValuesInDB(true);
}

MultiMeterInputComponent::~MultiMeterInputComponent()
{

}

void MultiMeterInputComponent::resized()
{
    if (!m_inputMutes.empty() && m_inputMutes.size() == m_inputGains.size() && m_inputMutes.size() == m_inputPositions.size())
    {
        auto usableBounds = getLocalBounds().reduced(20);

        auto fixedSizeCtrlsBounds = usableBounds.removeFromBottom(55);
        auto resizingCtrlsBounds = usableBounds.removeFromBottom(0.5f * usableBounds.getHeight());

        auto maxcontrolElementWidth = 30;

        auto controlElementWidth = fixedSizeCtrlsBounds.getWidth() / static_cast<int>(m_inputMutes.size());
        controlElementWidth = controlElementWidth > maxcontrolElementWidth ? maxcontrolElementWidth : controlElementWidth;

        auto positionComponentBounds = fixedSizeCtrlsBounds.removeFromTop(30);
        for (auto i = 0; i < m_inputPositions.size(); i++)
        {
            auto const& positionComponent = m_inputPositions.at(i);
            if (!positionComponent)
                continue;
            positionComponentBounds.removeFromLeft(10);
            positionComponent->setBounds(positionComponentBounds.removeFromLeft(controlElementWidth));
        }

        fixedSizeCtrlsBounds.removeFromTop(5);

        auto muteButtonBounds = fixedSizeCtrlsBounds.removeFromTop(20);
        for (auto i = 0; i < m_inputMutes.size(); i++)
        {
            auto const& muteButton = m_inputMutes.at(i);
            if (!muteButton)
                continue;
            muteButtonBounds.removeFromLeft(10);
            muteButton->setBounds(muteButtonBounds.removeFromLeft(controlElementWidth));
        }

        auto gainSliderBounds = resizingCtrlsBounds;
        for (auto i = 0; i < m_inputGains.size(); i++)
        {
            auto const& gainSlider = m_inputGains.at(i);
            if (!gainSlider)
                continue;
            gainSliderBounds.removeFromLeft(10);
            gainSlider->setBounds(gainSliderBounds.removeFromLeft(controlElementWidth));
        }
    }

    AbstractAudioVisualizer::resized();
}

void MultiMeterInputComponent::paint(Graphics& g)
{
    AbstractAudioVisualizer::paint(g);

	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

    auto controlElementsGap = 60 + (0.5f * (getHeight() - 60));

	// calculate what we need for our center circle
	auto width = getWidth();
	auto height = getHeight();
	auto outerMargin = 20;
    auto maxMeterWidth = 30;
	auto visuAreaWidth = width - 2 * outerMargin;
	auto visuAreaHeight = height - outerMargin - controlElementsGap;

	auto visuArea = Rectangle<int>(outerMargin, outerMargin, visuAreaWidth, visuAreaHeight);

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
        g.drawText(String(i), Rectangle<float>(meterLeft, visuAreaOrigY + controlElementsGap, meterWidth, float(outerMargin)), Justification::centred, true);

        meterLeft += meterWidth + meterSpacing;
    }
}

void MultiMeterInputComponent::setInputMute(unsigned int channel, bool muteState)
{
    if (channel > m_inputMutes.size())
        return;

    auto muteButtonIter = m_inputMutes.begin() + channel - 1;
    if (muteButtonIter != m_inputMutes.end() && muteButtonIter->get())
        muteButtonIter->get()->setToggleState(muteState, juce::dontSendNotification);
}

void MultiMeterInputComponent::setInputGain(unsigned int channel, float gainValue)
{
    if (channel > m_inputPositions.size())
        return;

    auto gainSliderIter = m_inputGains.begin() + channel - 1;
    if (gainSliderIter != m_inputGains.end() && gainSliderIter->get())
        gainSliderIter->get()->setValue(gainValue, juce::dontSendNotification);
}

void MultiMeterInputComponent::setPosition(unsigned int channel, juce::Point<float> position)
{
    if (channel > m_inputPositions.size())
        return;

    auto positionComponentIter = m_inputPositions.begin() + channel - 1;
    if (positionComponentIter != m_inputPositions.end() && positionComponentIter->get())
        positionComponentIter->get()->setCurrentPosition(position);
}

void MultiMeterInputComponent::setSpreadFactor(unsigned int channel, float spreadFactor)
{
    if (channel > m_inputPositions.size())
        return;

    ignoreUnused(spreadFactor);
}

void MultiMeterInputComponent::setReverbSendGain(unsigned int channel, float reverbSendGain)
{
    if (channel > m_inputPositions.size())
        return;

    ignoreUnused(reverbSendGain);
}

void MultiMeterInputComponent::processingDataChanged(AbstractProcessorData *data)
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

void MultiMeterInputComponent::processChanges()
{
    auto resizeRequired = false;

    if (m_inputMutes.size() != m_levelData.GetChannelCount())
    {
        if (m_inputMutes.size() < m_levelData.GetChannelCount())
        {
            auto missingCnt = m_levelData.GetChannelCount() - m_inputMutes.size();
            for (; missingCnt > 0; missingCnt--)
            {
                m_inputMutes.push_back(std::make_unique<TextButton>("M"));
                auto muteButton = m_inputMutes.back().get();
                muteButton->setColour(TextButton::ColourIds::buttonOnColourId, juce::Colours::red);
                muteButton->setClickingTogglesState(true);
                muteButton->onClick = [muteButton, this] {
                    auto foundMuteButtonIter = std::find_if(m_inputMutes.begin(), m_inputMutes.end(), [muteButton](std::unique_ptr<TextButton>& b) { return b.get() == muteButton; });
                    if (foundMuteButtonIter == m_inputMutes.end())
                        return;
                    auto channelIdx = foundMuteButtonIter - m_inputMutes.begin();
                    auto channel = static_cast<int>(channelIdx + 1);
                    auto muteState = muteButton->getToggleState();
                    inputMuteChange(channel, muteState);
                };
                addAndMakeVisible(*m_inputMutes.back());
            }
        }
        else if (m_inputMutes.size() > m_levelData.GetChannelCount())
        {
            auto overheadCnt = m_inputMutes.size() - m_levelData.GetChannelCount();
            for (; overheadCnt; overheadCnt--)
            {
                removeChildComponent(m_inputMutes.back().get());
                m_inputMutes.erase(m_inputMutes.end());
            }
        }

        resizeRequired = true;
    }

    if (m_inputGains.size() != m_levelData.GetChannelCount())
    {
        if (m_inputGains.size() < m_levelData.GetChannelCount())
        {
            auto missingCnt = m_levelData.GetChannelCount() - m_inputGains.size();
            for (; missingCnt > 0; missingCnt--)
            {
                m_inputGains.push_back(std::make_unique<Slider>("M"));
                auto gainSlider = m_inputGains.back().get();
                gainSlider->setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
                gainSlider->onValueChange = [gainSlider, this] {
                    auto foundGainSliderIter = std::find_if(m_inputGains.begin(), m_inputGains.end(), [gainSlider](std::unique_ptr<Slider>& b) { return b.get() == gainSlider; });
                    if (foundGainSliderIter == m_inputGains.end())
                        return;
                    auto channelIdx = foundGainSliderIter - m_inputGains.begin();
                    auto channel = static_cast<int>(channelIdx + 1);
                    auto gainValue = static_cast<float>(gainSlider->getValue());
                    inputGainChange(channel, gainValue);
                };
                addAndMakeVisible(*m_inputGains.back());
            }
        }
        else if (m_inputGains.size() > m_levelData.GetChannelCount())
        {
            auto overheadCnt = m_inputGains.size() - m_levelData.GetChannelCount();
            for (; overheadCnt; overheadCnt--)
            {
                removeChildComponent(m_inputGains.back().get());
                m_inputGains.erase(m_inputGains.end());
            }
        }

        resizeRequired = true;
    }

    if (m_inputPositions.size() != m_levelData.GetChannelCount())
    {
        if (m_inputPositions.size() < m_levelData.GetChannelCount())
        {
            auto missingCnt = m_levelData.GetChannelCount() - m_inputPositions.size();
            for (; missingCnt > 0; missingCnt--)
            {
                m_inputPositions.push_back(std::make_unique<PositionEditorComponent>());
                auto positionComponent = m_inputPositions.back().get();
                positionComponent->setCurrentPosition(juce::Point<float>(0.5f, 0.5f));
                positionComponent->setPositionCallback = [positionComponent, this] (juce::Component*, const juce::Point<float>& position) {
                    auto foundPositionComponentIter = std::find_if(m_inputPositions.begin(), m_inputPositions.end(), [positionComponent](std::unique_ptr<PositionEditorComponent>& b) { return b.get() == positionComponent; });
                    if (foundPositionComponentIter == m_inputPositions.end())
                        return;
                    auto channelIdx = foundPositionComponentIter - m_inputPositions.begin();
                    auto channel = static_cast<int>(channelIdx + 1);
                    positionChange(channel, position);
                };
                addAndMakeVisible(*m_inputPositions.back());
            }
        }
        else if (m_inputPositions.size() > m_levelData.GetChannelCount())
        {
            auto overheadCnt = m_inputPositions.size() - m_levelData.GetChannelCount();
            for (; overheadCnt; overheadCnt--)
            {
                removeChildComponent(m_inputPositions.back().get());
                m_inputPositions.erase(m_inputPositions.end());
            }
        }

        resizeRequired = true;
    }

    if (resizeRequired)
        resized();

    AbstractAudioVisualizer::processChanges();
}

}
