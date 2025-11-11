---
name: plugin-workflow
description: Implementation orchestrator for stages 2-6 (Foundation through Validation)
allowed-tools:
  - Task # REQUIRED - All stages 2-5 MUST invoke subagents
  - Bash # For git commits
  - Read # For contracts
  - Write # For documentation
  - Edit # For state updates
preconditions:
  - architecture.md must exist (from /plan)
  - plan.md must exist (from /plan)
  - Status must be 🚧 Stage 1 OR resuming from 🚧 Stage 2+
  - Plugin must NOT be ✅ Working or 📦 Installed (use /improve instead)
---

# plugin-workflow Skill

**Purpose:** Pure orchestrator for stages 2-6 of JUCE plugin implementation. This skill NEVER implements directly - it always delegates to specialized subagents and presents decision menus after each stage completes.

## Overview

This skill orchestrates plugin implementation stages 2-6. Stages 0-1 (Research & Planning) are handled by the `plugin-planning` skill.

**Implementation Stages:**
- **Stage 2:** Foundation - Create build system, verify compilation (foundation-agent)
- **Stage 3:** Shell - Implement APVTS with all parameters (shell-agent)
- **Stage 4:** DSP - Implement audio processing (dsp-agent)
- **Stage 5:** GUI - Integrate WebView UI with parameter bindings (gui-agent)
- **Stage 6:** Validation - Factory presets, pluginval, CHANGELOG (direct or validator)

**CRITICAL ORCHESTRATION RULES:**
1. Stages 2-5 MUST use Task tool to invoke subagents - NEVER implement directly
2. After EVERY subagent completion, orchestrator MUST commit, update state, and present decision menu
3. Stage 6 can run directly in orchestrator or via validator subagent
4. All subagents receive Required Reading (juce8-critical-patterns.md) to prevent repeat mistakes

Each stage is fully documented in its own reference file in `references/` subdirectory.

## Precondition Checking

**Before starting, verify contracts from plugin-planning:**

1. Check for required contract files:

```bash
test -f "plugins/$PLUGIN_NAME/.ideas/architecture.md"
test -f "plugins/$PLUGIN_NAME/.ideas/plan.md"
test -f "plugins/$PLUGIN_NAME/.ideas/creative-brief.md"
```

If any missing, BLOCK with message:

```
[PluginName] is missing required planning documents.

Missing files will be listed here:
- architecture.md (from Stage 0)
- plan.md (from Stage 1)
- creative-brief.md (from ideation)

Run /plan [PluginName] to complete planning stages 0-1.
```

2. Read PLUGINS.md to verify status:

```bash
grep "^### $PLUGIN_NAME$" PLUGINS.md
```

3. Verify status is appropriate:

   - If status = 🚧 Stage 1 → OK to proceed (just finished planning)
   - If status = 🚧 Stage N (N ≥ 2) → OK to proceed (resuming implementation)
   - If status = 💡 Ideated → BLOCK with message:
     ```
     [PluginName] needs planning before implementation.
     Run /plan [PluginName] to complete stages 0-1.
     ```
   - If status = ✅ Working or 📦 Installed → BLOCK with message:
     ```
     [PluginName] is already complete.
     Use /improve [PluginName] to make changes.
     ```

---

## Resume Entry Point

**Purpose:** Handle workflow resume from .continue-here.md handoff file.

**When invoked via context-resume skill or /continue command:**

1. **Check if handoff file exists:**
   ```bash
   if [ ! -f "plugins/${PLUGIN_NAME}/.continue-here.md" ]; then
       echo "No handoff file found. Starting fresh at Stage 2."
       CURRENT_STAGE=2
   fi
   ```

2. **Parse handoff metadata:**
   ```bash
   CURRENT_STAGE=$(grep "^stage:" plugins/${PLUGIN_NAME}/.continue-here.md | awk '{print $2}')
   NEXT_ACTION=$(grep "^next_action:" plugins/${PLUGIN_NAME}/.continue-here.md | awk '{print $2}')
   NEXT_PHASE=$(grep "^next_phase:" plugins/${PLUGIN_NAME}/.continue-here.md | awk '{print $2}')
   ```

3. **Determine resume behavior:**
   - If CURRENT_STAGE in [2, 3, 4, 5]: Invoke appropriate subagent via Task tool
   - If CURRENT_STAGE = 6: Execute validation (can be direct or via validator)
   - If NEXT_ACTION is set: Use it to determine which subagent to invoke
   - If NEXT_PHASE is set: Resume phased implementation at specified phase

4. **Always use orchestration pattern:**
   - Read next_action to determine subagent
   - Invoke subagent using Task tool
   - Wait for completion
   - Commit, update state, present decision menu
   - DO NOT implement directly in orchestrator context

---

## Stage Dispatcher

