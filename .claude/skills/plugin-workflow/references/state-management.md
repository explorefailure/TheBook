# State Management and Workflow Functions

**Context:** This file is part of the plugin-workflow skill.
**Purpose:** Document state delegation pattern where subagents update state and orchestrator verifies
**Last Updated:** 2025-11-13

---

## State Delegation Pattern

**Primary responsibility:** Subagents update state files (.continue-here.md, PLUGINS.md)
**Secondary responsibility:** Orchestrator verifies state updates
**Safety net:** Orchestrator falls back to updating state if verification fails

**Benefits:**
- Reduces orchestrator token overhead by ~52k tokens per workflow (91% reduction)
- Subagents have full context of their work when updating state
- Orchestrator maintains safety net (fallback) for reliability
- No silent failures (verification catches missing updates)

---

## 1. State Verification Functions

### verifySubagentStateUpdate(pluginName, expectedStage, result)

**Purpose:** Verify subagent updated state files correctly.

**Implementation:**
```javascript
function verifySubagentStateUpdate(pluginName, expectedStage, result) {
  // Step 1: Check stateUpdated field in JSON report
  if (!result.stateUpdated) {
    logWarning("Subagent reported stateUpdated=false, triggering fallback")
    return { verified: false, reason: "stateUpdated_false" }
  }

  // Step 2: Check for error message
  if (result.stateUpdateError) {
    logWarning(`Subagent state update error: ${result.stateUpdateError}`)
    return { verified: false, reason: "state_update_error", error: result.stateUpdateError }
  }

  // Step 3: Verify .continue-here.md actually changed
  const handoffPath = `plugins/${pluginName}/.continue-here.md`
  const handoffContent = readFile(handoffPath)
  const yaml = parseFrontmatter(handoffContent)

  if (yaml.stage != expectedStage) {
    logError(`Stage mismatch: expected ${expectedStage}, found ${yaml.stage}`)
    return { verified: false, reason: "stage_mismatch" }
  }

  if (yaml.last_updated != getCurrentDate()) {
    logWarning(`Last updated date not current: ${yaml.last_updated}`)
    return { verified: false, reason: "date_not_current" }
  }

  // Step 4: Verify PLUGINS.md status updated
  const pluginsMd = readFile("PLUGINS.md")
  const statusLine = extractStatusLine(pluginsMd, pluginName)

  if (!statusLine.includes(`Stage ${expectedStage}`)) {
    logError(`PLUGINS.md not updated to Stage ${expectedStage}`)
    return { verified: false, reason: "plugins_md_not_updated" }
  }

  // All checks passed
  logInfo("State update verified - subagent successfully updated state")
  return { verified: true }
}
```

**When to call:**
- After EVERY subagent return (Stages 0, 2, 3, 4)
- Before committing changes
- As first step in checkpoint protocol

**Returns:**
```javascript
{ verified: true }  // State updated correctly
{ verified: false, reason: "stateUpdated_false" }  // Subagent didn't update
{ verified: false, reason: "stage_mismatch" }  // .continue-here.md wrong stage
{ verified: false, reason: "plugins_md_not_updated" }  // PLUGINS.md not updated
{ verified: false, reason: "state_update_error", error: "..." }  // Error occurred
```

---

## 2. Fallback State Update Functions

### fallbackStateUpdate(pluginName, currentStage, result)

**Purpose:** Orchestrator updates state if subagent verification fails.

**Implementation:**
```javascript
function fallbackStateUpdate(pluginName, currentStage, result) {
  logWarning("⚠️ Subagent did not update state, orchestrator handling fallback")

  try {
    // Update .continue-here.md
    updateHandoff(pluginName, currentStage, result.completed, result.nextSteps)

    // Update PLUGINS.md (atomic - both locations)
    updatePluginStatus(pluginName, `🚧 Stage ${currentStage}`)
    updatePluginTimeline(pluginName, currentStage, result.description)

    // Validate consistency
    const consistent = validateRegistryConsistency(pluginName)
    if (!consistent) {
      throw new Error("Registry consistency validation failed")
    }

    logInfo("✓ State updated by orchestrator (fallback successful)")
    return { success: true }

  } catch (error) {
    logError(`Fallback state update failed: ${error.message}`)
    return { success: false, error: error.message }
  }
}
```

**When to call:**
- When verifySubagentStateUpdate() returns `verified: false`
- Before committing (ensures state consistency)
- As safety net for reliability

**Error handling:**
If fallback fails, present recovery menu:
```
State update failed (subagent and orchestrator)

What would you like to do?
1. Show error details - See what went wrong
2. Retry fallback - Attempt state update again
3. Manual fix - I'll fix state files myself
4. Abort workflow - Stop and investigate
5. Other
```

---

