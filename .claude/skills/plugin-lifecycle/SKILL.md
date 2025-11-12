---
name: plugin-lifecycle
description: Manage complete plugin lifecycle - install, uninstall, reset, destroy
allowed-tools:
  - Bash
  - Read
  - Edit # For PLUGINS.md updates
  - Write # For backup metadata
preconditions:
  - Varies by mode (see mode-specific preconditions)
---

# plugin-lifecycle Skill

**Purpose:** Manage the complete lifecycle of plugins from installation to removal with proper state tracking and safety features.

## Overview

This skill handles all plugin lifecycle operations on macOS:

- **Installation (Mode 1)**: Copy Release binaries to system folders (`~/Library/Audio/Plug-Ins/`)
- **Uninstallation (Mode 2)**: Clean removal from system folders (preserves source code)
- **Reset to Ideation (Mode 3)**: Remove implementation, keep idea/mockups (surgical rollback)
- **Destroy (Mode 4)**: Complete removal with backup (nuclear option)

All operations include proper permissions, cache clearing, state tracking, and safety features (confirmations, backups).

---

## Mode Dispatcher

This skill operates in different modes based on the invoking command:

| Mode | Operation | Command | Purpose |
|------|-----------|---------|---------|
| 1 | Installation | `/install-plugin` | Deploy to system folders |
| 2 | Uninstallation | `/uninstall` | Remove binaries, keep source |
| 3 | Reset to Ideation | `/reset-to-ideation` | Remove implementation, keep idea/mockups |
| 4 | Destroy | `/destroy` | Complete removal with backup |
| Menu | Interactive | `/clean` | Present menu, user chooses mode |

**Invocation Pattern:**
When user runs `/install-plugin [Name]`, the command expands to a prompt that invokes THIS skill.
You then read the mode from context and delegate to the appropriate reference file:
- Mode 1 → Read and execute `references/installation-process.md`
- Mode 2 → Read and execute `references/uninstallation-process.md`
- Mode 3 → Read and execute `references/mode-3-reset.md`
- Mode 4 → Read and execute `references/mode-4-destroy.md`
- Menu → Present menu, wait for selection, then route to chosen mode

**Installation targets (macOS):**
- VST3: `~/Library/Audio/Plug-Ins/VST3/`
- AU: `~/Library/Audio/Plug-Ins/Components/`
- AAX: `~/Library/Application Support/Avid/Audio/Plug-Ins/` (future)

DAWs scan these locations. Correct installation = plugin appears in DAW.

---

## Installation Workflow

<critical_sequence enforcement="strict" blocking="true">
The following steps MUST execute in order without skipping:

1. **Build Verification** - Check Release binaries exist, offer to build if missing
   - Delegate to: `references/installation-process.md` Step 1
   - Blocking: YES - cannot proceed without binaries

2. **Product Name Extraction** - Extract PRODUCT_NAME from CMakeLists.txt
   - Delegate to: `references/installation-process.md` Step 2
   - Required for: All subsequent steps that reference ${PRODUCT_NAME}

3. **Old Version Removal** - Remove existing installations to prevent conflicts
   - Delegate to: `references/installation-process.md` Step 3
   - Blocking: NO - can proceed if no old version exists

4. **Copy to System Folders** - Install VST3 and AU to macOS plugin directories
   - Delegate to: `references/installation-process.md` Step 4
   - Blocking: YES - core installation step

5. **Permissions Verification** - Set 755 permissions for DAW access
   - Delegate to: `references/installation-process.md` Step 5
   - Blocking: YES - DAWs cannot load plugins without correct permissions

6. **Cache Clearing** - Clear Ableton Live and Logic Pro caches
   - Delegate to: `references/cache-management.md`
   - Blocking: NO - warn if fails but continue

7. **Verification** - Confirm installation with file checks and size validation
   - Delegate to: `references/installation-process.md` Step 7
   - Blocking: YES - must confirm success before declaring completion

8. **PLUGINS.md Update** - Record installation status and locations
   - Delegate to: `references/installation-process.md` Step 8
   - Blocking: YES - state tracking is part of success criteria

</critical_sequence>

See **[references/installation-process.md](references/installation-process.md)** for complete implementation.

---

## Cache Management

DAWs cache plugin metadata for fast scanning. After installation/removal, caches must be cleared:

- **Ableton Live**: Delete `PluginCache.db` from all Live versions
- **Logic Pro**: Clear `AudioUnitCache/` and restart `AudioComponentRegistrar`
- **Other DAWs**: Reaper (no cache), Cubase (manual rescan), Pro Tools (future)

See **[references/cache-management.md](references/cache-management.md)** for detailed cache clearing procedures.

---

## Uninstallation Workflow

Complete uninstallation process:

1. **Locate Plugin Files** - Find installed VST3 and AU bundles
2. **Confirm Removal** - Ask user to confirm deletion
3. **Remove Files** - Delete from system folders (source code preserved)
4. **Clear Caches** - Same as installation
5. **Update PLUGINS.md** - Change status back to ✅ Working
6. **Confirmation** - Display uninstallation summary

See **[references/uninstallation-process.md](references/uninstallation-process.md)** for complete implementation.

---

## Reset to Ideation Workflow (Mode 3)

Surgical rollback that removes implementation but preserves ideation artifacts:

**What gets preserved:**
- Creative brief (the original idea)
- UI mockups (all versions)
- Parameter specifications

**What gets removed:**
- Source code (Source/ directory)
- Build configuration (CMakeLists.txt)
- Implementation docs (architecture.md, plan.md)
- Build artifacts and installed binaries

**Use case:** Implementation went wrong, but the concept and UI design are solid. Start fresh from Stage 0.

See **[references/mode-3-reset.md](references/mode-3-reset.md)** for complete implementation.

---

## Destroy Workflow (Mode 4)

Complete removal with backup for abandoned plugins:

**What gets removed:**
- Everything: source code, binaries, build artifacts, PLUGINS.md entry
- Optionally: troubleshooting docs mentioning the plugin

<decision_gate type="destructive_confirmation" bypass="never">
**Safety Protocol - Requires Explicit User Confirmation:**

<gate_conditions>
1. Check status ≠ 🚧 (block if in development)
2. Create timestamped backup to `backups/destroyed/`
3. Show deletion preview (files, sizes, locations)
4. Require exact plugin name match (case-sensitive)
</gate_conditions>

<confirmation_format>
User MUST type exact plugin name to proceed.
- Correct match → Proceed to deletion
- Incorrect match → Abort with error message
- Empty response → Abort
</confirmation_format>

You MUST NOT proceed with deletion until user provides correct confirmation.
This gate cannot be bypassed - it protects against accidental data loss.
</decision_gate>

**Use case:** Abandoned experiment, complete failure, duplicate by mistake. Never using this plugin again.

See **[references/mode-4-destroy.md](references/mode-4-destroy.md)** for complete implementation.

---

## Interactive Menu (Mode: Menu)

When invoked via `/clean [PluginName]`, present interactive menu:

```
Plugin cleanup options for [PluginName]:

1. Uninstall - Remove binaries from system folders (keep source code)
2. Reset to ideation - Remove implementation, keep idea/mockups
3. Destroy - Complete removal with backup (IRREVERSIBLE except via backup)
4. Cancel

Choose (1-4): _
```

