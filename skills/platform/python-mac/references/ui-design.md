# Python Mac App UI Design Rules

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
  --accent: #007AFF; --accent-active: #0062CC;
  --danger: #FF5F57;
  --border: rgba(0,0,0,0.08);
  --titlebar-bg: rgba(246,246,246,0.72);
}
@media (prefers-color-scheme: dark) {
  :root {
    --bg-primary: #1C1C1E; --bg-secondary: #2C2C2E; --bg-tertiary: #3A3A3C;
    --text-primary: #F5F5F7; --text-secondary: #98989D;
    --accent: #0A84FF; --accent-active: #409CFF;
    --danger: #FF5F57;
    --border: rgba(255,255,255,0.08);
    --titlebar-bg: rgba(44,44,46,0.72);
  }
}
```

- `--accent-active`: the pressed/active state of accent buttons.
- `--danger`: the destructive/close colour (same red as the close traffic light).
- `--titlebar-bg`: the translucent title bar fill, used with the vibrancy blur.

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

**Type scale**
Use only these font sizes:
- 11px: small labels, hints, secondary metadata
- 12px: segmented/tab buttons
- 13px: body text, buttons, inputs, title bar title (title bar title is 600 weight, `--text-secondary` colour)
- 16px: stepper +/- glyphs
- 24px: large status icons (e.g. a success checkmark circle)
- 26px bold: large titles

**A failed response looks like:**
- Using a font size outside this scale

---

**Motion tokens must be defined in :root, not only referenced.**
Every stylesheet must declare the motion tokens, not just use them:

```css
:root {
  --ease-out: cubic-bezier(0.25, 0.46, 0.45, 0.94);
  --duration-fast: 150ms;
  --duration-normal: 250ms;
}
```

All transitions use these tokens instead of literal values like `150ms ease-out`.

**A failed response looks like:**
- Referencing var(--ease-out) or var(--duration-*) with fallbacks while never defining the tokens
- Hardcoding transition durations or easing instead of using the tokens

---

**Apply the window shadow.**
The `.macos-window` shadow must be applied to the app's root window element, not just documented.

**A failed response looks like:**
- Defining the window shadow in the spec but not applying it to the window

---

**No hardcoded colours outside :root.**
Every colour in component styles must reference a token. Exceptions are the traffic-light colours (#FEBC2E, #28C840, inactive #CDCDCD). If a component needs a colour with no token, add the token to both light and dark blocks first; for example, white text on accent backgrounds becomes a `--text-on-accent: #FFFFFF` token, and a success green becomes a `--success` token.

**A failed response looks like:**
- Writing a raw hex or rgba value in a component rule instead of a token

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
