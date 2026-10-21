# How to set up a new project with Up-Skill

---

## What this does

Up-Skill is a library of AI instruction files. When you start a new project, you pick the ones that apply, combine them into one file, and point your AI assistant at it. From that point on the AI knows your rules.

---

## Step 1 — Fetch `/skill-me-up`

Open your new project's folder in VS Code (an empty folder is fine) and paste this into the chat panel (Copilot or Claude):

```
Run `mkdir -p .claude/skills/skill-me-up && curl -sL https://raw.githubusercontent.com/slowpulsestudio/up-skill/main/.claude/skills/skill-me-up/SKILL.md -o .claude/skills/skill-me-up/SKILL.md` in this project. Use curl, not a fetch tool — a fetch tool returns a summary, not the exact file.
```

This always pulls the latest version straight from GitHub — no need to clone or copy this repo.

---

## Step 2 — Run "Skill me up"

Start a new chat, so it picks up the new slash command, and type:

```
/skill-me-up
```

This always pulls the latest versions from GitHub, so any improvements made to Up-Skill will be included.

---

## Step 3 — Question time

The AI will ask you these as real interactive questions (buttons/checkboxes), not plain text:
1. What is the project/app name
2. Which platform this project uses (single-select)
3. Which workflow skills apply (multi-select checkboxes; `general`, `architecture`, `git`, `testing` and `docs-style` are pre-checked as recommended defaults)
4. Follow-up questions for specific skills:
   - **`workflow/git`** — GitHub repo URL
   - **`workflow/figma-read-from-mcp`** — Figma file URL
   - **`workflow/image-generation`** — which image generation provider
   - **`workflow/generative-video`** — Figma link to the reference images, saved to `.figma-references-url`

**Platform — pick one:**

```
platform/ios
platform/chrome-extension
platform/python-mac
platform/python-website
platform/python-cli
platform/juce-vst3-plugin
```

**Workflow: multi-select checkboxes, `general`/`architecture`/`git`/`testing`/`docs-style` pre-checked. Add or remove any others that apply:**

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

| Skill | When to include |
|---|---|
| `platform/ios` | Swift iOS app |
| `platform/chrome-extension` | Chrome browser extension |
| `platform/python-mac` | Python desktop app for Mac |
| `platform/python-website` | Python web app (FastAPI etc.) |
| `platform/python-cli` | Python local script/CLI tool |
| `platform/juce-vst3-plugin` | Building a VST3 audio plugin (JUCE/C++) |
| `workflow/general` | Core execution rules — pre-checked by default |
| `workflow/architecture` | General code structure rules — pre-checked by default |
| `workflow/git` | Source control rules — pre-checked by default |
| `workflow/testing` | Testing standards — pre-checked by default |
| `workflow/docs-style` | Writing style rules for project docs; pre-checked by default |
| `workflow/figma-read-from-mcp` | If the project uses Figma for design (design → code) |
| `workflow/figma-write-to-canvas` | If the project uses Figma write-to-canvas / code → canvas skills |
| `workflow/vercel-publish` | If the project deploys to Vercel |
| `workflow/vercel-password` | Password gate for Vercel preview deployments |
| `workflow/image-generation` | If the project calls an AI image-generation API |
| `workflow/web-scraper` | If the project scrapes data from websites |
| `workflow/deep-linking` | If the project has tree navigation where every node must be linkable by URL |
| `workflow/prototyping` | If the project builds prototypes for UX research sessions |
| `workflow/dsp-prototyping` | If the project involves tuning audio/DSP algorithms before a real-time port |
| `workflow/generative-video` | If the project renders procedural or generative animation to video files |
| `workflow/design-system-gallery` | If the project is a component gallery for a shared design system |
| `workflow/juce-ui-rendering` | If the project implements Figma artwork as JUCE painting code (building the design system library itself, not consuming it) |

---

## Done

The agent will now:
1. Fetch the latest version of each skill and combine them into `master-skills.md`
2. Copy any bundled files into the project (e.g. a UI component library)
3. Create `CLAUDE.md`, which both Claude and Copilot read automatically

That's it. The AI will have created `master-skills.md` and `CLAUDE.md` automatically.

---

## Updating later

Whenever Up-Skill is updated with new or improved skills, just run `/skill-me-up` again. It will re-fetch everything and overwrite `master-skills.md` with the latest version. Bundled files that already exist in your project will not be overwritten — you'll be warned about any conflicts so you can resolve them manually.

Some skills bundle their own extra slash commands alongside their other resource files — for example, `platform/juce-vst3-plugin` bundles `/build-vst3`, `/adopt-design-system` and `/system-my-design`. These are only installed in projects that selected that skill, the same way as any other bundled file.

---

## Upgrading a project set up before October 2026

Older projects keep their slash commands in `.github/prompts/`, which the Copilot harness ignores. Open the project in VS Code and paste this into the chat panel (Copilot or Claude), once:

```
Run `curl -sL https://raw.githubusercontent.com/slowpulsestudio/up-skill/main/migrate-to-skills.md` and follow the instructions it prints in this project. Use curl, not a fetch tool — a fetch tool returns a summary, not the exact file.
```

It removes the old prompt files, fixes how the rules load, installs the new `/skill-me-up` and rebuilds the skills. Commit or stash any changes first. After that, run `/skill-me-up` as normal.

---

For skill authors, see [how-to-create-skills.md](how-to-create-skills.md).
