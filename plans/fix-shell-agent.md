# Fix Plan: shell-agent

**Source audit:** /Users/lexchristopherson/Developer/plugin-freedom-system/audits/shell-agent-audit.md
**Target file:** /Users/lexchristopherson/Developer/plugin-freedom-system/.claude/agents/shell-agent.md
**Current grade:** B
**Target grade:** A

---

## Executive Summary

The shell-agent requires configuration metadata fixes to align with Claude Code standards. The system prompt is excellent and requires minimal changes. Primary issues: non-standard YAML field names (`allowed-tools` instead of `tools`, `type` field not recognized), missing invocation triggers in description, and preconditions stored in frontmatter instead of system prompt.

**Estimated effort:** 8 minutes
**Risk level:** Low
**Blocking issues:** 2 (tool restriction may not work, automatic invocation may fail)

---

## Fixes by Priority

### Critical (Must Fix) - 2 issues

#### Fix #1: Rename `allowed-tools` to `tools`

**Problem:**
Claude Code uses `tools` as the standard field name for tool restriction. The current `allowed-tools` field may not be recognized, potentially granting the subagent access to ALL tools instead of the restricted set. This creates a security issue where shell-agent could invoke tools like Bash or Task that it shouldn't have access to.

**Current state:**
```yaml
allowed-tools:
  - Read # Read contract files
  - Edit # Modify PluginProcessor files
  - Write # Create new files if needed
  - mcp__context7__resolve-library-id # Find JUCE library
  - mcp__context7__get-library-docs # JUCE documentation
```

**Target state:**
```yaml
tools: Read, Edit, Write, mcp__context7__resolve-library-id, mcp__context7__get-library-docs
```

**Verification:**
- [ ] YAML frontmatter contains `tools:` field (not `allowed-tools:`)
- [ ] Tools listed in comma-separated format
- [ ] No comments in tools field (moved to system prompt if needed)

---

#### Fix #2: Remove non-standard YAML fields (`type` and `preconditions`)

**Problem:**
Claude Code doesn't recognize `type: agent` or `preconditions:` fields. These fields are ignored during parsing, creating confusion about what the configuration actually controls. The preconditions contain valuable information but won't be seen by the subagent in YAML frontmatter.

**Current state:**
```yaml
name: shell-agent
type: agent
description: Implement plugin parameters and create APVTS shell (Stage 3)
allowed-tools:
  - Read # Read contract files
  - Edit # Modify PluginProcessor files
  - Write # Create new files if needed
  - mcp__context7__resolve-library-id # Find JUCE library
  - mcp__context7__get-library-docs # JUCE documentation
preconditions:
  - parameter-spec.md exists (from finalized UI mockup)
  - Stage 2 complete (foundation files exist)
  - Build system operational
```

**Target state:**
```yaml
name: shell-agent
description: Implements plugin parameters and creates APVTS shell (Stage 3). Use when plugin-workflow orchestrator invokes Stage 3 after foundation completion. Handles parameter system implementation from parameter-spec.md contract.
tools: Read, Edit, Write, mcp__context7__resolve-library-id, mcp__context7__get-library-docs
model: sonnet
```

**Verification:**
- [ ] `type` field removed from YAML
- [ ] `preconditions` field removed from YAML
- [ ] Only standard fields remain: `name`, `description`, `tools`, `model`

---

### High Priority (Should Fix) - 3 issues

#### Fix #3: Add invocation triggers to description field

**Problem:**
The current description states WHAT the subagent does but not WHEN to invoke it. Claude uses description field patterns like "Use when...", "Invoke when...", "Use proactively..." to automatically detect when to use subagents. Without these triggers, automatic invocation may fail.

**Current state:**
```yaml
description: Implement plugin parameters and create APVTS shell (Stage 3)
```

**Target state:**
```yaml
description: Implements plugin parameters and creates APVTS shell (Stage 3). Use when plugin-workflow orchestrator invokes Stage 3 after foundation completion. Handles parameter system implementation from parameter-spec.md contract.
```

