# Fix Plan: dsp-agent

**Source audit:** `./audits/dsp-agent-audit.md`
**Target file:** `.claude/agents/dsp-agent.md`
**Current grade:** A-
**Target grade:** A

---

## Executive Summary

The dsp-agent is the highest-rated subagent (A-) with exceptional task specificity and comprehensive documentation. The audit identified 8 improvements across configuration and documentation that will elevate it to A grade and establish it as the reference implementation for other subagents.

**Estimated effort:** 12 minutes
**Risk level:** Low
**Blocking issues:** 1 (non-standard `allowed-tools` field)

---

## Fixes by Priority

### Critical (Must Fix) - 1 issue

#### Fix #1: Rename `allowed-tools` to `tools`

**Problem:**
The frontmatter uses `allowed-tools` instead of the standard Claude Code `tools` field. This non-standard field name may not be recognized by the runtime, potentially failing to restrict tool access as intended.

**Current state:**
```yaml
allowed-tools:
  - Read # Read contract files
  - Edit # Modify PluginProcessor files
  - Write # Create DSP utility files if needed
  - mcp__context7__resolve-library-id # Find JUCE library
  - mcp__context7__get-library-docs # JUCE DSP documentation
  - mcp__sequential-thinking__sequentialthinking # For complex DSP (complexity >= 4)
```

**Target state:**
```yaml
tools: Read, Edit, Write, mcp__context7__resolve-library-id, mcp__context7__get-library-docs, mcp__sequential-thinking__sequentialthinking
```

**Verification:**
- [ ] Field renamed from `allowed-tools` to `tools`
- [ ] Tools listed as comma-separated string (not YAML array)
- [ ] All 6 tools preserved in new format
- [ ] No inline comments in YAML (moved to system prompt if needed)

---

### High Priority (Should Fix) - 3 issues

#### Fix #2: Enhance description with proactive triggers

**Problem:**
The description is clear but lacks trigger phrases for automatic invocation. Adding "PROACTIVELY" and specific trigger scenarios improves discoverability and ensures the workflow orchestrator knows when to invoke this agent.

**Current state:**
```yaml
description: Implement audio processing and DSP algorithms (Stage 4)
```

**Target state:**
```yaml
description: Implement audio processing and DSP algorithms for Stage 4. Use PROACTIVELY after shell-agent completes Stage 3, or when user requests DSP implementation, audio processing, or processBlock implementation.
```

**Verification:**
- [ ] Description includes "PROACTIVELY" keyword
- [ ] Mentions "after shell-agent completes Stage 3"
- [ ] Lists specific user request triggers (DSP implementation, audio processing, processBlock)
- [ ] Retains Stage 4 context

---

#### Fix #3: Add model selection clarification

**Problem:**
The system prompt documents conditional model selection (Opus vs. Sonnet based on complexity), but this creates ambiguity about who makes the decision. The orchestrator (plugin-workflow) actually handles model selection, not the subagent itself.

**Current state:**
```markdown
**Model Selection:** This agent uses **conditional model assignment**:

- **Opus + Extended Thinking:** Complexity score ≥4 (complex DSP, multiple algorithms, advanced features)
- **Sonnet (Standard):** Complexity score ≤3 (straightforward DSP, single algorithm, simple processing)
```

**Target state:**
```markdown
<model_selection>
## Model Selection

**Orchestrator responsibility:** The plugin-workflow skill selects the model based on complexity score from plan.md:

- **Complexity ≥4:** Invokes dsp-agent with Opus model + sequential-thinking tool
  - Use for: Complex DSP, multiple algorithms, advanced features
  - Enables: Deep algorithm design, performance optimization analysis

- **Complexity ≤3:** Invokes dsp-agent with Sonnet model (default)
  - Use for: Straightforward DSP, single algorithm, simple processing
  - Enables: Fast, cost-effective implementation

**Note:** This subagent does not self-select models. Model assignment is handled by the orchestrator before invocation.
</model_selection>
```

**Verification:**
- [ ] Wrapped in `<model_selection>` XML tags
- [ ] Clarifies orchestrator handles selection (not subagent)
- [ ] Documents both model tiers with use cases
- [ ] Adds explicit note about external assignment
- [ ] Placed at top of system prompt (after role/context, before workflow)

---

#### Fix #4: Add `model: sonnet` to frontmatter

