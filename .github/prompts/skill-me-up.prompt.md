---
mode: agent
description: Rebuild master-skills.md by fetching the latest skill files from the up-skill repo on GitHub, and copy any skill-bundled files into this project.
---

## Step -2 — Download a fresh repo snapshot

`raw.githubusercontent.com` is served through a CDN with a real ~5 minute cache that a cache-busting query string does **not** bypass — the cache key ignores query strings. The GitHub archive zip download below is not CDN-cached (`Cache-Control: max-age=0, private`) and is always current. So: download this zip once, at the very start of every run, and read every file needed by every later step from this one extracted snapshot. Never fetch individual files from `raw.githubusercontent.com` in this prompt.

There is never a reason to reuse a cached copy of this zip across runs — every run must hit the network fresh. Download it with a tool/method that cannot serve a locally-cached response (e.g. `curl -sL -H 'Cache-Control: no-cache' -H 'Pragma: no-cache'`, not a fetch tool that might return a cached page). If the tool you're using for downloads has its own cache, disable or bypass it for this request.

```
https://github.com/slowpulsestudio/up-skill/archive/refs/heads/main.zip
```

Extract it to a fresh temp location (do not reuse a temp directory from a previous run). Every path inside is prefixed with `up-skill-main/` (e.g. `up-skill-main/skills/platform/ios.md`).

## Step -1 — Self-update check

Read `.github/prompts/skill-me-up.prompt.md` from the extracted snapshot (`up-skill-main/.github/prompts/skill-me-up.prompt.md`).

Compare it to the current contents of `.github/prompts/skill-me-up.prompt.md` in this project.

- **If they are identical:** continue to Step 0.
- **If they differ:** tell the user: *"There are updates available for the skill-me-up prompt. Would you like me to update it now? You'll need to run `/skill-me-up` again after."*
  - If yes: overwrite `.github/prompts/skill-me-up.prompt.md` with the fetched version and stop. Do not continue setup.
  - If no: continue to Step 0 with the current version.

`/system-my-design` is not universal tooling — it's a per-skill bundled resource (currently only for `platform/juce-vst3-plugin`) and is delivered by Step 2's normal `## Resources` handling. Do not fetch or install it here.

## Step 0 — Skills setup

Check whether a `.skills` file exists in the root of this project. Whichever branch applies determines the **run type** for the rest of this prompt — remember it for Step 5:

- `.skills` already exists → this run is a **refresh** (re-running `/skill-me-up` on an already-set-up project)
- `.skills` does not exist → this run is the **initial setup**

**If `.skills` exists:** read it. It contains a list of skill names, one per line. Skip blank lines and any line starting with `#`.

**If `.skills` does not exist:** ask the user the following as real interactive questions (buttons/checkboxes via the ask-questions tool), not plain numbered lists in chat text. Ask the platform question and the workflow question separately, waiting for an answer before the next:

1. **Platform** (single-select — pick exactly one):
   - `platform/ios` — Swift iOS app
   - `platform/chrome-extension` — Chrome browser extension
   - `platform/python-mac` — Python desktop app for Mac
   - `platform/python-website` — Python web app (FastAPI etc.)
   - `platform/python-cli` — Python local script/CLI tool
   - `platform/juce-vst3-plugin` — Building a VST3 audio plugin (JUCE/C++)

