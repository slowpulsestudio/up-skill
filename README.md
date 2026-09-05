# AI Skills

This repository contains a reusable library of AI instruction files ("skills") that can be shared across multiple software projects.

## Purpose

The skills in this repository are the **source of truth** for AI coding guidance. They are designed to be maintained once and reused everywhere.

Projects should **not** point AI agents directly at individual skill files. Instead, a build step should combine the required skills into a single `master-skills.md` file that becomes the project's authoritative instruction document.

This approach avoids relying on AI agents to resolve nested imports or follow multiple levels of references, which can produce inconsistent results.

## Repository Structure

```text
skills/
    workflow/
        general.md          ← always included in every project
        architecture.md
        testing.md
        git.md
        deep-linking.md
        prototyping.md
        figma-read-from-mcp.md
        figma-write-to-canvas.md
        vercel-publish.md
        vercel-password.md
        image-generation.md
        web-scraper.md
    platform/
        ios.md
        chrome-extension.md
        python-mac.md
        python-website.md
        python-cli.md
```

Each file should cover **one topic only** and remain reusable across projects.

## Setup Steps

### 1. Fetch the initiation prompt

Open your new project's folder in VS Code (an empty folder is fine) and paste this into the Copilot chat panel:

```
Fetch https://raw.githubusercontent.com/slowpulsestudio/up-skill/main/.github/prompts/skill-me-up.prompt.md and save it to .github/prompts/skill-me-up.prompt.md in this project.
```

This always pulls the latest version straight from GitHub — no need to clone or copy this repo.

### 2. Run "Skill me up"

In the same Copilot chat panel, run:

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
  ```

- **Workflow — multi-select checkboxes, `general`/`architecture`/`git` pre-checked as recommended defaults. Add or remove any others that apply:**

  ```
  workflow/general
  workflow/architecture
  workflow/git
  workflow/testing
  workflow/figma-read-from-mcp
  workflow/figma-write-to-canvas
  workflow/vercel-publish
  workflow/vercel-password
  workflow/image-generation
  workflow/web-scraper
  ```

See [how-to-setup-projects.md](how-to-setup-projects.md) for the full walkthrough and a skill-by-skill reference table.

## Usage

For each project:

1. From `skills/workflow/`, pick via a multi-select checklist — `general.md`, `architecture.md`, and `git.md` are pre-checked as recommended defaults. Add or remove any others that apply.
2. From `skills/platform/`, pick **exactly one** via a single-select question to match the project's platform (e.g. `ios.md` for an iOS app, `python-mac.md` for a Mac desktop app). Never include more than one platform skill.
3. Concatenate them, in order, into a single `master-skills.md`.
4. Append any project-specific instructions.
5. Point both `CLAUDE.md` and `.github/copilot-instructions.md` at the generated `master-skills.md`.

The AI should consume only the generated file.

## Design Principles

* One responsibility per skill.
* Skills are modular and reusable.
* Shared guidance lives in this repository.
* Projects contain only project-specific context.
* AI agents receive a single, flattened instruction file.
* Never rely on recursive imports or multi-level instruction chains.

## Adding a Skill

When adding a new skill:

* Keep it focused on a single domain.
* Avoid duplicating content from other skills.
* Write clear, atomic rules.
* Use headings and bullet points rather than long prose.
* Assume the skill will be combined with others.

## Updating Skills

Because all projects consume generated instruction files, improvements made here can be propagated to every project simply by rebuilding the project's `master-skills.md`.

This repository is intended to be version-controlled independently and shared across multiple repositories.

## Maintaining This Repository

The `/skill-me-up` prompt and `master-skills.md` build step described above are for **consumer projects** — they are never run inside this repository.

This repo itself is maintained directly: when a gap, mistake, or new pattern is found (in this repo or while working on a consumer project), paste the relevant prompt/context straight into chat here and have the AI update or add the appropriate skill file by hand. There is no build/generation step for `up-skill` itself.
