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

**Purpose:** Pure orchestrator for stages 2-5 of JUCE plugin implementation. This skill NEVER implements directly - it always delegates to specialized subagents and presents decision menus after each stage completes.

## Overview

This skill orchestrates plugin implementation stages 2-5. Stages 0-1 (Research & Planning) are handled by the `plugin-planning` skill.

**Implementation Stages:**
- **Stage 2:** Foundation + Shell - Create build system and implement APVTS (foundation-shell-agent)
- **Stage 3:** DSP - Implement audio processing (dsp-agent)
- **Stage 4:** GUI - Integrate WebView UI with parameter bindings (gui-agent)
- **Stage 5:** Validation - Factory presets, pluginval, CHANGELOG (direct or validator)

<orchestration_rules enforcement_level="STRICT">
  <delegation_rule
    id="subagent-dispatch-only"
    stages="2-5"
    tool="Task"
    violation="IMMEDIATE_STOP">

    Stages 2-5 MUST be delegated to subagents via Task tool.
    This skill is a PURE ORCHESTRATOR - it NEVER implements plugin code directly.

    **Delegation sequence for every stage 2-5 invocation:**
    1. BEFORE invoking subagent, read contract files:
       - architecture.md (DSP design from Stage 0)
       - plan.md (implementation strategy from Stage 1)
       - parameter-spec.md (parameter definitions)
    2. Read Required Reading:
       - troubleshooting/patterns/juce8-critical-patterns.md (MANDATORY)
    3. Construct prompt with contracts + Required Reading prepended
    4. Invoke subagent via Task tool with constructed prompt
    5. AFTER subagent returns, execute checkpoint protocol (6 steps)

    <enforcement>
      IF stage in [2,3,4,5] AND action != "invoke_subagent_via_Task":
        STOP execution
        DISPLAY error: "Stage {stage} requires subagent delegation. Use Task tool to invoke {subagent_name}."
    </enforcement>

    <valid_delegations>
      - Stage 2: foundation-shell-agent
      - Stage 3: dsp-agent
      - Stage 4: gui-agent
    </valid_delegations>

    Stage 5 can optionally run directly in orchestrator or via validator subagent.
  </delegation_rule>

  <checkpoint_protocol
    id="stage-completion-checkpoint"
    frequency="after_every_subagent"
    auto_proceed="NEVER"
    violation="WORKFLOW_CORRUPTION">

    After EVERY subagent return (whether full stage or phase completion), orchestrator MUST execute checkpoint sequence.

    <critical_sequence enforce_order="true">
      <step order="1" required="true">Commit all changes with git</step>
      <step order="2" required="true">Update .continue-here.md with current state</step>
      <step order="3" required="true">Update PLUGINS.md status</step>
      <step order="4" required="true">Update plan.md if phased implementation</step>
      <step order="5" required="true">Present numbered decision menu</step>
      <step order="6" required="true" blocking="true">WAIT for user response</step>
    </critical_sequence>

    <enforcement>
      All 6 steps must complete in order before proceeding.
      Step 6 is blocking - NEVER auto-proceed to next stage.
    </enforcement>

    <applies_to>
      - Simple plugins (complexity ≤2): After stages 2, 3, 4, 5
      - Complex plugins (complexity ≥3): After stages 2 AND after EACH DSP/GUI phase (3.1, 3.2, 3.3+, 4.1, 4.2, 4.3+), then 5

      Note: Phase count determined by plan.md (varies by complexity)
    </applies_to>
  </checkpoint_protocol>

  <handoff_protocol id="subagent-orchestrator-handoff">
    Subagents NEVER commit - they only implement and return JSON report.
    Orchestrator handles all state management and git operations.

    <handoff_format>
      Subagent returns JSON:
      {
        "status": "success" | "error",
        "stage": 2-6,
        "completionStatement": "...",
        "filesCreated": [...],
        "nextSteps": [...]
      }
    </handoff_format>
  </handoff_protocol>

  <state_requirement id="required-reading-injection">
    All subagents (stages 2-5) MUST receive Required Reading file to prevent repeat mistakes.

    <enforcement>
      ALWAYS inject Required Reading at start of subagent prompt:

      Before invoking subagent via Task tool:
      1. Read troubleshooting/patterns/juce8-critical-patterns.md
      2. Prepend to subagent prompt with this format:

      ```typescript
      const criticalPatterns = await Read('troubleshooting/patterns/juce8-critical-patterns.md');
      const prompt = `CRITICAL PATTERNS (MUST FOLLOW):

      ${criticalPatterns}

      ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

      [Stage-specific instructions follow...]`;

      await Task({ subagent_type: "dsp-agent", description: prompt });
      ```

      This ensures subagents receive Required Reading before implementation instructions.
    </enforcement>
  </state_requirement>
