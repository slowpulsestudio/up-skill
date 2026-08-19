# Refinements To Do

---

## 1. Structural / Organizational

### 1.1 — README skill list is outdated
The README tree shows `iris-ui.md` and `iris-react.md` under `skills/project-type/`, but neither file exists. The actual directory is `skills/platform/`, not `skills/project-type/`. Three workflow skills (`deep-linking.md`, `prototyping.md`) exist on disk but are absent from the README entirely.

### 1.2 — `how-to-setup-projects.md` skill list is stale
Lists `workflow/general` through `workflow/image-generation` but omits `workflow/deep-linking` and `workflow/prototyping`. The setup flow and `/skill-me-up` prompt both have no awareness of these two skills — a user can't select them through the standard setup path.

### 1.3 — `/skill-me-up` prompt doesn't offer `deep-linking` or `prototyping`
`skill-me-up.prompt.md` Step 0 hardcodes the list of selectable workflow skills. `workflow/deep-linking` and `workflow/prototyping` are missing from that list.

### 1.4 — No `iris-react` platform skill exists
The `skill-resources/platform/iris-react/` folder and the `migrate-to-iris-react.prompt.md` prompt both exist, but there's no corresponding `skills/platform/iris-react.md` skill file. The README mentions one, but it was never created. `/skill-me-up` can't select it as a platform, and the migrate prompt references `master-skills.md` which won't contain Iris-specific rules.

### 1.5 — `angular.json` says `packageManager: "npm"`, not `pnpm`
The iris-ui-main `angular.json` sets `"cli": { "packageManager": "npm" }` while the project uses pnpm exclusively. This can cause Angular CLI to default to npm for schematics/installs if someone runs `ng add` or `ng generate`.

---

## 2. Content Gaps

### 2.1 — No web/React/Next.js platform skill
Every platform skill is either Python, iOS, or Chrome Extension. There's no platform skill for vanilla web/React/Next.js/Vite projects — which is what the iris-shell PoC actually is.

### 2.2 — `workflow/general` is designer-centric, not universally applicable
The General skill assumes the user is always "the Designer" — a Senior Product Designer with limited coding experience. The "About the Designer" section, 3-step-max rule, and "Mac-only" constraint are baked in as universal truths. This is awkward in a developer-to-developer context.

### 2.3 — No skill for the Figma Implementation Agent workflow
The iris-ui-main `Agent/INSTRUCTION.md` is a 650+ line guide for an automated Figma-to-Angular pipeline, but it lives only inside the bundled resource. There's no workflow skill that teaches the AI how to use or invoke this agent.

### 2.4 — `vercel-password.md` stores the password as a plain-text cookie value
The cookie value is literally `Set-Cookie: site_auth=${password}`. Anyone reading the cookie in DevTools sees the raw password. A one-way hash (even SHA-256) would cost nothing and avoid this.

---

## 3. Consistency / DRY

### 3.1 — Duplicated content across skills
Several rules appear nearly verbatim in multiple files:
- **Secrets handling** (`.env` / `.env.example` / `python-dotenv`): repeated in `python-cli.md`, `python-website.md`, `python-mac.md`, `web-scraper.md`, `image-generation.md`, `general.md`, `git.md` — 7 files
- **Git commit rules**: repeated in `python-mac.md`, `python-website.md`, `git.md`
- **Cost control / caching**: repeated in `python-cli.md`, `image-generation.md`
- **Provider abstraction**: repeated in `python-cli.md`, `image-generation.md`, `architecture.md`

When concatenated into `master-skills.md`, the same rules appear 3–7 times, wasting context window tokens and risking inconsistency if one copy is updated and others aren't.

### 3.2 — `python-mac.md` git section partially conflicts with `git.md`
`python-mac.md` says commit messages should be "short and imperative" (one line). `git.md` says "multi-line: summary + bullet list of what/why". If both are included, they contradict.

---

## 4. `/skill-me-up` Prompt

### 4.1 — No project name question
`how-to-setup-projects.md` says Step 3 starts with "What is the project/app name", but the actual `skill-me-up.prompt.md` never asks for a project name and never uses one.

### 4.2 — No follow-up for `workflow/figma-write-to-canvas`
The write-to-canvas skill depends on the Figma file URL too, but only `workflow/figma-read-from-mcp` triggers the Figma URL follow-up question.

### 4.3 — Zip download is fragile for large resource folders
Step 2 downloads the entire repo as a zip to copy bundled resources. For iris-ui-main (a full Angular monorepo), this pulls a large archive. The GitHub API tree/blob endpoints would be more surgical.

### 4.4 — Self-update check doesn't handle private repos
Step -1 fetches from `raw.githubusercontent.com` with no auth header. If the repo is or becomes private, this silently 404s with no helpful error.

---

## 5. Iris-UI Resource

### 5.1 — Agent `entrypoint.sh` hardcodes model name
Hardcodes `--model claude-opus-4.6`. Will break when the model is renamed/deprecated — the very issue warned about in `image-generation.md` and `python-website.md`.

### 5.2 — `entrypoint.sh` doesn't validate secrets before use
Reads secrets from CSI-mounted files and exports as env vars, but doesn't check that critical secrets (e.g. `github-app-private-key`) are non-empty before attempting to use them. A missing secret produces a cryptic JWT/API error instead of a clear early failure.

### 5.3 — iris-shell PoC has `strict: false` in tsconfig
Explicitly sets `"strict": false` with no `noImplicitAny`. For a project that demonstrates production patterns, this undermines type safety and contradicts the "production standards" rule in `general.md`.

### 5.4 — iris-shell PoC has no tests or lint
No test framework configured, no lint config. The README's architecture notes flag this as a known issue. For a bundled reference project, this sets a bad example.

### 5.5 — iris-shell 730KB icon manifest bundled at runtime
The README flags this ("A5 warning; needs build-time manifest generation") but it's shipped as-is. Every downstream project that copies this resource inherits the bloat.

---

## 6. Documentation

### 6.1 — `how-to-create-skills.md` is very thin
Only covers the `## Resources` bundling mechanism. Says nothing about the skill authoring conventions every existing skill follows (the `# Title` → `---` → bolded section → "A failed response looks like:" pattern). A new contributor would have to reverse-engineer the format from examples.

### 6.2 — No CHANGELOG or versioning
No changelog, no version tags, no way for downstream projects to know what changed when they re-run `/skill-me-up`. The self-update check is binary (same/different) with no "here's what's new" context.

### 6.3 — `how-to-setup-projects.md` Step 1 could be clearer
Says "Copy the entire `.github/` folder from this repo into your project." The `.github/` folder contains prompts that auto-update via Step -1. The instruction is correct but could note that the prompts are system-managed.

---

## 7. Minor / Low-Priority

### 7.1 — Figma MCP "remote server" block is duplicated
Identical sections (remote-server rule, connection-verification rule) appear in both `figma-read-from-mcp.md` and `figma-write-to-canvas.md`.

### 7.2 — `python-mac.md` says "commit and push" without confirmation
Says "After a successful, verified build, commit and push the change." This contradicts `git.md`'s rule that pushing requires explicit confirmation.

### 7.3 — `deep-linking.md` and `prototyping.md` use a different format
These use a "Success looks like / Failure looks like" code-example pattern instead of the "A failed response looks like:" bullet list pattern used by every other skill.

### 7.4 — No `.editorconfig` or formatting config for the repo itself
The skills tell downstream projects to use strict formatting, but the skill files themselves have no enforced formatting.
