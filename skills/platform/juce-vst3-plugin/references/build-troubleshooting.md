# JUCE VST3 Build Troubleshooting

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

**A full DAW restart may be required to load a rebuilt plugin**
Overwriting an installed `.vst3` in place and removing/re-adding the plugin instance in the DAW is not always enough to load the new binary — some hosts keep the old plugin module resident in process memory across instance add/remove. If a rebuilt plugin doesn't reflect recent changes after being re-added, fully quit and relaunch the DAW before assuming the build or install step failed.

**A failed response looks like:**
- Concluding the build/install pipeline is broken because a DAW still shows old behavior after only removing/re-adding the plugin instance
- Not mentioning a full DAW restart as a troubleshooting step when a rebuilt plugin appears stale in a host
