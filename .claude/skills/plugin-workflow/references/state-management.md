# State Management and Workflow Functions

**Context:** This file is part of the plugin-workflow skill.
**Invoked by:** All stages for state updates, git commits, and handoff management
**Purpose:** Centralized state machine functions, git operations, decision menus, and handoff file management

---

## State Machine Functions

### updatePluginStatus(pluginName, newStatus)

**Purpose:** Update plugin status emoji in PLUGINS.md after each stage checkpoint.

**Valid statuses for plugin-workflow (stages 2-6):**
- `🚧 Stage 2` - Foundation in progress
- `🚧 Stage 3` - Shell in progress
- `🚧 Stage 4` - DSP in progress
- `🚧 Stage N.M` - Phased implementation (stage.phase)
- `🚧 Stage 5` - GUI in progress
- `🚧 Stage 6` - Validation in progress
- `✅ Working` - Stage 6 complete, not installed
- `📦 Installed` - Deployed to system folders

**Note:** Statuses `💡 Ideated` and `🚧 Stage 0-1` are managed by plugin-planning skill.

**Implementation:**
1. Read PLUGINS.md
2. Find section: `### [pluginName]`
3. Update `**Status:**` line with new emoji and text
4. Validate transition is legal (see validateTransition below)
5. Write back to PLUGINS.md

**CRITICAL:** This must be called AFTER each subagent completes and before presenting decision menu.

**Example:**
```markdown
### TapeDelay
**Status:** 🚧 Stage 4 → **Status:** 🚧 Stage 5
```

### createPluginEntry(pluginName, type, brief)

**Purpose:** Create initial PLUGINS.md entry when starting new plugin.

**Implementation:**
1. Read PLUGINS.md
2. Check if entry already exists (search for `### [pluginName]`)
3. If not exists, append new entry:
   ```markdown
   ### [pluginName]
   **Status:** 💡 Ideated
   **Type:** [Audio Effect | MIDI Instrument | Synth]
   **Created:** [YYYY-MM-DD]

   [Brief description from creative-brief.md]

   **Lifecycle Timeline:**
   - **[YYYY-MM-DD]:** Creative brief created

   **Last Updated:** [YYYY-MM-DD]
   ```
4. Write back to PLUGINS.md

### updatePluginTimeline(pluginName, stage, description)

**Purpose:** Add timeline entry to PLUGINS.md when stage completes.

**Implementation:**
1. Read PLUGINS.md
2. Find plugin entry
3. Find `**Lifecycle Timeline:**` section
4. Append new entry:
   ```markdown
   - **[YYYY-MM-DD] (Stage N):** [description]
   ```
5. Update `**Last Updated:**` field
6. Write back to PLUGINS.md

### getPluginStatus(pluginName)

**Purpose:** Return current status emoji for routing logic.

**Implementation:**
1. Read PLUGINS.md
2. Find `### [pluginName]` section
3. Extract `**Status:**` line
4. Parse emoji: 💡, 🚧, ✅, or 📦
5. If 🚧, extract stage number (e.g., "🚧 Stage 4" → 4)
6. Return object: `{ emoji: "🚧", stage: 4, text: "Stage 4" }`

### validateTransition(currentStatus, newStatus)

**Purpose:** Enforce legal state machine transitions for plugin-workflow (stages 2-6).

**Legal transitions for plugin-workflow:**
```
🚧 Stage 1 → 🚧 Stage 2 (start implementation after planning)
🚧 Stage N → 🚧 Stage N+1 (sequential stages 2-6)
🚧 Stage N → 🚧 Stage N.M (enter phased implementation)
🚧 Stage N.M → 🚧 Stage N.M+1 (next phase within stage)
🚧 Stage N.M → 🚧 Stage N+1 (complete phased stage)
🚧 Stage 6 → ✅ Working (validation complete)
✅ Working → 📦 Installed (install plugin)
📦 Installed → 🚧 Improving (start improvement)
🚧 Improving → 📦 Installed (improvement complete)
```

