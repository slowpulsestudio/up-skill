# Python CLI / Local Tool Rules

---

**Environment setup**
Use a project-local virtualenv at `.venv/` — never install packages globally. Pin all dependencies in `requirements.txt`. Use Homebrew-installed Python, not the macOS system default (it's often outdated).

```zsh
source .venv/bin/activate
pip install -r requirements.txt
```

**A failed response looks like:**
- Installing packages globally instead of into `.venv/`
- Using the system Python instead of a Homebrew-managed version
- Adding a new dependency without pinning it in `requirements.txt`

---

**Secrets**
Load secrets via `python-dotenv` from a `.env` file. `.env` is gitignored; `.env.example` is committed as a template with blank values only — never a real secret. Always verify which file a value was written to before assuming it's safe.

**A failed response looks like:**
- Hardcoding an API key or secret directly in Python source
- Committing a real secret value in `.env.example`

---

**Distribution model**
This is a plain script-based tool, not a packaged/distributed app — run it directly with `python main.py`. Do not add PyInstaller or any bundling/packaging step unless the Designer explicitly asks for a distributable binary.

**A failed response looks like:**
- Adding PyInstaller, `.spec` files, or `.app` bundling when the project only needs to run as a script

---

**Configuration over hardcoding**
Expose creative/tunable parameters (dimensions, counts, seeds, thresholds, feature flags) through a config file (e.g. `config.yaml`) or CLI flags (`argparse`/`click`) — never bury them as constants inside logic files. The config schema should be self-explanatory enough that the Designer can tweak behaviour without reading Python code.

**A failed response looks like:**
- Hardcoding a tunable creative parameter directly in a function body instead of reading it from config
- Adding a new parameter to config.yaml without wiring it up to actually change behaviour

---

**Cost control for expensive operations**
When any step of the pipeline calls a paid/external API, build and prove the pipeline with a cheap local or synthetic stand-in first (a "preview" or "dry-run" mode). Support running a single unit of work (one item, not the whole batch) and regenerating a specific subset without redoing everything else. Cache successful expensive results (keyed by a deterministic seed/hash of inputs) so re-runs don't silently re-spend money.

**A failed response looks like:**
- Wiring up a paid API before the free/local parts of the pipeline are proven to work
- Regenerating an entire batch to fix or retry one bad item
- No caching, so an interrupted run has to start over from scratch

---

**Modularity for swappable providers**
Wrap any external service (image/text/model API) behind a small interface (e.g. a single `generate_x(...)` function or class) so the underlying provider can be swapped later without touching calling code.

**A failed response looks like:**
- Calling a specific vendor's SDK directly from multiple places in the pipeline instead of behind one interface

---

**Verification**
Validate outputs automatically wherever practical (dimensions, file counts, ordering, naming, missing/duplicate files) rather than relying on the Designer to spot problems visually. Run the full pipeline end-to-end against real or realistic synthetic data before declaring a milestone done — passing unit tests alone is not enough.

**A failed response looks like:**
- Declaring a milestone done after only mocked/unit tests, without an end-to-end run
- Leaving obvious structural checks (dimensions, ordering, missing files) to manual/visual inspection

---

**Dict/kwargs merging**
`dict.setdefault()` does NOT merge with caller-supplied values — it only fills in a key if it's absent, so a caller-supplied `headers`/`kwargs` dict silently replaces (rather than merges with) the defaults. Merge explicitly instead: `{**DEFAULTS, **overrides}`.

**A failed response looks like:**
- Using `kwargs.setdefault("headers", DEFAULT_HEADERS)` when a caller might also pass `headers` — this silently drops the defaults
- Assuming a dict-valued default and a per-call override combine automatically

---

**Third-party API/model identifiers**
Before hardcoding a model name or API version string, verify it's currently valid (e.g. a `list models` call) rather than trusting a name that may have been renamed or deprecated.

**A failed response looks like:**
- Hardcoding an API model name without verifying it's still a valid identifier
