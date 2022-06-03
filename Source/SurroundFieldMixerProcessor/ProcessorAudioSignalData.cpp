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

#include "ProcessorAudioSignalData.h"

namespace SurroundFieldMixer
{

ProcessorAudioSignalData::ProcessorAudioSignalData()
{
    m_type = AudioSignal;

    m_sampleRate = 0;
}

ProcessorAudioSignalData::~ProcessorAudioSignalData()
{

}

void ProcessorAudioSignalData::SetChannelCount(unsigned long count)
{
    setSize(static_cast<int>(count), getNumSamples(), true, false, true);
}

unsigned long ProcessorAudioSignalData::GetChannelCount()
{
    return getNumChannels();
}

void ProcessorAudioSignalData::SetSampleRate(unsigned long rate)
{
    m_sampleRate = rate;
}

unsigned long ProcessorAudioSignalData::GetSampleRate()
{
    return m_sampleRate;
}

}
