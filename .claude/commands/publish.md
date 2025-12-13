---
description: Publish plugin to GitHub and trigger multi-OS builds
arguments:
  - name: PluginName
    description: Name of the plugin to publish
    required: true
---

# Publish Plugin: $ARGUMENTS.PluginName

Invoke the `plugin-publish` skill to push this plugin to GitHub and trigger Windows/macOS builds.

## Context

- Plugin: $ARGUMENTS.PluginName
- GitHub repo: Read `github_plugins_repo` from `.claude/system-config.json`

## Instructions

1. **Check GitHub configuration first:**
   - Read `.claude/system-config.json` for `github_plugins_repo`
   - If not configured, tell user to run `/setup` and STOP
2. Verify plugin is complete (✅ Working or 📦 Installed in PLUGINS.md)
3. Check if plugin is in GitHub Actions build matrix
4. Add to matrix if missing
5. Commit any uncommitted changes
6. Push to trigger builds
7. Report status with link to GitHub Actions (using repo from config)