</orchestration_rules>

Each stage is fully documented in its own reference file in `references/` subdirectory.

<preconditions blocking="true">
  <contract_verification required="true" blocking="true">
    Before starting Stage 2, verify these contract files exist:

    <required_file path="plugins/$PLUGIN_NAME/.ideas/architecture.md" created_by="Stage 0" />
    <required_file path="plugins/$PLUGIN_NAME/.ideas/plan.md" created_by="Stage 1" />
    <required_file path="plugins/$PLUGIN_NAME/.ideas/creative-brief.md" created_by="ideation" />
    <required_file path="plugins/$PLUGIN_NAME/.ideas/parameter-spec.md" created_by="UI mockup finalization">
      <validation>
        Stage 2 (foundation-shell-agent) requires COMPLETE parameter specification.
        Draft specification (parameter-spec-draft.md) is NOT sufficient for implementation.

        <check>
          IF parameter-spec.md exists:
            Proceed to Stage 2
          ELSE IF parameter-spec-draft.md exists:
            BLOCK with message:
            "Draft parameters found, but full specification required for implementation.

            Next step: Complete UI mockup workflow to generate parameter-spec.md

            Run: /dream [PluginName] → option 2 (Full UI mockup first)

            After mockup finalized, parameter-spec.md will be generated and Stage 2 can proceed."
          ELSE:
            BLOCK with error: "No parameter specification found. Run /dream [PluginName] to create mockup."
        </check>
      </validation>
    </required_file>

    <on_missing_files action="BLOCK">
      Display error message:
      "[PluginName] is missing required planning documents.

      Missing files will be listed here:
      - architecture.md (from Stage 0)
      - plan.md (from Stage 1)
      - creative-brief.md (from ideation)

      Run /plan [PluginName] to complete planning stages 0-1."
    </on_missing_files>

    See [references/precondition-checks.md](references/precondition-checks.md) for bash implementation.
  </contract_verification>

  <status_verification required="true" blocking="true">
    Read PLUGINS.md and verify status is appropriate:

    <allowed_state status="🚧 Stage 1">
      OK to proceed (just finished planning)
    </allowed_state>

    <allowed_state status="🚧 Stage N" condition="N >= 2">
      OK to proceed (resuming implementation)
    </allowed_state>

    <blocked_state status="💡 Ideated">
      BLOCK with message:
      "[PluginName] needs planning before implementation.
      Run /plan [PluginName] to complete stages 0-1."
    </blocked_state>

    <blocked_state status="✅ Working">
      BLOCK with message:
      "[PluginName] is already complete.
      Use /improve [PluginName] to make changes."
    </blocked_state>

    <blocked_state status="📦 Installed">
      BLOCK with message:
      "[PluginName] is already complete.
      Use /improve [PluginName] to make changes."
    </blocked_state>
  </status_verification>
</preconditions>

---

## Resume Entry Point

**Purpose:** Handle workflow resume from .continue-here.md handoff file.

