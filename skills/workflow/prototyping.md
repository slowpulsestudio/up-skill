# UX Research Prototyping

Rules for managing prototypes that will be used in UX research sessions. The goal is a stable, unchanging snapshot — not a live development branch.

---

## Frozen branches, not live imports

When a prototype is selected for a research session, stamp the current state as a named branch and lock it. The prototype must not change after testing begins — not from ongoing development, not from component-library updates, not from dependency bumps.

**Success looks like:**
```bash
# Stamp a frozen prototype branch from the current state
git checkout -b prototype/research-2024-08-13
git push origin prototype/research-2024-08-13

# Pin the component-library import to an exact commit hash in package.json
# "ui-library": "github:org/ui-library#a3f9c12"
# Not a tag, not a range — a commit hash that cannot move
```

**Failure looks like:**
```bash
# BAD: using a semver range — will silently update on next install
# "ui-library": "^1.4.0"

# BAD: using a branch name — the branch tip moves
# "ui-library": "github:org/ui-library#main"

# BAD: continuing to commit to the prototype branch after testing starts
git checkout prototype/research-2024-08-13
git merge main  # pulls in changes that weren't tested
```

---

## No auto-refresh of component imports

Component-library imports in a prototype branch must be pinned to an exact version or commit. Do not rely on a package manager resolving "latest" or a semver range during a research period — a silent component update can change behaviour between sessions.

**Success looks like:**
```json
// package.json — exact version, not a range
{
  "dependencies": {
    "@org/components": "1.4.2"
  }
}
```

**Failure looks like:**
```json
// BAD: caret allows any patch/minor update
{ "@org/components": "^1.4.0" }

// BAD: tilde allows patch updates
{ "@org/components": "~1.4.0" }
```

---

## Prototype branches are reference-only

A frozen prototype branch exists for future reference — to replay exactly what participants saw. It is never rebased, force-pushed, or deleted after the research session ends.

**Success looks like:**
- Branch name includes date and purpose: `prototype/research-2024-08-13`
- Branch is pushed to remote immediately on creation
- A short note is added to the branch description (or a pinned commit message) recording what was being tested and with whom
- Main development continues on `main` or `dev` — the prototype branch is never touched again

**Failure looks like:**
- Deleting the branch after the research session ends
- Rebasing or amending commits on a prototype branch
- Using `main` directly as the prototype — it will change underneath you
- Creating the branch after testing has already started (state is no longer clean)

---

## Summary checklist

- [ ] Branch created from a clean, known-good state **before** the session starts
- [ ] Branch name includes date: `prototype/research-YYYY-MM-DD`
- [ ] All component-library imports pinned to exact versions or commit hashes
- [ ] Branch pushed to remote immediately
- [ ] No commits to the branch after the session begins
- [ ] Main development branch continues independently