**Verification:**
- [ ] Description includes "Use when..." trigger
- [ ] Description mentions plugin-workflow orchestrator (caller context)
- [ ] Description mentions Stage 3 (workflow position)
- [ ] Description mentions parameter-spec.md (key input contract)

---

#### Fix #4: Add explicit model specification

**Problem:**
No explicit model specified. While defaults work, the audit recommends documenting model choice intentionally. Stage 3 requires careful JUCE 8 API usage, making Sonnet appropriate.

**Current state:**
```yaml
# No model field present (uses default)
```

**Target state:**
```yaml
model: sonnet
```

**Verification:**
- [ ] `model: sonnet` present in YAML frontmatter
- [ ] Field appears after `tools` field for consistency

---

#### Fix #5: Move preconditions into system prompt

**Problem:**
Preconditions currently in YAML frontmatter aren't parsed by Claude Code. They need to be in the system prompt where the subagent can actually see them during execution.

**Current state (YAML):**
```yaml
preconditions:
  - parameter-spec.md exists (from finalized UI mockup)
  - Stage 2 complete (foundation files exist)
  - Build system operational
```

**Current state (System Prompt Opening):**
```markdown
# Shell Agent - Stage 3 Parameter Implementation

**Role:** Autonomous subagent responsible for implementing ALL parameters from parameter-spec.md and creating the APVTS (AudioProcessorValueTreeState) shell.

**Context:** You are invoked by the plugin-workflow skill after Stage 2 (foundation) completes. You run in a fresh context with complete specifications provided.
```

**Target state (System Prompt Opening):**
```markdown
# Shell Agent - Stage 3 Parameter Implementation

<preconditions>
Before execution, verify:
- parameter-spec.md exists (generated from finalized UI mockup)
- Stage 2 complete (foundation files exist: PluginProcessor.h/cpp, PluginEditor.h/cpp, CMakeLists.txt)
- Build system operational (CMake configured, JUCE linked)

If any precondition fails, return error report immediately (see "Contract Enforcement" section).
</preconditions>

<role>
You are an autonomous subagent responsible for implementing ALL parameters from parameter-spec.md and creating the APVTS (AudioProcessorValueTreeState - Parameter System that handles knobs, sliders, and switches).
</role>

<context>
You are invoked by the plugin-workflow skill after Stage 2 (foundation) completes. You run in a fresh context with complete specifications provided. You modify source files and return a JSON report. Build verification is handled by plugin-workflow orchestrator after you complete.
</context>
```

**Verification:**
- [ ] `<preconditions>` section added at start of system prompt
- [ ] All three preconditions present with expanded details
- [ ] Precondition failure handling references "Contract Enforcement" section
- [ ] `<role>` and `<context>` tags used for semantic structure
- [ ] Plain-language explanation added: "APVTS (AudioProcessorValueTreeState - Parameter System...)"

---

### Medium Priority (Nice to Have) - 2 issues

#### Fix #6: Add error recovery guidance to validation section

**Problem:**
When self-validation detects missing parameters, the subagent doesn't have explicit recovery steps. It just reports failure and stops. Adding recovery guidance reduces need for human intervention.

**Current state:**
```markdown
### 8. Self-Validation

Verify implementation (code only, build handled by plugin-workflow):

1. **Code verification:**
   - ✅ Read `Source/PluginProcessor.cpp`
   - ✅ Extract all `juce::ParameterID { "...", 1 }` declarations via regex
   - ✅ Compare extracted IDs with parameter-spec.md
   - ✅ Verify ALL parameter IDs from spec are present in code
   - ✅ Verify type mapping correct (Float→AudioParameterFloat, etc.)

**If validation fails:**
- List missing parameter IDs
- List mismatched types
- Set status="failure"
- Do NOT proceed
```

