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

**Verify exact JUCE API against the real FetchContent source, not memory**
Since JUCE is pinned via CMake `FetchContent`, the actual JUCE source is available on disk at `<build-dir>/_deps/juce-src`. When unsure of an exact method name, signature, or class (e.g. a LookAndFeel override or a Font method), grep that real source directly instead of guessing from memory — it is authoritative for the exact pinned version in use.

**A failed response looks like:**
- Guessing a JUCE method name/signature from memory and writing code against it without checking the actual fetched source when uncertain
- Assuming a newer/older JUCE API surface than the version actually pinned by FetchContent

---

**Install location: vendor subfolder, not the bare VST3 root**
Plugins install to a `SlowPulseStudio` subfolder inside the system VST3 folder, not directly into `~/Library/Audio/Plug-Ins/VST3/`. Set `VST3_COPY_DIR "$ENV{HOME}/Library/Audio/Plug-Ins/VST3/SlowPulseStudio"` on `juce_add_plugin(...)` alongside `COPY_PLUGIN_AFTER_BUILD TRUE`. This keeps every plugin from this studio grouped together in the DAW's plugin browser instead of mixed in with every other vendor's plugins.

**A failed response looks like:**
- Letting COPY_PLUGIN_AFTER_BUILD install straight to the bare VST3/ root without a vendor subfolder
- Using a different or inconsistent subfolder name across projects instead of SlowPulseStudio

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

**Preset-defining values vs. global mode toggles**
When a plugin has both save-able presets and boolean mode toggles that represent a general workflow preference (e.g. a hard/soft character switch, or a static-vs-dynamic processing mode), keep those toggles out of the preset-value struct/table entirely. Presets should only capture the continuous/creative parameters they're meant to tune — switching presets should never silently flip a mode switch the user deliberately set.

**A failed response looks like:**
- Bundling a general-purpose mode toggle into the same struct/table as preset-tunable values, causing preset switches to silently change it
- Forgetting to document which parameters are intentionally excluded from presets, leaving future changes to accidentally include them

---

**Validation**
After building, validate with `pluginval` (JUCE's own automated plugin validator) before considering the plugin "done" — this is the automated check, not a substitute for it. Then load it in a real DAW (rescan the plugin folder) and process real audio as the end-to-end smoke test; a clean `pluginval` pass alone is not sufficient.

**A failed response looks like:**
- Declaring the plugin done because it compiled, without running `pluginval`
- Running `pluginval` but never actually loading the plugin in a DAW with real audio

---

**DAW smoke-test project scaffold, gitignored from the start**
When scaffolding a new JUCE plugin project, create a `Testing/` folder containing a DAW test project (e.g. an Ableton Live `.als` project) that loads the plugin for manual smoke testing, and add `Testing/` to `.gitignore` in the same commit that creates it. DAW projects auto-generate large, constantly-churning subfolders on every save (Ableton: `Backup/` with timestamped project snapshots, `Samples/` with recorded/bounced audio) — these are local working state, not project source, and produce noisy binary diffs if tracked. Do not commit the `Testing/` folder first and gitignore it later; set this up correctly at project creation time.

**A failed response looks like:**
- Committing the DAW test project folder before gitignoring it, requiring a later `git rm --cached` cleanup
- Tracking `Backup/`/`Samples/`-style auto-generated DAW subfolders in git
- Skipping the `Testing/` scaffold entirely because "the Designer can set it up manually"