**Illegal transitions:**
```
🚧 Stage 2 → 🚧 Stage 5 (can't skip stages 3-4)
🚧 Stage 4 → 🚧 Stage 2 (can't go backward)
✅ Working → 🚧 Stage 3 (use /improve instead)
```

**Note:** Transitions involving `💡 Ideated` and `🚧 Stage 0-1` are managed by plugin-planning skill.

**Implementation:**
1. Parse current and new status
2. Check transition against rules
3. Return: `{ allowed: true }` or `{ allowed: false, reason: "..." }`


## Interactive Decision Menu System

### presentDecisionMenu(context)

**Purpose:** Present context-aware decision menu at every checkpoint.

**Context parameters:**
- `stage`: Current stage number (0-6)
- `completionStatement`: What was just accomplished
- `pluginName`: Plugin being worked on
- `errors`: Any errors/failures (optional)
- `options`: Custom options (optional)

**Format - Inline Numbered List (NOT AskUserQuestion):**

```
✓ [Completion statement]

What's next?
1. [Primary action] (recommended)
2. [Secondary action]
3. [Discovery feature] ← User discovers [capability]
4. [Alternative path]
5. [Escape hatch]
6. Other

Choose (1-6): _
```

**Implementation:**
1. Generate context-appropriate options (see generateContextualOptions below)
2. Format as inline numbered list
3. Display to user
4. Wait for response (number, keyword shortcut, or "Other")
5. Parse response
6. Execute chosen action or re-present menu if invalid

**Keyword Shortcuts:**
- "continue" → Option 1 (primary action)
- "pause" → Pause option (creates checkpoint)
- "review" → Review option (show code/context)

**Handle "Other" responses:**
```
User: Other
System: "What would you like to do?"
User: [Free-form request]
System: [Process request]
System: [Re-present decision menu afterward]
```

### generateContextualOptions(context)

**Purpose:** Generate situation-specific menu options after subagent completion.

**After Stage 2 (Foundation):**
```javascript
[
  { label: "Continue to Stage 3", recommended: true },
  { label: "Review foundation code" },
  { label: "Test build manually" },
  { label: "Review CMakeLists.txt" },
  { label: "Pause here" },
  { label: "Other" }
]
```

**After Stage 3 (Shell):**
```javascript
[
  { label: "Continue to Stage 4", recommended: true },
  { label: "Review APVTS implementation" },
  { label: "Test parameter automation" },
  { label: "Load in DAW to verify" },
  { label: "Pause here" },
  { label: "Other" }
]
```

**After Stage 4 (DSP):**
```javascript
[
  { label: "Continue to Stage 5", recommended: true },
  { label: "Review DSP code" },
  { label: "Test audio processing" },
  { label: "Run pluginval" },
  { label: "Pause here" },
  { label: "Other" }
]
```

**After Stage 5 (GUI):**
```javascript
[
  { label: "Continue to Stage 6", recommended: true },
  { label: "Review WebView integration" },
  { label: "Test parameter bindings" },
  { label: "Show standalone UI" },
  { label: "Pause here" },
  { label: "Other" }
]
```

**After Stage 6 (Validation):**
```javascript
[
  { label: "Install plugin to system folders", recommended: true },
  { label: "Test in DAW first" },
  { label: "Create another plugin" },
  { label: "Review complete plugin code" },
  { label: "Document this plugin" },
  { label: "Other" }
]
```

**Build Failure:**
```javascript
[
  { label: "Investigate", discovery: "Run deep-research to find root cause" },
  { label: "Show me the code" },
  { label: "Show full build output" },
  { label: "I'll fix it manually (say \"resume automation\" when ready)" },
  { label: "Other" }
]
```

**Validation Failure:**
```javascript
[
  { label: "Fix and re-validate", recommended: true },
  { label: "Re-run stage" },
  { label: "Override (not recommended)" },
  { label: "Other" }
]
```

### formatDecisionMenu(completionStatement, options)

**Purpose:** Format options as inline numbered list.

