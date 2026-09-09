---
mode: agent
description: Check bundled resource files (e.g. shared UI components) from this project's selected skills against the latest version in the up-skill repo, and show diffs for anything that's changed so you can choose whether to update.
---

## Step -1 — Download a fresh repo snapshot

`raw.githubusercontent.com` is served through a CDN with a real ~5 minute cache that a cache-busting query string does **not** bypass — the cache key ignores query strings. The GitHub archive zip download below is not CDN-cached (`Cache-Control: max-age=0, private`) and is always current. So: download this zip once, at the very start of every run, and read every file needed by every later step from this one extracted snapshot. Never fetch individual files from `raw.githubusercontent.com` in this prompt.

There is never a reason to reuse a cached copy of this zip across runs — every run must hit the network fresh. Download it with a tool/method that cannot serve a locally-cached response (e.g. `curl -sL -H 'Cache-Control: no-cache' -H 'Pragma: no-cache'`, not a fetch tool that might return a cached page). If the tool you're using for downloads has its own cache, disable or bypass it for this request.

```
https://github.com/slowpulsestudio/up-skill/archive/refs/heads/main.zip
```

Extract it to a fresh temp location (do not reuse a temp directory from a previous run). Every path inside is prefixed with `up-skill-main/` (e.g. `up-skill-main/skills/platform/ios.md`).

## Step 0 — Read .skills

Read `.skills` in the project root. It contains a list of skill names, one per line — skip blank lines and any line starting with `#`. If `.skills` doesn't exist, tell the Designer to run `/skill-me-up` first (this project hasn't been set up yet) and stop.

## Step 1 — Find changed bundled files

For each skill name in `.skills`:
1. Read the skill file from the snapshot at `up-skill-main/skills/{skill-name}.md`. If it has no `## Resources` section, skip this skill.
2. For each `source-folder/ -> dest-folder/` mapping in that section, find every file in the snapshot under `up-skill-main/skill-resources/{skill-name}/{source-folder}/`, and compare it against `{project-root}/{dest-folder}/{relative-path}`:
   - If the destination file doesn't exist yet in this project, skip it — that's `/skill-me-up`'s job for first-time bundling, not this prompt's.
   - If it exists and is byte-identical to the snapshot version, skip it — nothing to do.
   - If it exists and differs, add it to a list of changed files to review.

## Step 2 — Review and apply, one file at a time

If no files changed, tell the Designer everything is already up to date and stop.

For each changed file, show a diff (current project version vs. the latest up-skill version) and ask: "Update this file to the latest version?" (yes/no).

- If yes: overwrite the destination file with the snapshot version.
- If no: leave it untouched and move to the next file.

Never batch-apply changes without asking per file — a bundled component may have been intentionally customized for this specific plugin, and a silent overwrite would destroy that.

## Step 3 — Summary

Report which files were updated and which were left as-is.

## Notes

Never commit or push anything as part of this prompt — it only updates local files, and only ones the Designer explicitly approved.
