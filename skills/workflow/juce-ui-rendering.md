# JUCE UI Rendering Rules

Rules for implementing Figma artwork as JUCE `Component` painting code — vector import, shadows and glows, clipping, and the cost of repainting. Applies to whoever builds the shared design system library, not to plugins that merely consume it.

---

**JUCE's SVG renderer silently ignores `<filter>`**
Figma bakes drop shadows, inner shadows and layer blurs into SVG exports as `<filter>` elements. JUCE's `Drawable` parser drops them with no warning, so the artwork renders flat and nothing reports an error. Never assume an export carries its own effects — grep the SVG for `filter=` and redraw whatever you find in painting code.

**A failed response looks like:**
- Importing a Figma SVG and assuming its shadows/blurs came with it because the file parsed without error
- Concluding an effect "wasn't in the export" without grepping the SVG for `filter=`
- Reporting artwork as matching the design when only its flat geometry was checked

---

**An SVG `stdDeviation` is half the Figma blur radius**
Figma reports a blur *radius*; Gaussian maths wants *sigma*, which is half of it. Passing the radius straight through blurs at double strength. The convolution kernel must also be at least `2*ceil(3*sigma)+1` wide — a narrower kernel spreads the energy out when normalised instead of concentrating it, which reads as a weak blur rather than a clipped one.

**A failed response looks like:**
- Passing a Figma blur radius directly into a Gaussian blur as sigma
- Sizing a convolution kernel by eye instead of to `2*ceil(3*sigma)+1`
- Compensating for a too-wide blur by reducing opacity instead of halving the radius

---

**Map an asset onto its own SVG canvas, never onto its artwork bounding box**
`drawWithin` fits the *artwork* to the rectangle given, rescaling anything the designer padded inside the canvas. Because JUCE drops filters, a shadow-bearing export's visible artwork is smaller than its canvas — so `drawWithin` inflates it. A 110px bezel on a 122px canvas rendered at 122px: 11% oversized, and pushed over its own drop shadow.

**A failed response looks like:**
- Passing a component's bounds straight to `drawWithin` without accounting for the SVG's own canvas size
- Treating an export's artwork bounding box as its canvas
- Correcting the resulting size mismatch with a hand-tuned scale factor instead of mapping onto the real canvas

---

**Never round-trip a `Drawable` through an offscreen `juce::Image` if you need full strength**
Rendering vector artwork into an image and blitting it back costs most of a thin stroke's brightness to antialiasing and premultiplication. A 2px stroke measured green 91 against a specified 255 — 36% of its value. Solid fills survive this; strokes do not. Draw strokes straight to the context, and use an offscreen copy only for a blurred halo underneath.

**A failed response looks like:**
- Rasterising stroked artwork to an `Image` for convenience and losing most of its specified brightness
- Attributing a dull stroke to the blur step without measuring each stage separately
- Raising a colour's brightness to compensate instead of removing the offscreen round-trip

---

**A shadow must not rotate with the thing casting it**
Light comes from a fixed direction. Draw shadows before applying any rotation transform. Where the shape itself must rotate — a knob pointer, a lever on its side — keep the shadow inside the transform but counter-rotate its offset vector so it still falls down-and-right on screen.

**A failed response looks like:**
- Drawing a shadow inside a rotation transform so it swings around with the control
- Removing a rotating shadow entirely instead of counter-rotating its offset

---

**An overhanging shadow is clipped in two independent places**
JUCE clips a component's whole subtree at *every* ancestor, and `repaint(area)` trims the area to the component's own bounds before passing it upward. A shadow that overhangs its component needs both halves fixed, and fixing one alone does nothing:

- *Painting* — `setPaintingIsUnclipped(true)` on the component **and on every ancestor whose bounds end before the shadow reaches**. A 28px button in a 32px row has its shadow cut by the row, however unclipped the button itself is.
- *Invalidation* — the repaint must walk up the tree until it finds an ancestor large enough to hold the expanded area, or the overhang is discarded before it gets there.

Miss the first and the shadow is cut off square. Miss the second and it paints once, then leaves fragments behind whenever the component changes state — a shadow appearing where one is geometrically impossible (below the *up* position of a toggle) is stale pixels, not a drawing bug.

Both halves are the same tree walk, so write it once and call it everywhere rather than fixing ancestors by hand. Call it from `resized()`, not the constructor: the walk needs real bounds and a parent that exists, and a component is often configured before it is added to anything. Better still, size a component to contain its own shadow so containers never have to know.

The smaller the component, the worse this bites. A 4px marker bulb carries the same 11px glow as a 32px lamp, so almost all of what it draws falls outside itself — clipped, it collapses to a bare dot, which reads as "the glow was never implemented" rather than "the glow is being cropped". Before assuming an effect is missing, sample the background pixels just outside the component: a tint that stops dead at the bounds is clipping; no tint at all is a drawing problem. Separately, an offscreen layer that a blur is rasterised into must itself be padded by roughly three times the blur radius and drawn back at the matching negative offset, or the blur has nowhere to spread regardless of how ancestors clip.

**A failed response looks like:**
- Un-clipping the component but not the ancestors whose bounds cut the shadow, or vice versa
- Diagnosing stale shadow fragments as a drawing bug rather than missed invalidation
- Concluding a glow "isn't implemented" on a small component without sampling the pixels just outside its bounds
- Fixing each new nesting site by hand instead of calling one shared helper

---

