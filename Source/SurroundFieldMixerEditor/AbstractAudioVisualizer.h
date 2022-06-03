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

#pragma once

#include <JuceHeader.h>

#include "../SurroundFieldMixerProcessor/SurroundFieldMixerProcessor.h"
#include "../submodules/JUCE-AppBasics/Source/SplitButtonComponent.h"

namespace SurroundFieldMixer
{

//==============================================================================
/*
*/
class AudioVisualizerConfigBase : public Component, public ComboBox::Listener, public JUCEAppBasics::SplitButtonComponent::Listener
{
public:
    enum ConfigFeatures
    {
        ChannelMapping      = 0x01,
        UseValuesInDBToogle = 0x02,
    };

    enum MappingKey
    {
        invalid,
        L,
        R,
        C,
        LS,
        RS,
        LFE,
        X,
        Y,
        RTA,
        WFRTA
    };
    static std::string getMappingString(MappingKey key)
    {
        switch (key)
        {
            case L:
                return "Left";
            case R:
                return "Right";
            case C:
                return "Center";
            case LS:
                return "Left Surround";
            case RS:
                return "Right Surround";
            case LFE:
                return "LFE";
            case X:
                return "X";
            case Y:
                return "Y";
            case RTA:
                return "Analyzer channel";
            case WFRTA:
                return "Waterfall analyser channel";
            default:
                return std::string{};
        }
    }

public:
    AudioVisualizerConfigBase();
    ~AudioVisualizerConfigBase();

    //==============================================================================
    void paint(Graphics&) override;
    void resized() override;

    //==============================================================================
    void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;

    //==============================================================================
    void buttonClicked(JUCEAppBasics::SplitButtonComponent* button, uint64 buttonId) override;

    //==============================================================================
    void setConfigFeatures(int features);

    //==============================================================================
    void setChannelMapping(std::map<AudioVisualizerConfigBase::MappingKey, int> mapping);
    std::map<AudioVisualizerConfigBase::MappingKey, int> const& getChannelMapping();

    //==============================================================================
    void setUsesValuesInDB(bool useValuesInDB);
    bool getUsesValuesInDB();

private:
    std::map<AudioVisualizerConfigBase::MappingKey, std::unique_ptr<Label>>     m_visualizerMappingLabels;
    std::map<AudioVisualizerConfigBase::MappingKey, std::unique_ptr<ComboBox>>  m_visualizerMappingSelects;
    std::map<AudioVisualizerConfigBase::MappingKey, int>                        m_visualizerChannelMapping;

    std::unique_ptr<JUCEAppBasics::SplitButtonComponent>    m_usesValuesInDBSplitButton;
    bool                                                    m_usesValuesInDB{ false };

    int m_configFeatures{ 0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioVisualizerConfigBase)
};

//==============================================================================
/*
*/
class AbstractAudioVisualizer : public Component, 
                                public ProcessorDataAnalyzer::Listener,
                                public DrawableButton::Listener
{
public:
    enum VisuType
    {
        InvalidFirst,
        MultiMeter,
        Scope,
        TwoDField,
        Rta,
        Waterfall,
        Waveform,
        InvalidLast
    };

    static constexpr const char* VISUALIZER_CONFIG_OPEN_ID = "VISUALIZER_CONFIG_OPEN_ID";

public:
    AbstractAudioVisualizer();
    virtual ~AbstractAudioVisualizer();

    void showConfigButton(bool enable);
    
    //==============================================================================
    void notifyChanges();
    virtual void processChanges();
    
    //==============================================================================
    virtual void setPauseProcessing(bool pause);
    
    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    void mouseDown(const MouseEvent& event) override;

    //==============================================================================
    void buttonClicked(Button* button) override;
    
    //==============================================================================
    virtual VisuType getType() = 0;
    virtual void processChangedChannelMapping() = 0;
    virtual std::unique_ptr<AudioVisualizerConfigBase> openAudioVisualizerConfig();
    virtual void closeAudioVisualizerConfig();
    
    //==============================================================================
    static std::string VisuTypeToString(VisuType type);
    static VisuType StringToVisuType(std::string typeName);

protected:
    //==============================================================================
    void setConfigFeatures(int features);
    int getConfigFeatures();

    //==============================================================================
    void setChannelMapping(std::map<AudioVisualizerConfigBase::MappingKey, int> mapping);
    std::map<AudioVisualizerConfigBase::MappingKey, int> const& getChannelMapping();

    //==============================================================================
    void setUsesValuesInDB(bool useValuesInDB);
    bool getUsesValuesInDB();

private:
    void onOpenConfigClicked();

    std::unique_ptr<DrawableButton>	            m_openConfig;
    std::unique_ptr<AudioVisualizerConfigBase>  m_visualizerConfig;

    bool m_changesPending{ false };

    std::map<AudioVisualizerConfigBase::MappingKey, int>    m_channelMapping;
    bool                                                    m_usesValuesInDB{ 0 };
    int                                                     m_configFeatures{ 0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AbstractAudioVisualizer)
};

}
