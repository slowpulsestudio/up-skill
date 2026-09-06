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

**UI design (native macOS feel)**
A native Mac app is a system tool, not a website crammed into a window. Structure: draggable top bar (~50px, traffic lights integrated, keep it sparse) + optional sidebar (200-260px, only if there are 3+ nav destinations) + content area, which is the star — minimize chrome around it. Prefer a slide-out detail panel from the right over navigating to a new page, so the user keeps context.

Window chrome: 10px corner radius, traffic lights top-left (12px circles, 8px spacing, red #FF5F57 / yellow #FEBC2E / green #28C840, all gray #CDCDCD when the window is inactive).

```css
.macos-window {
  border-radius: 10px;
  box-shadow: 0 0 0 0.5px rgba(0,0,0,0.1), 0 2px 8px rgba(0,0,0,0.08), 0 8px 30px rgba(0,0,0,0.12);
}
```

Never set `easy_drag=True` on macOS if the window content has any interactive drag controls (sliders, custom drag-to-reorder, etc.) — it hijacks mouse-drag at the native view level for the whole window, not just the titlebar. Use `easy_drag=False` and rely on the `.pywebview-drag-region` class on the titlebar element for drag-to-move instead.

**A failed response looks like:**
- Cluttering the top bar (the window's draggable zone) with buttons
- Adding a sidebar when there are fewer than 3 nav destinations
- Navigating to a full new page for details instead of a slide-out panel
- Floating traffic lights that don't feel integrated into the top bar/sidebar
- Setting `easy_drag=True` on a frameless pywebview window that also contains draggable/slider controls in its content

---

**Light & dark mode**
Never invert colors between modes — dark mode needs MORE separation between background levels, not less. Design each mode's palette independently, never pure black (#000000) backgrounds in dark mode (Apple uses dark grays). Hierarchy comes from background levels, not borders; borders are 0.5px and low-opacity.

```css
:root {
  --bg-primary: #FFFFFF; --bg-secondary: #F5F5F7; --bg-tertiary: #E8E8ED;
  --text-primary: #1D1D1F; --text-secondary: #6E6E73;
  --accent: #007AFF;
  --border: rgba(0,0,0,0.08);
}
@media (prefers-color-scheme: dark) {
  :root {
    --bg-primary: #1C1C1E; --bg-secondary: #2C2C2E; --bg-tertiary: #3A3A3C;
    --text-primary: #F5F5F7; --text-secondary: #98989D;
    --accent: #0A84FF;
    --border: rgba(255,255,255,0.08);
  }
}
```

**A failed response looks like:**
- Generating dark mode by algorithmically inverting the light palette
- Pure black (#000000) dark-mode backgrounds
- Thick or dark borders instead of subtle 0.5px low-opacity ones

---

**Typography, spacing, vibrancy**
Font stack: `-apple-system, BlinkMacSystemFont, "SF Pro Display", "SF Pro Text", "Helvetica Neue", Helvetica, Arial, sans-serif`. Body text is 13px (smaller than typical web sizing); large titles 26px bold.

8px base grid: window padding 16-20px, section gap 24px, card gap 12-16px, button padding 6px 12px. Corner radii: window 10px, card 8px, button/input 6px, badge 4px.

Sidebars, toolbars, and popovers use vibrancy, never a flat color:

```css
.sidebar {
  background: rgba(246,246,246,0.72);
  backdrop-filter: saturate(180%) blur(20px);
}
```

Never blur the main content area or a modal's own background (use a solid overlay for modals instead). Shadows are layered, not a single drop-shadow, and always include a `0 0 0 0.5px` edge — that subtle edge is what reads as "macOS" instead of "web app":

```css
box-shadow: 0 0 0 0.5px rgba(0,0,0,0.06), 0 4px 16px rgba(0,0,0,0.1);
```

**A failed response looks like:**
- Body text sized like a website (15px+) instead of 13px
- A flat, non-blurred sidebar/toolbar background
- A single flat box-shadow instead of a layered shadow with the `0 0 0 0.5px` edge
- Blurring the main content area or a modal's own background

---

**Interactions**
Every primary action needs a keyboard shortcut, shown inline as a small `<kbd>`-style hint next to the action — not hidden in a menu only. Standard conventions: `⌘N` new, `⌘F` find, `⌘W` close, `⌘,` preferences, `⌘K`/`⌘Space` command palette, `Esc` dismiss.

Every state change (panel open/close, hover, drag, toast) needs a transition — an interaction with no visual feedback reads as broken:

```css
--ease-out: cubic-bezier(0.25, 0.46, 0.45, 0.94);
--duration-fast: 150ms;   /* hover */
--duration-normal: 250ms; /* panels */
```

Use optimistic UI for save/delete actions: update the UI immediately, show a toast, do the actual I/O in the background, and revert + show an error toast on failure.

Numeric inputs meant for frequent adjustment use a custom stepper, never bare native spin buttons — hide the native ones and flank the input with large (≥28px square) −/+ buttons. Native `<input type="number">` spin buttons are OS-rendered pseudo-elements ~12-14px tall, too small to reliably click and impossible to resize via CSS alone. This is a recurring bug class: any numeric field a user is expected to adjust frequently (batch size, count, quantity, intensity steps) should default to a custom large stepper control from the start, not get patched after a "the buttons are too small" bug report.

```css
input[type="number"]::-webkit-inner-spin-button,
input[type="number"]::-webkit-outer-spin-button {
  -webkit-appearance: none;
  margin: 0;
}
.stepper { display: flex; border-radius: 6px; overflow: hidden; border: 0.5px solid var(--border); }
.stepper-btn { width: 32px; font-size: 16px; font-weight: 600; border: none; cursor: pointer; }
```

**A failed response looks like:**
- A primary action with no keyboard shortcut or no visible shortcut hint
- An instant state change (no transition) on hover, panel open, or drag
- Blocking the UI on a network/disk write instead of optimistic update + background sync
- Shipping a frequently-adjusted numeric field with bare native spin buttons instead of a custom large stepper from the start
