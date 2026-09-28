# JUCE VST3 Plugin Rules

---

**Default target configuration**
Unless the Designer explicitly asks for more, target VST3 only, macOS only, no Standalone build. `juce_add_plugin(... FORMATS VST3 ...)` — do not add `AU`, `Standalone`, or a Windows/Linux target speculatively.

**A failed response looks like:**
- Adding `Standalone`, `AU`, or other formats "for completeness" when only VST3/macOS was asked for
- Building cross-platform CMake logic for a project that only targets macOS

---

**JUCE acquisition and build**
Bring JUCE in via CMake `FetchContent`, pinned to a specific release tag — never a moving branch. Use `juce_add_plugin(... COPY_PLUGIN_AFTER_BUILD TRUE)` so a plain `cmake --build` deploys the compiled `.vst3` straight to `~/Library/Audio/Plug-Ins/VST3/`, with no manual copy step.

**A failed response looks like:**
- Pinning `FetchContent` to `main`/`develop` instead of a release tag
- Leaving out `COPY_PLUGIN_AFTER_BUILD`, forcing a manual copy step after every build
- Suggesting Projucer or a manually-downloaded JUCE install when the Designer has no existing JUCE setup

---

**Build with hidden symbol visibility**
By default, CMake/Clang exports every symbol from a plugin bundle with default visibility. For a JUCE VST3 this means hundreds of internal JUCE symbols end up globally exported per plugin (confirmed with `nm -g <plugin>.vst3/Contents/MacOS/*` — typically 300-700+ T/D symbols), when only a handful of VST3 entry points (`GetPluginFactory`, `bundleEntry`, `bundleExit`) need to be visible at all.

When a host loads more than one plugin built from the same JUCE version in one process, macOS's dynamic linker can coalesce matching default-visibility symbols across the separately-loaded bundles — including function-local statics such as JUCE's Typeface font cache and its mutex. Two independently-built plugins can end up sharing one instance of that static at runtime. Unloading one plugin then destroys the shared instance out from under the other, aborting the host (confirmed: Ableton Live, "mutex lock failed: Invalid argument", on quit or on unloading any one of the affected plugins).

Fix: every plugin's CMakeLists.txt must set, before `FetchContent_MakeAvailable(JUCE)`:

```cmake
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
```

Verify with `nm -g <plugin>.vst3/Contents/MacOS/<name> | grep -E " T | D | S " | wc -l` — should drop to roughly 3 (just the required entry points), not hundreds.

**A failed response looks like:**
- Treating a crash on host shutdown/plugin-unload as unfixable "JUCE noise" without checking exported symbol counts first
- Fixing symbol visibility in only one of two plugins loaded together — the other plugin still exporting default-visibility symbols is enough to still collide
- Assuming this only matters for plugins sharing the exact same design-system version; it collides on matching JUCE version/ABI, independent of which design system is used

---

**Design system acquisition**
Every plugin's UI is built from the shared Slow Pulse Studio design system, never from stock JUCE widgets or a bespoke LookAndFeel. Bring it in the same way JUCE itself is brought in — CMake `FetchContent`, pinned to a specific release tag, never a moving branch:

```cmake
FetchContent_Declare(
    SpsDesignSystem
    GIT_REPOSITORY https://github.com/slowpulsestudio/sps-juce-design-system.git
    GIT_TAG v0.1.0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(SpsDesignSystem)
target_link_libraries(<TargetName> PRIVATE sps::DesignSystem)
```

Do not copy design system source files into the plugin's own `Source/` folder. The library carries its own fonts, icons and colours; a plugin that copies them will drift the moment the design system is updated. Every colour, radius, spacing value and font size comes from the design system's tokens rather than being written as a literal in the plugin.

Continuous parameters use `sps::RotaryKnob`. Choice parameters use `sps::SwitchSelector`. Boolean parameters use `sps::Toggle`. Numeric entry uses `sps::Adjustor`. Readouts use `sps::Quartz`. The preset toolbar is `sps::PresetToolbar`. The chassis is `sps::ModulePanel`.

A plugin that is not yet connected to the design system is migrated with `/adopt-design-system`, which is a one-time job. After that, run `/system-my-design` periodically to check whether a newer design system version is available.

