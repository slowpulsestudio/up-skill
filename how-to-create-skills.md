# How to create a skill

---

## Skill file format

Every skill is one markdown file in `skills/workflow/` or `skills/platform/`, starting with this frontmatter:

```
---
load: on-demand
description: What the skill covers. Read before {the tasks it applies to}.
---
```

- `load`: `always` puts the whole skill into every project's `AGENTS.md`, so it is in context on every turn. Keep this for rules that apply to every task (currently `workflow/general` and `workflow/architecture`). Everything else is `on-demand`.
- `description`: for an on-demand skill this is the only part the agent sees until it decides to read the skill, so it must say when to read it, not just what is in it. Name the actions that trigger it ("Read before any git commit, push, merge..."). If one rule is too important to wait for the agent to open the file, state it in the description too, as `workflow/git` does for pushing. Keep it to one line, avoid a colon followed by a space (it breaks the YAML) and don't start it with a backtick.

## The three levels

1. **Always loaded:** `AGENTS.md` holds every `load: always` skill in full, plus a one-line index of the on-demand skills.
2. **Loaded when a task matches:** `/skill-me-up` installs each on-demand skill as `.claude/skills/upskill-{category}-{name}/SKILL.md`. The agent reads it when a task matches its description.
3. **Loaded when the skill says so:** long material that only some of a skill's tasks need goes in `skills/{category}/{name}/references/*.md`, and the skill's body says which file to read before which work. `/skill-me-up` copies these to `.claude/skills/upskill-{category}-{name}/references/`.

Rules for reference files:
- Refer to them as `references/{file}.md`, the path relative to the installed `SKILL.md`.
- A reference never points to another reference. Two levels below `AGENTS.md` is the limit.
- If a rule in a reference must apply to every task, repeat it in one line in the skill body, as `platform/juce-vst3-plugin` does for the preset toolbar.
- A `load: always` skill can't have references: its body lives in `AGENTS.md`, where `references/` paths don't resolve.
- Split a skill only when it is long and has whole sections that only some tasks need. Most skills stay as one file.

---

## Bundling files with a skill

Some skills need to copy actual files into the project (e.g. a starter template or a component library). To bundle files with a skill:

**1. Add the files** to `skill-resources/{skill-name}/` in this repo, in a named subfolder. The subfolder name becomes the source in the mapping. Mirror the `skills/` path — e.g. `skills/platform/example.md` → `skill-resources/platform/example/`.

For example, a platform skill bundling a starter project might look like:

```
skill-resources/
└── platform/
    └── example/
        └── starter-main/       ← source folder
            ├── src/
            └── config/
```

**2. Add a `## Resources` section** to the skill's `.md` file, with one directory mapping per line:

```
## Resources
starter-main/ -> src/starter/
```

The left side is relative to `skill-resources/{skill-name}/`. The right side is the destination in the project root.

When `/skill-me-up` runs, it reads the `## Resources` section, looks up the files in the up-skill repo via the GitHub API, and copies them into the new project at the specified paths. Files that already exist and differ are skipped with a warning.

## Bundling a slash command with a skill

Put each slash command in its own folder as `skill-resources/{skill-name}/skills/{command-name}/SKILL.md`, and map that folder into the project:

```
## Resources
skills/ -> .claude/skills/
```

Start each `SKILL.md` with this header. `name` must match the folder name, and `disable-model-invocation: true` means it only runs when someone types `/{command-name}`:

```
---
name: {command-name}
description: What it does. Only run when the user types /{command-name}.
disable-model-invocation: true
---
```

Both Claude and the Copilot harness read `.claude/skills/`. Do not use `.prompt.md` files: the Copilot harness ignores them.