2. **Workflow skills** (multi-select checkboxes; `workflow/general`, `workflow/architecture`, `workflow/git`, `workflow/testing` and `workflow/docs-style` pre-checked as recommended defaults, the rest start unchecked. If `platform/juce-vst3-plugin` was selected in the platform question, also pre-check `workflow/dsp-prototyping`):
   - `workflow/general` — core execution rules (recommended, pre-checked)
   - `workflow/architecture` — general code structure rules (recommended, pre-checked)
   - `workflow/git` — source control rules (recommended, pre-checked)
   - `workflow/testing` — testing standards (recommended, pre-checked)
   - `workflow/docs-style`: writing style rules for human-readable project docs (recommended, pre-checked)
   - `workflow/web-scraper` — if the project scrapes data from websites
   - `workflow/figma-read-from-mcp` — if the project uses Figma (design → code direction)
   - `workflow/figma-write-to-canvas` — if the project uses Figma write-to-canvas / code → canvas skills
   - `workflow/vercel-publish` — if the project deploys to Vercel
   - `workflow/vercel-password` — password gate for Vercel preview deployments
   - `workflow/image-generation` — if the project calls an AI image-generation API
   - `workflow/dsp-prototyping` — if the project involves tuning audio/DSP algorithms before a real-time port (pre-checked when `platform/juce-vst3-plugin` was selected)
   - `workflow/design-system-gallery` — if the project is a component gallery for a shared design system
   - `workflow/juce-ui-rendering` — if the project implements Figma artwork as JUCE painting code (building the design system library itself, not consuming it)

   If no interactive question tool is available, fall back to asking both as plain numbered-list questions in chat, noting `workflow/general`, `workflow/architecture`, `workflow/git`, `workflow/testing` and `workflow/docs-style` as the recommended defaults.