**A failed response looks like:**
- Building a plugin editor from stock `juce::Slider` / `juce::TextButton` / `GenericAudioProcessorEditor` instead of the design system's components
- Copying design system source files into the plugin's own `Source/` folder instead of linking the library
- Pinning the design system's `FetchContent` to `main` instead of a release tag
- Writing a hex colour, corner radius or font size directly into a plugin when a design system token already defines it
- Writing a bespoke `LookAndFeel` to restyle stock JUCE widgets

---

**Verify exact JUCE API against the real FetchContent source, not memory**
Since JUCE is pinned via CMake `FetchContent`, the actual JUCE source is available on disk at `<build-dir>/_deps/juce-src`. When unsure of an exact method name, signature, or class (e.g. a LookAndFeel override or a Font method), grep that real source directly instead of guessing from memory — it is authoritative for the exact pinned version in use.

**A failed response looks like:**
- Guessing a JUCE method name/signature from memory and writing code against it without checking the actual fetched source when uncertain
- Assuming a newer/older JUCE API surface than the version actually pinned by FetchContent

---

**JUCE 9's juce_audio_processors_headless VST3 module can fail to compile on newer Clang/libc++ with 'shared_ptr' is not a member of 'std'**
The Steinberg VST3 SDK headers vendored inside JUCE 9's juce_audio_processors_headless module use std::shared_ptr without an explicit #include <memory>. Recent Apple Clang/libc++ releases (Xcode 16+) have stopped transitively pulling in <memory> through other standard headers, so this compiles fine on older toolchains but fails on newer ones with no fix in JUCE's newest release tag (9.0.2 as of writing — verify no newer tag has fixed it before applying this). Since the failing file lives in FetchContent-downloaded, non-persistent vendor source, don't patch it directly — force the include via a compiler flag on the plugin's own target instead: target_compile_options(<TargetName> PRIVATE -include memory)

**A failed response looks like:**
- Editing the vendored JUCE header directly in build/_deps/juce-src — it gets wiped and re-fetched on the next clean configure
- Concluding the Designer's own Source/ code or repo is broken/incomplete when the compile error is inside JUCE's own headers before any of their code is even reached
- Not checking whether a newer JUCE release tag has already fixed this before applying the -include memory workaround

---

**The -include memory workaround must be scoped to C++ translation units**
The rule above is correct but incomplete: applied unscoped, `-include memory` is also passed to JUCE's C and Objective-C sources, which fail immediately with `fatal error: 'memory' file not found` — a confusing error that looks like a broken toolchain rather than a scoping mistake. Scope it to C++ only:

```cmake
target_compile_options(<Target> PRIVATE
    $<$<COMPILE_LANGUAGE:CXX,OBJCXX>:-include;memory>)
```

**A failed response looks like:**
- Applying `-include memory` to the whole target instead of scoping it to `CXX,OBJCXX`
- Concluding the compiler or JUCE checkout is broken when a `.c`/`.m` file can't find a C++ header

---

**Tooltips must be applied through a composite control's children**
JUCE's tooltip window asks only the single component directly under the mouse for its tip (a `dynamic_cast<TooltipClient*>` on that one component) — it never walks up to a parent looking for one. Several `sps` design system controls are built from child pieces rather than being one component (e.g. `sps::Adjustor`'s numeric readout and its step buttons are separate children); calling `setTooltip` on the outer control only covers whatever background area isn't occupied by a child, which for some controls is almost none of the visible surface. `sps::Adjustor` specifically is not a `juce::SettableTooltipClient` at all, so it has no `setTooltip` to call in the first place — a subclass mixing in `juce::SettableTooltipClient` is needed before a tip can be set on it.

The fix is a small recursive helper: set the tooltip on the control, then walk `getChildComponent(i)` for every child and set the same tooltip wherever the child is itself a `SettableTooltipClient` (`dynamic_cast`, skip silently if not). Apply this to every control on the panel, not just `Adjustor` — knobs, faders and switches are also built from sub-parts in places.

