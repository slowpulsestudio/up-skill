# Design System Gallery Rules

---

**Figma is the single source of truth for every atom**
Every component in the gallery is a direct implementation of a Figma component/atom, not an approximation. Before implementing or updating any atom, pull its exact spec via the Figma MCP tools (`get_metadata`, `get_design_context`, `get_variable_defs`) — layout, spacing, colors, typography, and every state/variant, not just the default. Never invent a value that isn't found via the MCP tools; stop and ask instead.

**A failed response looks like:**
- Eyeballing a component from a screenshot instead of pulling its real spec via Figma MCP
- Implementing only the default state/variant and skipping the others shown in Figma
- Guessing a color, spacing, or type value instead of reading it from `get_variable_defs`/`get_design_context`

---

**Derive every value from structured MCP data, never from a rendered image**
Colors, sizes, positions, rotations, fonts, spacing and variant structure must come from the Figma MCP's structured output (`get_design_context`, `get_metadata`, `get_variable_defs`) — this is the mechanism behind "never invent a value" in the rule above. A screenshot — including one returned by `get_screenshot` — must never be used to establish, confirm, or infer a value; use it only to sanity-check something you already derived from data. Read the structured output closely: details like non-integer bounding-box dimensions (e.g. `width="48.00000038159624"` where a sibling reports a clean `32`) encode real transforms such as an instance-level rotation that a flattened code response may omit. When two MCP responses disagree, prefer the one carrying the raw geometry and say which you used and why. Downloading the exported asset bytes from URLs returned by the MCP is the prescribed workflow and is not an exception to this rule; redrawing artwork by hand is.

Check what a node actually *is* before trusting the exporter's asset. The MCP exporter will hand over a raster PNG for what is a vector node in the file; resampling that raster destroys its contrast, while the node itself carries exact geometry. A dashed graticule exported as a 200x2 PNG rendered at 13% of its intended strength — read as a `LINE` with `strokeWeight: 1` and `dashPattern: [3,3]`, it drew correctly first time. Exported assets also go stale the moment the Figma source is edited: colours and geometry transcribed from an export are a snapshot, so when the Designer says a component "looks wrong", diff the live node properties against the code *before* investigating anything else. A lever slot rendering near-black instead of mid-grey was a stale fill, not the shadow bug it appeared to be.

Design tools are a static medium and cannot express behaviour, so distinguish a control's **inherent mechanism** from its **decorative state**. The mechanism is the control's reason to exist — a knob turns, a fader drags, a slider sweeps its full continuous range — and the absence of motion in a Figma frame is never a specification that it is static. Variant frames showing a component at different values are illustrations that the thing moves, not a table of exact angles or positions to hardcode: implement the real range, drive it from the control's value, and derive geometry from the live size rather than a design-size constant, because organisms routinely stretch an atom wider than it was drawn (a slider atom drawn at 213 but instanced at 245 never reached 100%). Decorative state is the opposite — hover washes, press displacement, focus rings and glows exist only if the design defines them. If a component set has no hover variant, it has no hover behaviour; do not add one because the control "should feel tactile".

**A failed response looks like:**
- Reading a value off a screenshot, or writing a justification like "confirmed against the Figma render"
- Overlooking transform evidence (float residue on dimensions, swapped width/height, wrapper offsets) that is present in the structured data
- Treating a flattened code response as authoritative when it contradicts the raw geometry
- Accepting a raster export for a node that is really a vector, and inheriting its resampling losses
- Investigating a "looks wrong" report without first diffing the live node properties against the code
- Shipping a knob that cannot be turned or a slider that cannot be dragged because the design frame was static
- Hardcoding the angles/offsets from variant frames, or sizing geometry from a design-size constant instead of the live size
- Inventing a hover, press, or focus state the component set does not define
- Redrawing an icon or glyph by hand instead of rendering the exported asset

---

**Verify the typeface name, not just its metrics**
Font identity is a property to verify like any other, and it is the one that measuring position and size will never catch. Read `fontName.family` off the text node and compare it against the name table of the font the code actually loads — do not assume the asset in the repo is the asset in the design. A build shipped a different seven-segment face than Figma specified and it went unnoticed for the whole project, because every measurable property agreed: same nominal size, same advance width, same cap height. Only the glyph shapes differed.

