# THE BOOK - Project Instructions

## Session Start Display

At the beginning of each session:

1. Display the SessionStart hook output (environment validation)
2. **Immediately run `/startup`** - This displays the banner and context-aware checkpoint info

The startup skill shows:
- ASCII banner
- Last checkpoint (if any active work exists)
- Uncommitted changes warning (if applicable)
- Suggested next action

## Architecture

### Dual-Repo Setup

THE BOOK uses two repositories:

| Repo | Path | GitHub | Purpose |
|------|------|--------|---------|
| thebook | `/Users/grot/thebook` | explorefailure/TheBook (public fork) | Framework: skills, agents, workflows |
| Plugins | `/Users/grot/Plugins` | explorefailure/Plugins (private) | Plugin source code + GitHub Actions CI/CD |

**Key detail:** `thebook/plugins/` is a symlink to `Plugins/plugins/`. All plugin development happens in the Plugins repo.

### Plugin Development Flow

1. **Develop** - Work in `plugins/PluginName/` (resolves via symlink to Plugins repo)
2. **Build locally** - `cmake -B build && cmake --build build --target PluginName_VST3`
3. **Publish** - `/publish PluginName` pushes to GitHub, triggers Windows + macOS builds
4. **Download** - Get cross-platform binaries from https://github.com/explorefailure/Plugins/actions

### Config Files

- `.claude/system-config.json` - System paths (JUCE, pluginval, plugins_repo, etc.)
- `PLUGINS.md` - Plugin registry (symlink to Plugins repo)

## Completed Plugins

| Plugin | Type | Status |
|--------|------|--------|
| Bitcrusher | Lo-Fi Effect | Installed |
| WaveFolder | Waveshaper | Working |

## Key Commands

- `/dream` - Brainstorm new plugin
- `/plan` - Design DSP architecture
- `/implement` - Build plugin (Stages 1-3)
- `/publish [Name]` - Push to GitHub, trigger multi-OS builds
- `/install-plugin [Name]` - Install to system folders
