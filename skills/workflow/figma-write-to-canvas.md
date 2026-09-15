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
