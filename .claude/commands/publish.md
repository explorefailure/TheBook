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
- Plugins repo: /Users/grot/Plugins
- GitHub: explorefailure/Plugins (private)

## Instructions

1. Verify plugin is complete (✅ Working or 📦 Installed in PLUGINS.md)
2. Check if plugin is in GitHub Actions build matrix
3. Add to matrix if missing
4. Commit any uncommitted changes
5. Push to trigger builds
6. Report status with link to GitHub Actions
