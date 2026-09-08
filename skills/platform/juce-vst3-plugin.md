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

**Install location: vendor subfolder, not the bare VST3 root**
Plugins install to a `SlowPulseStudio` subfolder inside the system VST3 folder, not directly into `~/Library/Audio/Plug-Ins/VST3/`. Set `VST3_COPY_DIR "$ENV{HOME}/Library/Audio/Plug-Ins/VST3/SlowPulseStudio"` on `juce_add_plugin(...)` alongside `COPY_PLUGIN_AFTER_BUILD TRUE`. This keeps every plugin from this studio grouped together in the DAW's plugin browser instead of mixed in with every other vendor's plugins.

**A failed response looks like:**
- Letting COPY_PLUGIN_AFTER_BUILD install straight to the bare VST3/ root without a vendor subfolder
- Using a different or inconsistent subfolder name across projects instead of SlowPulseStudio

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
After building, validate with `pluginval` (JUCE's own automated plugin validator) before considering the plugin "done" — this is the automated check, not a substitute for it. Then load it in a real DAW (rescan the plugin folder) and process real audio as the end-to-end smoke test; a clean `pluginval` pass alone is not sufficient.

**A failed response looks like:**
- Declaring the plugin done because it compiled, without running `pluginval`
- Running `pluginval` but never actually loading the plugin in a DAW with real audio
