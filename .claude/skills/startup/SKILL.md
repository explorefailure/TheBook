---
name: startup
description: Context-aware session startup. Displays banner, last checkpoint, uncommitted changes warning. Auto-runs after SessionStart hook.
allowed-tools:
  - Read
  - Bash
  - Glob
---

# Startup Skill

**Purpose:** Lightweight context-aware greeting after environment validation. Shows where you left off and if there's uncommitted work.

## Process

### 1. Display Banner

```
████████╗██╗  ██╗███████╗    ██████╗  ██████╗  ██████╗ ██╗  ██╗
╚══██╔══╝██║  ██║██╔════╝    ██╔══██╗██╔═══██╗██╔═══██╗██║ ██╔╝
   ██║   ███████║█████╗      ██████╔╝██║   ██║██║   ██║█████╔╝
   ██║   ██╔══██║██╔══╝      ██╔══██╗██║   ██║██║   ██║██╔═██╗
   ██║   ██║  ██║███████╗    ██████╔╝╚██████╔╝╚██████╔╝██║  ██╗
   ╚═╝   ╚═╝  ╚═╝╚══════╝    ╚═════╝  ╚═════╝  ╚═════╝ ╚═╝  ╚═╝
        A CHOOSE YOUR OWN AU/VST PLUGIN ADVENTURE
```

### 2. Find Last Checkpoint

Search for `.continue-here.md` files in priority order:

```bash
# Find most recently modified handoff file
find plugins -name ".continue-here.md" -type f 2>/dev/null | head -1
```

If found:
1. Extract plugin name from path (e.g., `plugins/ChopShop/.continue-here.md` → ChopShop)
2. Parse YAML frontmatter for `stage` and `status` fields
3. Build one-liner: `Last: [PluginName] ([Stage/Status])`

If no handoff files exist:
- Check PLUGINS.md for any plugins with status `🚧 Stage N` (in-progress)
- If none in progress: `No active projects`

### 3. Check Uncommitted Changes

```bash
git status --porcelain plugins/
```

If output is non-empty: `Uncommitted changes`

### 4. Display Output

**Format when checkpoint exists:**
```
Last: ChopShop (Stage 2 - DSP)
→ /continue ChopShop

⚠️ Uncommitted changes
```

**Format when no active work:**
```
No active projects → /dream
```

**Format when checkpoint exists, no uncommitted changes:**
```
Last: ChopShop (Stage 2 - DSP)
→ /continue ChopShop
```

---

## Output Rules

- **One-liner for checkpoint** - Don't dump full handoff content
- **Only show uncommitted warning if changes exist** - Skip the line entirely if clean
- **No verbose explanations** - Keep it minimal
- **Always show suggestion** - Either `/continue [Name]` or `/dream`

---

## Success Criteria

1. Banner displays correctly
2. Checkpoint shown as single line (or "No active projects")
3. Uncommitted changes warning shown only when applicable
4. Clear next action suggested
5. Total output is 6-8 lines max (banner + 2-3 lines context)