**A helper that walks up the tree needs a stop condition**
A helper that un-clips each ancestor until one is large enough is right for a lamp inside a label, and wrong the moment the walk reaches a `Viewport` — un-clipping a scroll view lets the scrolled page paint over everything outside it. "The first ancestor big enough" is not a sufficient rule when some ancestors clip deliberately. The same applies to any helper that mutates ancestors rather than reading them: it must know which containers own a behaviour it must not override.

Note the failure signature. An invisible-but-still-clickable control indicates a paint problem rather than a layout or hit-test one, because hit-testing is entirely independent of painting — and it will not appear in a static snapshot of a page, only in the assembled app after an interaction. Where a bug is invisible to the render harness, add a structural assertion instead: walking the live component tree and counting un-clipped `Viewport`s catches this in one line.

**A failed response looks like:**
- Writing an ancestor-mutating tree walk with no stop condition for containers that clip on purpose
- Treating an invisible-but-clickable control as a layout or hit-test bug
- Relying on a static render harness to catch a bug that only appears after interaction

---

**Watch for notifications that get suppressed**
`setValue(x, dontSendNotification)` — which preset loads and programmatic updates use — skips callbacks entirely, so anything driven off those callbacks silently desyncs. A Mix parameter read 100% while its thumb sat at 62%.

**A failed response looks like:**
- Driving a visual update solely from a callback that `dontSendNotification` bypasses
- Testing a control only by dragging it, never by loading a preset into it

---

**An animation that only looks right because it repaints will freeze wrong**
An effect driven by a shared clock is sampled at whatever the clock happens to read when it is painted. Dropped into a surface that paints once and never again — a tooltip window, a static export — it freezes at that arbitrary value and can look broken half the time. Let the caller pass the intensity, defaulting to full.

**A failed response looks like:**
- Reading an animation clock directly in a component that may be painted only once
- Assuming a component that animates correctly in the gallery will animate at all wherever it is reused

---

**Separate static chrome from dynamic value, and cache the chrome**
A readout's screen, bezel, border and shadows depend only on its size; only the value changes. Re-deriving all of it every frame is the most expensive mistake available here, because blurs and offscreen compositing dominate everything else. Render the chrome once into an `Image` on `resized()` and blit it. The same goes for text metrics — cell pitch, glyph runs, a fixed burn row — which depend only on size and font, never on the value.

This is easy to miss because each component looks cheap in isolation. One 45px lamp redrawing itself is nothing; sixty readouts each doing two Gaussian blurs and eleven text layouts per frame is a slideshow. Cost is per-instance, so judge it at the count the busiest screen actually uses, not at the count on the atom page.

**A failed response looks like:**
- Re-deriving size-dependent chrome, blurs or text metrics inside `paint()` on every frame
- Judging a component's painting cost at the one-instance count shown on a specimen page
- Multiplying an expensive component across the busiest screen without measuring afterwards

---

**A setter that repaints must compare first**
`setValueText(s) { text = s; repaint(); }` looks harmless until a parent calls it from its own `paint()` to sync an injected child. Then *every* parent repaint schedules a second repaint wave for the child, forever, whether or not the value changed. Guard every such setter with an early return when the value is unchanged.

**A failed response looks like:**
- Writing a setter that repaints unconditionally without comparing against the current value
- Guarding one setter and leaving its sibling in the same class unguarded

---

**Animation should modulate a cached bitmap, not re-render artwork**
A pulsing lamp only varies in brightness. Redrawing its blurred glow sixty times a second to achieve that is pure waste — draw the glow once and animate the opacity. If an effect can be expressed as opacity or transform over a cached layer, it must be.

**A failed response looks like:**
- Re-rendering blurred artwork every frame to animate a property that is only opacity or transform
- Caching the artwork but still recomputing its blur per frame

---

**Allocate nothing in `paint()`**
A helper returning a `std::vector` by value, called per frame per component, is a heap allocation sixty times a second multiplied by the instance count. Use a fixed-size array or a `constexpr` table. This is the painting-thread counterpart to the `processBlock` allocation rule.

**A failed response looks like:**
- Returning a container by value from a helper called inside `paint()`
- Building a temporary string or array per frame to format a value

---

**One shared clock beats a timer per component**
Independent timers produce independent repaint waves that cannot coalesce; a single clock gives one wave per frame. Where the animation models a shared physical cause — dirty supply voltage feeding every lamp in a unit — sharing the clock is also the *correct* behaviour, not merely the cheaper one: the lamps should move together. Randomised per-component phase can feel more organic while contradicting the physics the effect is imitating.

**A failed response looks like:**
- Giving each animated instance its own `Timer` instead of subscribing to a shared clock
- Randomising phase per instance where the effect models one shared cause

---

**Measure painting cost before and after, with the real binary**
An offscreen render of every page gives a stable figure and settles an argument about where time is going in one run. A cold full-page render and interactive repaint cost are different problems — readouts measured only 12% of a cold render while being the main cause of interactive lag, because interaction repaints the same expensive thing repeatedly. Time the one that matches the complaint.

**A failed response looks like:**
- Reasoning about which component is slow instead of measuring
- Quoting a cold-render profile to answer a complaint about interactive lag
- Reporting a performance fix without a before-and-after number from the real binary

---

## Reference values worth keeping

- DSEG7's all-segments glyph is `8`; DSEG14's is `~`. Verified by ink coverage, not guessed.
- `std::atan2` returns `(-pi, pi]`, but `juce::Slider::setRotaryParameters` needs both angles positive and increasing. A negative start angle breaks drag-to-value silently.
