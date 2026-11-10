---
load: on-demand
description: Rules for this JUCE VST3 plugin. Target configuration (VST3, arm64, macOS 11), release policy, JUCE and the design system via FetchContent, real-time audio safety, parameters, DSP porting and validation. Read before writing, building or changing any plugin code, CMake file or project doc. It points to reference files for CMake and install details, editor UI and presets, and the project scaffold.
---

# JUCE VST3 Plugin Rules

---

**Reference files**
This skill has three reference files in its `references/` folder. Read the matching file in full before starting the work it covers, not after:
- `references/editor-ui.md`: before writing or changing any editor or GUI code, control, tooltip, preset, Randomise behaviour or the input and output strips.
- `references/project-scaffold.md`: before scaffolding a plugin project, editing `.gitignore` or `.vscode/`, or writing any of `readme.md`, `dsp-maths.md`, `dsp-testing.md`, `nomenclature.md`, `presets.md` or `visualizer.md`.
- `references/build-troubleshooting.md`: when a build or compile fails, or a rebuilt plugin seems stale in a DAW.

One rule from `references/editor-ui.md` applies to every task, so it is repeated here: every plugin has the `sps::PresetToolbar` preset and Randomise toolbar. If an existing plugin is missing it, read `references/editor-ui.md` and add it as part of whatever task you were given.

**A failed response looks like:**
- Changing the editor, presets or project docs without first reading the reference file that covers them
- Reading only the part of a reference file that seems relevant
- Working on an existing plugin that has no preset toolbar without adding one

---

**Default target configuration**
Unless the Designer explicitly asks for more, or a specific plugin has a documented technical requirement that prevents it, every Slow Pulse Studio VST3 targets: VST3 only, Apple Silicon/arm64 only (never universal, never Intel/x86_64), macOS 11.0 as the minimum deployment target, Release configuration, and plugin version `0.1.0` for its initial release. `juce_add_plugin(... FORMATS VST3 ...)` — do not add `AU`, `Standalone`, or a Windows/Linux target speculatively. Set the architecture and deployment target explicitly (`CMAKE_OSX_ARCHITECTURES "arm64"`, `CMAKE_OSX_DEPLOYMENT_TARGET "11.0"`) rather than leaving them to the toolchain default — never raise the minimum OS merely because the development machine's SDK happens to be newer. Do not create installer/distribution packaging unless explicitly requested. This is the default for effects; an instrument keeps these settings and adds the instrument flags and bus layout in `workflow/vst-midi-synth`.

**A failed response looks like:**
- Adding `Standalone`, `AU`, or other formats "for completeness" when only VST3/macOS was asked for
- Building cross-platform CMake logic for a project that only targets macOS
- Building a universal binary or an Intel/x86_64 variant instead of arm64-only
- Raising the macOS deployment target above 11.0 because the dev machine's SDK is newer
- Treating "targets macOS 11" as equivalent to "targets Apple Silicon" — the build must explicitly produce arm64
- Creating installer/distribution packaging without being asked

---

**Release artifact policy: the repo state is the release, not a separate build machine**
A release is produced from a clean, reproducible checkout: configure fresh in Release, with the arm64/macOS 11.0 settings from the rule above set explicitly, build the VST3 bundle, then verify the actual binary — not just the build configuration — before calling it done (e.g. `lipo -info` or `vtool -show-build` on the bundle's binary, confirming arm64-only and the real minimum OS). Only after that does the project's full DSP/state/plugin validation suite run, and only then is the installed `.vst3` confirmed to match what the Release build actually produced.

**A failed response looks like:**
- Calling a release done because the CMake configuration requested arm64/macOS 11, without checking the actual built binary
- Skipping the project's validation suite after a release build
- Treating a Debug-configuration build as a release artifact

---

**Signing, notarization, and distribution packaging are not part of the standard release process**
Developer ID signing, notarization, stapling, Gatekeeper distribution testing, and installer signing are not required for a Slow Pulse Studio VST3 release and must never be treated as a release blocker. Add any of these only if explicitly requested for a specific plugin.

**A failed response looks like:**
- Blocking a release on missing notarization/signing when it wasn't asked for
- Adding signing/notarization steps to a build script "to be safe" without being asked

---

**Deviating from the standard target requires a verified technical reason, not a convenience**
If a plugin genuinely cannot support macOS 11.0, arm64-only, or VST3-only, do not silently change the target. Identify the specific API, framework, dependency, SDK, or architectural requirement causing the incompatibility, verify it against the actual build/binary rather than a guess, report the required deviation and why, and get an explicit product decision before changing the standard target for that plugin. This baseline applies to every future VST3 project unless the Designer explicitly overrides it for that particular plugin.

**A failed response looks like:**
- Quietly raising the minimum macOS version or switching to a universal/Intel build without flagging it
- Changing the target configuration because it was easier, without a demonstrated technical requirement
- Proceeding with a deviation before getting an explicit go-ahead

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
Rhythmic triggering derives from the host playhead's PPQ position, and any probabilistic choice hashes the step index with a seed rather than drawing from a running RNG. A bounced render must match what was heard, and replaying a bar must fire the same pattern. This applies to tempo-synced triggering; for an instrument, MIDI event offsets are the schedule (see `workflow/vst-midi-synth`).

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

**Validation**
After building, validate with `pluginval` (JUCE's own automated plugin validator) before considering the plugin "done" — this is the automated check, not a substitute for it. Then load it in a real DAW (rescan the plugin folder) and process real audio as the end-to-end smoke test; a clean `pluginval` pass alone is not sufficient. For an instrument, processing real audio means playing a MIDI clip through it (see `workflow/vst-midi-synth`).

**A failed response looks like:**
- Declaring the plugin done because it compiled, without running `pluginval`
- Running `pluginval` but never actually loading the plugin in a DAW with real audio

---

**Telling the Designer to go test in the DAW is one line, not a checklist**
When the next step is simply "go test the plugin in your DAW" (no other setup needed), say exactly: "Time to test the VST3 in your DAW!" — not a multi-step checklist with headers. Only break it into steps if the Designer asks how.

**A failed response looks like:**
- Turning "go test the plugin in your DAW" into a multi-step checklist instead of just saying "Time to test the VST3 in your DAW!"

---

## Resources
skills/ -> .claude/skills/