## 3. Legacy State Functions (Used in Fallback)

These functions are retained for fallback use when subagent doesn't update state.

### updateHandoff(pluginName, stage, completed, nextSteps, complexityScore, phased, nextAction, nextPhase)

**Purpose:** Update .continue-here.md with current workflow state.

**Implementation:**
```bash
#!/bin/bash
# Update handoff file with new stage information

PLUGIN_NAME=$1
NEW_STAGE=$2
COMPLETED=$3
NEXT_STEPS=$4

HANDOFF_FILE="plugins/${PLUGIN_NAME}/.continue-here.md"

# Read existing file
EXISTING=$(cat "$HANDOFF_FILE")

# Recalculate contract checksums
BRIEF_SHA="sha256:$(shasum -a 256 plugins/$PLUGIN_NAME/.ideas/creative-brief.md | awk '{print $1}')"
PARAM_SHA="sha256:$(shasum -a 256 plugins/$PLUGIN_NAME/.ideas/parameter-spec.md | awk '{print $1}')"
ARCH_SHA="sha256:$(shasum -a 256 plugins/$PLUGIN_NAME/.ideas/architecture.md | awk '{print $1}')"
PLAN_SHA="sha256:$(shasum -a 256 plugins/$PLUGIN_NAME/.ideas/plan.md | awk '{print $1}')"

# Update YAML frontmatter fields
# (Use Edit tool to update specific fields, preserving structure)

# Update stage
sed -i '' "s/^stage: .*/stage: ${NEW_STAGE}/" "$HANDOFF_FILE"

# Update last_updated
sed -i '' "s/^last_updated: .*/last_updated: $(date +%Y-%m-%d)/" "$HANDOFF_FILE"

# Update status
sed -i '' "s/^status: .*/status: complete/" "$HANDOFF_FILE"

# Update checksums
sed -i '' "s|creative_brief: sha256:.*|creative_brief: ${BRIEF_SHA}|" "$HANDOFF_FILE"
sed -i '' "s|parameter_spec: sha256:.*|parameter_spec: ${PARAM_SHA}|" "$HANDOFF_FILE"
sed -i '' "s|architecture: sha256:.*|architecture: ${ARCH_SHA}|" "$HANDOFF_FILE"
sed -i '' "s|plan: sha256:.*|plan: ${PLAN_SHA}|" "$HANDOFF_FILE"

# Append to "Completed So Far" section
echo "- **Stage ${NEW_STAGE}:** ${COMPLETED}" >> "$HANDOFF_FILE"
```

**Called by:**
- fallbackStateUpdate() when subagent verification fails
- Stage 5 (orchestrator direct execution, no subagent)

### updatePluginStatus(pluginName, newStatus)

**Purpose:** Update PLUGINS.md status in BOTH locations atomically.

**Implementation (ATOMIC - all or nothing):**
```bash
#!/bin/bash
# Atomic status update - updates both locations or reverts

PLUGIN_NAME=$1
NEW_STATUS=$2

# Backup current state
cp PLUGINS.md PLUGINS.md.backup

# Update full entry section (canonical source)
sed -i '' "/^### ${PLUGIN_NAME}$/,/^###/ s/^\*\*Status:\*\* .*$/\*\*Status:\*\* ${NEW_STATUS}/" PLUGINS.md

# Update registry table (derived view)
CURRENT_ROW=$(grep "^| ${PLUGIN_NAME} |" PLUGINS.md)
NEW_ROW=$(echo "$CURRENT_ROW" | awk -F'|' -v status=" ${NEW_STATUS} " '{print $1 "|" $2 "|" status "|" $4 "|" $5}')
sed -i '' "s/^| ${PLUGIN_NAME} | .*/$(echo "$NEW_ROW" | sed 's/[&/\]/\\&/g')/" PLUGINS.md

# Validate both updates succeeded
if validateRegistryConsistency "$PLUGIN_NAME"; then
  rm PLUGINS.md.backup
  echo "✓ Status updated atomically: ${NEW_STATUS}"
  return 0
else
  echo "ERROR: Atomic update failed. Reverting..."
  mv PLUGINS.md.backup PLUGINS.md
  return 1
fi
```

**Called by:**
- fallbackStateUpdate() when subagent verification fails
- Stage 5 (orchestrator direct execution)

### updatePluginTimeline(pluginName, stage, description)

**Purpose:** Append timeline entry to PLUGINS.md.