**Target state:**
```markdown
### 8. Self-Validation

Verify implementation (code only, build handled by plugin-workflow):

1. **Code verification:**
   - ✅ Read `Source/PluginProcessor.cpp`
   - ✅ Extract all `juce::ParameterID { "...", 1 }` declarations via regex
   - ✅ Compare extracted IDs with parameter-spec.md
   - ✅ Verify ALL parameter IDs from spec are present in code
   - ✅ Verify type mapping correct (Float→AudioParameterFloat, etc.)

**If validation fails:**

1. Re-read parameter-spec.md to confirm expected parameters
2. List missing parameter IDs in outputs.missing_parameters
3. List mismatched types in outputs.type_mismatches
4. For each missing parameter, implement it following the examples in Step 3
5. Re-run validation after adding missing parameters
6. If validation still fails after retry, set status="failure" and return error report

**Only proceed to Step 9 after validation passes.**
```

**Verification:**
- [ ] Recovery steps numbered and actionable
- [ ] Retry logic specified (implement missing, re-run validation)
- [ ] Final failure condition specified (fails after retry)
- [ ] Explicit gate before proceeding to Step 9

---

#### Fix #7: Add debugging output guidance

**Problem:**
No guidance on logging parameter extraction before implementation. When implementation fails, it's hard to diagnose whether the issue is parameter parsing or code generation.

**Current state (Step 1):**
```markdown
### 1. Parse parameter-spec.md

Read `plugins/[PluginName]/.ideas/parameter-spec.md` and extract for each parameter:

- **Parameter ID** (e.g., "gain", "delayTime", "filterCutoff")
- **Type:** Float | Choice | Bool
- **Range:** min to max (for Float)
- **Default value**
- **Skew factor** (if specified for non-linear scaling)
- **Choices** (for Choice type)
- **Units** (e.g., "dB", "ms", "Hz")
```

**Target state (Step 1):**
```markdown
### 1. Parse parameter-spec.md

Read `plugins/[PluginName]/.ideas/parameter-spec.md` and extract for each parameter:

- **Parameter ID** (e.g., "gain", "delayTime", "filterCutoff")
- **Type:** Float | Choice | Bool
- **Range:** min to max (for Float)
- **Default value**
- **Skew factor** (if specified for non-linear scaling)
- **Choices** (for Choice type)
- **Units** (e.g., "dB", "ms", "Hz")

**Debugging output:**

Before implementing parameters, output the extracted parameter list for verification:

```
Extracted parameters from parameter-spec.md:
1. gain (Float, -60.0 to 12.0 dB, default: 0.0)
2. bypass (Bool, default: false)
3. filterType (Choice, 4 options, default: Lowpass)
```

This helps diagnose parsing issues before code generation.
```

**Verification:**
- [ ] Debugging output section added
- [ ] Example output format provided
- [ ] Purpose explained (diagnose parsing issues)

---

### Low Priority (Polish) - 0 issues

**Note:** The audit suggested consolidating JUCE 8 requirements and adding parameter-spec.md format examples, but these are polish items that don't affect functionality. The current structure is clear and comprehensive.

---

## Strengths to Preserve

**DO NOT change these elements** (identified as strengths in audit):

- **XML-structured system prompt** - `<role>`, `<context>`, `<inputs>`, `<task>`, `<implementation>` sections
- **9-step implementation workflow** - Clear, sequential, comprehensive
- **Contract enforcement** - Blocks execution if parameter-spec.md missing
- **JUCE 8 specificity** - ParameterID format examples, APVTS initialization patterns
- **Real-time safety awareness** - Atomic parameter access patterns, processBlock guidelines
- **"What you DON'T do" section** - Prevents scope creep (no compilation, no build verification)
- **Required Reading injection** - juce8-critical-patterns.md reference
- **Self-validation** - Regex extraction, contract compliance checks
- **Comprehensive examples** - Float/Bool/Choice parameter implementations
- **JSON report format** - Success/failure/build-failure examples with schema reference

---

## Implementation Sequence