**Purpose:** Pure orchestration dispatcher that ONLY invokes subagents via Task tool.

**Entry point:** Called by /implement command or /continue command after plugin-planning completes.

### CRITICAL RULE: Never Implement Directly

**This skill is a PURE ORCHESTRATOR:**
- Stages 2-5 MUST be delegated to subagents
- NEVER write plugin code directly in this skill
- ALWAYS use Task tool to invoke subagents
- Only Stage 6 validation can optionally run directly

### Implementation

1. **Determine current stage:**

```bash
# Check if handoff file exists (resuming)
if [ -f "plugins/${PLUGIN_NAME}/.continue-here.md" ]; then
    # Parse stage from handoff YAML frontmatter
    CURRENT_STAGE=$(grep "^stage:" plugins/${PLUGIN_NAME}/.continue-here.md | awk '{print $2}')
    echo "Resuming from Stage ${CURRENT_STAGE}"
else
    # Starting fresh after planning
    CURRENT_STAGE=2
    echo "Starting implementation at Stage 2"
fi
```

2. **Verify preconditions for target stage:**

See `references/state-management.md` for `checkStagePreconditions()` function.

3. **Route to subagent invocation (NEVER direct implementation):**

```javascript
async function dispatchStage(pluginName, stageNumber) {
  // Check preconditions
  const preconditionCheck = checkStagePreconditions(pluginName, stageNumber)

  if (!preconditionCheck.allowed) {
    console.log(`✗ BLOCKED: ${preconditionCheck.reason}`)
    console.log(`Action: ${preconditionCheck.action}`)
    return { status: 'blocked', reason: preconditionCheck.reason }
  }

  // ALWAYS invoke subagents via Task tool for stages 2-5
  switch(stageNumber) {
    case 2:
      // Invoke foundation-agent subagent
      return await invokeSubagent('foundation-agent', {
        pluginName,
        contracts: loadContracts(pluginName),
        requiredReading: 'juce8-critical-patterns.md'
      })
    case 3:
      // Invoke shell-agent subagent
      return await invokeSubagent('shell-agent', {
        pluginName,
        contracts: loadContracts(pluginName),
        requiredReading: 'juce8-critical-patterns.md'
      })
    case 4:
      // Invoke dsp-agent subagent
      return await invokeSubagent('dsp-agent', {
        pluginName,
        contracts: loadContracts(pluginName),
        requiredReading: 'juce8-critical-patterns.md'
      })
    case 5:
      // Invoke gui-agent subagent
      return await invokeSubagent('gui-agent', {
        pluginName,
        contracts: loadContracts(pluginName),
        requiredReading: 'juce8-critical-patterns.md'
      })
    case 6:
      // Can run directly or invoke validator subagent
      return executeStage6Validation(pluginName)  // See references/stage-6-validation.md
    default:
      return { status: 'error', reason: `Invalid stage: ${stageNumber}` }
  }
}
```

4. **Checkpoint enforcement after EVERY subagent:**

```javascript
async function runWorkflow(pluginName, startStage = 2) {
  let currentStage = startStage
  let shouldContinue = true

  while (shouldContinue && currentStage <= 6) {
    console.log(`\n━━━ Stage ${currentStage} ━━━\n`)

    // ALWAYS invoke subagent (never implement directly)
    const result = await dispatchStage(pluginName, currentStage)

    if (result.status === 'blocked' || result.status === 'error') {
      console.log(`\nWorkflow blocked: ${result.reason}`)
      return result
    }

    // CHECKPOINT: Commit, update state, present menu
    await commitStage(pluginName, currentStage, result.description)
    await updateHandoff(pluginName, currentStage + 1, result.completed, result.nextSteps)
    await updatePluginStatus(pluginName, `🚧 Stage ${currentStage}`)
    await updatePluginTimeline(pluginName, currentStage, result.description)

    // Present decision menu and WAIT for user
    const choice = presentDecisionMenu({
      stage: currentStage,
      completionStatement: result.completionStatement,
      pluginName: pluginName
    })

    // Handle user choice
    if (choice === 'continue' || choice === 1) {
      currentStage++
    } else if (choice === 'pause') {
      console.log("\n✓ Workflow paused. Resume anytime with /continue")
      shouldContinue = false
    } else {
      // Handle other menu options (review, test, etc.)
      handleMenuChoice(choice, pluginName, currentStage)
    }
  }

  if (currentStage > 6) {
    console.log("\n✓ All stages complete!")
    await updatePluginStatus(pluginName, '✅ Working')
  }
}
```

**Usage:**

```javascript
// From /implement command (after planning complete):
runWorkflow(pluginName, 2)

// From /continue command:
const handoff = readHandoffFile(pluginName)
const resumeStage = handoff.stage
runWorkflow(pluginName, resumeStage)
```

