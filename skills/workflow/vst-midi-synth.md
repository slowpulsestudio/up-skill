---
load: on-demand
description: Rules for JUCE VST3 instruments played from MIDI. Instrument target flags and buses, the output-only strip, sample-accurate MIDI, authored voice behaviour, declicking cut voices, pitching pre-renders, MIDI testing across the keyboard and a note-driven display. Read before building a synth or converting an effect into one, and before touching IS_SYNTH, NEEDS_MIDI_INPUT, MIDI note handling, voices, retrigger, polyphony, velocity or pitch bend.
---

# VST MIDI Synth Rules

Rules that apply only when a JUCE VST3 plugin is a played MIDI instrument rather than an effect. This skill sits on top of `platform/juce-vst3-plugin`, which still owns the build, design system, real-time safety ("Real-time audio safety in `processBlock`"), pre-rendering ("Pre-render per-setting work off the audio thread") and the preset and Randomise rules in its `references/editor-ui.md`. Tuning still follows "Round-based tuning" and "Bias tonal judgment calls toward dark, warm, thick-bodied genres" in `workflow/dsp-prototyping`. Where a rule there is written for effects, the rule here replaces it for instruments.

---

**Instruments are a separate target configuration**
A played synth sets `IS_SYNTH TRUE`, `NEEDS_MIDI_INPUT TRUE`, `NEEDS_MIDI_OUTPUT FALSE` and `VST3_CATEGORIES Instrument Synth`, declares an output bus only (no input bus), returns `true` from `acceptsMidi()` and `false` from `producesMidi()` and `isMidiEffect()`, and reports its longest possible release from `getTailLengthSeconds()`. `isBusesLayoutSupported()` checks the output only (mono or stereo). Everything else in "Default target configuration" (VST3 only, arm64 only, macOS 11.0) still applies. Converting an existing effect into an instrument changes how the host classifies it, so rescan the plugin folder in the DAW after the first build, and restart the DAW fully if it still shows the old type.

**A failed response looks like:**
- Leaving a stereo input bus on an instrument "for compatibility"
- Changing `IS_SYNTH` but leaving `VST3_CATEGORIES` as `Fx`
- Returning 0 from `getTailLengthSeconds()` on a voice with a release tail
- `isBusesLayoutSupported()` still requiring the input layout to match the output

---

**Instruments wear an output strip only**
The two-strip chassis in "Input and output belong in the layout, not among the parameters" is for effects. An instrument has no input to trim and no dry signal to mix, so it carries the output strip alone. Input trim and mix are not added as dead controls. Output gain stays out of presets and Randomise, as it does for effects. The section-membership declaration ("Section membership is authored, not coded") simply has no INPUT members.

**A failed response looks like:**
- Adding an input trim or mix strip to an instrument because the chassis rule says every plugin has both
- Moving output gain into the parameter grid once the left strip is gone
- Drawing an empty left strip on an instrument as a "reminder" that it is unfinished

---

**MIDI is handled sample-accurately**
Process each `MidiBuffer` event at its own `samplePosition` inside the block, interleaved with sample generation, never all at the start of the block. These event offsets are the instrument's schedule; "Host-position scheduling, not a free-running counter" applies only to tempo-synced triggering. A note-on with velocity 0 is a note-off: JUCE's `isNoteOn()` already excludes it, so don't special-case it back in. All Notes Off (CC 123) and All Sound Off (CC 120) stop sound immediately. Clear the output buffer at the start of `processBlock`, since an instrument has no input to pass through.

**A failed response looks like:**
- Applying every note-on in a block at sample 0, so timing jitters by up to a block
- Ignoring All Notes Off, leaving a stuck note after the transport stops
- Adding voices into an output buffer that was never cleared

---

**Voice behaviour is authored and documented**
Before writing voice code, decide with the Designer and write down in `readme.md` and `nomenclature.md`: mono or poly (and the voice count), one-shot or held for as long as the key is down, whether a new note cuts or stacks on the last one, which note wins when several are held, the velocity-to-level curve, the playable key range and root (build) note, and whether pitch bend, the mod wheel and the sustain pedal are supported. These are design decisions for the Designer, not defaults an agent picks silently. Unsupported controllers are documented as unsupported.

**A failed response looks like:**
- Settling note priority or retrigger behaviour in a code comment only
- Silently ignoring pitch bend or sustain without saying so in the docs
- Picking a linear velocity curve without it being a decision the Designer made

---

**A cut or stolen voice is declicked**
When a new note cuts a sounding voice, or a voice is stolen, fade the outgoing sound over a few milliseconds (or crossfade into the new one) rather than jumping straight to the new note's first sample. Fading only the edges of a pre-rendered buffer does not cover a cut partway through its tail. Verify by rendering a retrigger mid-tail and checking the waveform around the cut for a step discontinuity, not by ear. Found in Plasma Beam: note-on reset `beamPosition` to `0.0` while the previous hit was still playing.

**A failed response looks like:**
- Resetting the playback position on note-on while the previous tail is still sounding
- Relying on the limiter to hide the click
- Judging the retrigger by ear instead of inspecting the rendered waveform at the cut

---

**Pitching a pre-render by playback rate has costs**
Transposing a pre-rendered sound by reading it faster aliases going up and shortens it in time; reading it slower dulls and lengthens it. Choose deliberately and document the choice: render per played pitch off the audio thread (cached per note, as in "Pre-render per-setting work off the audio thread"), band-limit the resampler, or accept and document the trade-off with the key range clamped to where it holds up. Measure aliasing at the top of the key range with a spectrogram. Found in Plasma Beam: it builds at C2 and reads up to +48 semitones (16x speed) with linear interpolation.

**A failed response looks like:**
- Linear-interpolation playback at +4 octaves of content that already reaches high frequencies at the root
- Only listening at the root key
- Not mentioning that envelope and sweep times scale with pitch

---

**Instruments are tested with MIDI, across the keyboard**
Offline render harnesses, the prototype port comparison ("A DSP port is verified against the prototype, not against the compiler") and round-based tuning all cover several keys (at least low, root, high and the extremes of the playable range) and several velocities, not only the root. The test harness drives the real `processBlock` with `MidiBuffer` events and covers: a retrigger mid-tail, a note exactly on a block boundary, two notes in one block, a note-on with velocity 0, All Notes Off, and silence with no notes (output must be exactly zero). `pluginval` runs as usual. In "Validation", "process real audio" means playing a MIDI clip through the instrument, and the `Testing/` project from "DAW smoke-test project scaffold, gitignored from the start" holds an instrument track with a MIDI clip on it.

**A failed response looks like:**
- Tuning a synth entirely at one note and discovering at the DAW stage that other keys sound wrong
- A harness that calls the voice directly instead of going through `processBlock` with MIDI
- A `Testing/` project with no MIDI clip in it

---

**The display follows played notes, not a clock**
Any visualiser stamps hits from note-on events on the audio thread's own sample clock (lock-free, as with existing display state) and shows the played pitch, so the display matches what is heard. When a trigger clock is removed during a conversion, remove its display hooks, parameters, preset fields, tooltips and docs in the same change, sweeping every call site.

**A failed response looks like:**
- Leaving tempo, grid, swing or seed parameters or their tooltips behind after a plugin stops being clock-triggered
- A display still keyed to a removed clock
