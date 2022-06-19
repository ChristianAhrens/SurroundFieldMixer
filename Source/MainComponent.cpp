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

#include "MainComponent.h"

#include "SurroundFieldMixer.h"

#include <iOS_utils.h>

MainComponent::MainComponent()
    : juce::Component()
{
    m_ssm = std::make_unique<SurroundFieldMixer::SurroundFieldMixer>();
    addAndMakeVisible(m_ssm->getUIComponent());

    m_setupToggleButton = std::make_unique<TextButton>("Setup");
    m_setupToggleButton->onClick = [this] {
        auto setupComponent = m_ssm->getDeviceSetupComponent();
        if (setupComponent)
        {
            if (setupComponent->isVisible())
            {
                removeChildComponent(setupComponent);
                setupComponent->setVisible(false);
            }
            else
            {
                setupComponent->setVisible(true);
                addAndMakeVisible(setupComponent);
            }

            resized();
        }
    };
    addAndMakeVisible(m_setupToggleButton.get());

    setSize(300, 500);
}

MainComponent::~MainComponent()
{
}

void MainComponent::paint(Graphics &g)
{
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId).darker());
}

void MainComponent::resized()
{
    auto safety = JUCEAppBasics::iOS_utils::getDeviceSafetyMargins();
    auto safeBounds = getLocalBounds();
    safeBounds.removeFromTop(safety._top);
    safeBounds.removeFromBottom(safety._bottom);
    safeBounds.removeFromLeft(safety._left);
    safeBounds.removeFromRight(safety._right);

    auto SurroundFieldMixerComponent = m_ssm->getUIComponent();
    if (SurroundFieldMixerComponent)
        SurroundFieldMixerComponent->setBounds(safeBounds);

    if (m_setupToggleButton)
        m_setupToggleButton->setBounds(safeBounds.reduced(6).removeFromRight(50).removeFromTop(20));

    auto setupComponent = m_ssm->getDeviceSetupComponent();
    if (setupComponent && setupComponent->isVisible())
        setupComponent->setBounds(safeBounds.reduced(15));
}