<decision_gate type="resume_or_fresh_start">
  <condition check="handoff_file_exists">
    IF plugins/${PLUGIN_NAME}/.continue-here.md exists:
      - Parse YAML frontmatter
      - Extract current_stage, next_action, next_phase
      - Resume at specified stage/phase
    ELSE:
      - Start fresh at Stage 2
      - No handoff context available
  </condition>

  <routing_logic>
    Based on current_stage value:
    - Stage 2 → invoke foundation-shell-agent
    - Stage 3 → invoke dsp-agent
    - Stage 4 → invoke gui-agent
    - Stage 5 → execute validation

    If next_phase is set: Resume phased implementation at specified phase.
  </routing_logic>
</decision_gate>

### Pre-Resume Verification Checklist

When resuming via `/continue [PluginName]`, verify state integrity before proceeding:

```
Pre-Resume Checklist:
- [ ] .continue-here.md exists and is valid YAML
- [ ] verifyStateIntegrity() passes (exit 0)
- [ ] Contracts unchanged since last checkpoint (checksums match)
- [ ] Git working directory clean (no uncommitted changes)
- [ ] PLUGINS.md status matches .continue-here.md stage
```

**Verification sequence:**
1. Read .continue-here.md and parse YAML
2. Run verifyStateIntegrity() from state-management.md
3. Check contract file checksums vs. checksums in .continue-here.md
4. Run `git status --porcelain` (expect empty output)
5. Compare PLUGINS.md status emoji to .continue-here.md stage field

**If all checks pass**: Resume at stage specified in .continue-here.md
**If any fail**: Present recovery menu (reconcile / clean working directory / review changes)

---

## Stage Dispatcher

**Purpose:** Pure orchestration dispatcher that ONLY invokes subagents via Task tool.

**Entry point:** Called by /implement command or /continue command after plugin-planning completes.

<enforcement_reminder ref="subagent-dispatch-only" severity="CRITICAL">
This orchestrator MUST NEVER implement plugin code directly. ALL stages 2-5 MUST be delegated via Task tool.
</enforcement_reminder>

### Implementation

1. **Verify state integrity BEFORE dispatch:**

```bash
# CRITICAL: Verify state consistency before proceeding
# Prevents corruption from non-atomic commits or contract tampering
bash -c "$(cat references/state-management.md | grep -A100 'verifyStateIntegrity')"
```

See `references/state-management.md` for `verifyStateIntegrity()` function.

**What it checks:**
- Handoff file exists (exit 1 if missing)
- Stage consistency between .continue-here.md and PLUGINS.md (exit 2 if mismatch)
- Contract checksums match (exit 3 if tampered)
- No stale handoffs from completed plugins (exit 4, auto-cleanup)

**If verification fails:**

State mismatch detected (exit 2):
- This is a BLOCKING error - workflow cannot continue
- User must manually run `/reconcile [PluginName]` to fix
- Do NOT auto-invoke reconciliation (requires user decision on which source is correct: planning state or implementation state)
- After reconciliation completes: Resume workflow from corrected state

2. **Determine current stage:**

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

3. **Verify preconditions for target stage:**

See `references/state-management.md` for `checkStagePreconditions()` function.

<dispatcher_pattern>
  The orchestrator routes stages to subagents using this logic:

  1. Verify state integrity (verifyStateIntegrity) → BLOCK if corrupted
  2. Check preconditions → If failed, BLOCK with reason
  3. **MANDATORY: design-sync validation before Stage 2** → BLOCK if drift detected
  4. Route to subagent based on stage number:
     - Stage 2 → foundation-shell-agent (single-pass, creates build system + parameters)
     - Stage 3 → dsp-agent (phase-aware dispatch)
     - Stage 4 → gui-agent (phase-aware dispatch)
     - Stage 5 → validator (single-pass or direct execution)
  5. Pass contracts and Required Reading to subagent
  6. Wait for subagent completion

  See [references/dispatcher-pattern.md](references/dispatcher-pattern.md) for full pseudocode.