3. For each of the following selected skills, ask a follow-up question:

   - **`workflow/git`** — *"What is the GitHub repo URL for this project?"*
   - **`workflow/figma-read-from-mcp`** or **`workflow/figma-write-to-canvas`** — *"What is the Figma file URL for this project?"* Give the user two options:
     - Paste the URL now — save it to `.figma-url` in the project root
     - *"I'll paste it in this chat when I have it"* — reply: *"No problem — paste the Figma URL in this chat whenever you're ready and I'll save it to `.figma-url`."* then continue setup. When the user later pastes a URL starting with `https://www.figma.com/`, write it to `.figma-url`.
   - **`workflow/image-generation`** — *"Which image generation provider does this project use?"* (e.g. OpenAI / DALL·E, Replicate, Stability AI)

   Only ask one Figma file URL question even if both `workflow/figma-read-from-mcp` and `workflow/figma-write-to-canvas` were selected.

   Only ask follow-up questions for skills that were selected. Skip any that weren't.

   Once all questions are answered, write the `.skills` file with the standard comment header followed by the chosen skills, one per line — the selected platform skill first, then the selected workflow skills in the order presented above (so `workflow/general` comes first among them if it's checked):

   ```
   # This file is only a pseudo-import list for the Up-Skill mechanism — like a
   # requirements.txt for skills. It just names reusable skill files to fetch.
   # It carries NO information about what this project actually is or does.
   # The project's real identity, purpose, and requirements come from the
   # original build/meta-prompt used to create it — not from this file, and
   # not from the generated master-skills.md. Never infer project intent from
   # the skill names listed below.
   ```

Do not proceed to Step 1 until the `.skills` file exists and the skill list is confirmed.

## Step 0b — Git setup (if applicable)

If `workflow/git` is in the skills list, check whether a git remote is already configured by running `git remote get-url origin`.

- If a remote **is already set**, skip this step entirely.
- If **no remote is set** and a repo URL was provided in Step 0, then:
  1. Run `git init` if the folder is not already a git repository
  2. Run `git remote add origin {url}`
  3. Confirm the remote was set successfully before continuing
- If **no remote is set** and no URL was provided, ask: *"What is the GitHub repo URL for this project?"* then follow the steps above.

Do not proceed to the next step until this is resolved.

## Step 0c — Connect Figma MCP (if applicable)

If `workflow/figma-read-from-mcp` or `workflow/figma-write-to-canvas` is in the skills list, the Figma MCP server must be connected before continuing. If both skills are selected, this only needs to happen once — do not repeat it.

First check whether it's already connected: call `get_metadata` on the file in `.figma-url` (or a lightweight `use_figma` read). If real data comes back, the connection already works — skip the walkthrough below.

If it does not work, walk the user through connecting via Figma's own UI. Never write or edit `mcp.json` by hand, never use VS Code's "Add MCP Server" command, and never consider a non-cloud/local server address:

1. Open the Figma desktop app (or figma.com), open the target file, and switch to **Dev Mode**.
2. Open the **MCP** panel and go to **Clients**.
3. Next to **Visual Studio Code**, click **+** / **Get Figma integration**.
4. Figma auto-installs the integration, opens VS Code, and completes the connection automatically — no config file, no copy-pasted URL, no command palette steps.

After the walkthrough, call `get_metadata` again to confirm the connection now works before continuing to Step 1.

## Step 1 — Rebuild master-skills.md

For each skill name, read the corresponding skill file from the extracted snapshot downloaded in Step -2, at `up-skill-main/skills/{skill-name}.md`. If a skill name has no matching file in the snapshot, treat it as a failed fetch for the Step 4 report.

Concatenate them in the order they appear in `.skills`, with a blank line between each, and write the result to `master-skills.md` in the project root, overwriting whatever was there before.

## Step 2 — Copy skill-bundled files

For each skill, take the exact skill-file content you already fetched from the Step -2 snapshot in Step 1, and scan *that fetched content* for a `## Resources` section — never a workspace search tool (e.g. grep across the current project), and never a re-fetch. This project's workspace has no `skills/` folder to search, so a workspace-scoped search always finds nothing and silently skips bundling even when the fetched skill file has a `## Resources` section. If a skill's fetched content has no `## Resources` section, skip this step for that skill.

The `## Resources` section contains directory copy mappings, one per line, in the format:

```
source-folder/ -> dest-folder/
```

- `source-folder/` is a path relative to `skill-resources/{skill-name}/` in the up-skill repo
- `dest-folder/` is the destination path relative to this project's root

For each mapping, extract from the snapshot already downloaded in Step -2 (do not download the zip again):
1. Extract only the files whose path within the zip starts with `up-skill-main/skill-resources/{skill-name}/{source-folder}/`
2. Write each extracted file to `{project-root}/{dest-folder}/{relative-path}`, where `relative-path` is the portion after `up-skill-main/skill-resources/{skill-name}/{source-folder}/`. Create any necessary directories.
3. If a file already exists at the destination and its content differs, show the user what changed and ask whether to update it. If they decline, leave the existing file untouched. Never overwrite without asking.

Reuse the single Step -2 snapshot for all resource mappings across all skills.

## Step 3 — Create AI instruction files

Check for the following three files and create them if they don't already exist:

**`.github/copilot-instructions.md`**
```
Read master-skills.md and project-specific-agent-instructions.md for your operating instructions.
```

**`CLAUDE.md`**
```
Read master-skills.md and project-specific-agent-instructions.md for your operating instructions.
```

**`project-specific-agent-instructions.md`**
```
# Project-specific agent instructions

<!-- Add project-specific notes here — design decisions, constraints, what's
     being tested, known issues, personas, edge cases, anything the AI should
     know about this particular project that isn't covered by master-skills.md.
     This file is never overwritten by /skill-me-up. -->
```

If any of these files already exist, leave them untouched — do not overwrite or append.

## Step 4 — Report

When done, report:
- Which skills were fetched successfully
- The total line count of the new `master-skills.md`
- Any skills that failed to fetch (404 or network error)
- Which bundled files were copied (grouped by skill), and any that were skipped due to conflicts
- Whether `.github/copilot-instructions.md`, `CLAUDE.md`, and `project-specific-agent-instructions.md` were created or already existed
- If `platform/juce-vst3-plugin` is selected: whether `Input/`, `Output/` and the six project docs (including `presets.md` and `visualizer.md`) were created or already existed

## Step 4b — VST3 project scaffold (platform/juce-vst3-plugin only)

If `platform/juce-vst3-plugin` is the selected platform skill, scaffold the following now, before the git commit/push question in Step 5, so it's included in that first commit. Skip this step entirely for every other platform.

**Folders:** create empty `Input/` and `Output/` folders at the project root if they don't already exist (`Input/` for source audio test files, `Output/` for rendered/bounced results), and add both to `.gitignore` if not already present; these are local working state, not project source.

**Docs:** create the following six files at the project root if they don't already exist. Never overwrite a file that's already there. These are committed, not gitignored, since they're project documentation rather than working state. Use the actual project name in place of `<Project Name>`, and follow the style rules in `workflow/docs-style` for anything you write into them.

`visualizer.md` is scoped here only because `platform/juce-vst3-plugin` is the only scaffold step that currently exists; the spec it describes (an animated display driven by live app/DSP state) isn't conceptually JUCE-specific, and any project with that kind of display would want it. If a future skill adds its own scaffold step for a non-JUCE platform, move `visualizer.md`'s creation there (or to a shared step) rather than assuming it belongs to JUCE.

Each template starts with an `<!-- UP-SKILL SCAFFOLD PLACEHOLDER -->` marker comment. When real content is later written into one of these files, delete the entire placeholder block (marker comment and all) rather than writing new content above or below it; a file must never contain both the placeholder and real content at once. When linking between these files, use the literal filenames below (`dsp-maths.md`, `nomenclature.md`, `presets.md`, `visualizer.md`), not a remembered name like `maths.md` from a different project's convention. Every heading written into these files must have real content under it before the file is considered done; never leave a trailing heading with nothing underneath. A refresh run creates any of the six that are missing and never overwrites one that exists.

**`readme.md`**
```
<!-- UP-SKILL SCAFFOLD PLACEHOLDER -->
# <Project Name>

## <One-line concept>

<!-- The concept in plain English, the architecture/mechanism overview, and
     how to build and validate the plugin. -->

Point to [dsp-maths.md](dsp-maths.md) for the actual transfer functions;
where the two disagree, the code is unfinished. Point to
[nomenclature.md](nomenclature.md) for what each control means, to
[presets.md](presets.md) for the named presets and to
[visualizer.md](visualizer.md) for the animated display.
```

**`dsp-maths.md`**
```
<!-- UP-SKILL SCAFFOLD PLACEHOLDER -->
# <Project Name>: Mathematical Model

## 1. Purpose

This document defines the mathematical foundations of <Project Name>: the
transfer functions and formulas the prototype and the real-time port both
have to agree with. Add one heading per mechanism/engine as the design
solidifies.

### Central equations

<!-- The key equations as display math ($$ blocks), each with a bold label
     and one plain line. Collected here, with the detail in the numbered
     sections below. -->

## 2. Control maps

<!-- A padded table of the control maps with columns Control, Model value
     and Map. Plain text in the cells, cells of 40 characters or fewer, any
     note in a sentence under the table. -->
```

**`dsp-testing.md`**
```
<!-- UP-SKILL SCAFFOLD PLACEHOLDER -->
# <Project Name>: Testing Specification

## 1. Testing Philosophy

<!-- What layers of testing exist for this project (behavioural prototype
     checks, a numerical comparison harness, a DSP/state/plugin validation
     suite), what each layer can and cannot prove and current known gaps.
     Distinct from the shared workflow/testing.md skill, which is
     general-purpose rather than specific to this plugin. -->
```

**`nomenclature.md`**
```
<!-- UP-SKILL SCAFFOLD PLACEHOLDER -->
Glyphs and transfer functions are in [dsp-maths.md](dsp-maths.md).

### <Group name>

| Glyph | Name |
| ----- | ---- |

<!-- One row per control or quantity, grouped under thematic headings. Pad
     the columns. Add a Status column only if some entries are not
     implemented, abstract or conceptual. -->
<!-- Under each table, one bullet per row written "**NAME:** meaning",
     keeping any note in italics. -->
```

**`presets.md`**
```
<!-- UP-SKILL SCAFFOLD PLACEHOLDER -->
# <Project Name>: Presets

Named starting points for <Project Name>. Control values are positions from
0 to 100 and are tuned by listening, so expect them to change. What each
control means is in [nomenclature.md](nomenclature.md).

<!-- One or two sentences on the idea the set of presets follows and how it
     maps to the sound. -->

## What a preset stores

<!-- Which controls a preset stores, which it never stores (Input and
     Output section controls; Randomise never moves them either), that
     Default comes first as the neutral baseline, and what any shorthand in
     the table means. -->

## Presets

<!-- A padded table: preset name, any choice parameter, then one
     right-aligned numeric column per preset-defining control (separator row
     uses --:). No prose column, cells of 40 characters or fewer, rows no
     wider than 110 characters. Default first. One table per group of
     presets, each under its own heading. -->

## What each one means

<!-- One plain-English bullet per preset, in table order, written as
     "**Name:** description". -->
```

**`visualizer.md`**
```
<!-- UP-SKILL SCAFFOLD PLACEHOLDER -->
# <Project Name>: Visualizer

The spec for the plugin's animated display. Glyphs and transfer functions
are defined in [dsp-maths.md](dsp-maths.md), control meanings are in
[nomenclature.md](nomenclature.md) and the named settings are in
[presets.md](presets.md).

## 1. Scope

<!-- What the display depicts and what it does not claim to simulate. Note
     that the visualizer is a Slow Pulse Studio concept shared across
     plugins, and that this plugin's own mappings are in section 3. -->

## 2. Core principle

The display draws the structure the DSP is actually running, driven by the
same state the audio is driven by. It is an instrument, not an ornament.

A display fed from a free-running frame counter looks right on day one and
drifts out of correspondence with the audio straight away. Every animated
quantity must trace back to a real DSP value: a state that moved, a tap
that fired, a mode that changed pitch or a control the user turned.

**Consequences of this principle:**
* If a control has no visible effect, that is evidence the control may be
  inert. The display doubles as a liveness check.
* If the display and the audio disagree, the display is the bug report.
* Anything that cannot be traced to a DSP value does not get drawn.
* Phases come from states and never from clocks.

## 3. Visual language

### 3.1 Geometry

<!-- The picture and its geometry, with the equations in display math. -->

### 3.2 DSP state to visual state

<!-- A padded table with two columns, DSP source and Drawn as, each cell 40
     characters or fewer and in plain text. One row per quantity that is
     drawn. Under it, one bullet per row written "**DSP source:** full
     description", with any LaTeX there. -->

## 4. Implementation constraints

**Threading.** The audio thread cannot paint, allocate or lock. State
crosses to the UI through a lock-free FIFO and painting happens on the
message thread on a timer.

**Paint cost.** Measure it with an offscreen bench and record milliseconds
per frame and headroom at 60 fps. Do not assume it. Decide on
juce::OpenGLContext only from that measurement.

**Persistence.** The previous frame is kept in a back buffer and faded by a
semi-transparent rectangle. The opacity comes from a DSP value. There is no
hard frame reset.

**Design system.** Every colour, line weight, spacing value and font comes
from the shared design system's tokens and never from a literal. The
background may use juce::Colours::black directly because the palette
carries no pure black; record it as a known deviation.

## 5. Reference images and designs

<!-- Link reference images and Figma frames here as they are produced. When
     a Figma file is used, the URL belongs in .figma-url at the project
     root, and workflow/figma-read-from-mcp needs adding to .skills before
     the design can be read into code. -->

## 6. Open questions

<!-- Decisions still to be made, each as a bullet. -->
```

## Step 5 — Post-setup actions (ask in order, only if applicable)

Ask the following questions one at a time, only for the skills that are active. Skip any that aren't.

**If `workflow/git` is active:**
Use the run type determined in Step 0 to phrase the question and commit message — never call a refresh an "initial setup":
- **Initial setup run:** *"Would you like me to commit and push this initial setup to GitHub?"* If yes: stage all files, commit with the message `Initial project setup`, and push to origin.
- **Refresh run:** *"Would you like me to commit and push this skills refresh to GitHub?"* If yes: stage all files, commit with the message `Refresh skills via /skill-me-up`, and push to origin.
- If no: skip.

**A failed response looks like:**
- Asking "commit and push this initial setup" on a refresh run where `.skills` already existed before this run

**If `workflow/vercel-publish` is active** (ask after the git question is resolved):
Check whether `.vercel/project.json` exists in the project root.

- **If it exists:** Vercel is already connected — skip this question entirely.
- **If it does not exist:** ask: *"Would you like me to walk you through setting up auto-publish from your GitHub repo to Vercel?"*
  - If yes: guide the user through connecting the repo to Vercel for automatic deploys, one step at a time.
  - If no: skip.
