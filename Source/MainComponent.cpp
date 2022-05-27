/* Copyright (c) 2022, Christian Ahrens
 *
 * This file is part of SpaConBridge <https://github.com/ChristianAhrens/SurroundSoundMatrix>
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

#include "SurroundSoundMatrix.h"

MainComponent::MainComponent()
    : juce::Component()
{
    m_ssm = std::make_unique<SurroundSoundMatrix::SurroundSoundMatrix>();
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

    setSize(800, 600);
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
    auto surroundSoundMatrixComponent = m_ssm->getUIComponent();
    if (surroundSoundMatrixComponent)
        surroundSoundMatrixComponent->setBounds(getLocalBounds());

    if (m_setupToggleButton)
        m_setupToggleButton->setBounds(getLocalBounds().reduced(6).removeFromRight(50).removeFromTop(20));

    auto setupComponent = m_ssm->getDeviceSetupComponent();
    if (setupComponent && setupComponent->isVisible())
        setupComponent->setBounds(getLocalBounds().reduced(15));
}

