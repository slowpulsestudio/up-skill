#pragma once

#include <functional>
#include <juce_gui_basics/juce_gui_basics.h>

namespace sps
{

/**
 * Reusable top-toolbar component used by every Slow Pulse Studio VST3 plugin.
 * Combines a preset ComboBox with an adjacent Randomise button, and shows a
 * dirty-state indicator (italic text + trailing '*') when the current preset
 * has been manually tweaked. See skills/platform/juce-vst3-plugin.md rules:
 *   - "Every VST3 plugin has presets and a Randomise button in a top toolbar"
 *   - "Preset dirty-state indicator"
 *
 * This component owns no parameter/preset data itself - the host plugin's
 * editor wires up setPresetNames(), onPresetSelected, onRandomise, and isDirty,
 * and calls refreshDisplay() whenever a tracked parameter changes.
 */
class PresetToolbar : public juce::Component
{
public:
    PresetToolbar()
    {
        presetBox.setLookAndFeel (&presetBoxLookAndFeel);
        presetBox.onChange = [this]
        {
            if (onPresetSelected != nullptr)
                onPresetSelected (presetBox.getSelectedId() - 1);
            refreshDisplay();
        };
        addAndMakeVisible (presetBox);

        randomiseButton.onClick = [this]
        {
            if (onRandomise != nullptr)
                onRandomise();
            refreshDisplay();
        };
        addAndMakeVisible (randomiseButton);
    }

    ~PresetToolbar() override { presetBox.setLookAndFeel (nullptr); }

    /// Replaces the preset list. Call once at startup and whenever presets change.
    void setPresetNames (const juce::StringArray& names)
    {
        presetBox.clear (juce::dontSendNotification);
        for (int i = 0; i < names.size(); ++i)
            presetBox.addItem (names[i], i + 1);
    }

    /// Selects a preset by index without triggering onPresetSelected, then refreshes the display.
    void setSelectedPreset (int index)
    {
        presetBox.setSelectedId (index + 1, juce::dontSendNotification);
        refreshDisplay();
    }

    /// Re-evaluates isDirty and updates the italic/asterisk display. Call this whenever a
    /// tracked parameter changes (e.g. from an APVTS::Listener::parameterChanged callback).
    void refreshDisplay()
    {
        const auto dirty = isDirty != nullptr && isDirty();
        presetBoxLookAndFeel.italic = dirty;
        const auto itemIndex = presetBox.indexOfItemId (presetBox.getSelectedId());
        const auto name = itemIndex >= 0 ? presetBox.getItemText (itemIndex) : juce::String();
        presetBox.setText (dirty ? name + "*" : name, juce::dontSendNotification);
        presetBox.repaint();
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        randomiseButton.setBounds (bounds.removeFromRight (randomiseButtonWidth));
        bounds.removeFromRight (spacing);
        presetBox.setBounds (bounds);
    }

    /// Called with the newly-selected preset index when the Designer picks one from the dropdown.
    std::function<void (int)> onPresetSelected;

    /// Called when the Randomise button is clicked. Jitter the plugin's own randomisable
    /// parameters here - exclude global mode toggles per the "Preset-defining values vs.
    /// global mode toggles" rule.
    std::function<void()> onRandomise;

    /// Returns true when the current parameter values no longer match the selected preset.
    /// Wire this to compare live parameter values against the selected preset's stored values.
    std::function<bool()> isDirty;

private:
    /// Italicises the preset ComboBox's text when the current preset has been manually tweaked.
    class DirtyStateLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        bool italic = false;

        juce::Font getComboBoxFont (juce::ComboBox& box) override
        {
            auto font = juce::LookAndFeel_V4::getComboBoxFont (box);
            return italic ? font.italicised() : font;
        }
    };

    static constexpr int randomiseButtonWidth = 100;
    static constexpr int spacing = 8;

    DirtyStateLookAndFeel presetBoxLookAndFeel;
    juce::ComboBox presetBox;
    juce::TextButton randomiseButton { "Randomise" };
};

} // namespace sps
