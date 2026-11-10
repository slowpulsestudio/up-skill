# JUCE VST3 Editor UI and Preset Rules

---

**Tooltips must be applied through a composite control's children**
JUCE's tooltip window asks only the single component directly under the mouse for its tip (a `dynamic_cast<TooltipClient*>` on that one component) — it never walks up to a parent looking for one. Several `sps` design system controls are built from child pieces rather than being one component (e.g. `sps::Adjustor`'s numeric readout and its step buttons are separate children); calling `setTooltip` on the outer control only covers whatever background area isn't occupied by a child, which for some controls is almost none of the visible surface. `sps::Adjustor` specifically is not a `juce::SettableTooltipClient` at all, so it has no `setTooltip` to call in the first place — a subclass mixing in `juce::SettableTooltipClient` is needed before a tip can be set on it.

The fix is a small recursive helper: set the tooltip on the control, then walk `getChildComponent(i)` for every child and set the same tooltip wherever the child is itself a `SettableTooltipClient` (`dynamic_cast`, skip silently if not). Apply this to every control on the panel, not just `Adjustor` — knobs, faders and switches are also built from sub-parts in places.

**A failed response looks like:**
- Calling `setTooltip` once on the outer control and assuming that covers it
- Testing tooltips by hovering one spot (e.g. the control's edge) and concluding the whole control shows a tip, when the readout or buttons in the middle show nothing
- Treating a missing tooltip on a design-system component as a bug in the design system rather than in how the tooltip was applied

---

**Rotary knobs are the default control, not linear sliders**
For continuous plugin parameters, use a rotary knob (`juce::Slider::RotaryHorizontalVerticalDrag` or equivalent rotary style) as the default control. Only use a linear `juce::Slider` (horizontal or vertical bar) when the Designer explicitly asks for one, or the parameter is inherently linear/positional in a way a knob can't represent (e.g. a playhead/scrub position). Don't default to a linear slider just because it's the JUCE `Slider` default style.

**A failed response looks like:**
- Adding a horizontal/vertical bar `Slider` for a generic gain/frequency/mix-style parameter without being asked
- Leaving a `Slider` on its default linear style instead of setting a rotary style
- Using a linear slider "for now" with intent to swap to a knob later

---

**Round displayed parameter values to whole integers**
Any numeric value shown in the UI (parameter readouts, labels, tooltips) is rounded up to a whole integer by default — no decimal places — unless the Designer explicitly asks for decimal precision on a specific parameter. Round the display text only; keep the underlying parameter value at full float precision internally.

**A failed response looks like:**
- Displaying a parameter value like `-3.42 dB` or `440.0 Hz` in the UI by default
- Truncating/flooring instead of rounding
- Rounding the underlying stored/automated parameter value itself instead of only the displayed text

---

**Every control has a hover tooltip using classic industry terminology**
Every parameter control in the editor (knob, slider, button, toggle) gets a hover tooltip (`juce::Component::setTooltip` or equivalent) explaining what it does, written using the classic, industry-standard term a working audio engineer would recognize (e.g. "Attack", "Release", "Q", "Drive", "Wet/Dry") rather than an invented or marketing-style name. This applies to every parameter, not just the ones that seem non-obvious.

When a control's on-screen label is a whimsical/thematic metaphor rather than a classic industry term (e.g. "Wiggle", "New Worm"), the tooltip opens with a plain-language definition of what that metaphor means before giving the technical/industry-standard explanation — never the technical term alone with no explanation of the metaphor, and never the metaphor's definition omitted in favor of jumping straight to jargon.

**A failed response looks like:**
- Shipping a knob/slider/button with no tooltip at all
- Only adding tooltips to a subset of "confusing" parameters instead of every control
- Using a made-up or branded label in the tooltip instead of the classic industry term (e.g. "Squish" instead of "Ratio")
- Opening a metaphor-labeled control's tooltip with the technical explanation instead of defining the metaphor first

---

**Every VST3 plugin has presets and a Randomise button in a top toolbar**
Every VST3 plugin ships with a save-able preset system (a `ComboBox` populated from named presets) and a "Randomise" button that jitters the creative/tunable parameters, both placed together in a toolbar strip across the top of the editor — not buried in a submenu or absent entirely. The Randomise button sits immediately to the right of the preset `ComboBox`, not elsewhere in the toolbar. This is a baseline UX expectation for every plugin from this studio, not an opt-in feature to be asked about per-project. Use the shared `sps::PresetToolbar` component from the design system library instead of reimplementing the toolbar from scratch each time. Run `/system-my-design` periodically to check whether a newer design system version is available.

Pressing Randomise deselects any preset entirely — the toolbar shows the literal text "Random" (no italics, no trailing `*`) until the Designer explicitly picks a preset again from the dropdown or cycle buttons, at which point normal preset-name + dirty-state display resumes. `sps::PresetToolbar` has a dedicated `showUnsavedLabel()` method for this — never fake it by clearing the `ComboBox` selection directly.

Randomise only jitters preset-tunable creative parameters — it must never touch any control in an Input or Output section of the GUI (input gain/trim, output gain, dry/wet mix, bypass), per the "Preset-defining values vs. global mode toggles" rule below.

**A failed response looks like:**
- Shipping a VST3 editor with only the generic parameter list and no preset `ComboBox` or Randomise button
- Adding presets/randomise but placing them somewhere other than a top toolbar (e.g. buried at the bottom, in a separate tab/page)
- Placing the Randomise button somewhere other than immediately to the right of the preset `ComboBox`
- Treating presets or the Randomise button as a nice-to-have the Designer has to explicitly request for each new plugin
- Reimplementing the toolbar/dirty-state logic from scratch instead of using `sps::PresetToolbar`
- Leaving the previously-selected preset's name (dirty or not) displayed after Randomise instead of showing "Random"
- Jittering an Input/Output-section control (input gain, output gain, dry/wet mix, bypass) when Randomise is pressed

---

**Preset dirty-state indicator**
When the current parameter values no longer match the saved preset they were loaded from (the Designer tweaked something, or Randomise was pressed), show the preset name in the `ComboBox` in italics with a trailing `*` (e.g. `Warm Pad*`). Revert to the plain, non-italic name with no `*` the moment the values match a saved preset again (including right after saving). `sps::PresetToolbar` (see above) already implements this via its `isDirty` callback — wire it up rather than reimplementing the italics/`*` logic.

When a plugin has named presets and is opened fresh (no saved DAW state to restore), initialize the editor to the first preset in the list via the same code path used when the Designer picks a preset from the toolbar (`sps::PresetToolbar::setSelectedPreset (0)` plus loading that preset's values into the plugin's parameters) — never leave the raw `AudioParameterFloat`/etc. defaults from the parameter layout in place. The toolbar must show that first preset selected and non-dirty (no `*`) immediately on open.

That first preset must be a dedicated "Default" preset with neutral/baseline parameter values — not just whichever preset happens to be first alphabetically or by creation order — so the Designer always has a known, unmodified state to return to via the dropdown or cycle buttons. `presets.md` is where this Default preset is documented, listed first, alongside every other named preset.

**A failed response looks like:**
- Leaving the preset name unchanged (no italics, no `*`) after a parameter has been edited or Randomise pressed
- Leaving the italics/`*` in place after the Designer saves the current values as/over that preset
- Using a different dirty-state indicator than italics + trailing `*` (e.g. a separate icon, a color change, a modal dialog)
- Leaving the editor on raw parameter defaults on first open instead of the first named preset
- Treating an arbitrary/creative preset as the first-in-list default instead of a dedicated neutral "Default" preset

---

**Retrofit the preset/Randomise toolbar on any existing plugin missing it**
The preset+Randomise toolbar and dirty-state indicator rules above are a baseline requirement for every plugin from this studio, not just new ones. If a Designer opens an existing plugin project that predates these rules (or only partially implements them) and asks for unrelated work on it, add the missing toolbar/indicator — using `sps::PresetToolbar` — as part of that same task instead of only mentioning it's missing.

**A failed response looks like:**
- Noticing the toolbar or dirty-state indicator is missing/incomplete on an existing plugin but only mentioning it instead of adding it
- Waiting for the Designer to explicitly ask for the toolbar to be retrofitted before adding it
- Treating these rules as applying only to brand-new plugins, not existing ones opened for other work

---

**Preset-defining values vs. global mode toggles**
When a plugin has both save-able presets and boolean mode toggles that represent a general workflow preference (e.g. a hard/soft character switch, or a static-vs-dynamic processing mode), keep those toggles out of the preset-value struct/table entirely. Presets should only capture the continuous/creative parameters they're meant to tune — switching presets should never silently flip a mode switch the user deliberately set.

A seed/determinism parameter (one that seeds the plugin's own internal RNG or generative trajectory) is not a global mode toggle under this rule, even if it also has its own dedicated regenerate control (e.g. a "New Worm"/"New Pattern" button) — it's part of the creative variation Randomise exists to produce, so it stays in the preset-tunable set and Randomise must still jitter it. Only exclude parameters that are genuinely non-preset workflow settings.

Any control that lives in an Input or Output section of the GUI (e.g. input gain/trim, output gain, dry/wet mix, bypass) is always excluded from presets and from Randomise, the same as a global mode toggle — these are gain-staging/session-level settings the Designer sets for their current mix, not creative content a preset should recall or Randomise should jitter. `presets.md` documents which parameters a preset stores and which excluded parameters (mode toggles, Input/Output section controls) it never stores.

**A failed response looks like:**
- Bundling a general-purpose mode toggle into the same struct/table as preset-tunable values, causing preset switches to silently change it
- Forgetting to document which parameters are intentionally excluded from presets, leaving future changes to accidentally include them
- Excluding a seed/determinism parameter from Randomise because it has its own dedicated regenerate button
- Including an Input/Output-section control (input gain, output gain, dry/wet mix, bypass) in a preset's saved values or in Randomise's jitter set

---

**Input and output belong in the layout, not among the parameters**
Every plugin wears a strip down each side: input trim above its fader on the left, mix above the output fader on the right. These are not creative parameters — they never appear in the parameter grid, and Randomise never touches them (see the rule above). Every plugin has both strips; this is chassis, not a per-plugin decision. Four of five plugins were missing one before anyone checked, because nothing made their absence visible. Both strips apply to effects; an instrument carries the output strip only (see `workflow/vst-midi-synth`).

**A failed response looks like:**
- Placing input trim, mix, or output gain in the main parameter grid alongside creative controls
- Shipping a plugin with only one of the two strips, or neither
- Treating the strips as an optional layout choice to raise with the Designer per plugin

---

**Section membership is authored, not coded**
Each plugin declares, in one place in its own source, which of its controls belong to INPUT, PERFORMANCE and OUTPUT, and which Randomise may touch. Randomise derives its scope from that same declaration rather than a second list, so moving a control into a strip excludes it with nothing else to update. The design system supplies the strips, the components and the naming/tooltip convention — it does not define which parameters a plugin has.

Slots fill from the bottom, so a lone parameter lands on the fader rather than the readout above it. An unclaimed slot draws nothing: an empty strip is a visible reminder that a plugin is unfinished, rather than a default quietly standing in for a decision nobody made.

**A failed response looks like:**
- Keeping two separate lists — one for layout, one for Randomise scope — that can disagree with each other
- Putting a plugin's parameter names into the shared design system
- Filling an empty slot with a placeholder or sensible default rather than leaving it blank
- Filling slots from the top, so a single parameter lands on the readout instead of the fader
