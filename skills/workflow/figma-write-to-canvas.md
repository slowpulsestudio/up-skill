# Figma Write to Canvas Rules

---

**Confirm before deleting or modifying anything on the canvas**
A clarifying question from the Designer ("is this the thing you mean?") only confirms which node is being discussed — it is never permission to act on it. Before deleting, detaching, or destructively restructuring any node, state exactly what will be changed and wait for an explicit, unambiguous go-ahead ("yes, delete it" / "go ahead") in a separate reply.

**A failed response looks like:**
- Deleting or modifying a node in the same turn as answering "is this the one?" — treating identification as authorization
- Proceeding with a destructive canvas edit because a reasonable-sounding action was implied, without a separate explicit confirmation
- Assuming a screenshot or verification step counts as approval to then delete the reference

---

**Always link, never use raw node numbers**
When referring to any object on the Figma canvas in a message to the Designer, give a clickable Figma URL (`https://www.figma.com/design/<fileKey>/<name>?node-id=<id>`), never a bare node ID like `844:2951`. Node numbers are meaningless to the Designer and cannot be clicked to verify.

**A failed response looks like:**
- Writing "node 844:2951" or "id 316:44420" in a response instead of a full clickable link
- Making the Designer manually construct or guess the URL from a node ID

---

**Only the page linked in `.figma-url` is in scope**
Read the `.figma-url` file at the project root before any Figma MCP work. The file key and page/node in that URL are the only page in scope for edits. Every other page in the file is off-limits — do not create, delete, or modify anything outside that page, even for temporary reference material.

**A failed response looks like:**
- Creating or editing frames on a page other than the one in `.figma-url`
- Assuming any page in the file is fair game because it shares the same file key
- Not checking `.figma-url` before starting Figma MCP work

---

**Always use the remote MCP server**
All Figma MCP tool calls must go through the remote MCP server (`https://mcp.figma.com/mcp`). This is a hard requirement — never use a local/desktop MCP server. The write-to-canvas skills are not available on the desktop MCP server, and this assumption applies to every other MCP call in this skill as well.

**A failed response looks like:**
- Configuring or falling back to a local/desktop MCP server
- Assuming the Figma desktop app must be running for MCP connectivity
- Treating a "desktop app not running" message as a blocker instead of using the remote server

---

