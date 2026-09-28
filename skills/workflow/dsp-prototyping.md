# DSP Prototyping Rules

---

**Prototype in a throwaway script before porting to real-time code**
When building audio/DSP-heavy functionality (dynamic EQ, envelope followers, filters, saturation, etc.), first validate the algorithm in Python/NumPy against real sample audio, not directly in the final target language/runtime. The prototype is disposable — it exists only to prove the algorithm sounds right before committing to a real-time port.

**A failed response looks like:**
- Writing untested DSP logic directly into the final real-time codebase (C++/JUCE, audio worklet, etc.) without validating the algorithm offline first
- Treating the throwaway prototype script as part of the shipped product

---

**Input/Output audio folders at project root, gitignored from the start**
When scaffolding prototyping work, create `Input/` and `Output/` folders at the project root by default — `Input/` holds source audio files to test against, `Output/` holds rendered/bounced results — and add both to `.gitignore` in the same commit that creates them. Audio files are binary and churn constantly; they are local working state, not project source.

**A failed response looks like:**
- Rendering into ad-hoc locations instead of a root-level `Output/` folder
- Committing audio files into `Input/`/`Output/` before gitignoring the folders

---

**Parameter sweeps over guessing values**
Render labeled output files covering a range of candidate values, rather than guessing a single value and asking the Designer to imagine alternatives. Randomly sample full sets of parameters together (not one parameter at a time, and not an exhaustive grid search of every combination) and render every candidate into a single flat folder — never split into a folder per parameter — so the Designer can listen straight through in one place.

**A failed response looks like:**
- Picking one arbitrary value per parameter and asking "does this sound right?" instead of rendering a range to compare
- Splitting sweep renders into a separate folder per parameter instead of one folder the Designer can listen through in sequence
- Sweeping parameters one at a time in isolation, or exhaustively grid-searching every combination, instead of randomly sampling full parameter sets together

---

**Visual + measured validation, not just listening**
Alongside audio renders, generate before/after spectrograms and a zoomed waveform view around a representative transient/event, plus basic level metrics (RMS/peak before vs after). Use these to confirm the change actually did what was intended (e.g. reduced energy in a specific frequency band), not just that it "sounds different."

**A failed response looks like:**
- Relying on listening alone with no visual/measured evidence of what changed
- Declaring a perceptual goal (e.g. "reduced harshness") met without a spectrogram or level comparison showing it

---

**A band share cannot show a boost in a band that already dominates**
Energy expressed as a share of the total saturates: if a band already holds most of the signal, adding more to it barely moves the number, and the control under test reads as inert. Keep two separate measurements — share of total (how the energy is divided) and absolute level within the band (how much is there) — and pick the one that answers the question being asked. This mistake was made twice in one project, on two different controls.

**A failed response looks like:**
- Concluding a control does nothing from a share metric alone
- Widening a parameter's range to fix what is actually a measurement fault

---

**Verify the instrument before trusting the reading**
A measurement that produces an impossible value is a broken instrument, not a finding. Real examples: an envelope smoother whose window was longer than the decay it was measuring, reporting a 40 ms tail as 450 ms; an unbiased autocorrelation that picked the double-period peak and read every note an octave flat; a decay fit extrapolating a 17 second tail from a 0.19 second file. Guard estimators against implausible output and prefer a method with no free parameters (per-period peaks, zero-crossing spacing) over one that needs a smoothing window chosen by hand.

**A failed response looks like:**
- Adjusting the DSP to satisfy a metric that is itself wrong
- Reporting a number that cannot physically be true
- Loosening a failing threshold instead of asking whether the check measures the right thing

---

**Check whether a reference measurement is reliable before designing against it**
Spectral band energy from a plain FFT is robust. Pitch tracking, glide depth and decay fitting on short, noisy or decaying reference material frequently are not — two versions of the same tracker produced wildly different answers on the same files. State plainly which measurements from a reference are trustworthy and which are not, and do not quote an unreliable one as a design target.

**A failed response looks like:**
- Quoting a measured glide depth to two decimal places from a tracker that has not been validated
- Silently re-using a figure after the tool that produced it has been changed

---

**Round-based tuning**
Treat tuning as iterative rounds: Round 1 randomly samples full parameter-set combinations across the full plausible range (including extremes) to find sane bounds; Round 2 refines within the range the Designer responded well to. Lock in final values only after the Designer has given explicit feedback on renders, not by guessing "reasonable" defaults upfront.

**A failed response looks like:**
- Inventing final parameter defaults without an actual round of Designer feedback on real renders
- Skipping straight to porting the algorithm into the real-time codebase before any round of tuning feedback

---

**Bias tonal judgment calls toward dark, warm, thick-bodied genres**
This studio's DSP work is for UK Bass / Future Garage and related dark, warm, bass-heavy genres. Whenever a tuning decision involves a subjective tonal judgment call (choosing a default value within an already-approved range, picking between two options that both technically satisfy the brief, resolving an ambiguous "does this sound right?"), bias toward a dark, warm, thick-bodied result. Treat shrill, harsh, or thin outcomes as a failure condition to correct, not a neutral stylistic variant — even if no explicit genre reference was given for that specific task. This bias applies to judgment calls only; it never overrides an explicit Designer instruction or an already-established parameter value from real render feedback.

**A failed response looks like:**
- Defaulting to a bright/thin/shrill setting because it was technically simplest or most "neutral," when a tonal judgment call was actually needed
- Treating a shrill or harsh result as acceptable because the Designer didn't explicitly rule it out for that specific parameter
- Applying this bias to override an explicit instruction or a value the Designer already confirmed from a real render

---

**Ad-hoc test renders go in a subfolder, never the Output/ root**
One-off A/B renders (e.g. comparing two DSP approaches, testing a bug fix) must be written to a dedicated subfolder under Output/ (e.g. `Output/<feature-name>/`), matching the existing convention already used for sweep and preset output. Never write loose WAV/PNG files directly into Output/'s root.

**A failed response looks like:**
- Writing a quick comparison render straight to Output/some_test.wav instead of Output/some_test/some_test.wav
- Leaving the Designer to manually clean up/organize stray files the agent wrote to the Output/ root
