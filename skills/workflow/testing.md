# Testing Rules

---

**Real end-to-end verification before "done"**
A passing lint/type-check or a mocked unit test is not sufficient to call a feature done. Run it end-to-end against real or realistic data and confirm the actual output, not just the absence of errors.

**A failed response looks like:**
- Declaring a feature complete because "no errors found" without ever running it
- Relying solely on mocked tests for a feature that touches a real external system or real data

---

**Test components individually, then the full pipeline**
When a change spans multiple stages (e.g. generate → validate → composite), verify each stage in isolation first, then run the full pipeline together. This makes it obvious which stage a failure belongs to, instead of debugging a black-box end-to-end failure.

**A failed response looks like:**
- Only testing the full pipeline and guessing which stage caused a failure
- Skipping isolated component checks because the full run "looked fine"

---

**Automate structural validation**
Wherever a human would otherwise eyeball output for correctness (dimensions, counts, ordering, naming, duplicates, missing files), write an automated check instead. Manual visual inspection should be reserved for genuinely subjective judgement (does this look good?), not structural correctness (is this the right size/order/count?).

**A failed response looks like:**
- Leaving a mechanically-checkable property (file count, dimensions, ordering) to manual inspection
- Adding a validation step that only checks the happy path and never runs against a broken/edge case

---

**Measure pixels against the specified value; don't eyeball screenshots**
Where a visual property has a specified value — a hex colour, a stroke weight, a position — sample it and compare numbers. A trace rendering at 36% of its specified brightness looked merely "a bit dull"; measurement found it in one step after looking at it had missed it repeatedly.

Align two images before diffing them: minimise total pixel difference to find the offset first, because a crop that is off by a few pixels produces confident, wrong conclusions about colour and geometry. Never zoom to judge — a 3x upscale of a 4px dot turns it square, the artefact gets blamed on the resampler, and the real defect ships. Export the source node at 1:1 and compare at that size. Equally, do not trust a coordinate read from design metadata to locate the thing being measured: an auto-layout wrapper offset a bulb by 27px, the sampled region came back as uniform background, and that was reported as a match. Locate the feature *in the image* by searching for the extreme pixel, and sanity-check that the region is not flat before drawing any conclusion from it.

**A failed response looks like:**
- Judging a colour, weight or brightness by looking at a screenshot instead of sampling it
- Diffing two images without first aligning them
- Zooming in to assess a small feature, then explaining away the resampling artefacts
- Sampling at a coordinate taken from metadata without confirming the feature is actually there
- Reporting a match from a region that is uniformly flat

---

**Bisect to isolate; don't reason about probable causes**
Disable one contributor at a time and measure after each. Two rounds located a brightness loss in an offscreen compositing step *after* the obvious suspect had already been ruled out by measurement — faster and more reliable than reading framework internals to build a theory.

**A failed response looks like:**
- Proposing a likely cause from reasoning instead of disabling contributors one at a time
- Stopping at the first plausible suspect without measuring whether removing it actually helps

---

**Static renders cannot catch state-transition bugs**
A batch/offscreen render draws each component once, in its initial state. Stale shadows, animation phase, hover and press artwork are all invisible to it. Reason explicitly about what changes on interaction and whether that region gets repainted — "not reproducible in the render harness" is not an answer for a bug seen in the running app.

Where a bug is structurally invisible to the harness, add an assertion against the live tree instead of a visual check. **Then break the fix on purpose and confirm the assertion fires** — a check written against an already-fixed bug has never been shown to be capable of failing, and is worth nothing until it has.

**A failed response looks like:**
- Treating a clean static render as evidence that interaction states are correct
- Writing an assertion for a bug and never verifying it fails when the bug is reintroduced
- Dismissing a Designer-reported bug as unreproducible because the render harness cannot express it

---

**Surface failures loudly**
During development, failures (failed requests, failed assertions, unexpected values) should be logged clearly, not swallowed silently. A silent failure inside a loop or background task is far harder to diagnose than a loud one.

**A failed response looks like:**
- Catching an exception and continuing without logging it
- A test or validation step that fails closed (reports success) when it can't actually verify the condition