**A failed response looks like:**
- Calling `setTooltip` once on the outer control and assuming that covers it
- Testing tooltips by hovering one spot (e.g. the control's edge) and concluding the whole control shows a tip, when the readout or buttons in the middle show nothing
- Treating a missing tooltip on a design-system component as a bug in the design system rather than in how the tooltip was applied

---

**A DSP port is verified against the prototype, not against the compiler**
When porting a tuned prototype to real-time code, build a small offline harness that renders through the shipping code path and compares it numerically against the prototype — pitch, envelope times, per-band energy, peak. "It builds" and "it sounds about right" are not evidence. Keep the harness in the repo as a separate target so the comparison can be re-run after any change to the DSP.

**A failed response looks like:**
- Reporting a port complete because it compiles and loads
- Judging a port by ear alone
- Deleting the comparison harness once it passes

---

**Pre-render per-setting work off the audio thread**
Where a prototype renders one fixed event per setting and replays it, the real-time form is a pre-rendered buffer, not per-sample synthesis. Render on a background thread into a spare buffer and publish it with a single atomic store; the audio thread only ever reads. Anything expensive and setting-dependent (large additive partial counts, filter design, normalisation passes) belongs there.

**A failed response looks like:**
- Summing dozens of partials per sample in `processBlock`
- Rebuilding a wavetable or buffer inside a parameter callback on the audio thread

---

**Host-position scheduling, not a free-running counter**
Rhythmic triggering derives from the host playhead's PPQ position, and any probabilistic choice hashes the step index with a seed rather than drawing from a running RNG. A bounced render must match what was heard, and replaying a bar must fire the same pattern.

**A failed response looks like:**
- A sample counter that drifts against the host
- A `std::random` generator whose sequence depends on how many blocks have been processed

---

**Install location: vendor subfolder, not the bare VST3 root**
Plugins install to a `Slow Pulse Studio` subfolder inside the system VST3 folder, not directly into `~/Library/Audio/Plug-Ins/VST3/`. Set `VST3_COPY_DIR "$ENV{HOME}/Library/Audio/Plug-Ins/VST3/Slow Pulse Studio"` on `juce_add_plugin(...)` alongside `COPY_PLUGIN_AFTER_BUILD TRUE`. This keeps every plugin from this studio grouped together in the DAW's plugin browser instead of mixed in with every other vendor's plugins.

**A failed response looks like:**
- Letting COPY_PLUGIN_AFTER_BUILD install straight to the bare VST3/ root without a vendor subfolder
- Using a different or inconsistent subfolder name across projects instead of Slow Pulse Studio

---

**COMPANY_NAME controls DAW vendor grouping, separate from install path**
`COMPANY_NAME` in `juce_add_plugin()` is the metadata a DAW's own plugin browser uses to group plugins by vendor — this is completely independent of the filesystem install subfolder (`VST3_COPY_DIR`). Setting one without the other will not fix vendor-grouping issues in the DAW.

**A failed response looks like:**
- Assuming the install-path vendor subfolder also controls how the DAW browser groups the plugin — it doesn't, `COMPANY_NAME` does
- Changing `VST3_COPY_DIR` to fix a DAW-browser vendor-grouping complaint instead of `COMPANY_NAME`

---

**A full DAW restart may be required to load a rebuilt plugin**
Overwriting an installed `.vst3` in place and removing/re-adding the plugin instance in the DAW is not always enough to load the new binary — some hosts keep the old plugin module resident in process memory across instance add/remove. If a rebuilt plugin doesn't reflect recent changes after being re-added, fully quit and relaunch the DAW before assuming the build or install step failed.

**A failed response looks like:**
- Concluding the build/install pipeline is broken because a DAW still shows old behavior after only removing/re-adding the plugin instance
- Not mentioning a full DAW restart as a troubleshooting step when a rebuilt plugin appears stale in a host

---

**Real-time audio safety in `processBlock`**
`processBlock` runs on the audio thread and must never allocate, lock, log, or do file/network I/O — any of these can cause audible dropouts/glitches in the DAW. Parameter changes must be smoothed (`juce::SmoothedValue`), never applied as a hard jump, to avoid zipper noise/clicks.

**A failed response looks like:**
- Allocating a `std::vector`, `juce::String`, or any heap object inside `processBlock`
- Calling `DBG`/`std::cout`/file I/O from `processBlock`
- Reading a raw parameter value directly into a filter coefficient every block instead of smoothing it

---

**Parameters**
Expose all user-facing parameters through a single `AudioProcessorValueTreeState`, defined once in `createParameterLayout()`. Internal/derived values (e.g. automatic makeup gain) are not user parameters and must not be added to the APVTS.

**A failed response looks like:**
- Reading raw member variables from the editor instead of going through the APVTS
- Exposing an internal implementation detail as a user-facing parameter without being asked

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

That first preset must be a dedicated "Default" preset with neutral/baseline parameter values — not just whichever preset happens to be first alphabetically or by creation order — so the Designer always has a known, unmodified state to return to via the dropdown or cycle buttons.

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

Any control that lives in an Input or Output section of the GUI (e.g. input gain/trim, output gain, dry/wet mix, bypass) is always excluded from presets and from Randomise, the same as a global mode toggle — these are gain-staging/session-level settings the Designer sets for their current mix, not creative content a preset should recall or Randomise should jitter.

**A failed response looks like:**
- Bundling a general-purpose mode toggle into the same struct/table as preset-tunable values, causing preset switches to silently change it
- Forgetting to document which parameters are intentionally excluded from presets, leaving future changes to accidentally include them
- Excluding a seed/determinism parameter from Randomise because it has its own dedicated regenerate button
- Including an Input/Output-section control (input gain, output gain, dry/wet mix, bypass) in a preset's saved values or in Randomise's jitter set

---

**Input and output belong in the layout, not among the parameters**
Every plugin wears a strip down each side: input trim above its fader on the left, mix above the output fader on the right. These are not creative parameters — they never appear in the parameter grid, and Randomise never touches them (see the rule above). Every plugin has both strips; this is chassis, not a per-plugin decision. Four of five plugins were missing one before anyone checked, because nothing made their absence visible.

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

---

**Validation**
After building, validate with `pluginval` (JUCE's own automated plugin validator) before considering the plugin "done" — this is the automated check, not a substitute for it. Then load it in a real DAW (rescan the plugin folder) and process real audio as the end-to-end smoke test; a clean `pluginval` pass alone is not sufficient.

**A failed response looks like:**
- Declaring the plugin done because it compiled, without running `pluginval`
- Running `pluginval` but never actually loading the plugin in a DAW with real audio

---

**Telling the Designer to go test in the DAW is one line, not a checklist**
When the next step is simply "go test the plugin in your DAW" (no other setup needed), say exactly: "Time to test the VST3 in your DAW!" — not a multi-step checklist with headers. Only break it into steps if the Designer asks how.

**A failed response looks like:**
- Turning "go test the plugin in your DAW" into a multi-step checklist instead of just saying "Time to test the VST3 in your DAW!"

---

**DAW smoke-test project scaffold, gitignored from the start**
When scaffolding a new JUCE plugin project, create a `Testing/` folder containing a DAW test project (e.g. an Ableton Live `.als` project) that loads the plugin for manual smoke testing, and add `Testing/` to `.gitignore` in the same commit that creates it. DAW projects auto-generate large, constantly-churning subfolders on every save (Ableton: `Backup/` with timestamped project snapshots, `Samples/` with recorded/bounced audio) — these are local working state, not project source, and produce noisy binary diffs if tracked. Do not commit the `Testing/` folder first and gitignore it later; set this up correctly at project creation time.

**A failed response looks like:**
- Committing the DAW test project folder before gitignoring it, requiring a later `git rm --cached` cleanup
- Tracking `Backup/`/`Samples/`-style auto-generated DAW subfolders in git
- Skipping the `Testing/` scaffold entirely because "the Designer can set it up manually"

---

**Input/Output audio folders at project root, gitignored from the start**
When scaffolding a new JUCE plugin project, create `Input/` and `Output/` folders at the project root — `Input/` holds source audio files to feed the plugin for manual testing, `Output/` holds rendered/bounced results for comparison — and add both to `.gitignore` in the same commit that creates them. Audio files are binary and churn constantly; they are local working state, not project source.

**A failed response looks like:**
- Committing audio files into `Input/`/`Output/` before gitignoring the folders
- Skipping the `Input/`/`Output/` scaffold entirely because "the Designer can set it up manually"

---

## Resources
prompts/ -> .github/prompts/
