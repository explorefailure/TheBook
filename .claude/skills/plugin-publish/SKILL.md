# Plugin Publish Skill

Publish plugins to GitHub and trigger multi-OS builds.

## Trigger Detection

Activate when user:
- Runs `/publish [PluginName]`
- Says "publish [PluginName]", "push [PluginName] to GitHub", "build [PluginName] for Windows"
- Asks about cross-platform builds or GitHub Actions

## Prerequisites

- Plugin must be in `✅ Working` or `📦 Installed` state (check PLUGINS.md)
- Plugin source must exist in `plugins/[PluginName]/`
- GitHub CLI authenticated (`gh auth status`)
- **GitHub must be configured in system-config.json** (run `/setup` if not)

## GitHub Configuration Check

**CRITICAL:** Before any publish operation, check that GitHub is configured:

```bash
# Read github_plugins_repo from config
cat .claude/system-config.json | jq -r '.github_plugins_repo'
```

**If `github_plugins_repo` is missing or null:**
```
GitHub is not configured for publishing.

Please run /setup to configure your GitHub username and Plugins repository.

This prevents accidental pushes to the wrong repository.
```
**STOP and do not proceed with publish.**

**If configured, use the repo from config for all GitHub operations.**

## Workflow

### 1. Validate Plugin State

```bash
# Check plugin exists
ls plugins/[PluginName]/CMakeLists.txt

# Check PLUGINS.md for status
grep "[PluginName]" PLUGINS.md
```

Only publish plugins that are `✅ Working` or `📦 Installed`. Reject plugins in `🚧 Stage N` or `💡 Ideated`.

### 2. Check GitHub Workflow Matrix

Read `/Users/grot/Plugins/.github/workflows/build-plugins.yml` and check if plugin is in the matrix:

```yaml
matrix:
  plugin: [Bitcrusher, WaveFolder, ...]
```

If plugin is NOT in the matrix, add it.

### 3. Commit and Push

```bash
cd /Users/grot/Plugins

# Check for uncommitted changes
git status

# If changes exist, commit them
git add plugins/[PluginName]
git commit -m "feat([PluginName]): [brief description]"

# Push to trigger GitHub Actions
git push origin main
```

### 4. Report Status

After push:
1. Provide link to GitHub Actions: `https://github.com/{github_plugins_repo}/actions` (read repo from system-config.json)
2. Explain builds run on Windows and macOS
3. Mention artifacts will be available for download when complete

## GitHub Actions Overview

The workflow (`build-plugins.yml`) does:
- Builds on `windows-latest` and `macos-latest`
- Clones JUCE 8.0.3
- Builds VST3 format
- Uploads artifacts for each OS

## Manual Trigger

Users can also trigger builds manually:
1. Go to `https://github.com/{github_plugins_repo}/actions` (use repo from system-config.json)
2. Select "Build Plugins" workflow
3. Click "Run workflow"

## Error Handling

| Issue | Solution |
|-------|----------|
| Plugin not found | Check spelling, verify plugin exists |
| Plugin in development | Complete implementation first |
| Git push fails | Check authentication: `gh auth status` |
| Build fails | Check GitHub Actions logs for errors |

## Example Usage

```
User: /publish WaveFolder
Assistant: Publishing WaveFolder to GitHub...

✓ Plugin verified: WaveFolder (✅ Working)
✓ Already in build matrix
✓ Pushed to origin/main

GitHub Actions is now building WaveFolder for:
- Windows (VST3)
- macOS (VST3)

View progress: https://github.com/{github_plugins_repo}/actions

When complete, download artifacts from the workflow run.
```
