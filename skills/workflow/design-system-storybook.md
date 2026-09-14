# Design System Storybook Rules

---

**Figma is the single source of truth for every atom**
Every component in the storybook is a direct implementation of a Figma component/atom, not an approximation. Before implementing or updating any atom, pull its exact spec via the Figma MCP tools (`get_metadata`, `get_design_context`, `get_variable_defs`) — layout, spacing, colors, typography, and every state/variant, not just the default. Never invent a value that isn't found via the MCP tools; stop and ask instead.

**A failed response looks like:**
- Eyeballing a component from a screenshot instead of pulling its real spec via Figma MCP
- Implementing only the default state/variant and skipping the others shown in Figma
- Guessing a color, spacing, or type value instead of reading it from `get_variable_defs`/`get_design_context`

---

**Reconcile with existing shared components before building a new one**
Before adding a new storybook component, check whether an equivalent already exists as a shared component elsewhere (e.g. a component library bundled by another skill). If one exists, compare it against the Figma node it's meant to represent and extend/reuse it rather than creating a divergent duplicate implementation.

**A failed response looks like:**
- Building a new component from scratch that duplicates an existing shared one with a different API or visual result
- Overwriting an existing shared component's public API without flagging it first

---

**Storybook components are presentation-only**
Every component in the gallery is a pure presentation component: it takes props/values and exposes `std::function`-style (or platform-equivalent) callbacks for interaction, with no direct coupling to business logic, persistence, or a specific host's parameter/state system. The storybook exists to preview appearance and interaction states in isolation, not to demonstrate real data wiring.

**A failed response looks like:**
- Coupling a storybook component directly to a specific app's state/parameter system "for the demo"
- Adding persistence, network calls, or other side effects inside a storybook component

---

**Gallery layout: sidebar list + real-size preview pane**
The storybook's main view is a sidebar listing every atom/component, with a preview pane that renders the selected one live at its real design size, alongside its name and description. Match the "Card" layout from the design system's Component Library section (title, description, divider, preview) if one is defined in Figma — don't invent a different gallery layout.

**A failed response looks like:**
- Rendering components at an arbitrary scale instead of their real design size
- Omitting the name/description alongside the live preview
- Inventing a gallery layout instead of matching the Figma-defined Card layout when one exists

---

**Cover every state and variant, not just the default**
For each atom, implement and make browsable every state/variant defined in Figma (e.g. default/hover/active/disabled, or per-type variants like a button's "Default" vs "Input" state) — not only the first or most obvious one.

**A failed response looks like:**
- Shipping only the default-state rendering of a component and skipping documented variants
- Discovering missing states only when the Designer points them out, instead of checking every variant/state in Figma up front
