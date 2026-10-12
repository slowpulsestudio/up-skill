# Documentation Style Rules

Rules for every human-readable `.md` file in a project: `readme.md`, `dsp-maths.md`, `dsp-testing.md`, `nomenclature.md`, `presets.md`, `visualizer.md` and any other doc meant for a person to read. These rules do not apply to code, maths, quoted source text, `master-skills.md` or the files in `.github/prompts`, all of which are regenerated rather than hand-maintained.

---

**No em dashes**
Human-readable `.md` content never uses an em dash, in prose, in a title, in a heading or in a table cell. Use a colon, parentheses, a semicolon, a full stop or a comma instead, whichever reads most naturally in context. A name that used to carry a dash takes parentheses instead: "Earth (Aurora Borealis)" rather than a dash. Code, maths, quoted source text and generated files are exempt.

**A failed response looks like:**
- Writing "the signal splits here, rejoins later" with an em dash standing in for the comma
- Renaming "Earth (Aurora Borealis)" with a dash instead of parentheses
- Leaving an em dash inside a heading or a table cell because it "reads better"

---

**No Oxford commas**
A list of three or more items is written "A, B and C", with no comma before "and" or "or". Do not remove a comma that is doing other work: one that joins two independent clauses, or one that follows an introductory phrase, stays exactly where it is.

**A failed response looks like:**
- Writing "A, B, and C" instead of "A, B and C"
- Stripping the comma from "After the signal clips, the limiter engages" because it looks like a list comma

---

**Tables must be readable in the raw file and on GitHub**
Every table in a human-readable `.md` file has to work in two places at once: the raw file open in a code editor, and the rendered HTML on GitHub. That means:
- Every cell, including the header row and the separator row, is padded so every row has the same display width and the pipes line up in the raw file.
- Numeric columns are right-aligned, using `--:` in the separator row. Text columns stay left-aligned.
- No cell holds more than 40 characters and no row is wider than 110 characters.
- A cell holds a label, a number or a few words. It never holds a full sentence, LaTeX or a pipe character. Write a symbol as plain text or a unicode character instead, for example "clip(4u − k + ½, 0, 1)".
- Anything that does not fit goes in a bullet list directly under the table, one bullet per row in the same order, written "**First column name:** description". The exact LaTeX belongs in that bullet or in the surrounding prose, never in the table itself.
- A separate group of rows gets its own heading and its own table rather than being merged into one.

**A failed response looks like:**
- A ragged table where the raw markdown pipes do not line up because cells are not padded to equal width
- A numeric column left-aligned instead of right-aligned with `--:`
- A table cell holding a full sentence, a LaTeX expression or a pipe character
- One table covering several unrelated groups instead of one table per group under its own heading

---

**Glossaries are tables, not bullet lists**
A glossary is a table with a Glyph column and a Name column, padded as above, with the meaning of each entry written in a bullet underneath the table: "**NAME:** meaning". Add a Status column only when some entries genuinely need one, for example "not implemented", "abstract" or "conceptual", and keep the full note in the bullet as well.

**A failed response looks like:**
- Writing a glossary as a plain bullet list instead of a Glyph and Name table
- Putting the full meaning inside the table cell instead of in the bullet underneath
- Adding a Status column to every glossary out of habit when nothing in that glossary needs one

---

**Defining bullets use a bold colon**
A bullet that defines a term is written "**Name:** description", with the colon inside the bold span. It never uses a dash in place of the colon.

**A failed response looks like:**
- Writing "**Name** - description" with a dash instead of "**Name:** description"
- Putting the colon outside the bold span: "**Name**: description"

---

**Titles and headings**
A document's title is written "# <Project>: <Document title>". A sub-heading is either "Name (symbol)" or "Name: description". No heading is ever left with nothing written underneath it.

**A failed response looks like:**
- A title missing the project name or the colon, for example just "# Mathematical Model"
- A heading followed immediately by another heading, with nothing written in between

---

**dsp-maths.md leads with its equations**
`dsp-maths.md` opens with a "Central equations" section, near the top of the document, showing every key equation as display math (`$$` blocks), each with a bold label and one plain line explaining it. Inline math is reserved for a symbol mentioned inside prose, not for a full equation. The document describes what the code actually does: if the code changes, the maths changes with it, and any disagreement between the two is reported, never silently resolved in either direction.

**A failed response looks like:**
- Scattering the key equations through the document instead of collecting them in one "Central equations" section near the top
- Writing a full equation inline instead of as a `$$` block
- Quietly changing the maths to match the code, or the code to match the maths, instead of reporting the mismatch

---

**Scope**
These rules apply to every human-readable `.md` file meant for a person to read. They do not apply to code, to maths, to quoted source text, to `master-skills.md` or to the files under `.github/prompts`, all of which are regenerated rather than hand-maintained.

**A failed response looks like:**
- Rewriting a code block or a quoted error message to remove its dashes
- Restyling `master-skills.md` by hand instead of leaving it to be regenerated

---

**Verify before reporting done**
Before calling a documentation pass finished:
- Search every changed file for an em dash, and read every ", and" and ", or" to check for a serial comma.
- Audit every table: check whether every row has the same display width (count wide characters as two), note the longest cell and the widest row, and fail any table that breaks the table rule above.
- Render each changed `.md` file with a markdown library that has tables enabled, and confirm every table parses and that no stray pipe character remains outside maths.
- Confirm every math delimiter is balanced.

**A failed response looks like:**
- Reporting a documentation pass done without searching for em dashes or serial commas
- Skipping the table audit and assuming the padding is correct by eye
- Leaving an unbalanced `$$` or inline `$` delimiter in the file