**Implementation:**
```
output = `✓ ${completionStatement}\n\n`
output += `What's next?\n`

options.forEach((opt, i) => {
  output += `${i+1}. ${opt.label}`

  if (opt.recommended) {
    output += ` (recommended)`
  }

  if (opt.discovery) {
    output += ` ← ${opt.discovery}`
  }

  output += `\n`
})

output += `\nChoose (1-${options.length}): _`

return output
```

**Progressive Disclosure:**
Use discovery markers (`← User discovers [feature]`) to surface hidden capabilities:
- "Save as template ← Add to UI template library"
- "Design sync ← Validate brief matches mockup"
- "/troubleshoot-juce ← Document problems for knowledge base"

### handleMenuChoice(choice, options, context)

**Purpose:** Parse user response and execute chosen action.

**Implementation:**
```javascript
// Parse response
if (isNumber(choice)) {
  const index = parseInt(choice) - 1
  if (index >= 0 && index < options.length) {
    return executeOption(options[index], context)
  } else {
    return { error: "Invalid choice", reprompt: true }
  }
}

// Handle keyword shortcuts
if (choice.toLowerCase() === "continue") {
  return executeOption(options[0], context) // First option
}

if (choice.toLowerCase() === "pause") {
  const pauseOption = options.find(o => o.label.includes("Pause"))
  return executeOption(pauseOption, context)
}

if (choice.toLowerCase() === "review") {
  const reviewOption = options.find(o => o.label.includes("Review"))
  return executeOption(reviewOption, context)
}

// Handle "Other"
if (choice.toLowerCase() === "other" || options[choice - 1].label === "Other") {
  return { action: "ask_freeform", reprompt: true }
}
```

**After executing action:**
- Re-present menu if action was exploratory (review, show code)
- Continue workflow if action was directive (continue, pause)

## Git Commit Functions

### commitStage(pluginName, stage, description)

**Purpose:** Create standardized git commit after stage completion.

**Commit message format:**
```
feat: [PluginName] Stage [N] - [description]

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>
```

**For phased stages:**
```
feat: [PluginName] Stage [N.M] - [phase description]

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>
```

**Implementation:**
1. Stage changes atomically in single commit:
   ```bash
   git add plugins/[PluginName]/Source/ (if exists)
   git add plugins/[PluginName]/.ideas/ (contracts)
   git add plugins/[PluginName]/.continue-here.md (handoff)
   git add PLUGINS.md (state)
   ```

2. Commit with standardized message using heredoc:
   ```bash
   git commit -m "$(cat <<'EOF'
   feat: [PluginName] Stage [N] - [description]

   🤖 Generated with Claude Code

   Co-Authored-By: Claude <noreply@anthropic.com>
   EOF
   )"
   ```

3. Verify commit succeeded:
   ```bash
   git log -1 --format='%h'
   ```

4. Display commit hash to user:
   ```
   ✓ Committed: abc1234 - Stage [N] complete
   ```

5. If commit fails:
   - Warn user
   - Suggest manual commit
   - Continue workflow (don't block)

**Atomic state transitions:**
- PLUGINS.md update + handoff update + code changes = Single commit
- If commit fails → Rollback state changes (or warn user about inconsistency)

**Commit variations by stage:**
- Stage 0: `feat: [Plugin] Stage 0 - research complete`
- Stage 1: `feat: [Plugin] Stage 1 - planning complete`
- Stage 2: `feat: [Plugin] Stage 2 - foundation compiles`
- Stage 3: `feat: [Plugin] Stage 3 - shell loads in DAW`
- Stage 4: `feat: [Plugin] Stage 4 - DSP complete`
- Stage 4.1: `feat: [Plugin] Stage 4.1 - core processing`
- Stage 4.2: `feat: [Plugin] Stage 4.2 - parameter modulation`
- Stage 5: `feat: [Plugin] Stage 5 - GUI complete`
- Stage 6: `feat: [Plugin] Stage 6 - validation complete`

### verifyGitAvailable()

**Purpose:** Check git is available before workflow starts.

**Implementation:**
```bash
if ! command -v git &> /dev/null; then
    echo "⚠️ Warning: git not found. Commits will be skipped."
    echo "Install git to enable automatic commit workflow."
    return false
