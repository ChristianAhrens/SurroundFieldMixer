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

#include "AudioSelectComponent.h"

namespace SurroundFieldMixer
{

//==============================================================================
AudioSelectComponent::AudioSelectComponent(AudioDeviceManager *deviceManager,
	int minAudioInputChannels, 
	int maxAudioInputChannels, 
	int minAudioOutputChannels, 
	int maxAudioOutputChannels, 
	bool showMidiInputOptions, 
	bool showMidiOutputSelector, 
	bool showChannelsAsStereoPairs, 
	bool hideAdvancedOptionsWithButton)
	: AudioDeviceSelectorComponent(*deviceManager, 
		minAudioInputChannels, 
		maxAudioInputChannels, 
		minAudioOutputChannels, 
		maxAudioOutputChannels, 
		showMidiInputOptions, 
		showMidiOutputSelector, 
		showChannelsAsStereoPairs, 
		hideAdvancedOptionsWithButton)
{
}

AudioSelectComponent::~AudioSelectComponent()
{
}

void AudioSelectComponent::paint (Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId).darker());

	g.setColour(Colours::grey);
	g.drawRect(getLocalBounds(), 1);   // draw an outline around the component

	AudioDeviceSelectorComponent::paint(g);
}

void AudioSelectComponent::resized()
{
	AudioDeviceSelectorComponent::resized();
}

}