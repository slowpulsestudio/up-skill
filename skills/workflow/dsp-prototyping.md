# DSP Prototyping Rules

---

**Prototype in a throwaway script before porting to real-time code**
When building audio/DSP-heavy functionality (dynamic EQ, envelope followers, filters, saturation, etc.), first validate the algorithm in Python/NumPy against real sample audio, not directly in the final target language/runtime. The prototype is disposable — it exists only to prove the algorithm sounds right before committing to a real-time port.

**A failed response looks like:**
- Writing untested DSP logic directly into the final real-time codebase (C++/JUCE, audio worklet, etc.) without validating the algorithm offline first
- Treating the throwaway prototype script as part of the shipped product

---

**Parameter sweeps over guessing values**
Render one labeled output file per candidate value of each tunable parameter (holding others at a neutral baseline), rather than guessing a single value and asking the Designer to imagine alternatives. Sweep one parameter at a time first to isolate its effect; combine only after individual ranges are known.

**A failed response looks like:**
- Picking one arbitrary value per parameter and asking "does this sound right?" instead of rendering a range to compare
- Sweeping multiple parameters at once before any single-parameter baseline has been established

---

**Visual + measured validation, not just listening**
Alongside audio renders, generate before/after spectrograms and a zoomed waveform view around a representative transient/event, plus basic level metrics (RMS/peak before vs after). Use these to confirm the change actually did what was intended (e.g. reduced energy in a specific frequency band), not just that it "sounds different."

**A failed response looks like:**
- Relying on listening alone with no visual/measured evidence of what changed
- Declaring a perceptual goal (e.g. "reduced harshness") met without a spectrogram or level comparison showing it

---

**Round-based tuning**
Treat tuning as iterative rounds: Round 1 sweeps the full plausible range per parameter (including extremes) to find sane bounds; Round 2 refines within the range the Designer responded well to. Lock in final values only after the Designer has given explicit feedback on renders, not by guessing "reasonable" defaults upfront.

**A failed response looks like:**
- Inventing final parameter defaults without an actual round of Designer feedback on real renders
- Skipping straight to porting the algorithm into the real-time codebase before any round of tuning feedback

---

**Ad-hoc test renders go in a subfolder, never the Output/ root**
One-off A/B renders (e.g. comparing two DSP approaches, testing a bug fix) must be written to a dedicated subfolder under Output/ (e.g. `Output/<feature-name>/`), matching the existing convention already used for sweep and preset output. Never write loose WAV/PNG files directly into Output/'s root.

**A failed response looks like:**
- Writing a quick comparison render straight to Output/some_test.wav instead of Output/some_test/some_test.wav
- Leaving the Designer to manually clean up/organize stray files the agent wrote to the Output/ root

---

**Random combined-parameter batches for interaction effects**
After one-parameter-at-a-time sweeps establish each parameter's usable low/high range, render a second batch of combinations by randomly sampling several parameters at once within their discovered ranges (not exhaustively grid-searching every combination). This surfaces interaction effects — e.g. two parameters that sound fine individually but clash or reinforce unexpectedly together — that isolated sweeps cannot reveal. Use both methods together: one-at-a-time sweeps to find sane bounds, random combined batches to confirm those bounds still hold once parameters interact.

**A failed response looks like:**
- Only ever testing parameters in isolation and never validating combined settings before locking in defaults
- Exhaustively grid-searching every combination of every parameter instead of random sampling within already-discovered ranges
- Randomly sampling parameter combinations before any individual-parameter bounds have been established
