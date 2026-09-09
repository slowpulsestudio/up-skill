# How to set up a new project with Up-Skill

---

## What this does

Up-Skill is a library of AI instruction files. When you start a new project, you pick the ones that apply, combine them into one file, and point your AI assistant at it. From that point on the AI knows your rules.

---

## Step 1 — Fetch the initiation prompt

Open your new project's folder in VS Code (an empty folder is fine) and paste this into the Copilot chat panel:

```
Fetch https://raw.githubusercontent.com/slowpulsestudio/up-skill/main/.github/prompts/skill-me-up.prompt.md and save it to .github/prompts/skill-me-up.prompt.md in this project.
```

This always pulls the latest version straight from GitHub — no need to clone or copy this repo.

---

## Step 2 — Run "Skill me up"

Open the project in VS Code. In the Copilot chat panel, type:

```
/skill-me-up
```

This always pulls the latest versions from GitHub, so any improvements made to Up-Skill will be included.

---

## Step 3 — Question time

The AI will ask you these as real interactive questions (buttons/checkboxes), not plain text:
1. What is the project/app name
2. Which platform this project uses (single-select)
3. Which workflow skills apply (multi-select checkboxes — `general`, `architecture`, and `git` are pre-checked as recommended defaults)
4. Follow-up questions for specific skills:
   - **`workflow/git`** — GitHub repo URL
   - **`workflow/figma-read-from-mcp`** — Figma file URL
   - **`workflow/image-generation`** — which image generation provider

**Platform — pick one:**

```
platform/ios
platform/chrome-extension
platform/python-mac
platform/python-website
platform/python-cli
platform/juce-vst3-plugin
```

**Workflow — multi-select checkboxes, `general`/`architecture`/`git` pre-checked. Add or remove any others that apply:**

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
workflow/dsp-prototyping
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
| `workflow/testing` | Testing standards |
| `workflow/figma-read-from-mcp` | If the project uses Figma for design (design → code) |
| `workflow/figma-write-to-canvas` | If the project uses Figma write-to-canvas / code → canvas skills |
| `workflow/vercel-publish` | If the project deploys to Vercel |
| `workflow/vercel-password` | Password gate for Vercel preview deployments |
| `workflow/image-generation` | If the project calls an AI image-generation API |
| `workflow/web-scraper` | If the project scrapes data from websites |
| `workflow/dsp-prototyping` | If the project involves tuning audio/DSP algorithms before a real-time port |

---

## Done

The agent will now:
1. Fetch the latest version of each skill and combine them into `master-skills.md`
2. Copy any bundled files into the project (e.g. a UI component library)
3. Create `.github/copilot-instructions.md` and `CLAUDE.md`

That's it. The AI will have created `master-skills.md`, `.github/copilot-instructions.md`, and `CLAUDE.md` automatically.

---

## Updating later

Whenever Up-Skill is updated with new or improved skills, just run `/skill-me-up` again in Copilot chat. It will re-fetch everything and overwrite `master-skills.md` with the latest version. Bundled files that already exist in your project will not be overwritten — you'll be warned about any conflicts so you can resolve them manually.

`/skill-me-up` also keeps a second slash command, `/system-my-design`, installed and current automatically. Run `/system-my-design` any time you want to check whether the bundled resource files it left behind conflict-skipped (e.g. a shared UI component) have since been improved in Up-Skill — it shows a diff for each changed file and asks before overwriting anything, instead of the silent skip `/skill-me-up` does.

---

For skill authors, see [how-to-create-skills.md](how-to-create-skills.md).
