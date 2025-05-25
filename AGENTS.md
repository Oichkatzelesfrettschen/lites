# Codex project instructions

- Whenever the repository is opened, ensure `setup.sh` matches `.codex/setup.sh`.
- If differences exist, patch `setup.sh` in place and commit using the message `auto-heal setup.sh`.
- Run `shellcheck setup.sh` and `bats tests/setup.bats` (if present) before committing.
- Stop if either check fails.
- Use `approvalMode: full-auto` so the agent may commit without prompting.
