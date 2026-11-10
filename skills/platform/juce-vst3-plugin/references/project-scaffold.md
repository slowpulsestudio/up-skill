# JUCE VST3 Project Scaffold Rules

---

**DAW smoke-test project scaffold, gitignored from the start**
When scaffolding a new JUCE plugin project, create a `Testing/` folder containing a DAW test project (e.g. an Ableton Live `.als` project) that loads the plugin for manual smoke testing, and add `Testing/` to `.gitignore` in the same commit that creates it. DAW projects auto-generate large, constantly-churning subfolders on every save (Ableton: `Backup/` with timestamped project snapshots, `Samples/` with recorded/bounced audio) — these are local working state, not project source, and produce noisy binary diffs if tracked. Do not commit the `Testing/` folder first and gitignore it later; set this up correctly at project creation time. For an instrument, the test project holds an instrument track with a MIDI clip on it (see `workflow/vst-midi-synth`).

**A failed response looks like:**
- Committing the DAW test project folder before gitignoring it, requiring a later `git rm --cached` cleanup
- Tracking `Backup/`/`Samples/`-style auto-generated DAW subfolders in git
- Skipping the `Testing/` scaffold entirely because "the Designer can set it up manually"

---

**`.vscode/` and `.claude/settings.json` are committed, not gitignored**
Plugin repos from this studio are always private and single-developer, so editor and agent settings are shared project configuration, not personal clutter. That covers `.vscode/settings.json` (e.g. `chat.tools.terminal.autoApprove` entries for `cmake`, which only Copilot reads) and `.claude/settings.json` (e.g. `"permissions": { "allow": ["Bash(cmake:*)", "Bash(ctest:*)"] }`, which is what Claude Code reads) — set up whichever matches the harness in use, both if both are used, so builds don't stop for approval every time. Commit them so their settings survive a reclone instead of being re-created by hand on every machine. This does not loosen anything else — build output, `build/`, `Testing/`, and any real secret stay out regardless. If a plugin repo already has `.vscode/` in its `.gitignore`, remove that line rather than leaving the two conventions side by side.

**A failed response looks like:**
- Adding `.vscode/` to `.gitignore` out of habit, or because a public-repo template does it
- Leaving `.vscode/` or `.claude/settings.json` untracked and re-creating the same settings by hand after each clone
- Adding Copilot's `chat.tools.terminal.autoApprove` for a Claude Code user and expecting it to stop the approval prompts — Claude Code only reads `.claude/settings.json`
- Committing `.claude/settings.local.json` — that file is personal and stays out
- Extending this to a non-plugin project, where the private-repo assumption may not hold

---

**Input/Output audio folders at project root, gitignored from the start**
`/skill-me-up` creates empty `Input/` and `Output/` folders at the project root automatically for every `platform/juce-vst3-plugin` project, and adds both to `.gitignore` in the same commit that creates them — `Input/` holds source audio files to feed the plugin for manual testing, `Output/` holds rendered/bounced results for comparison. Audio files are binary and churn constantly; they are local working state, not project source.

**A failed response looks like:**
- Committing audio files into `Input/`/`Output/` before gitignoring the folders
- Skipping the `Input/`/`Output/` scaffold entirely because "the Designer can set it up manually"

---

**Project documentation scaffold: readme.md, dsp-maths.md, dsp-testing.md, nomenclature.md, presets.md, visualizer.md**
`/skill-me-up` creates six markdown docs at the project root automatically, as a template skeleton, for every `platform/juce-vst3-plugin` project, following the style rules in `workflow/docs-style`, committed (not gitignored, since this is project documentation rather than working state), filled in with real content as the design solidifies:

- `readme.md`: the concept in plain English, the architecture/mechanism overview, and how to build and validate the plugin. Points to `dsp-maths.md` for the actual transfer functions and says plainly that where the two disagree, the code is unfinished. Points to `nomenclature.md` for what each control means, to `presets.md` for the named presets and to `visualizer.md` for the animated display.
- `dsp-maths.md`: the plugin's DSP formulas and transfer functions, opening with a "Central equations" section of display math near the top, kept current as the contract the prototype and the real-time port both have to agree with. Below that, a padded table of the control maps (Control, Model value, Map).
- `dsp-testing.md`: this project's own validation record: what layers of testing exist (behavioural prototype checks, a numerical comparison harness, a DSP/state/plugin validation suite), what each layer can and cannot prove and current known gaps. This is distinct from the shared `workflow/testing.md` skill, which is general-purpose rather than specific to this plugin.
- `nomenclature.md`: a glossary of every control as padded Glyph and Name tables grouped under thematic subheadings, with one meaning bullet per entry ("**NAME:** meaning"), mapping each control's on-screen/metaphor label to what it actually does.
- `presets.md`: the named presets as a padded table, one row per preset and one right-aligned numeric column per preset-defining control, no prose column. One or two sentences on the idea the preset set follows, which controls a preset stores and which it never stores (the Input and Output section controls), with Default listed first as the neutral baseline. One plain-English bullet per preset underneath the table, in table order ("**Name:** description"). Preset values change after every listening round, which is why they live in their own file rather than in readme.md.
- `visualizer.md`: the spec for the animated display, built on the principle that every animated quantity traces back to a real DSP value rather than a free-running frame counter. A short padded table maps DSP state to visual state, with a detail bullet per row, plus a section recording implementation constraints (threading, measured paint cost, design-system tokens).

**A failed response looks like:**
- Writing real content above or below the `<!-- UP-SKILL SCAFFOLD PLACEHOLDER -->` block instead of deleting it entirely, leaving the placeholder and the real doc stacked in the same file
- Linking to a remembered filename like `maths.md` or `testing.md` from a different project's convention instead of the literal `dsp-maths.md` / `dsp-testing.md` on disk
- Leaving a heading with nothing written under it when calling the file finished
- Embedding the glyph glossary inline in `readme.md` instead of giving it its own `nomenclature.md`
- Letting `dsp-maths.md` drift from what the code actually does instead of treating it as the contract
- Gitignoring any of the six, or leaving `dsp-testing.md` unwritten so the only record of what's validated is scattered terminal history
- Placing named presets or per-preset descriptions in `readme.md` or `nomenclature.md` instead of `presets.md`
- A table that is ragged in the raw file, or that has cells longer than 40 characters, a sentence, LaTeX or a pipe character inside a cell
- Writing the glyph glossary as a bullet list, or as a table with long meaning cells instead of a short cell plus a meaning bullet underneath
- A visualizer element that cannot be traced back to a real DSP value
