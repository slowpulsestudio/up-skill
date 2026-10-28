---
load: on-demand
description: Rules for this Python Mac desktop app. Clean PyInstaller build and launch check after every change, venv, secrets, API identifiers and git. Read before writing or changing any code or dependency. It points to a reference file for the native macOS UI rules.
---

# Python Mac App Rules

---

**Building**
After every code change, do a clean build before finishing. The `dist/` folder can have read-only files from a previous build — always `chmod` before deleting it.

```zsh
chmod -R u+w dist 2>/dev/null; rm -rf dist
source .venv/bin/activate
python -m PyInstaller -y "<AppName>.spec" 2>&1 | tail -10
```

**A failed response looks like:**
- Finishing a code change without rebuilding first
- Running `rm -rf dist` without the `chmod` step first — this fails silently when dist contains read-only files
- Running `pyinstaller` globally instead of from inside the activated venv

---

**Verifying the build**
After every successful build, launch the binary directly to catch import errors before the user tries it:

```zsh
"dist/<AppName>.app/Contents/MacOS/<AppName>" 2>&1 &
sleep 5 && kill %1 2>/dev/null
```

A `ModuleNotFoundError` means a dependency wasn't installed in the venv before building — install it and do a clean rebuild.

**A failed response looks like:**
- Declaring a build successful without launching the binary to check for import errors
- Telling the user to test the app before verifying it launches cleanly

---

**Venv and dependencies**
The venv lives at `.venv/` in the project root, using Homebrew-installed Python, not the macOS system default. `pyinstaller` must be installed inside the venv, not globally. Pin all dependencies in `requirements.txt`. After installing any new package, always do a clean rebuild (delete both `dist/` and `build/`) so PyInstaller re-analyses imports from scratch.

```zsh
source .venv/bin/activate
pip install -r requirements.txt
```

**A failed response looks like:**
- Installing `pyinstaller` (or any package) globally instead of into the venv
- Using the system Python instead of a Homebrew-managed version
- Skipping the clean rebuild after installing a new package — PyInstaller caches import analysis and will miss new dependencies

---

**Secrets**
Load secrets via `python-dotenv` from a `.env` file. `.env` is gitignored; `.env.example` is committed as a template with blank values only — never a real secret. Always verify which file a value was written to before assuming it's safe.

**A failed response looks like:**
- Hardcoding an API key or secret directly in Python source
- Committing a real secret value in `.env.example`

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

---

**Git**
Commit only changed source files — `dist/` and `build/` are gitignored and must never be committed. After a successful, verified build, commit and push the change.

Commit messages must be short and imperative, e.g. `Fix YouTube: use android client to avoid SABR streaming`.

**A failed response looks like:**
- Committing without a prior successful build and binary verification
- Including `dist/` or `build/` files in a commit
- Writing a vague or past-tense commit message

---

**UI rules live in a reference file**
Before writing or changing any UI (HTML, CSS, window chrome, layout, colours, type, motion, keyboard shortcuts or input controls), read `references/ui-design.md` in this skill's folder in full and follow it. It covers native macOS window structure, light and dark palettes, typography, spacing and vibrancy, the type scale, motion and colour tokens, and interactions.

**A failed response looks like:**
- Writing or restyling UI without reading `references/ui-design.md` first
- Reading only the part of `references/ui-design.md` that seems relevant to the change