**Problem:**
Model selection is documented in the system prompt but not in frontmatter. Adding the default model to frontmatter aligns configuration with documentation and provides explicit default behavior.

**Current state:**
```yaml
---
name: dsp-agent
type: agent
description: Implement audio processing and DSP algorithms (Stage 4)
allowed-tools:
  [...]
preconditions:
  [...]
---
```

**Target state:**
```yaml
---
name: dsp-agent
description: Implement audio processing and DSP algorithms for Stage 4. Use PROACTIVELY after shell-agent completes Stage 3, or when user requests DSP implementation, audio processing, or processBlock implementation.
tools: Read, Edit, Write, mcp__context7__resolve-library-id, mcp__context7__get-library-docs, mcp__sequential-thinking__sequentialthinking
model: sonnet
---
```

**Verification:**
- [ ] `model: sonnet` added to frontmatter
- [ ] Placed after `tools` field
- [ ] Documents default model (orchestrator can override for complexity ≥4)

---

### Medium Priority (Nice to Have) - 3 issues

#### Fix #5: Move preconditions to system prompt

**Problem:**
The `preconditions` field in frontmatter may not be honored by Claude Code runtime. Moving it to the system prompt allows the subagent to actually read and verify these conditions at invocation.

**Current state (frontmatter):**
```yaml
preconditions:
  - architecture.md exists (DSP component specifications)
  - parameter-spec.md exists (parameter definitions)
  - plan.md exists (complexity score, phase breakdown if complex)
  - Stage 3 complete (parameters implemented in APVTS)
```

**Target state (system prompt - add after Model Selection section):**
```markdown
<preconditions>
## Precondition Verification

Before starting DSP implementation, verify these conditions are met:

1. **architecture.md exists** - Contains DSP component specifications and processing chain
2. **parameter-spec.md exists** - Defines all parameters and their DSP mappings
3. **plan.md exists** - Contains complexity score and phase breakdown (if complex)
4. **Stage 3 complete** - APVTS parameters implemented in shell

**If any precondition fails:**
```json
{
  "agent": "dsp-agent",
  "status": "failure",
  "outputs": {
    "error_type": "precondition_failure",
    "missing_files": ["architecture.md"],
    "error_message": "Cannot implement DSP without architecture specifications"
  },
  "issues": ["Precondition failure: architecture.md not found"],
  "ready_for_next_stage": false
}
```

Return immediately without attempting implementation.
</preconditions>
```

**Verification:**
- [ ] Preconditions removed from frontmatter
- [ ] Preconditions added to system prompt with XML tags
- [ ] Includes verification instructions
- [ ] Provides failure report example
- [ ] Placed before "YOUR ROLE (READ THIS FIRST)" section

---

#### Fix #6: Add explicit XML tags to major sections

**Problem:**
The system prompt uses Markdown headers rather than explicit XML tags. Adding XML structure improves LLM parsing and consistency with Claude Code best practices.

**Current state:**
```markdown
## YOUR ROLE (READ THIS FIRST)

You implement DSP algorithms and return a JSON report...

## Inputs (Contracts)

You will receive the following contract files...

## Task

Implement audio processing from architecture.md...
```

**Target state:**
```markdown
<role>
## YOUR ROLE (READ THIS FIRST)

You implement DSP algorithms and return a JSON report...
</role>

<inputs>
## Inputs (Contracts)

You will receive the following contract files...
</inputs>

<task>
## Task

Implement audio processing from architecture.md...
</task>
```

**Apply to these sections:**
- `<role>` - YOUR ROLE (READ THIS FIRST)
- `<inputs>` - Inputs (Contracts)
- `<task>` - Task
- `<required_reading>` - CRITICAL: Required Reading
- `<complexity_aware>` - Complexity-Aware Implementation
- `<workflow>` - Implementation Steps
- `<json_report>` - JSON Report Format
- `<safety_checklist>` - Real-Time Safety Checklist
- `<best_practices>` - JUCE DSP Best Practices
- `<success_criteria>` - Success Criteria
- `<model_and_thinking>` - Model and Extended Thinking
- `<next_stage>` - Next Stage

**Verification:**
- [ ] All major sections wrapped in XML tags
- [ ] Opening and closing tags match
- [ ] Markdown headers preserved inside XML blocks
- [ ] No syntax errors introduced

---

#### Fix #7: Add error recovery guidance

**Problem:**
No guidance on handling malformed contracts or unexpected states. Adding error recovery instructions makes the agent more robust.

