# Architecture Rules

---

**Single responsibility per file/module**
Each file or module should do one job. Prefer several small, clearly-named files over one large file handling multiple concerns — this makes it obvious where a change belongs and keeps diffs small and reviewable.

**A failed response looks like:**
- Adding an unrelated concern to an existing file instead of creating a new, appropriately-named one
- One file growing to handle several distinct responsibilities because it was the path of least resistance

---

**Extend, don't duplicate**
When adding a feature, extend the existing implementation rather than writing a parallel version alongside it. Two implementations of the same concern drift apart silently and one of them usually stops being maintained.

**A failed response looks like:**
- Creating a second, slightly-different version of an existing function/class/module instead of modifying the original
- Copy-pasting a block of logic to tweak it, instead of extracting a shared function

---

**One thing has one name, and any second name is derived from the first**
Where the same thing is identified in two places, derive the second from the first rather than typing it twice. Two of five plugins carried a heading and a title that disagreed — one was filed under "Phase-Worm" and looked up as "Wormhole" — so everything assigned to them silently failed to appear, with nothing broken enough to notice. Removing the possibility is the only fix worth making for a class of bug whose symptom is silence; correcting the individual instance leaves the next one free to happen.

**A failed response looks like:**
- Storing a display name and a lookup key as two independently-typed values
- Fixing the one mismatched instance instead of removing the ability for them to diverge
- Adding a validation check that the two agree, when deriving one from the other would make the check unnecessary

---

**A component asserts its own requirements rather than relying on every container to remember**
If a component needs something from whatever contains it, it should assert that itself, at the point it has the information to do so. A fix written as "the Label un-clips its bulb" left every standalone bulb still broken; moving the same call into the bulb's own `resized()` fixed every use at once, including ones not written yet. Any requirement that lives in the consumer is a requirement someone will forget on the next call site.

**A failed response looks like:**
- Placing a required setup call in one parent, leaving other consumers of the same component broken
- Documenting a "containers must do X" requirement instead of making the component do X itself
- Fixing each new call site by hand as it is discovered, rather than moving the assertion into the component

---

**Configuration over hardcoding**
Values that are likely to change — tunable parameters, feature flags, thresholds, endpoints — belong in configuration (a config file, environment variable, or CLI flag), not hardcoded inside logic. Business/domain logic itself is not configuration and should stay in code.

**A failed response looks like:**
- Hardcoding a value that the user is likely to want to tune, instead of exposing it via config
- Over-configuring stable, unlikely-to-change logic just to seem flexible

---

**Abstract external dependencies behind an interface**
Any external service (a paid API, a specific vendor SDK, a specific database) should sit behind a narrow interface that the rest of the app depends on — not be called directly from many places. This is what makes a provider swappable later without a rewrite.

**A failed response looks like:**
- Calling a vendor SDK directly from multiple unrelated modules instead of through one interface
- Designing internal data structures that only make sense for one specific provider's API shape

---

**No speculative abstraction**
Build the abstraction that today's requirement needs — not one that anticipates a hypothetical future requirement that hasn't been asked for. Unused flexibility is a maintenance cost, not a benefit.

**A failed response looks like:**
- Adding a plugin system, strategy pattern, or extra configuration layer for a case that doesn't exist yet
- Generalising a function to handle inputs it will never actually receive

