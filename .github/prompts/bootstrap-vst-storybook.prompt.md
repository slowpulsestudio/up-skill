Copy the contents below into the first chat message of a new, separate GitHub
repo for the SPS VST design system storybook app. This is not a slash command
run from within `up-skill` — it's a reference doc for that bootstrap message,
kept here so it isn't lost.

---

I'm setting up a new JUCE standalone app that acts as a visual "storybook" for the
SPS VST design system — a component gallery for browsing every shared `sps::`
UI atom/component at real size, outside of any actual plugin.

**Source of truth for design values:** Figma file
https://www.figma.com/design/O5nlVlusBTsRBa3eyGwESp/SPS-VST-Design-System
Read `skills/workflow/figma-read-from-mcp.md` rules from the up-skill repo (or run
`/skill-me-up` first to pull that skill in) before touching Figma — all colors,
spacing, radii, fonts, and component structure must come from the Figma MCP tools
(`get_metadata`, `get_design_context`, `get_variable_defs`), never guessed.

**Atoms to build** (from the file's "Atoms" section), each as a header-only
`sps::` JUCE component:
- Rotary Knob
- Vertical Fader
- Horizontal Slider
- Text Input Field
- Dropdown
- Checkbox
- Button (states: Default, Input)
- Transport Controls (Bypass/Mute/Sync toggle group)
- Action Button Row
- Window Title Bar (Win98-style with controls)
- Graph Display (recessed graph/waveform panel)
- Preset Navigation Bar (browser with arrows, name, save)
- Oscilloscope
- Quartz Numbers (LCD-style digit display)

**Important:** `sps::PresetToolbar` already exists in the `up-skill` repo at
`skill-resources/platform/juce-vst3-plugin/components/PresetToolbar.h` and looks
like it corresponds to the Figma "Preset Navigation Bar" atom. Before building a
new preset component here, compare it against that Figma node and reconcile —
extend/reuse it rather than creating a divergent duplicate. Do NOT edit anything
in the `up-skill` repo directly from this session; if a change belongs there,
tell me and I'll confirm before doing it separately.

**What to build in this repo:**
1. A standard JUCE (CMake + FetchContent) standalone app target — no VST3/plugin
   wrapper needed, just a runnable app.
2. Run `/skill-me-up` against `up-skill` to bootstrap this project's skills/AI
   instructions and pull in any existing `sps::` components (e.g. `PresetToolbar`).
3. For each atom/component above: pull its exact spec from Figma MCP (layout,
   spacing, colors, states, typography — check every state/variant, not just one),
   then implement it as `sps::<Name>` under `Source/Components/`, matching Figma
   exactly.
4. Build a `StorybookMainComponent` with a sidebar list of all atoms/components;
   selecting one shows it live in a preview pane at its real design size, with its
   name/description shown alongside (matching the "Card" layout in the Figma
   Component Library section: title, description, divider, preview).
5. Every component should follow the existing `sps::` conventions already
   established in `up-skill` (namespace `sps`, `std::function` callbacks for
   decoupling, no direct parameter/APVTS coupling — these are pure presentation
   components here).

Ask me before assuming any value not found via the Figma MCP tools, and before
overwriting `PresetToolbar`'s existing API.