fi

if ! git rev-parse --git-dir &> /dev/null; then
    echo "⚠️ Warning: Not a git repository. Commits will be skipped."
    echo "Run 'git init' to enable automatic commit workflow."
    return false
fi

return true
```

Call at beginning of Stage 0.

## Handoff Management Functions

### createHandoff(pluginName, stage, context)

**Purpose:** Create initial handoff file after Stage 0 completion.

**Implementation:**
1. Read handoff template from `.claude/skills/plugin-workflow/assets/continue-here-template.md`
2. Fill in YAML frontmatter:
   - plugin: [pluginName]
   - stage: [stage number]
   - phase: null (only for complex plugins)
   - status: "in_progress"
   - last_updated: [current timestamp]
   - complexity_score: null (filled in Stage 1)
   - phased_implementation: null (filled in Stage 1)
   - orchestration_mode: true (enable dispatcher pattern)
   - next_action: null (filled when stage/phase completes)
   - next_phase: null (filled for phased implementations)
3. Fill in markdown sections with context:
   - Current State: "Stage [N] - [description]"
   - Completed So Far: [what's done]
   - Next Steps: [prioritized actions]
   - Context to Preserve: [key decisions, files, build status]
4. Write to `plugins/[pluginName]/.continue-here.md`

### updateHandoff(pluginName, stage, completed, nextSteps, complexityScore, phased, nextAction, nextPhase)

**Purpose:** Update handoff file after each stage/phase completion.

**Implementation:**
1. Read existing `plugins/[pluginName]/.continue-here.md`
2. Update YAML frontmatter:
   - stage: [new stage number]
   - phase: [phase number if complex]
   - status: [in_progress | complete]
   - last_updated: [current timestamp]
   - complexity_score: [score if known]
   - phased_implementation: [true/false if known]
   - orchestration_mode: true (keep enabled for dispatcher pattern)
   - next_action: [e.g., "invoke_dsp_agent", "invoke_gui_agent"]
   - next_phase: [e.g., "4.4", "5.1"]
3. Append to "Completed So Far" section
4. Update "Next Steps" with new actions
5. Update "Context to Preserve" with latest context
6. Write back to file

**Determining next_action:**
- Stage 2 → "invoke_foundation_agent"
- Stage 3 → "invoke_shell_agent"
- Stage 4 → "invoke_dsp_agent"
- Stage 5 → "invoke_gui_agent"
- Stage 6 → "invoke_validator"
- Phased stages: specify exact phase (e.g., "4.4" for DSP phase 4)

### deleteHandoff(pluginName)

**Purpose:** Remove handoff file when plugin reaches ✅ Working or 📦 Installed.

**Implementation:**
1. Check if `plugins/[pluginName]/.continue-here.md` exists
2. Delete file
3. Log deletion (workflow complete)

**When to call:**
- After Stage 6 complete (status → ✅ Working)
- After plugin installation (status → 📦 Installed)

## Checkpoint Types

### Hard Checkpoints (MUST pause for user decision)

**All implementation stages are hard checkpoints:**
- Stage 2: Foundation complete (after foundation-agent returns)
- Stage 3: Shell complete (after shell-agent returns)
- Stage 4: DSP complete (after dsp-agent returns)
- Stage 5: GUI complete (after gui-agent returns)
- Stage 6: Validation complete

**Behavior (ENFORCED by orchestrator):**
1. Subagent completes and returns to orchestrator
2. Orchestrator auto-commits changes
3. Orchestrator updates handoff file
4. Orchestrator updates PLUGINS.md
5. Orchestrator presents decision menu
6. **Orchestrator WAITS for user response** - NEVER auto-continue
7. Orchestrator executes user choice

**Note:** Stages 0-1 checkpoints are managed by plugin-planning skill.

### Soft Checkpoints (can auto-continue with user permission)

**Phases within complex stages (complexity ≥3):**
- Stage 4.1, 4.2, 4.3: DSP phases (managed by dsp-agent)
- Stage 5.1, 5.2: GUI phases (managed by gui-agent)

**Behavior:**
1. Subagent completes phase
2. Subagent auto-commits changes
3. Subagent updates handoff file with phase info
4. Subagent presents decision menu: "Continue to next phase" (recommended)
5. If user chooses continue: subagent proceeds to next phase
6. If user chooses pause: subagent returns to orchestrator

### Decision Checkpoints

**Occur before significant choices:**
- Build failures (show 4-option menu)
- Validation failures (show 3-option menu)
- Manual pause requests

**Behavior:**
1. Subagent or orchestrator updates handoff with current context
2. Present situation-specific menu
3. Wait for user choice
4. Execute chosen path

## Resume Handling

**Support "resume automation" command:**

If user paused and says "resume automation" or chooses to continue:

1. Read `.continue-here.md` to determine current stage/phase
2. Parse YAML frontmatter for stage, phase, complexity_score, phased_implementation
3. Continue from documented "Next Steps"
4. Load relevant context (contracts, research, plan)

---

## Stage Boundary Protocol

**At every stage completion (enforced by orchestrator):**

1. **Subagent returns** to orchestrator with completion status

2. **Orchestrator shows completion statement:**

```
✓ Stage [N] complete: [description]
```

3. **Orchestrator runs automated tests** (Stages 4, 5 only):

   - Invoke plugin-testing skill
   - If fail: STOP, show results, wait for fixes
   - If pass: Continue

4. **Orchestrator auto-commits:**

```bash
git add [files]
# Message format: feat: [Plugin] Stage [N] - [description]
# For complex: feat: [Plugin] Stage [N.M] - [phase description]
```

5. **Orchestrator updates `.continue-here.md`** with new stage, timestamp, context

6. **Orchestrator updates PLUGINS.md** with new status

7. **Orchestrator presents decision menu** with context-appropriate options

8. **Orchestrator waits for user response**

**CRITICAL: Orchestrator NEVER auto-proceeds to next stage without user confirmation.**

**Subagent responsibilities:**
- Complete stage work
- Report completion status to orchestrator
- Return control to orchestrator for checkpoint

**Orchestrator responsibilities:**
- Commit changes
- Update state files (.continue-here.md, PLUGINS.md)
- Present decision menu
- Wait for user input
- Invoke next subagent based on user choice

---

## Integration Points

**Invoked by:**

- `/implement` command
- `plugin-ideation` skill (after creative brief)
- `context-resume` skill (when resuming)

**Invokes:**

- `plugin-testing` skill (Stages 4, 5, 6)
- `plugin-lifecycle` skill (after Stage 6, if user chooses install)

**Creates:**

- `.continue-here.md` (handoff file)
- `architecture.md` (Stage 0 - DSP specification)
- `plan.md` (Stage 1)
- `CHANGELOG.md` (Stage 6)
- `Presets/` directory (Stage 6)

**Updates:**

- PLUGINS.md (status changes throughout)

---

## Error Handling

**If contract files missing at Stage 1:**
Block and guide to create UI mockup first.

**If build fails at any stage:**
Present menu:

```
Build error at [stage]:
[Error context]

What would you like to do?
1. Investigate (triggers deep-research)
2. Show me the code
3. Show full build output
4. I'll fix it manually (say "resume automation" when ready)
5. Other

Choose (1-5): _
```

**If tests fail:**
Present menu with investigation options.

**If git staging fails:**
Continue anyway, log warning.

---

## Success Criteria

Workflow is successful when:

- Plugin compiles without errors
- All stages completed in sequence
- Tests pass (if run)
- PLUGINS.md updated to ✅ Working
- Handoff file deleted (workflow complete)
- Git history shows all stage commits
- Ready for installation or improvement
