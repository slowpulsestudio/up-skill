# Migrate this project to the new Up-Skill slash commands

Up-Skill used to install its slash commands as `.prompt.md` files in `.github/prompts/`. The Copilot harness in VS Code no longer loads those files. Up-Skill now installs slash commands as skills in `.claude/skills/`, which both the Copilot and Claude harnesses read.

Follow these steps once, in order, in this project. Speak to the user in short, plain sentences. Do not delete or change anything that these steps don't name.

## Step 1 — Check it's safe

- If there is no `.skills` file in the project root, this project was never set up with Up-Skill. Tell the user to follow the new-project setup in https://github.com/slowpulsestudio/up-skill instead, and stop.
- If this is a git repository and `git status --porcelain` shows any changes, tell the user to commit or stash them first, and stop. This migration may end with a commit, and that commit must not include unrelated work.

## Step 2 — Remove Up-Skill's old prompt files

Delete each of these files if it exists:

- `.github/prompts/skill-me-up.prompt.md`
- `.github/prompts/build-vst3.prompt.md`
- `.github/prompts/adopt-design-system.prompt.md`
- `.github/prompts/system-my-design.prompt.md`

Step 5 reinstalls the ones this project needs, as skills.

If any other `.prompt.md` files are left in `.github/prompts/`, the user made them for this project. List them and ask which to convert. For each one the user picks:

1. Create `.claude/skills/{name}/SKILL.md`, where `{name}` is the file name without `.prompt.md`, in lowercase letters, numbers and hyphens only. If that folder already exists, ask the user for a different name.
2. Give it this header, keeping the original description:
   ```
   ---
   name: {name}
   description: {original description}. Only run when the user types /{name}.
   disable-model-invocation: true
   ---
   ```
3. Copy everything below the original header word for word.
4. Delete the original `.prompt.md` file.

The new format has no `mode`, `agent`, `model` or `tools` settings. If the original had any of these, tell the user which ones were dropped. Leave any prompt files the user didn't pick untouched.

If `.github/prompts/` is now empty, delete it.

## Step 3 — Remove `.github/copilot-instructions.md`

Older setups put this line in `.github/copilot-instructions.md`:

```
Read master-skills.md and project-specific-agent-instructions.md for your operating instructions.
```

Up-Skill now writes everything into `AGENTS.md`, which Copilot reads directly, so this file is no longer needed.

- If it contains only the old line, delete it.
- If it contains anything else, show the user the extra text and ask whether to move it into `project-specific-agent-instructions.md`. If yes, add it to the end of that file and delete `.github/copilot-instructions.md`. If no, remove only the old line and leave the rest. Step 5 then moves `project-specific-agent-instructions.md` into `AGENTS.md`.

Do not add `@` lines to `.github/copilot-instructions.md`. Copilot doesn't follow them.

If `.github/` is now empty, delete it.

Leave `CLAUDE.md`, `master-skills.md` and `project-specific-agent-instructions.md` alone here. Step 5 handles all three.

## Step 4 — Install the new `/skill-me-up`

Download a fresh copy of Up-Skill. Use a method that can't return a cached copy, for example:

```
curl -sL -H 'Cache-Control: no-cache' -H 'Pragma: no-cache' https://github.com/slowpulsestudio/up-skill/archive/refs/heads/main.zip
```

Extract it to a new temporary folder. Copy `up-skill-main/.claude/skills/skill-me-up/SKILL.md` to `.claude/skills/skill-me-up/SKILL.md` in this project, replacing any existing copy. Then delete the temporary folder.

## Step 5 — Rebuild the skills

Read the `.claude/skills/skill-me-up/SKILL.md` you just installed and carry out all of its steps now, in this chat, as if the user had typed `/skill-me-up`. This writes the skills into `AGENTS.md`, moves the notes from `project-specific-agent-instructions.md` into it, points `CLAUDE.md` at it, deletes `master-skills.md` and `project-specific-agent-instructions.md`, and installs the slash commands this project's skills include.

When it reaches its git question, ask instead: *"Would you like me to commit and push this Up-Skill migration to GitHub?"* If yes, commit with the message `Migrate Up-Skill slash commands from prompts to skills`.

## Step 6 — Report

Tell the user, in a short list:

- Which old files were removed
- Which of their own prompts were converted, and any settings that were dropped
- What changed in `.github/copilot-instructions.md`, `AGENTS.md` and `CLAUDE.md`
- Which slash commands are now in `.claude/skills/`

Finish with: *"Start a new chat to use the slash commands. From now on, run `/skill-me-up` to refresh."*