**Menu logic:**
1. Read current status from PLUGINS.md
2. Filter options based on status:
   - IF status = "💡 Ideated" → Show options 2, 3, 4 (cannot uninstall what isn't built)
   - IF status = "✅ Working" → Show options 1, 2, 3, 4 (not installed yet)
   - IF status = "📦 Installed" → Show ALL options (full lifecycle available)
   - IF status = "🚧 In Progress" → Show only option 4 (Cancel)
   - ELSE (unknown status) → Show ALL options with warning: "Unknown status '[status]' - showing all options"
3. Present filtered menu, wait for user choice
4. Route to selected mode (1-4) or exit on Cancel

---

## Error Handling

Common error scenarios with troubleshooting:

- **Build Files Not Found**: Guide to build Release mode or check build status
- **Permission Denied**: Create directories, fix permissions, check disk space
- **Cache Clear Failed**: Manual DAW-specific cache clearing instructions
- **Plugin Doesn't Appear in DAW**: Comprehensive troubleshooting checklist

See **[references/error-handling.md](references/error-handling.md)** for all error scenarios and fixes.

---

## Decision Menu After Installation

<decision_gate type="checkpoint" enforcement="strict">
After successful installation, you MUST present this decision menu and WAIT for user response.

<checkpoint_protocol>
1. Display completion statement with checkmark
2. Present numbered menu (5 options)
3. STOP execution and wait for user choice
4. Do NOT auto-proceed or make assumptions
5. Only continue after receiving user selection (1-5)
</checkpoint_protocol>

<menu_format>
```
✓ [PluginName] installed successfully

What's next?
1. Test in DAW (recommended) → Load in Logic/Ableton/Reaper
2. Create another plugin → /dream
3. Document this plugin → Create user guide/manual
4. Share plugin (export build) → Future: Package for distribution
5. Other

Choose (1-5): _
```
</menu_format>

<response_handlers>
- **Option 1:** Provide DAW testing guidance (terminal - ends workflow)
- **Option 2:** Invoke `plugin-ideation` skill via natural language (not direct Tool call)
- **Option 3:** Suggest creating user manual in `.ideas/` directory
- **Option 4:** Provide build export instructions (future feature note)
- **Option 5:** Ask open-ended "What would you like to do?"
</response_handlers>

This is a system-wide checkpoint protocol enforcement. Never skip this menu.
</decision_gate>

---

## Integration Points

**Invoked by:**

- `/install-plugin [PluginName]` → Mode 1 (Installation)
- `/uninstall [PluginName]` → Mode 2 (Uninstallation)
- `/reset-to-ideation [PluginName]` → Mode 3 (Reset)
- `/destroy [PluginName]` → Mode 4 (Destroy)
- `/clean [PluginName]` → Interactive menu
- `plugin-workflow` skill → After Stage 6 (auto-installs)
- `plugin-improve` skill → After successful changes (offers reinstallation)
- Natural language: "Install [PluginName]", "Remove [PluginName]", "Clean up [PluginName]"

**Invokes:**

- None (terminal skill, doesn't invoke others)

<handoff_protocol>
## Invocation Protocol

When this skill is invoked, check the triggering context:

<invocation_modes>
1. **Direct command** (e.g., `/install-plugin GainKnob`)
   - Extract plugin name from command argument
   - Set mode = 1 (Installation)
   - Execute installation workflow

2. **From plugin-workflow** (after Stage 6 completion)
   - Plugin name provided in context
   - Automatically invoked after validation completes
   - Set mode = 1 (Installation) and execute

3. **From plugin-improve** (after successful changes)
   - Plugin name in context
   - Offer reinstallation (only if previously installed)
   - IF user accepts → Set mode = 1 and execute

4. **Natural language** ("Install TapeAge")
   - Parse plugin name from utterance
   - Set mode = 1 (Installation)
   - Execute installation workflow

5. **Interactive menu** (from `/clean` command)
   - Plugin name provided
   - Set mode = Menu
   - Present options, wait for selection, route to chosen mode
</invocation_modes>

<return_protocol>
After completion, this skill MUST:
1. Present decision menu (checkpoint protocol)
2. Wait for user response
3. Return control to user (terminal skill - does not invoke other skills)

Do NOT automatically proceed to next action.
</return_protocol>
</handoff_protocol>

**Updates:**

- `PLUGINS.md` → Status changes to 📦 Installed, adds installation metadata

**Creates:**

- System folder installations (non-git-tracked):
  - `~/Library/Audio/Plug-Ins/VST3/[Product].vst3`
  - `~/Library/Audio/Plug-Ins/Components/[Product].component`

**Blocks:**

- None (installation is optional, plugins can be tested without installing)

---

## Success Criteria

Installation is successful when:

- ✅ Both VST3 and AU files copied to system folders
- ✅ Permissions set correctly (755) on all files
- ✅ Caches cleared for all detected DAWs
- ✅ Verification shows recent timestamps (< 60 seconds ago)
- ✅ File sizes are reasonable (> 100 KB typically)
- ✅ PLUGINS.md updated with 📦 status and installation locations
- ✅ User knows next steps (restart DAW, rescan plugins)

**NOT required for success:**

- Plugin appearing in DAW immediately (may need rescan/restart)
- All DAW caches cleared (some DAWs may not be installed)
- Code signing (optional for local development)

---

## Notes for Claude - Mode 1 (Installation)

**When executing the installation workflow (Mode 1):**

1. Always check for Release build first - offer to build if missing
2. Extract PRODUCT_NAME from CMakeLists.txt (may contain spaces)
3. Use quotes around paths with spaces: `"$PRODUCT_NAME"`
4. Remove old versions before installing (prevents conflicts)
5. Cache clearing is important but not blocking (warn if fails)
6. Verification checks should be comprehensive (permissions, timestamps, sizes)
7. PLUGINS.md status update is part of success criteria
8. Provide clear next steps after installation
