#pragma once

#include <functional>
#include <juce_gui_basics/juce_gui_basics.h>

namespace sps
{

/**
 * Reusable top-toolbar component used by every Slow Pulse Studio VST3 plugin.
 * Combines a preset ComboBox (no dropdown chevron - flanked by "<"/">" cycle
 * buttons instead) with an adjacent Randomise button, and shows a dirty-state
 * indicator (italic text + trailing '*') when the current preset has been
 * manually tweaked. See skills/platform/juce-vst3-plugin.md rules:
 *   - "Every VST3 plugin has presets and a Randomise button in a top toolbar"
 *   - "Preset dirty-state indicator"
 *
 * Clicking the "<"/">" buttons cycles to the previous/next preset (wrapping
 * at the ends). Clicking anywhere else on the preset control (the name/box
 * area between them) opens the dropdown, same as a normal ComboBox.
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

        prevPresetButton.onClick = [this] { cyclePreset (-1); };
        addAndMakeVisible (prevPresetButton);

        nextPresetButton.onClick = [this] { cyclePreset (1); };
        addAndMakeVisible (nextPresetButton);

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

    /// Moves to the previous (-1) or next (+1) preset, wrapping at the ends. Triggers
    /// onPresetSelected and refreshDisplay the same as picking a preset from the dropdown.
    void cyclePreset (int delta)
    {
        const auto numItems = presetBox.getNumItems();
        if (numItems <= 0)
            return;

        const auto currentIndex = presetBox.indexOfItemId (presetBox.getSelectedId());
        const auto newIndex = ((currentIndex < 0 ? 0 : currentIndex) + delta % numItems + numItems) % numItems;
        presetBox.setSelectedId (newIndex + 1);
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
        prevPresetButton.setBounds (bounds.removeFromLeft (cycleButtonWidth));
        nextPresetButton.setBounds (bounds.removeFromRight (cycleButtonWidth));
        presetBox.setBounds (bounds);
    }

    /// Called with the newly-selected preset index when the Designer picks one from the
    /// dropdown or uses the "<"/">" cycle buttons.
    std::function<void (int)> onPresetSelected;

    /// Called when the Randomise button is clicked. Jitter the plugin's own randomisable
    /// parameters here - exclude global mode toggles per the "Preset-defining values vs.
    /// global mode toggles" rule.
    std::function<void()> onRandomise;

    /// Returns true when the current parameter values no longer match the selected preset.
    /// Wire this to compare live parameter values against the selected preset's stored values.
    std::function<bool()> isDirty;

private:
    /// Italicises the preset ComboBox's text when the current preset has been manually tweaked,
    /// and suppresses the default dropdown chevron - the "<"/">" cycle buttons replace it.
    class PresetBoxLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        bool italic = false;

        juce::Font getComboBoxFont (juce::ComboBox& box) override
        {
            auto font = juce::LookAndFeel_V4::getComboBoxFont (box);
            return italic ? font.italicised() : font;
        }

        void drawComboBox (juce::Graphics& g, int width, int height, bool /*isButtonDown*/,
                            int /*buttonX*/, int /*buttonY*/, int /*buttonW*/, int /*buttonH*/,
                            juce::ComboBox& box) override
        {
            const juce::Rectangle<float> boxBounds (0.0f, 0.0f, (float) width, (float) height);

            g.setColour (box.findColour (juce::ComboBox::backgroundColourId));
            g.fillRoundedRectangle (boxBounds, 3.0f);

            g.setColour (box.findColour (juce::ComboBox::outlineColourId));
            g.drawRoundedRectangle (boxBounds.reduced (0.5f), 3.0f, 1.0f);
            // Deliberately no arrow here - the "<"/">" cycle buttons replace it.
        }

        void positionComboBoxText (juce::ComboBox& box, juce::Label& labelToPosition) override
        {
            labelToPosition.setBounds (1, 1, box.getWidth() - 2, box.getHeight() - 2);
            labelToPosition.setFont (getComboBoxFont (box));
        }
    };

    static constexpr int randomiseButtonWidth = 100;
    static constexpr int cycleButtonWidth = 24;
    static constexpr int spacing = 8;

    PresetBoxLookAndFeel presetBoxLookAndFeel;
    juce::ComboBox presetBox;
    juce::TextButton prevPresetButton { "<" };
    juce::TextButton nextPresetButton { ">" };
    juce::TextButton randomiseButton { "Randomise" };
};

} // namespace sps
