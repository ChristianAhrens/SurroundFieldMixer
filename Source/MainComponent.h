/* Copyright (c) 2022, Christian Ahrens
 *
 * This file is part of SpaConBridge <https://github.com/ChristianAhrens/SpaConBridge>
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


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   :  public AudioAppComponent
{
public:
    MainComponent();
    ~MainComponent() override;

    void setChannelSetup(int numInputChannels, int numOutputChannels, const XmlElement* const storedSettings = nullptr);

    std::pair<int, int> getCurrentDeviceChannelCount();

    //==========================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    
    //==========================================================================
    void paint(Graphics&) override;
    void resized() override;

    //==========================================================================
    //void onNewAudiofileLoaded() override;

private:
    //==========================================================================
    //std::unique_ptr<AudioPlayerComponent>                   m_playerComponent;
    //std::vector<std::unique_ptr<CircleComponent>>           m_playerConCircles;
    //std::unique_ptr<RoutingComponent>                       m_routingComponent;
    //std::vector<std::unique_ptr<CircleComponent>>           m_routingConCircles;
    //std::map<int, std::unique_ptr<ChannelStripComponent>>   m_stripComponents;
    //std::vector<std::unique_ptr<CircleComponent>>           m_stripConCircles;
    //std::unique_ptr<AnalyserComponent>                      m_analyserComponent;

    std::vector<Colour> m_channelColours;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