---

## Stage Reference Files

Implementation stages reference files (Stages 0-1 removed, now in plugin-planning skill):

- **[Stage 2: Foundation](references/stage-2-foundation.md)** - Create build system, verify compilation (foundation-agent)
- **[Stage 3: Shell](references/stage-3-shell.md)** - Implement APVTS with all parameters (shell-agent)
- **[Stage 4: DSP](references/stage-4-dsp.md)** - Implement audio processing (dsp-agent)
- **[Stage 5: GUI](references/stage-5-gui.md)** - Integrate WebView UI with parameter bindings (gui-agent)
- **[Stage 6: Validation](references/stage-6-validation.md)** - Factory presets, pluginval, CHANGELOG (direct or validator)
- **[State Management](references/state-management.md)** - State machine, git commits, handoffs, decision menus

**Note:** Stage reference files contain subagent prompts and context. The orchestrator reads these files to construct Task tool invocations but never implements stage logic directly.

---

## Integration Points

**Invoked by:**

- `/implement` command (after plugin-planning completes)
- `context-resume` skill (when resuming implementation stages)
- `/continue` command (for stages 2-6)

**ALWAYS invokes (via Task tool):**

- `foundation-agent` subagent (Stage 2) - REQUIRED, never implement directly
- `shell-agent` subagent (Stage 3) - REQUIRED, never implement directly
- `dsp-agent` subagent (Stage 4) - REQUIRED, never implement directly
- `gui-agent` subagent (Stage 5) - REQUIRED, never implement directly
- `validator` subagent (Stage 6) - Optional, can run directly

**Also invokes:**

- `build-automation` skill (build coordination across stages)
- `plugin-testing` skill (validation after stages 4, 5, 6)
- `plugin-lifecycle` skill (if user chooses to install after Stage 6)

**Reads (contracts from plugin-planning):**

- `architecture.md` (DSP specification from Stage 0)
- `plan.md` (implementation strategy from Stage 1)
- `creative-brief.md` (vision from ideation)
- `parameter-spec.md` (parameter definitions)

**Creates:**

- `.continue-here.md` (handoff file for checkpoints)
- `CHANGELOG.md` (Stage 6)
- `Presets/` directory (Stage 6)

**Updates:**

- PLUGINS.md (status changes after each stage)
- `.continue-here.md` (after each stage completes)

---

## Error Handling

**If contract files missing before Stage 2:**
Block and instruct user to run `/plan [PluginName]` to complete stages 0-1.

**If build fails during subagent execution:**
Subagent returns error. Orchestrator presents 4-option menu:
1. Investigate (deep-research)
2. Show me the code
3. Show full build output
4. Manual fix (resume with /continue)

**If tests fail:**
Present menu with investigation options. Do NOT auto-proceed to next stage.

**If subagent fails to complete:**
Present menu allowing retry, manual intervention, or workflow pause.

**If git staging fails:**
Continue anyway, log warning.

---

## Success Criteria

Workflow is successful when:

- All subagents (stages 2-5) invoked successfully via Task tool
- Plugin compiles without errors at each stage
- All stages completed in sequence (2 → 3 → 4 → 5 → 6)
- Decision menus presented after EVERY stage
- Tests pass (if run)
- PLUGINS.md updated to ✅ Working after Stage 6
- Handoff file updated after each stage
- Git history shows atomic commits for each stage

---

## Notes for Claude

**CRITICAL ORCHESTRATION REQUIREMENTS:**

1. **NEVER implement stages 2-5 directly** - You MUST use Task tool to invoke subagents
2. **ALWAYS present decision menu after subagent completes** - User must confirm next action
3. **ALWAYS commit after each stage** - Use `commitStage()` from state-management.md
4. **ALWAYS update state files** - .continue-here.md and PLUGINS.md after every stage
5. **ALWAYS inject Required Reading** - Pass juce8-critical-patterns.md to all subagents

**When executing this skill:**

1. Read contracts (architecture.md, plan.md) before starting
2. Verify preconditions block if contracts missing
3. Use Task tool for ALL stages 2-5 - no exceptions
4. Stage reference files contain subagent prompts, not direct implementation instructions
5. Decision menus use inline numbered lists, not AskUserQuestion tool
6. Handoff files preserve orchestration state across sessions
7. Build failures bubble up from subagents to orchestrator for menu presentation

**Common pitfalls to AVOID:**

- ❌ Implementing stage logic directly in orchestrator (ALWAYS use Task tool)
- ❌ Auto-proceeding without user confirmation (ALWAYS wait for menu choice)
- ❌ Not updating handoff file after stage completes
- ❌ Skipping decision menu after subagent returns
- ❌ Proceeding to next stage when tests fail
- ❌ Not injecting Required Reading to subagents
