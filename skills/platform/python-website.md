# Python Website Rules

---

**Environment setup**
Use a project-local virtualenv at `.venv/` — never install packages globally. Pin all dependencies in `requirements.txt`. Use Homebrew-installed Python, not the macOS system default (it's often outdated).

```zsh
source .venv/bin/activate
pip install -r requirements.txt
```

**A failed response looks like:**
- Installing packages globally instead of into `.venv/`
- Using the system Python instead of a Homebrew-managed version
- Adding a new dependency without pinning it in `requirements.txt`

---

**Secrets**
Load secrets via `python-dotenv` from a `.env` file. `.env` is gitignored; `.env.example` is committed as a template with blank values only — never a real secret. Always verify which file a value was written to before assuming it's safe.

**A failed response looks like:**
- Hardcoding an API key or secret directly in Python source
- Committing a real secret value in `.env.example`

---

**Stack choice**
Prefer lightweight stacks for small/internal tools: SQLite over a client-server database, FastAPI + server-rendered templates + vanilla JS/CSS over a full frontend framework.

**A failed response looks like:**
- Reaching for a heavier stack (PostgreSQL, React, etc.) when SQLite and server-rendered templates are sufficient for the project's scale

---

**Development workflow**
Verify with real end-to-end runs against live data before declaring something done — "no errors found" and mocked tests are not enough. Smoke-test each component individually first, then test the full pipeline together.

When adding a new feature, extend existing files rather than rewriting from scratch or creating parallel implementations.

Give automated pipelines a manual escape hatch — a way to add or override an item without going through automation.

Favour native platform capabilities over adding a dependency for something simple (e.g. native HTML5 drag-and-drop instead of a JS library; native `fetch`/`beacon` instead of a wrapper).

**A failed response looks like:**
- Declaring a feature done after mocked tests pass without a real end-to-end run
- Creating a new parallel implementation instead of extending the existing one
- Building automation before the manual workflow is proven and the user has asked for it
- Adding a library dependency for something the browser or standard library already handles natively

---

**Local dev server behaviour**
Disable caching on static assets during active local development — a stale cached asset is harder to diagnose than a slightly slower reload. Surface failures loudly (log to console on failed background fetches/saves) — silent failures are hard to catch against a live-reloading dev server.

**A failed response looks like:**
- Leaving asset caching enabled during local development
- Catching an error silently without logging it during development

---

**UI hygiene**
When a view accumulates too many controls as features grow, split into dedicated tabs/views rather than cramming everything onto one screen. Support bulk actions (multi-select + bulk apply) once a list-based UI has enough items that one-at-a-time actions become tedious.

---

**Async state**
Watch for async state bugs in UI polling and reload logic — reload loops are a common failure mode. Guard against them with a per-run token or session-scoped flag.

**A failed response looks like:**
- Implementing a polling/reload loop without a guard against re-entrant or overlapping runs

---

**Filtering and scoring pipelines**
Prefer a staged approach over one monolithic pass/fail rule: a cheap fast first-pass filter, then a more expensive second-stage scorer, with sensible auto-thresholds rather than a single all-or-nothing check.

---

**Git**
Commit in small focused increments per feature or fix. Write multi-line commit messages: a short summary line, then a bullet list explaining what changed and why (the reasoning and trade-off, not just a restatement of the diff).

**A failed response looks like:**
- Batching multiple unrelated changes into one commit
- Writing a commit message that only restates the diff without explaining the reasoning

---

**Dict/kwargs merging**
`dict.setdefault()` does NOT merge with caller-supplied values — it only fills in a key if it's absent, so a caller-supplied `headers`/`kwargs` dict silently replaces (rather than merges with) the defaults. Merge explicitly instead: `{**DEFAULTS, **overrides}`.

**A failed response looks like:**
- Using `kwargs.setdefault("headers", DEFAULT_HEADERS)` when a caller might also pass `headers` — this silently drops the defaults
- Assuming a dict-valued default and a per-call override combine automatically

---

**Third-party API/model identifiers**
Before hardcoding a model name or API version string, verify it's currently valid (e.g. a `list models` call) rather than trusting a name that may have been renamed or deprecated.

**A failed response looks like:**
- Hardcoding an API model name without verifying it's still a valid identifier
