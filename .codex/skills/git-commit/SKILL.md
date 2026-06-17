---
name: git-commit
description: Use when the user asks to create a git commit in this project, especially with their brief + details English commit message format.
---

# Create Git Commits

Use this skill when the user asks to commit changes.

Workflow:

1. Run `git status --short` and inspect the changed files.
2. Stage only the files that belong to the requested commit.
3. Write the commit message in English.
4. Use this format:

```text
Brief summary

Detail line 1.
Detail line 2.
Detail line 3.
```

Rules:

- Prefer the user's requested wording when provided.
- If the user says to use the previous/final message, translate it to English and keep it concise.
- Details must be consecutive lines without extra blank lines between them.
- If the project git hook rewrites the commit subject or message, do not amend it unless the user explicitly asks.
- After committing, report the short commit hash and subject.
- Confirm whether the working tree is clean.

Recommended command shape:

```bash
git add <files>
git commit -m "Brief summary" -m "Detail line 1.
Detail line 2.
Detail line 3."
```

Do not:

- Use `git add .` when unrelated files are present.
- Revert unrelated changes.
- Amend hook-modified commit messages unless the user asks for that exact change.