**Verify the Figma MCP connection before relying on it**
Connection is set up once during `/skill-me-up` (Figma's Dev Mode → MCP → Clients → **Get Figma integration** — never manual `mcp.json` edits or "Add MCP Server"). If both `figma-read-from-mcp` and `figma-write-to-canvas` are in use, that setup only happens once. Before doing any Figma MCP work in a session, confirm the connection still works with a real tool call (e.g. `get_metadata` on the file in `.figma-url`) rather than assuming it from a prior setup.

**A failed response looks like:**
- Giving manual `mcp.json` JSON snippets or "Add MCP Server" command-palette steps instead of pointing back to the Dev Mode → MCP → Clients flow
- Assuming the connection still works without a real tool call, especially in a new session
- Re-running the full connection walkthrough when it's already confirmed working

---

**Write to canvas goes code → Figma, not the other way**
The write-to-canvas skills place real design frames onto the Figma canvas from running code. They do not generate code. Use the `figma-read-from-mcp` skill for the reverse direction (reading from Figma to implement code).

---

**Don't invent Figma slash commands**
Figma's real, installable skill set is `figma-use`, `figma-use-figjam`, `figma-use-slides`, `figma-swiftui`, `figma-code-connect`, `figma-create-new-file`, `figma-generate-diagram`, `figma-generate-library`, and `figma-generate-design`. These are genuine Figma-provided skills, invokable as `/skill-name` in clients that support the Figma plugin (auto-installed on remote MCP server setup), or manually installed from `github.com/figma/mcp-server-guide` if the client doesn't support plugins. They are not part of this repo, and `up-skill` does not install them.

Capturing a running localhost app's UI into Figma (code → canvas) has no dedicated named skill — there is no `/prototype-to-figma` command. It's the raw `generate_figma_design` MCP tool, triggered by describing the goal in plain language (e.g. "Start a local server for my app and capture the UI in this Figma file: `<url>`"), never a slash command.

**A failed response looks like:**
- Referring to `/prototype-to-figma` as an installable skill, or attempting to invoke it as a slash command
- Searching for `/prototype-to-figma` as a prompt file, or treating its absence as a broken installation
- Inventing a slash command for capturing a running prototype instead of using plain-language prompting to trigger `generate_figma_design`
- If `/figma-generate-design`, `/figma-generate-library`, `/figma-use`, or another real Figma skill is unavailable, substituting an invented command instead of reporting it as an environment/installation issue

---

**Use the right approach for the job**

| Goal | How to invoke |
| --- | --- |
| Capture a running local prototype (localhost URL) into Figma | Plain-language prompt describing the goal — triggers the `generate_figma_design` MCP tool directly. Not a slash command. |
| Put coded screens and tokens into Figma | `/figma-generate-design` and `/figma-generate-library` |
| Explore a design direction from a problem statement or existing Figma design | `/figma-use` |

**A failed response looks like:**
- Using `/figma-use` when the goal is to capture a running prototype — use plain-language prompting to trigger `generate_figma_design` instead
- Using plain-language prototype-capture prompting when the goal is to explore a new design direction — use `/figma-use` instead
- Running a write-to-canvas skill without the Figma file open and ready to receive frames
- Treating the agent's canvas output as a finished design — it is always a starting point to refine

---

**Default approach: rough reference, then design system**

Before starting any capture, state the plan to the Designer in one short sentence, e.g.:

> "First I'll do a rough version for reference, then I'll link up the project's design system components and tokens. We can then refine after, issue by issue."

Then follow these steps, in order:

1. **Capture a rough reference** — use plain-language prompting to trigger the `generate_figma_design` MCP tool (there is no dedicated slash command for this) to capture the running prototype pixel-for-pixel. This is raw DOM/CSS, disconnected from the design system, and exists only as a temporary visual reference.
2. **Look for an existing screen to clone** — before building anything from scratch, search the target Figma file/page for a screen that's already structurally close to the target. Cloning and adapting real, already-composed component instances (auto-layout, bound variables) is far more reliable than assembling one from `search_design_system` results component-by-component.
3. **Rebuild using the project's design system** — always real design-system components and variables/tokens, never disconnected colours, shapes, or hardcoded text styling. Detach nested instances only where a structural change is required (column reorder, re-parenting children) — Figma blocks structural edits on instance descendants.
4. **Refine one section at a time** — screenshot after each section (header, table, action bar, etc.) before moving to the next, and fix issues one at a time rather than making sweeping changes across the whole screen at once.
5. **Deep-review design system linkage** — after wiring up the attempted components and tokens for a section, audit every element individually: confirm it is a real bound instance of a design-system component (not a plain frame/rectangle/text node styled to merely look like one), and confirm every style value (color, spacing, radius, typography, elevation, etc.) is bound to an actual Figma variable/token, not a raw hardcoded value that happens to visually match. Produce a full report listing every component and token checked, marked with a tick (✅) for anything successfully linked/bound and a cross (❌) for anything that could not be matched — for each ❌, state plainly what stand-in was used instead and that a matching component/token could not be found. Present this report to the Designer and ask them to point to the correct component or token for each ❌ item.
6. **Delete the rough reference only once the rebuild is verified** — and only with the Designer's explicit confirmation (see "Confirm before deleting or modifying anything on the canvas" above).

**A successful response looks like:**
- Stating the two-phase plan to the Designer before starting a capture
- Checking for an existing similar screen before building from scratch
- A full ✅/❌ report covering every component and token used, with each ❌ explained and handed to the Designer to resolve
- Deleting the rough reference only after verification and explicit confirmation

**A failed response looks like:**
- Starting a capture without first stating the two-phase plan to the Designer
- Building the design-system version from scratch instead of checking for an existing similar screen first
- Leaving any part of the rebuilt screen using raw/disconnected styling instead of real design-system components and tokens
- Refining multiple sections at once with no screenshot checkpoint in between
- Declaring a section complete because it looks right on screen, without auditing individual component/token bindings
- Silently substituting a "close enough" component or a hardcoded value for a ❌ item instead of flagging it
- Deleting the rough reference capture before the rebuild is verified, or without explicit confirmation

---

**Include enough context in the prompt**
When invoking a write-to-canvas skill or tool, the prompt must include:
- The skill name if using a real Figma skill (e.g. `/figma-use`), or a plain-language description of the goal if triggering `generate_figma_design` directly
- The relevant URL or problem statement
- The target Figma file URL
- Any constraints (design system components to use, screens to include, tokens to map)

The more specific the prompt, the more accurate the output. A vague prompt produces frames that need more manual correction.

Example (capturing a running prototype — plain language, not a slash command):

```text
Start a local server for my app and capture the running UI at
http://localhost:5173 in this Figma file:
<Figma file URL>

Include every unique screen. Use the existing design-system components and map
the project's tokens where possible.
```

**A failed response looks like:**
- Prefixing the prototype-capture prompt with an invented `/prototype-to-figma` command
- Calling `/figma-use` with only "make it better" — include the user research insight or specific problem to solve
- Omitting which design system components or variable collections should be used when multiple exist in the file

---

**Refine on canvas, not by re-prompting**
Once frames are on the canvas, edit them directly in Figma rather than re-running the skill with adjusted instructions. Canvas iteration is faster and produces better results than prompt iteration for visual and layout decisions.

**A failed response looks like:**
- Re-running a write-to-canvas skill to fix a spacing or colour issue that could be corrected directly on canvas
- Treating re-prompting as the default feedback loop for visual refinement

---

**Push tokens back to code after refining variables**
If `/figma-generate-library` was used and variables were edited in Figma, prompt the agent to update the design system tokens in the codebase to match. Figma becomes the source of truth for that token set from that point forward.

**A failed response looks like:**
- Editing variables in Figma and not syncing them back to code
- Updating tokens in code independently after a `/figma-generate-library` run, which would create a divergence

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