</dispatcher_pattern>

## Phase-Aware Dispatch

For Stages 3-4 with complexity ≥3, use phase-aware dispatch to incrementally implement complex plugins.

**When to use:**
- Stage 3 (DSP) or 4 (GUI)
- Complexity score ≥3 (from plan.md)
- plan.md contains phase markers (### Phase 3.X or ### Phase 4.X)

**How it works:**
1. Detect phases by scanning plan.md for phase markers
2. Loop through phases sequentially (Phase 3.1 → 3.2 → 3.3...)
3. Invoke subagent once per phase with phase-specific prompt
4. Execute checkpoint protocol after each phase
5. Present decision menu showing progress ("Phase 2 of 4 complete")

**Phase prompt construction:**
- Include all contracts (architecture.md, plan.md, parameter-spec.md)
- Include Required Reading (juce8-critical-patterns.md)
- Include ONLY current phase section from plan.md
- Mark previously completed phases

For detailed algorithm, pseudocode, and examples, see [references/phase-aware-dispatch.md](references/phase-aware-dispatch.md).

<design_sync_gate enforcement_level="MANDATORY">
  **Purpose:** Prevent design drift before implementation begins.

  **When:** BEFORE dispatching Stage 2 (foundation-agent), IF mockup exists.

  **Conditions:**
  - IF plugins/[PluginName]/.ideas/mockups/ directory exists
  - AND parameter-spec.md exists (mockup finalized)
  - THEN design-sync validation is REQUIRED

  **Implementation:**
  ```
  Before dispatching Stage 2:

  1. Check for mockup:
     - Look in plugins/[PluginName]/.ideas/mockups/
     - Find latest version (highest v[N] prefix: v1-*, v2-*, etc.)
     - If any mockup files exist: Invoke design-sync skill to validate brief ↔ mockup alignment

  2. If mockup exists:
     - Run design-sync validation automatically
     - Present findings with decision menu
     - BLOCK Stage 2 until one of:
       a) No drift detected (continue)
       b) Acceptable evolution (user confirms)
       c) Drift resolved (user updates brief or mockup)
       d) User explicitly overrides (logged)

  3. If no mockup:
     - Skip design-sync (no visual design to validate)
     - Proceed to Stage 2 directly
  ```

  **Decision menu when drift detected:**
  ```
  ⚠️ Design-brief drift detected

  [Findings from design-sync]

  Cannot proceed to Stage 2 until resolved:

  1. Update brief - Document evolution and continue
  2. Update mockup - Fix mockup to match brief
  3. Override (not recommended) - Accept drift and proceed anyway
  4. Cancel - Stop workflow, fix manually
  5. Other

  Choose (1-5): _
  ```

  **Why mandatory:**
  - Catches misalignments before Stage 2 generates boilerplate
  - Prevents implementing features not in brief
  - Prevents missing features mentioned in brief
  - Avoids 10+ minutes of wasted work on wrong implementation
  - Ensures contracts remain single source of truth

  **Override logging:**
  If user chooses override, log to .validator-overrides.yaml:
  ```yaml
  - timestamp: [ISO-8601]
    validator: design-sync
    stage: pre-stage-2
    severity: [none|attention|critical]
    override-reason: "User proceeded despite drift"
    findings: "[brief summary]"
  ```
</design_sync_gate>

4. **Checkpoint enforcement after EVERY subagent:**

<workflow_loop>
  <stage_iteration from="2" to="5">
    <dispatch_phase>
      Display: "━━━ Stage ${currentStage} ━━━"

      <delegation_enforcement ref="subagent-dispatch-only">
        Invoke subagent via Task tool (NEVER implement directly)
      </delegation_enforcement>

      IF result.status == 'blocked' OR 'error':
        Display error and return (workflow blocked)
    </dispatch_phase>

    <checkpoint_phase>
      <checkpoint_enforcement enforce_order="true">
        <step order="1" required="true" function="commitStage">
          commitStage(pluginName, currentStage, result.description)
          Auto-commit all changes from subagent completion
          VERIFY: git commit succeeded (check exit code)
        </step>

        <step order="2" required="true" function="updateHandoff">
          updateHandoff(pluginName, currentStage + 1, result.completed, result.nextSteps)
          Update .continue-here.md with new stage, timestamp, next_action
          VERIFY: File exists and contains expected stage number
        </step>

        <step order="3" required="true" function="updatePluginStatus">
          updatePluginStatus(pluginName, `🚧 Stage ${currentStage}`)
          Update PLUGINS.md status emoji in BOTH locations atomically
          VERIFY: PLUGINS.md contains new status in full entry section
        </step>

        <step order="3.5" required="true" function="validateRegistryConsistency">
          validateRegistryConsistency(pluginName)
          Verify registry table matches full entry after status update
          VERIFY: Both locations show identical status
          BLOCK: If mismatch detected, present drift resolution menu
        </step>

        <step order="4" required="true" function="updatePluginTimeline">
          updatePluginTimeline(pluginName, currentStage, result.description)
          Append timeline entry to PLUGINS.md
          VERIFY: Timeline entry exists in PLUGINS.md
        </step>

        <step order="5" required="true" function="verifyCheckpoint">
          verifyCheckpoint(pluginName, currentStage)
          Validate all checkpoint steps completed successfully
          BLOCK: If any step failed, present retry menu before continuing
        </step>

        <step order="6" required="true" blocking="true" function="presentDecisionMenu">
          presentDecisionMenu({ stage, completionStatement, pluginName })
          Present numbered decision menu and WAIT for user response
        </step>
      </checkpoint_enforcement>

      <checkpoint_verification>
        **Purpose:** Verify all checkpoint steps completed before presenting decision menu.

        **Implementation:**
        ```
        After steps 1-4 complete, run verification:

        CHECKPOINT VERIFICATION:
        ✓ Step 1: Git commit [commit-hash]
        ✓ Step 2: Handoff updated (.continue-here.md stage: N+1)
        ✓ Step 3: Plugin status updated (PLUGINS.md: 🚧 Stage N)
        ✓ Step 4: Timeline appended (PLUGINS.md: [date] Stage N complete)

        IF all verified:
          Proceed to decision menu

        IF any failed:
          Display failure report:
          ━━━ Checkpoint Incomplete ━━━
          ✗ Step 2: Handoff update failed (file not found)
          ✓ Step 1: Commit succeeded
          ✓ Step 3: Status updated
          ✓ Step 4: Timeline appended

          Cannot proceed - checkpoint integrity required.

          1. Retry failed steps - Attempt automatic fix
          2. Show details - See error messages
          3. Manual fix - I'll fix it myself (pause workflow)
          4. Other
        ```

        **Verification checks:**
        - Step 1: `git log -1 --oneline` contains stage reference
        - Step 2: `.continue-here.md` exists and contains `stage: N+1`
        - Step 3: `PLUGINS.md` contains `**Status:** 🚧 Stage N`
        - Step 4: `PLUGINS.md` timeline has entry dated today for Stage N

        **Why critical:**
        Incomplete checkpoints cause state corruption:
        - Missing handoff → /continue can't resume
        - Missing status → PLUGINS.md out of sync
        - Missing timeline → no audit trail
        - Missing commit → changes lost on crash
      </checkpoint_verification>
    </checkpoint_phase>

    <decision_gate blocking="true">
      WAIT for user response (NEVER auto-proceed)

      <routing>
        IF choice == 'continue' OR choice == 1:
          currentStage++
        ELSE IF choice == 'pause':
          Display: "✓ Workflow paused. Resume anytime with /continue"
          Exit workflow loop
        ELSE:
          handleMenuChoice(choice, pluginName, currentStage)
      </routing>
    </decision_gate>
  </stage_iteration>

  <completion_check>
    IF currentStage > 5:
      Display: "✓ All stages complete!"
      updatePluginStatus(pluginName, '✅ Working')
  </completion_check>
</workflow_loop>

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

<reference_files>
  Each stage has a reference file containing subagent prompt templates:

  - [stage-2-foundation-shell.md](references/stage-2-foundation-shell.md) - foundation-shell-agent
  - [stage-3-dsp.md](references/stage-3-dsp.md) - dsp-agent
  - [stage-4-gui.md](references/stage-4-gui.md) - gui-agent
  - [stage-5-validation.md](references/stage-5-validation.md) - validator
  - [state-management.md](references/state-management.md) - State machine functions
  - [dispatcher-pattern.md](references/dispatcher-pattern.md) - Routing logic
  - [precondition-checks.md](references/precondition-checks.md) - Contract validation

  <usage_pattern>
    Orchestrator reads reference files → constructs Task invocation → NEVER implements directly
  </usage_pattern>
</reference_files>

---

## Integration Points

**Invoked by:**

- `/implement` command (after plugin-planning completes)
- `context-resume` skill (when resuming implementation stages)
- `/continue` command (for stages 2-5)

**ALWAYS invokes (via Task tool):**

- `foundation-shell-agent` subagent (Stage 2) - REQUIRED, never implement directly
- `dsp-agent` subagent (Stage 3) - REQUIRED, never implement directly
- `gui-agent` subagent (Stage 4) - REQUIRED, never implement directly
- `validator` subagent (Stage 5) - Optional, can run directly

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

- All subagents (stages 2-4) invoked successfully via Task tool
- Plugin compiles without errors at each stage
- All stages completed in sequence (2 → 3 → 4 → 5)
- Decision menus presented after EVERY stage
- Tests pass (if run)
- PLUGINS.md updated to ✅ Working after Stage 5
- Handoff file updated after each stage
- Git history shows atomic commits for each stage

---

## Decision Menu Protocol

**Use AskUserQuestion tool ONLY when:**
- NEVER in this skill - plugin-workflow is pure orchestration
- All decisions are workflow navigation, not ideation

**Use Inline Numbered Menu for:**
- After EVERY stage completion (checkpoint gates)
- Build failure recovery options
- Test failure investigation options
- Phase completion (for complex plugins)
- Examples:
  - "Stage 3 complete. What's next? 1) Continue to Stage 4..."
  - "Build failed. What should I do? 1) Retry build..."
  - "Phase 4.1 complete. 1) Continue to Phase 4.2..."

**Key difference:** This skill ONLY uses inline menus. No creative decisions, only workflow navigation.

**Inline menu format:**

```
✓ [Stage/Phase completion statement]

What's next?

1. [Continue to next stage/phase] (recommended)
2. [Run tests] - Validate current implementation
3. [Pause workflow] - Save progress and exit
4. [Review changes] - Show git diff
5. Other

Choose (1-5): _
```

ALWAYS wait for user response. NEVER auto-proceed.

---

## Integration Contracts

Summary of subagent and system component contracts:

| Component | Stage | Input | Output | Purpose |
|-----------|-------|-------|--------|---------|
| foundation-shell-agent | 2 | Contracts + Required Reading | JSON report | Create build system + implement APVTS |
| dsp-agent | 3 | Contracts + Required Reading | JSON report | Implement audio processing |
| gui-agent | 4 | Contracts + Required Reading | JSON report | Integrate WebView UI |
| validator | 1-4 | Stage-specific expectations | JSON report | Advisory validation |
| build-automation | 2-5 | Plugin name + build config | Build result | Verify compilation |
| context-resume | N/A | Handoff context | Workflow resumption | Resume from checkpoint |
| /implement | N/A | Plugin name | Full workflow | Entry point command |

For detailed contract specifications, JSON schemas, error handling patterns, and examples, see [references/integration-contracts.md](references/integration-contracts.md).

---

## Error Handling

All errors follow a consistent format: **What failed** → **Why** → **How to fix** → **Alternatives**

Common error scenarios:
- Subagent returns invalid JSON → Fallback parsing → Present raw output if all parsing fails
- Schema validation failure → Block progression → Offer retry or risky continue
- Build failure → Present build log excerpt → Retry/investigate/manual fix/rollback
- State file corruption → Reconstruct from git log → User verifies before continuing
- Checkpoint step failure → Attempt remaining steps → Present partial status

Graceful degradation when components unavailable (validator, build-automation, Required Reading).

For detailed error patterns, recovery strategies, and reporting format, see [references/error-handling.md](references/error-handling.md).

---

<execution_guidance>
  <critical_reminders>
    <reminder priority="CRITICAL" ref="subagent-dispatch-only">
      NEVER implement stages 2-5 directly - MUST use Task tool to invoke subagents
    </reminder>

    <reminder priority="CRITICAL" ref="stage-completion-checkpoint">
      ALWAYS present decision menu after subagent completes - user MUST confirm next action
    </reminder>

    <reminder priority="HIGH">
      ALWAYS commit after each stage using commitStage() from state-management.md
    </reminder>

    <reminder priority="HIGH">
      ALWAYS update state files (.continue-here.md and PLUGINS.md) after every stage
    </reminder>

    <reminder priority="HIGH">
      ALWAYS inject Required Reading (juce8-critical-patterns.md) to all subagents
    </reminder>
  </critical_reminders>

  <execution_checklist>
    When executing this skill:
    1. Read contracts (architecture.md, plan.md) before starting
    2. Verify preconditions - block if contracts missing
    3. Use Task tool for ALL stages 2-5 - no exceptions
    4. Stage reference files contain subagent prompts, not direct implementation instructions
    5. Decision menus use inline numbered lists, not AskUserQuestion tool
    6. Handoff files preserve orchestration state across sessions
    7. Build failures bubble up from subagents to orchestrator for menu presentation
  </execution_checklist>

  <anti_patterns>
    Common pitfalls to AVOID:

    <anti_pattern severity="CRITICAL" ref="subagent-dispatch-only">
      ❌ Implementing stage logic directly in orchestrator
      ✓ ALWAYS use Task tool to invoke appropriate subagent
    </anti_pattern>

    <anti_pattern severity="CRITICAL" ref="phase-aware-dispatch">
      ❌ Sending "Implement ALL phases" to subagent for Stages 4-5
      ✓ ALWAYS detect phases in plan.md and loop through them one at a time

      This error caused DrumRoulette Stage 5 to fail with compilation errors.
      Phase-aware dispatch (lines 270-578) is MANDATORY for complex plugins.
    </anti_pattern>

    <anti_pattern severity="CRITICAL">
      ❌ Auto-proceeding without user confirmation
      ✓ ALWAYS wait for menu choice after presenting options
    </anti_pattern>

    <anti_pattern severity="HIGH">
      ❌ Skipping phase detection for Stages 3-4 when complexity ≥3
      ✓ Read plan.md to check for phases BEFORE invoking dsp-agent or gui-agent
    </anti_pattern>

    <anti_pattern severity="HIGH">
      ❌ Not updating handoff file after stage completes
      ✓ Update .continue-here.md immediately after subagent returns
    </anti_pattern>

    <anti_pattern severity="HIGH">
      ❌ Skipping decision menu after subagent returns
      ✓ Present context-appropriate menu at every checkpoint
    </anti_pattern>

    <anti_pattern severity="MEDIUM">
      ❌ Proceeding to next stage when tests fail
      ✓ Present investigation menu and wait for user decision
    </anti_pattern>

    <anti_pattern severity="MEDIUM">
      ❌ Not injecting Required Reading to subagents
      ✓ Always pass juce8-critical-patterns.md to prevent repeat mistakes
    </anti_pattern>
  </anti_patterns>
</execution_guidance>