1. **Fix #2: Remove non-standard fields** - 1 min - Clean slate for configuration
2. **Fix #1: Rename allowed-tools to tools** - 1 min - Fix tool restriction (already part of Fix #2, but verify format)
3. **Fix #3: Add invocation triggers** - 1 min - Enable automatic invocation
4. **Fix #4: Add model specification** - 30 sec - Document model choice
5. **Fix #5: Move preconditions to system prompt** - 2 min - Make preconditions visible to agent
6. **Fix #6: Add error recovery guidance** - 2 min - Self-healing validation
7. **Fix #7: Add debugging output** - 1.5 min - Diagnostic logging

**Total estimated time:** 9 minutes

---

## Verification Checklist

After all fixes applied:

**YAML Frontmatter:**
- [ ] `name: shell-agent` present
- [ ] `description` includes "Use when..." trigger and mentions plugin-workflow
- [ ] `tools` field (not `allowed-tools`) with comma-separated list
- [ ] `model: sonnet` specified
- [ ] No `type` field
- [ ] No `preconditions` field
- [ ] YAML syntax valid

**System Prompt:**
- [ ] `<preconditions>` section at top with 3 preconditions
- [ ] `<role>` and `<context>` tags wrapping existing content
- [ ] Plain-language APVTS explanation in role
- [ ] Step 1 includes debugging output guidance
- [ ] Step 8 includes error recovery with retry logic
- [ ] All existing strengths preserved (Required Reading, examples, anti-patterns, etc.)

**File Integrity:**
- [ ] Total line count approximately 545-560 (was 543, adding ~10-15 lines)
- [ ] No existing content removed (only moved and enhanced)
- [ ] Markdown structure valid

---

## Risk Assessment

**What could go wrong:**

1. **Tool restriction breaks if format incorrect**
   - Mitigation: Use exact comma-separated format from audit example
   - Rollback: Git restore if subagent can't access Read/Edit/Write

2. **Preconditions section breaks system prompt flow**
   - Mitigation: Place at very beginning before role, use clear XML tags
   - Rollback: Move back to YAML if causes execution issues

3. **Error recovery creates infinite retry loop**
   - Mitigation: Explicit "retry once" limit specified in Step 8
   - Rollback: Remove retry logic if causes issues

**Rollback plan:**
- Git commit before changes: `git commit -m "chore: backup shell-agent before audit fixes"`
- Keep copy in `.claude/agents/backups/shell-agent-original.md`

---

## Post-Fix Testing

**How to validate the fixes:**

1. **Configuration parsing:**
   - Invoke shell-agent via plugin-workflow
   - Verify it has access to Read, Edit, Write tools
   - Verify it CANNOT access Bash, Task, or other tools

2. **Automatic invocation:**
   - Ask "implement Stage 3 for Scatter plugin"
   - Verify Claude automatically selects shell-agent based on description triggers

3. **Preconditions enforcement:**
   - Attempt to invoke shell-agent without parameter-spec.md
   - Verify it checks preconditions and returns error report immediately

4. **Error recovery:**
   - Manually create incomplete createParameterLayout() (missing 1 parameter)
   - Verify validation detects missing parameter
   - Verify it attempts to implement missing parameter
   - Verify it re-runs validation after fix

**Success criteria:**
- [ ] Agent loads without YAML parsing errors
- [ ] Tools properly restricted (only Read, Edit, Write, MCP tools available)
- [ ] Description triggers automatic invocation when "Stage 3" mentioned
- [ ] Preconditions checked before implementation starts
- [ ] Error recovery attempts fix before failing
- [ ] All existing functionality preserved

---

## Notes

**Why this matters:**
Shell-agent creates the parameter infrastructure that all other stages depend on. If tool restriction fails, it could invoke builds prematurely (violates scope boundaries). If automatic invocation fails, plugin-workflow would need manual fallback routing.

**Audit grade improvement:**
- Current: B (good functionality, configuration issues)
- Target: A (production-ready, full Claude Code compliance)
- Blockers to A: YAML field naming, missing invocation triggers
- After fixes: Should achieve A grade with exemplary PFS integration

**Related files that may need updates:**
- None - shell-agent is self-contained
- plugin-workflow already correctly invokes shell-agent via Task tool
- No other subagents depend on shell-agent's configuration
