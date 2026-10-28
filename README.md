# AI Skills

This repository contains a reusable library of AI instruction files ("skills") that can be shared across multiple software projects.

## Purpose

The skills in this repository are the **source of truth** for AI coding guidance. They are designed to be maintained once and reused everywhere.

Projects should **not** point AI agents directly at individual skill files in this repository. Instead, `/skill-me-up` builds each project's skills in three levels, so an agent's context only holds what the current task needs:

1. **Always loaded:** the project's `AGENTS.md`, the cross-tool instruction file that Copilot, Codex, Cursor, Gemini CLI and most other coding agents read from the project root. It holds the `load: always` skills in full (`workflow/general` and `workflow/architecture`) and a one-line index of every other selected skill saying when to read it. The project's own notes sit below the generated section. `CLAUDE.md` holds only `@AGENTS.md`, so Claude loads the same file.
2. **Loaded when a task matches:** every other skill is installed as `.claude/skills/upskill-{category}-{name}/SKILL.md`. Claude and Copilot discover these from their descriptions; other agents find them through the index in `AGENTS.md`.
3. **Loaded when a skill says so:** long, task-specific sections of a skill live in its `references/` folder and are read only when the skill's body points to them.

Only Claude follows `@` imports, so `AGENTS.md` never uses them. Levels 2 and 3 are plain file reads by path, which every agent can do.

## Repository Structure

```text
skills/
    workflow/
        general.md          ← always included in every project
        architecture.md
        testing.md
        git.md
        docs-style.md
        deep-linking.md
        prototyping.md
        figma-read-from-mcp.md
        figma-write-to-canvas.md
        vercel-publish.md
        vercel-password.md
        image-generation.md
        web-scraper.md
        dsp-prototyping.md
        generative-video.md
        design-system-gallery.md
        juce-ui-rendering.md
    platform/
        ios.md
        chrome-extension.md
        python-mac.md
        python-mac/references/
        python-website.md
        python-cli.md
        juce-vst3-plugin.md
        juce-vst3-plugin/references/
```

Each file should cover **one topic only** and remain reusable across projects. Each starts with frontmatter giving its `load` level and a `description` of when to read it; see [how-to-create-skills.md](how-to-create-skills.md).

## Setup Steps

### 1. Fetch `/skill-me-up`

Open your new project's folder in VS Code (an empty folder is fine) and paste this into the chat panel (Copilot or Claude):

```
Run `mkdir -p .claude/skills/skill-me-up && curl -sL https://raw.githubusercontent.com/slowpulsestudio/up-skill/main/.claude/skills/skill-me-up/SKILL.md -o .claude/skills/skill-me-up/SKILL.md` in this project. Use curl, not a fetch tool — a fetch tool returns a summary, not the exact file.
```

This always pulls the latest version straight from GitHub — no need to clone or copy this repo.

### 2. Run "Skill me up"

Start a new chat, so it picks up the new slash command, and run:

```
/skill-me-up
```

### 3. Answer the questions

The AI asks real interactive questions (buttons/checkboxes), not plain text:

- **Platform — single-select, pick exactly one:**

  ```
  platform/ios
  platform/chrome-extension
  platform/python-mac
  platform/python-website
  platform/python-cli
  platform/juce-vst3-plugin
  ```

- **Workflow: multi-select checkboxes, `general`/`architecture`/`git`/`testing`/`docs-style` pre-checked as recommended defaults. Add or remove any others that apply:**

  ```
  workflow/general
  workflow/architecture
  workflow/git
  workflow/testing
  workflow/docs-style
  workflow/figma-read-from-mcp
  workflow/figma-write-to-canvas
  workflow/vercel-publish
  workflow/vercel-password
  workflow/image-generation
  workflow/web-scraper
  workflow/deep-linking
  workflow/prototyping
  workflow/dsp-prototyping
  workflow/generative-video
  workflow/design-system-gallery
  workflow/juce-ui-rendering
  ```

See [how-to-setup-projects.md](how-to-setup-projects.md) for the full walkthrough and a skill-by-skill reference table.

## Upgrading an existing project

Projects set up before slash commands moved to `.claude/skills/` still have `.github/prompts/skill-me-up.prompt.md`, which the Copilot harness ignores. Open the project in VS Code and paste this into the chat panel (Copilot or Claude), once:

```
Run `curl -sL https://raw.githubusercontent.com/slowpulsestudio/up-skill/main/migrate-to-skills.md` and follow the instructions it prints in this project. Use curl, not a fetch tool — a fetch tool returns a summary, not the exact file.
```

It removes the old prompt files, installs the new `/skill-me-up` and rebuilds the skills into `AGENTS.md`, moving any project notes across. Commit or stash any changes first. After that, run `/skill-me-up` as normal.

## Usage

For each project:

1. From `skills/workflow/`, pick via a multi-select checklist; `general.md`, `architecture.md`, `git.md`, `testing.md` and `docs-style.md` are pre-checked as recommended defaults. Add or remove any others that apply.
2. From `skills/platform/`, pick **exactly one** via a single-select question to match the project's platform (e.g. `ios.md` for an iOS app, `python-mac.md` for a Mac desktop app). Never include more than one platform skill.
3. Write the `load: always` skills in full, plus an index of the rest, into the marked Up-Skill section at the top of `AGENTS.md`.
4. Install every `load: on-demand` skill, with its `references/` folder, into `.claude/skills/upskill-{category}-{name}/`.
5. Keep project-specific instructions in `AGENTS.md` below that section, where `/skill-me-up` never changes them.
6. Create `CLAUDE.md` containing just `@AGENTS.md`, so Claude loads the same file every other harness reads.

The AI starts from `AGENTS.md` and reads the installed skills only when a task needs them.

## Design Principles

* One responsibility per skill.
* Skills are modular and reusable.
* Shared guidance lives in this repository.
* Projects contain only project-specific context.
* Only rules that apply to every task are always loaded; everything else loads when a task needs it.
* Never rely on `@` imports, and never go deeper than three levels: `AGENTS.md`, a skill, one of its reference files.

## Adding a Skill

When adding a new skill:

* Keep it focused on a single domain.
* Avoid duplicating content from other skills.
* Write clear, atomic rules.
* Use headings and bullet points rather than long prose.
* Assume the skill will be combined with others.

## Updating Skills

Because all projects consume generated instruction files, improvements made here can be propagated to every project simply by rerunning `/skill-me-up` in the project, which rebuilds the Up-Skill section of its `AGENTS.md` and reinstalls its on-demand skills.

This repository is intended to be version-controlled independently and shared across multiple repositories.

## Maintaining This Repository

The `/skill-me-up` slash command and `AGENTS.md` build step described above are for **consumer projects** — they are never run inside this repository.

This repo itself is maintained directly: when a gap, mistake, or new pattern is found (in this repo or while working on a consumer project), paste the relevant prompt/context straight into chat here and have the AI update or add the appropriate skill file by hand. There is no build/generation step for `up-skill` itself.