**Target state (add new section after Precondition Verification):**
```markdown
<error_recovery>
## Error Recovery

If contracts are malformed or missing critical information:

1. **Document the specific missing/invalid data**
   - List missing sections, invalid formats, or conflicting specifications

2. **Return failure report immediately**
   ```json
   {
     "agent": "dsp-agent",
     "status": "failure",
     "outputs": {
       "error_type": "invalid_contract",
       "contract_file": "architecture.md",
       "error_message": "architecture.md missing 'DSP Components' section"
     },
     "issues": [
       "Contract validation failed: architecture.md incomplete",
       "Required section 'DSP Components' not found"
     ],
     "ready_for_next_stage": false
   }
   ```

3. **Include specific guidance on what needs correction**
   - Reference the expected contract format
   - Suggest which planning stage needs to be rerun

4. **Do NOT attempt to guess or infer missing specifications**
   - Never assume component types or parameter mappings
   - Contract violations should block implementation

**Common contract issues:**
- Missing DSP component specifications
- Invalid parameter mappings (parameter ID doesn't exist)
- Conflicting complexity scores (plan.md vs. architecture.md)
- Empty or malformed sections
</error_recovery>
```

**Verification:**
- [ ] Section added after Precondition Verification
- [ ] Wrapped in `<error_recovery>` XML tags
- [ ] Includes failure report example
- [ ] Documents 4-step process
- [ ] Lists common issues

---

### Low Priority (Polish) - 1 issue

#### Fix #8: Remove `type: agent` field

**Problem:**
The `type: agent` field is not documented in Claude Code best practices. Verify if it's actually used by the runtime; if not, remove to reduce frontmatter clutter.

**Current state:**
```yaml
---
name: dsp-agent
type: agent
description: [...]
```

**Target state:**
```yaml
---
name: dsp-agent
description: [...]
```

**Verification:**
- [ ] Verify `type: agent` is not used by Claude Code runtime
- [ ] If unused, remove from frontmatter
- [ ] If used by PFS system, document in system prompt why it's needed

**Note:** This fix requires verification before implementation. Check other subagents and skill configurations to determine if `type` field is system convention.

---

## Strengths to Preserve

**DO NOT change these elements** (identified as strengths in audit):

### Configuration Strengths
- Lowercase-with-hyphens name: `dsp-agent`
- Clear role definition: "Autonomous subagent responsible for implementing audio processing algorithms"

### System Prompt Strengths
- Exceptional task specificity (DSP implementation only, Stage 4 only)
- Clear separation of concerns ("What you DON'T do" section)
- Complexity-aware workflows (different paths for simple vs. complex plugins)
- Comprehensive code examples for every major pattern
- Real-time safety focus (critical for audio processing)
- Phased implementation protocol (4.1 → 4.2 → 4.3 for complex plugins)
- Self-validation checklist
- JSON report schema with multiple examples
- Required Reading integration (`juce8-critical-patterns.md`)
- Detailed workflow steps (numbered 1-10 with sub-steps)
- JUCE DSP best practices section
- Common DSP patterns with working code (delay lines, filters, gain staging)

### PFS Integration Strengths
- References all contract files (architecture.md, parameter-spec.md, plan.md)
- Understands dispatcher pattern (fresh context per invocation)
- JSON report format for orchestrator communication
- No build responsibility (defers to build-automation skill)
- Complexity-aware from plan.md (reads phase breakdown)

---

## Implementation Sequence

1. **Fix #1: Rename `allowed-tools` to `tools`** - 1 min - Critical compliance fix, must be done first
2. **Fix #4: Add `model: sonnet` to frontmatter** - 1 min - Completes frontmatter configuration
3. **Fix #8: Remove `type: agent` (after verification)** - 1 min - Clean up frontmatter
4. **Fix #2: Enhance description with proactive triggers** - 1 min - Improve discoverability
5. **Fix #5: Move preconditions to system prompt** - 2 min - Enables runtime verification
6. **Fix #3: Add model selection clarification** - 2 min - Document orchestrator responsibility
7. **Fix #7: Add error recovery guidance** - 2 min - Improve robustness
8. **Fix #6: Add explicit XML tags to major sections** - 2 min - Structural enhancement

**Total estimated time:** 12 minutes

---

## Verification Checklist

After all fixes applied:

### Frontmatter Validation
- [ ] Uses `tools` field (not `allowed-tools`)
- [ ] Tools listed as comma-separated string
- [ ] All 6 tools preserved: Read, Edit, Write, mcp__context7__resolve-library-id, mcp__context7__get-library-docs, mcp__sequential-thinking__sequentialthinking
- [ ] `description` includes proactive triggers
- [ ] `model: sonnet` specified
- [ ] `preconditions` field removed (moved to system prompt)
- [ ] `type: agent` removed (if unused by runtime)
- [ ] YAML syntax valid

### System Prompt Validation
- [ ] Model selection clarification added with XML tags
- [ ] Precondition verification section added
- [ ] Error recovery guidance added
- [ ] All major sections wrapped in XML tags
- [ ] All strengths preserved (code examples, workflows, safety rules)
- [ ] Markdown syntax valid

### Functional Validation
- [ ] Agent loads without errors in Claude Code
- [ ] Tools properly restricted to 6 specified tools
- [ ] Description triggers automatic invocation in workflow context
- [ ] All strengths documented in audit remain intact

---

## Risk Assessment

**What could go wrong:**

1. **YAML syntax errors after frontmatter changes**
   - Mitigation: Validate YAML structure before committing
   - Rollback: Restore from git history

2. **Removing `type: agent` breaks PFS system**
   - Mitigation: Verify field usage before removal (check other subagents)
   - Rollback: Re-add field if needed

3. **XML tags create parsing issues**
   - Mitigation: Test with simple invocation after changes
   - Rollback: Remove XML wrapper tags if issues occur

4. **Description changes prevent orchestrator from finding agent**
   - Mitigation: Preserve "Stage 4" and "DSP" keywords
   - Rollback: Restore original description

**Rollback plan:**
1. Git commit before changes: `git commit -m "checkpoint: before dsp-agent fixes"`
2. Keep backup copy: `.claude/agents/backups/dsp-agent-original.md`
3. If issues occur: `git checkout HEAD~1 .claude/agents/dsp-agent.md`

---

## Post-Fix Testing

**How to validate the fixes:**

### 1. Syntax validation
```bash
# Verify YAML frontmatter is valid
head -20 .claude/agents/dsp-agent.md | python3 -c "import sys, yaml; yaml.safe_load(sys.stdin)"
```

### 2. Agent loading test
- Invoke dsp-agent in a test context
- Verify no parsing errors
- Confirm tools are restricted correctly

### 3. Workflow integration test
- Run `/implement [TestPlugin]` through Stage 3
- Verify orchestrator auto-invokes dsp-agent for Stage 4
- Check that model selection happens correctly (Sonnet for simple, Opus for complex)

### 4. Contract verification test
- Create plugin with missing architecture.md
- Invoke dsp-agent
- Verify precondition failure report generated

### 5. Comparison with audit
- Review each audit recommendation
- Confirm all critical and high-priority fixes applied
- Verify all strengths preserved

**Success criteria:**
- [ ] Agent loads without errors
- [ ] Tools properly restricted to 6 specified
- [ ] Description triggers automatic invocation
- [ ] Preconditions verified at runtime
- [ ] Error recovery works for malformed contracts
- [ ] All audit strengths preserved
- [ ] YAML and Markdown syntax valid
- [ ] Model selection documentation clear

---

## Notes

### Implementation order rationale
- Critical fixes first (tools field) ensure runtime compatibility
- Frontmatter fixes grouped together for efficient editing
- System prompt enhancements done after frontmatter is stable
- XML tagging done last (largest change, easiest to rollback)

### Model selection context
The orchestrator (plugin-workflow skill) reads `plan.md` complexity score and dynamically selects the model:
- Complexity ≤3: Invokes dsp-agent with Sonnet (default from frontmatter)
- Complexity ≥4: Invokes dsp-agent with Opus + sequential-thinking override

The `model: sonnet` in frontmatter provides the default; the orchestrator can override for complex plugins.

### Reference for other subagents
After these fixes, dsp-agent becomes the A-grade reference implementation. Apply similar patterns to:
- foundation-agent (A- grade)
- shell-agent (B+ grade)
- gui-agent (B+ grade)

### Verification importance
The `type: agent` field must be verified before removal. Check:
1. Other subagent configurations (foundation-agent, shell-agent, gui-agent)
2. Orchestrator code in plugin-workflow skill
3. PFS documentation for agent configuration standards

If `type: agent` is a PFS convention, keep it and document its purpose in system prompt.
