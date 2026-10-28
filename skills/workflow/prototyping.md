---
load: on-demand
description: Rules for UX research prototypes. Freeze the prototype on a dated branch before testing, pin component-library imports and leave prototype branches untouched. Read before preparing a prototype for a research session, or changing a prototype branch or its dependencies.
---

# UX Research Prototyping Rules

Rules for managing prototypes that will be used in UX research sessions. The goal is a stable, unchanging snapshot, not a live development branch.

---

**Freeze the prototype on a dated branch before testing starts**
When a prototype is chosen for a research session, create a branch from the current clean, known-good state before the first session, named with the date and purpose (e.g. `prototype/research-2024-08-13`), and push it to the remote straight away. From then on the prototype must not change: not from ongoing development, not from component-library updates, not from dependency bumps. Main development carries on separately on `main` or `dev`.

**A failed response looks like:**
- Using `main` directly as the prototype, so it changes underneath the sessions
- Creating the branch after testing has already started, when the state is no longer clean
- Committing to the prototype branch, or merging `main` into it, after testing starts
- Leaving the branch unpushed, so it only exists on one machine

---

**Pin every component-library import to an exact version or commit**
In a prototype branch, pin each component-library dependency to an exact version (`"@org/components": "1.4.2"`) or, for a git dependency, an exact commit hash (`"github:org/ui-library#a3f9c12"`). Never let the package manager resolve a range, a tag or "latest" during a research period: a silent component update can change behaviour between sessions.

**A failed response looks like:**
- A caret (`^1.4.0`) or tilde (`~1.4.0`) range on a component-library dependency
- A git dependency pointing at a branch name such as `#main`, whose tip moves
- Pinning to a tag instead of a commit hash

---

**Prototype branches are kept for reference, untouched**
A frozen prototype branch exists so anyone can later replay exactly what participants saw. Record what was being tested and with whom in a short note on the branch (its description or a pinned commit message). After the sessions end, the branch is never rebased, force-pushed, amended or deleted.

**A failed response looks like:**
- Deleting the branch after the research session ends
- Rebasing, amending or force-pushing commits on a prototype branch
- A prototype branch with no note of what was tested or with whom
