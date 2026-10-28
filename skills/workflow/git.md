---
load: on-demand
description: Source control rules. Never push, force-push, hard-reset or amend a published commit without explicit confirmation, and after every push offer the user a /clear. Read before any git commit, push, merge, rebase or reset, and before staging files.
---

# Git Rules

---

**Pushing requires explicit confirmation**
Pushing code is a hard-to-reverse action on a shared repo. Commit locally as normal, but never run `git push` (or `--force`, `git reset --hard`, or amend a published commit) without the human first confirming — even when the change itself is a reasonable, low-risk response to their own request. Reasonable idea does not equal permission to push.

**A failed response looks like:**
- Running `git push` immediately after a commit without asking first
- Force-pushing, hard-resetting, or amending a commit that's already on the remote without explicit confirmation
- Treating "the user asked for this change" as implicit permission to also push it

---

**Offer a fresh context after every push**
A push usually ends a piece of work, and every skill and file read during it is still in context, costing tokens and attention on every later turn. After each successful push, end your reply by offering a clean slate in one line: *"Pushed. If you're moving on to something else, type `/clear` (or start a new chat) so the next task starts with a fresh context."* You can't clear the context yourself, so only offer; never stop working or refuse to continue if the user carries on in the same chat.

**A failed response looks like:**
- Finishing a push without the one-line offer
- Turning the offer into a paragraph about context windows
- Claiming to have cleared the context, or refusing further work until the user clears it

---

**Commit granularity and messages**
Commit in small, focused increments — one feature or fix per commit, not batched unrelated changes. Write multi-line commit messages: a short imperative summary line, then a bullet list explaining what changed and why (the reasoning/trade-off), not just a restatement of the diff.

**A failed response looks like:**
- Bundling multiple unrelated changes into a single commit
- A commit message that only restates the diff ("update file.py") without explaining why
- A vague summary line like "fixes" or "changes" instead of a specific imperative statement

---

**What never gets committed**
Build output (`dist/`, `build/`), local virtualenvs (`.venv/`), macOS Finder's `.DS_Store`, and real secrets never go in a commit. `.env` is gitignored; `.env.example` is a template with blank values only. Verify `.gitignore` covers these before the first commit in a new project.

**A failed response looks like:**
- Committing `dist/`, `build/`, or `.venv/` because `.gitignore` wasn't checked first
- Leaving `.DS_Store` untracked and un-gitignored until it accidentally gets committed
- Committing a real secret value, even accidentally, in an example/template file

---

**Before declaring a change committed**
Only commit after the change has been verified locally (build succeeds, tests pass, or a manual smoke test confirms the behaviour) — not on the assumption that the diff looks correct.

**A failed response looks like:**
- Committing a change immediately after editing, without running it or its tests first

