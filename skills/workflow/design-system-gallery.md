# Design System Gallery Rules

---

**Figma is the single source of truth for every atom**
Every component in the gallery is a direct implementation of a Figma component/atom, not an approximation. Before implementing or updating any atom, pull its exact spec via the Figma MCP tools (`get_metadata`, `get_design_context`, `get_variable_defs`) — layout, spacing, colors, typography, and every state/variant, not just the default. Never invent a value that isn't found via the MCP tools; stop and ask instead.

**A failed response looks like:**
- Eyeballing a component from a screenshot instead of pulling its real spec via Figma MCP
- Implementing only the default state/variant and skipping the others shown in Figma
- Guessing a color, spacing, or type value instead of reading it from `get_variable_defs`/`get_design_context`

---

**Reconcile with existing shared components before building a new one**
Before adding a new gallery component, check whether an equivalent already exists as a shared component elsewhere (e.g. a component library bundled by another skill). If one exists, compare it against the Figma node it's meant to represent and extend/reuse it rather than creating a divergent duplicate implementation.

**A failed response looks like:**
- Building a new component from scratch that duplicates an existing shared one with a different API or visual result
- Overwriting an existing shared component's public API without flagging it first

---

**Gallery components are presentation-only**
Every component in the gallery is a pure presentation component: it takes props/values and exposes `std::function`-style (or platform-equivalent) callbacks for interaction, with no direct coupling to business logic, persistence, or a specific host's parameter/state system. The gallery exists to preview appearance and interaction states in isolation, not to demonstrate real data wiring.

**A failed response looks like:**
- Coupling a gallery component directly to a specific app's state/parameter system "for the demo"
- Adding persistence, network calls, or other side effects inside a gallery component

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
