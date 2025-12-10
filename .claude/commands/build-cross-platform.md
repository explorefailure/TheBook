# /build-cross-platform

Build plugins for Windows and macOS via GitHub Actions.

When user runs `/build-cross-platform [PluginName?]`:

1. Load the cross-platform-build skill from `.claude/skills/cross-platform-build/SKILL.md`
2. Follow the execution steps in the skill

## Quick Flow

1. **If no plugin specified:** List plugins that can be built, ask which one
2. **Verify plugin builds locally:** `cmake --build build --target [Name]_VST3`
3. **Check workflow exists:** `.github/workflows/build-plugins.yml`
4. **Update plugin matrix if needed:** Add plugin to matrix if not present
5. **Commit and push:**
   ```bash
   git add -A
   git commit -m "chore: trigger cross-platform build for [PluginName]"
   git push origin main
   ```
6. **Provide GitHub Actions link** for monitoring and artifact download

## Output

After successful push:
```
✓ Pushed to GitHub. Cross-platform build triggered.

Build progress: https://github.com/[owner]/[repo]/actions

When complete (~5-10 min), download:
  - [PluginName]-Windows-VST3
  - [PluginName]-macOS-VST3
```