When they disagree, say so and ask rather than substituting the nearest available face. The Designer may not own the font the code uses, or vice versa; the resolution is usually to get the same file into both (the repo's own `.ttf` can normally just be installed). Whoever holds the real source of truth should be the one to move.

**A failed response looks like:**
- Treating matching size, spacing and alignment as evidence that the typeface matches
- Quietly swapping in the nearest available font to close a mismatch instead of naming it
- Changing the code to paper over the gap when the design file is the thing that is wrong

---

**A small size variant is rarely a stripped-down one**
Read the small variant's own layer tree; do not reason about what "must" fit. A 4px Indicator looked too small to hold the 32px version's bezel and was built lens-only — but Figma keeps the bezel, and at 4px its 2px inside stroke swallows the fill and leaves a black disc. That dark ring is exactly what makes the dot read as crisp rather than as a smudge.

**A failed response looks like:**
- Dropping detail from a small variant because it seems geometrically implausible, without reading its layers
- Assuming a size variant is the large one scaled down rather than its own design

---

**Where a rule generates the layout, the rule outranks the drawing**
Once positions are generated from a rule — a radius and an evenly divided sweep, a grid pitch, a ratio — that generator is the source of truth, not the Figma frame. Verify the rule reproduces the design at its drawn count, then trust it everywhere else. Do not "correct" generated positions back towards hand-measured ones: the drawn values are an artist's approximation of the rule, and the disagreements between them are error, not intent. A switch selector's four tags were transcribed as four hardcoded corners that turned out not to lie on a circle (two at radius 41, two at 45.8–48), which was invisible until a lit marker's glow reached the pointer on the near pair only. Replacing them with one radius and a divided sweep fixed that and made the position count a parameter rather than a redraw.

Say this in the component's own header too — the next person to diff it against Figma will find offsets of a few pixels and assume the code has drifted.

**A failed response looks like:**
- Re-transcribing coordinates from a later Figma edit into a component whose positions are generated, reintroducing the removed defect
- Adjusting generated values to match hand-placed ones without checking whether the rule itself is wrong
- Leaving no note in the component explaining why its positions deliberately differ from the frame

---

**Don't sharpen something the design draws softly**
A 1px rule sitting on a whole coordinate straddles two device rows at half weight, and that softness is part of how it reads. Snapping it to a pixel centre makes it harder than the design intends. Match the geometry, not an idea of crispness.

**A failed response looks like:**
- Pixel-snapping a hairline the design deliberately placed on a whole coordinate
- Treating a soft edge in the design as an error to be corrected

---

**Truncating text can change its meaning, so abbreviate instead**
A fixed-width readout cut `NATURAL MINOR` down to `NATURAL`. Both are real scales, so nothing looked broken — it just said something false. The same trap catches the fix: abbreviating `MINOR PENTATONIC` to `MIN.` is worse than the original, because Minor is also a real scale.

Cut each word to its first syllable so the phrase keeps its shape; when that still does not fit, trim the longest word a character at a time rather than dropping a word. Mark whatever is left with a point, and offer the full text on hover — but only claim the tooltip when something was actually lost, or it shadows the tooltip of whatever contains it. Write the rule once and let each caller define what "fits" means; it will otherwise arrive twice, days apart, and the two copies will quietly disagree.

**A failed response looks like:**
- Truncating a label to a substring that is itself a different valid value
- Abbreviating to something that collides with another real value in the same set
- Claiming a hover tooltip on text that was not actually shortened
- Writing a second fitting/abbreviation helper without checking for the existing one

---

**Drawing an atom in the gallery is not the same as the product using it**
A component the framework owns needs a `LookAndFeel` override (or platform equivalent), not just a class. A tooltip chip had been built, matched against Figma and placed on the atoms page while every real tooltip in the app was still the framework's default yellow box, because nothing had connected the atom to `TooltipWindow`. Check what the product actually renders, not what the specimen page renders.

**A failed response looks like:**
- Declaring a component done because it renders correctly on the gallery page, without checking the product's real usage
- Building an atom for something the framework draws, without installing it into the framework's own hook

---

**Reconcile with existing shared components before building a new one**
Before adding a new gallery component, check whether an equivalent already exists as a shared component elsewhere (e.g. a component library bundled by another skill). If one exists, compare it against the Figma node it's meant to represent and extend/reuse it rather than creating a divergent duplicate implementation.

**A failed response looks like:**
- Building a new component from scratch that duplicates an existing shared one with a different API or visual result
- Overwriting an existing shared component's public API without flagging it first

---

**Components own their mechanism, but not the application's logic**
Every component takes values and exposes `std::function`-style (or platform-equivalent) callbacks, with no direct coupling to business logic, persistence, or a specific host's parameter/state system. That constraint is about *logic*, not about interaction: where the mechanism **is** the interaction — a fader track, a slider track — the atom handles its own drag and reports the result through a callback. An organism that needs control disables the atom's mouse handling and feeds it state instead. An atom that is purely presentational, with its mechanism living in whatever contains it, cannot be tested in isolation and will be reimplemented slightly differently by every consumer.

Do not wire an internal detail to a public callback the consuming plugin will need — assigning `juce::Slider::onValueChange` internally silently breaks any downstream consumer who assigns it too. `juce::Slider` repaints on every `setValue` even under `dontSendNotification`, so syncing an injected child from `paint()` is both more reliable and leaves the callback free.

**A failed response looks like:**
- Coupling a component directly to a specific app's state/parameter system "for the demo"
- Adding persistence, network calls, or other side effects inside a gallery component
- Leaving a draggable atom's drag handling to its parent, so the atom cannot be used or tested alone
- Occupying a public callback that the consuming plugin will need to assign

---

**Gallery layout: sidebar list + real-size preview pane**
The gallery's main view is a sidebar listing every atom/component, with a preview pane that renders the selected one live at its real design size, alongside its name and description. Match the "Card" layout from the design system's Component Library section (title, description, divider, preview) if one is defined in Figma — don't invent a different gallery layout.

**A failed response looks like:**
- Rendering components at an arbitrary scale instead of their real design size
- Omitting the name/description alongside the live preview
- Inventing a gallery layout instead of matching the Figma-defined Card layout when one exists

---

**Cover every state and variant, not just the default**
For each atom, implement and make browsable every state/variant defined in Figma (e.g. default/hover/active/disabled, or per-type variants like a button's "Default" vs "Input" state) — not only the first or most obvious one.

**A failed response looks like:**
- Shipping only the default-state rendering of a component and skipping documented variants
- Discovering missing states only when the Designer points them out, instead of checking every variant/state in Figma up front

---

**Build icon vectors as filled paths, never stroked lines**
When creating icon components via SVG paths (`$fig.svg`, raw vector nodes, etc.), never rely on `stroke`/`stroke-width` for the visible geometry — bake it into a filled path instead (draw the stroke's outline as a fill, or call `node.outlineStroke()` immediately after creating it). Figma's instance resize scales a node's position/dimensions via constraints, but never scales `strokeWeight` — resizing an icon instance to any size other than the master's native size leaves the stroke weight unchanged, distorting corners and making the icon look disproportionately thick or thin at its new size. Fills have no such dependency and always scale correctly with the shape's bounding box.

**A failed response looks like:**
- Building an icon's geometry with `stroke`/`stroke-width` in the SVG and never outlining it
- Only noticing the stroke-scaling bug after a Designer points out a warped/distorted icon in a resized instance
- Fixing one broken instance's `strokeWeight` by hand instead of outlining the stroke on the master (so every instance is fixed at once)

---

**Review every icon component for dynamic scaling before calling it done**
After creating or editing any icon component, verify it will render correctly at a size other than its native/authored size — not just the size it happens to be shown at first. Check that: (1) no vector in the icon relies on strokes (see rule above), and (2) every child's `constraints` are `SCALE`/`SCALE` (not `MIN`/`MIN`) so its position and size are recalculated proportionally when the containing frame/instance is resized. Do this for every icon in a set, not just the one currently reported as broken — a scaling bug in one icon usually means the same authoring mistake was repeated across the others built alongside it.

**A failed response looks like:**
- Declaring an icon set "done" after checking only the icon the Designer complained about, without auditing the others built the same way
- Leaving a new icon's constraints at the default `MIN`/`MIN` because it "looks right" at its native size
- Treating a visually-correct screenshot at native size as proof the icon will scale correctly at other sizes

---

**Prove scaling with an oversized test instance, not a native-size screenshot**
A screenshot at the icon's authored size cannot tell `SCALE` and `MIN`/`MIN` constraints apart — both render identically when the instance size equals the master size. To actually verify a fix, create a temporary instance, `resize()` it to something clearly larger than native (e.g. 5-6x), screenshot it, and confirm the geometry fills the new bounds rather than staying pinned at its original size in the middle. Delete the temporary instance once verified.

**A failed response looks like:**
- Screenshotting a fixed icon at its native 32x32 size and calling that proof the constraint fix worked
- Skipping the oversized test because the constraint value was manually set to `SCALE` and "should" be correct
- Leaving temporary oversized test instances on the canvas after verification instead of cleaning them up

---

**`outlineStroke()` does not remove the original node or preserve its parent/position**
Calling `node.outlineStroke()` creates a brand-new node parented directly to the current page — it does not replace `node` in place, and does not delete `node`. If the original stroked node isn't explicitly removed, it stays behind (often invisibly overlapping the new fill-based replacement), and it still carries the un-scalable `strokeWeight` bug the outline was meant to fix. Before calling `outlineStroke()`, record the original node's local `x`/`y` relative to its own parent; after creating the outline, `remove()` the original, `insertChild()` the new node at the same index in the same parent, and explicitly set its `x`/`y` to the saved values. Do not derive the new position from `absoluteTransform` math — it does not reliably map back to the original parent's coordinate space.

**A failed response looks like:**
- Calling `outlineStroke()` and leaving the original stroked node in the tree, assuming it was replaced automatically
- Computing the outlined node's new position via `absoluteTransform` subtraction instead of restoring the saved local `x`/`y`
- Only discovering the leftover original node later, via an audit, instead of verifying immediately after outlining

---

**`combineAsVariants()` reparents its inputs — rebuild any row that showcased them as bare children**
`figma.combineAsVariants(components, parent)` moves each input component out of wherever it currently lives and into a new ComponentSet under `parent`. If those components were previously bare children of a display row/frame (e.g. a flat icon showcase with no card chrome), that row loses those children and is left broken or empty — this is a silent side effect, not a visible error. After running `combineAsVariants`, check every frame that used to directly contain the input components, and repopulate any such display row with fresh instances of the new ComponentSet's default variant (never with the raw masters, which now live elsewhere).

**A failed response looks like:**
- Running `combineAsVariants` and only checking the new ComponentSet, without checking what the moved components used to belong to
- Leaving a showcase row silently empty/broken because its children were reparented away
- Re-inserting the raw master components back into the display row instead of instances (which would just move them again)

---

**When auditing constraints across a whole file, only children of non-auto-layout parents matter**
Figma ignores a node's `constraints` property entirely when its parent has `layoutMode` other than `NONE` — auto-layout uses `layoutAlign`/`layoutGrow` instead. A blanket scan that flags every non-`SCALE` constraint in the file will produce mostly false positives (button labels, auto-layout icon slots, etc.). Filter to `parent.layoutMode === 'NONE'` before flagging a constraint as a real scaling bug.

**A failed response looks like:**
- Reporting dozens of "constraint issues" that are actually harmless auto-layout children, burying the real bugs in noise
- Fixing constraints on auto-layout children, which has no visual effect and wastes a turn

---

**Icon fill color must be overridden per placement context**
A single icon master's default fill color will not have adequate contrast on every background it gets placed on (e.g. a dark icon fill is invisible on a dark bezel button). Don't add per-context variants of the icon itself for this — override the fill at the instance level (`instance.findAll(...)` over descendant shapes, set `fills` on each) for placements that need a different color than the master's default.

**A failed response looks like:**
- Leaving an icon instance invisible on a dark background instead of applying an instance-level fill override
- Creating a duplicate icon component per color instead of overriding fills on the instance
