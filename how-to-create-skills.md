# How to create a skill

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