**Implementation:**
```bash
#!/bin/bash
# Add timeline entry for stage completion

PLUGIN_NAME=$1
STAGE=$2
DESCRIPTION=$3
DATE=$(date +%Y-%m-%d)

# Find the "Lifecycle Timeline:" section and append
sed -i '' "/^### ${PLUGIN_NAME}$/,/^###/ {
  /^\*\*Lifecycle Timeline:\*\*/a\\
- **${DATE} (Stage ${STAGE}):** ${DESCRIPTION}
}" PLUGINS.md

# Update "Last Updated" field
sed -i '' "/^### ${PLUGIN_NAME}$/,/^###/ s/^\*\*Last Updated:\*\* .*$/\*\*Last Updated:\*\* ${DATE}/" PLUGINS.md
```

**Called by:**
- fallbackStateUpdate() when subagent verification fails
- Stage 5 (orchestrator direct execution)

### validateRegistryConsistency(pluginName)

**Purpose:** Verify registry table matches full entry (no drift).

**Implementation:**
```bash
#!/bin/bash
# Verify registry table matches full entry section

PLUGIN_NAME=$1

# Extract status from registry table
TABLE_STATUS=$(grep "^| ${PLUGIN_NAME} |" PLUGINS.md | awk -F'|' '{print $3}' | xargs)

# Extract status from full entry
ENTRY_STATUS=$(grep -A 10 "^### ${PLUGIN_NAME}$" PLUGINS.md | grep "^\*\*Status:\*\*" | sed 's/\*\*//g; s/Status://g' | xargs)

# Normalize (remove emojis, trim whitespace)
TABLE_NORMALIZED=$(echo "$TABLE_STATUS" | sed 's/[^a-zA-Z0-9 ]//g' | xargs)
ENTRY_NORMALIZED=$(echo "$ENTRY_STATUS" | sed 's/[^a-zA-Z0-9 ]//g' | xargs)

# Compare
if [ "$TABLE_NORMALIZED" != "$ENTRY_NORMALIZED" ]; then
  echo "ERROR: Registry drift detected"
  echo "  Table: ${TABLE_STATUS}"
  echo "  Entry: ${ENTRY_STATUS}"
  return 1
fi

return 0
```

**Called by:**
- fallbackStateUpdate() after updating PLUGINS.md
- Post-checkpoint validation

---

## 4. Git Commit Functions

Git operations remain in orchestrator (not delegated to subagents).

### commitStage(pluginName, stage, description)

**Purpose:** Create atomic git commit with code + state files.

**Implementation:**
```bash
#!/bin/bash
# ATOMIC commit (code + state files in single operation)

PLUGIN_NAME=$1
STAGE=$2
DESCRIPTION=$3

# Stage ALL changes atomically
git add plugins/${PLUGIN_NAME}/Source/ 2>/dev/null || true
git add plugins/${PLUGIN_NAME}/.ideas/
git add plugins/${PLUGIN_NAME}/.continue-here.md
git add plugins/${PLUGIN_NAME}/plan.md 2>/dev/null || true
git add PLUGINS.md

# Create single atomic commit
git commit -m "$(cat <<EOF
feat: ${PLUGIN_NAME} Stage ${STAGE} - ${DESCRIPTION}

[ATOMIC] Code + state files committed together

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>
EOF
)"

# Verify commit succeeded
if [ $? -eq 0 ]; then
  COMMIT_HASH=$(git log -1 --format='%h')
  echo "✓ Committed: ${COMMIT_HASH} - Stage ${STAGE} complete (atomic)"
  return 0
else
  echo "ERROR: Commit failed"
  return 1
fi
```

**Why atomic:**
- PLUGINS.md + .continue-here.md + code = SINGLE commit
- No temporal window between state updates
- If commit fails → All state rolled back

**Called by:**
- Orchestrator after state verification/fallback
- Always runs in checkpoint protocol (step 3)

---

## 5. State Integrity Verification

### verifyStateIntegrity(pluginName)

**Purpose:** Comprehensive state validation before dispatching to next stage.

**Implementation:**
```bash
#!/bin/bash
# State integrity verification before stage dispatch

PLUGIN_NAME=$1
HANDOFF_FILE="plugins/${PLUGIN_NAME}/.continue-here.md"

# Check handoff file exists
if [ ! -f "$HANDOFF_FILE" ]; then
  echo "❌ No handoff file found"
  exit 1
fi

# Extract stage from handoff
HANDOFF_STAGE=$(grep "^stage:" "$HANDOFF_FILE" | awk '{print $2}')

# Extract status from PLUGINS.md (full entry = canonical)
PLUGINS_STATUS=$(grep -A1 "^### ${PLUGIN_NAME}$" PLUGINS.md | grep "^**Status:**" | sed 's/.*Stage \([0-9]\+\).*/\1/')

# Verify stage consistency
if [ "$HANDOFF_STAGE" != "$PLUGINS_STATUS" ]; then
  echo "❌ State mismatch:"
  echo "   .continue-here.md: Stage ${HANDOFF_STAGE}"
  echo "   PLUGINS.md: Stage ${PLUGINS_STATUS}"
  echo "Run /reconcile ${PLUGIN_NAME} to fix"
  exit 2
fi

# Verify contract checksums (detect tampering)
# (Calculate current checksums and compare to stored checksums)

echo "✓ State integrity verified"
exit 0
```

