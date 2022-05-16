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
}