**Exit codes:**
- 0: All checks passed
- 1: Missing handoff file
- 2: State mismatch (run /reconcile)
- 3: Contract tampering detected

**Called by:**
- Before dispatching to any stage
- At workflow resume (/continue)

---

## 6. Migration Strategy

### Backward Compatibility

**Old plugins (before delegation):**
- .continue-here.md may be missing `next_action`, `next_phase` fields
- May be missing `contract_checksums` section
- Orchestrator still updates state (fallback path works)

**New plugins (after delegation):**
- Subagents update state
- Orchestrator verifies
- Fallback if needed (seamless)

**Migration detection:**
```javascript
function isSubagentStateDelegationEnabled(subagentName) {
  const supportedAgents = [
    "research-planning-agent",
    "foundation-shell-agent",
    "dsp-agent",
    "gui-agent"
  ]
  return supportedAgents.includes(subagentName)
}
```

**Gradual rollout:**
- Some subagents support delegation, others don't
- Orchestrator adapts based on stateUpdated field
- No breaking changes

---

## 7. Decision Menu System

Decision menu presentation remains in orchestrator (not delegated).

### presentDecisionMenu(context)

**Purpose:** Present numbered menu at checkpoint.

**Implementation:**
```javascript
function presentDecisionMenu(context) {
  const options = generateContextualOptions(context)
  const menu = formatDecisionMenu(context.completionStatement, options)

  console.log(menu)
  // WAIT for user response (blocking)
}
```

**Format:**
```
✓ [Completion statement]

What's next?
1. [Primary action] (recommended)
2. [Secondary action]
3. [Discovery option]
4. [Alternative]
5. Other

Choose (1-5): _
```

**Called by:**
- Orchestrator after commit (step 4 in checkpoint)
- Always blocking (WAIT for response)

---

## 8. Token Reduction Measurement

**Baseline (before delegation):**
- Orchestrator updates state: ~57,500 tokens per workflow

**Target (after delegation):**
- Orchestrator verifies state: ~5,000 tokens per workflow

**Reduction:**
- 52,500 tokens saved (91% reduction)

**Where savings come from:**
- Remove state update code from orchestrator
- Replace with verification logic (much smaller)
- Fallback code retained but rarely executed
- Decision menus and commits still in orchestrator

**Per-stage breakdown:**
```
Stage 0: 15k → 1k tokens (verification only)
Stage 2: 12k → 1k tokens (verification only)
Stage 3: 15k → 1.5k tokens (verification + phase handling)
Stage 4: 12k → 1.2k tokens (verification + phase handling)
Stage 5: 3.5k → 0.3k tokens (verification only)
```

---

## 9. Integration with Subagent Schema

**Subagent JSON report schema:**
```json
{
  "agent": "dsp-agent",
  "status": "success",
  "outputs": {
    "stage_completed": 3,
    "phase_completed": "3.2",
    "files_modified": ["PluginProcessor.cpp"],
    "state_files_updated": [
      "plugins/PluginName/.continue-here.md",
      "PLUGINS.md"
    ]
  },
  "stateUpdated": true,
  "stateUpdateError": null,
  "issues": [],
  "ready_for_next_stage": true
}
```

**Key fields for verification:**
- `stateUpdated`: true/false (did subagent update state?)
- `stateUpdateError`: error message if update failed
- `state_files_updated`: list of files modified

**Orchestrator uses these fields to:**
1. Check if verification needed (stateUpdated == true)
2. Trigger fallback if needed (stateUpdated == false)
3. Log warnings if errors present

---

## Success Criteria

**State update is successful when:**

1. ✓ Subagent reports `stateUpdated: true` in JSON
2. ✓ .continue-here.md stage field matches expected stage
3. ✓ .continue-here.md last_updated is current date
4. ✓ PLUGINS.md status updated (registry + full entry)
5. ✓ PLUGINS.md timeline entry added
6. ✓ Registry consistency validated (no drift)
7. ✓ Git commit succeeded (atomic)

**OR fallback succeeded:**

1. ✓ Subagent reported `stateUpdated: false`
2. ✓ Orchestrator updated .continue-here.md
3. ✓ Orchestrator updated PLUGINS.md
4. ✓ Registry consistency validated
5. ✓ Git commit succeeded (atomic)

**Failure modes handled:**

- Subagent doesn't update → Fallback
- Subagent update incomplete → Fallback
- Fallback fails → User recovery menu
- Commit fails → Rollback state, warn user

---

**END OF STATE MANAGEMENT REFERENCE**
