# Subagent Audit Report: shell-agent

**File:** .claude/agents/shell-agent.md
**Audited:** 2025-11-12
**Auditor:** Claude Code (Subagent Best Practices)

---

## Executive Summary

The shell-agent is a well-structured, task-specific subagent with clear documentation and implementation guidance. It demonstrates strong adherence to PFS (Plugin Freedom System) integration patterns, including Required Reading injection and contract enforcement. However, it uses non-standard configuration field names that deviate from Claude Code conventions, and lacks proactive invocation triggers in its description field.

**Overall Grade:** B

- Strengths: Excellent system prompt structure, comprehensive documentation, PFS-specific patterns
- Issues: Non-standard YAML fields, description lacks invocation triggers, tool specification uses non-canonical format

---

## Configuration Analysis

### Required Fields

#### name: `shell-agent`
- ✅ Status: Compliant
- Finding: Follows lowercase-with-hyphens convention, unique within PFS ecosystem
- Recommendation: None

#### description: `Implement plugin parameters and create APVTS shell (Stage 3)`
- ⚠️ Status: Partial Compliance
- Finding: Clearly states WHAT the subagent does, but lacks invocation triggers for automatic selection. Does not include phrases like "Use when...", "Use proactively...", or "Invoke when..." that help Claude automatically detect when to use this subagent.
- Recommendation: Revise to include invocation context. Example: "Implements plugin parameters and creates APVTS shell (Stage 3). Use when plugin-workflow orchestrator invokes Stage 3 after foundation completion. Handles parameter system implementation from parameter-spec.md contract."

### Optional Fields

#### tools: (configured as `allowed-tools`)
- ❌ Status: Non-compliant field name
- Finding: Uses `allowed-tools` instead of standard `tools` field name. Lists: Read, Edit, Write, mcp__context7__resolve-library-id, mcp__context7__get-library-docs
- Tool selection is appropriate for the task (read contracts, modify code, fetch JUCE docs), but field name doesn't match Claude Code conventions.
- Recommendation: Rename `allowed-tools` to `tools`. Use comma-separated format as shown in best practices:
  ```yaml
  tools: Read, Edit, Write, mcp__context7__resolve-library-id, mcp__context7__get-library-docs
  ```

#### model: (not specified)
- ⚠️ Status: Uses default (implicit)
- Finding: No explicit model specified, will use default subagent model (typically sonnet). Given the complexity of Stage 3 (parameter implementation with JUCE 8 API specifics), this is appropriate but not documented.
- Recommendation: Consider explicitly setting `model: sonnet` for clarity, or document the rationale for using the default.

### Non-Standard Fields

#### type: `agent`
- ❌ Status: Not recognized by Claude Code
- Finding: Includes `type: agent` field which is not part of Claude Code subagent specification
- Recommendation: Remove this field - it serves no purpose in Claude Code's subagent system

#### preconditions:
- ❌ Status: Not recognized by Claude Code
- Finding: Lists preconditions as YAML array, but this field isn't parsed by Claude Code. The information is valuable but should be in the system prompt, not frontmatter.
- Recommendation: Remove from YAML frontmatter and incorporate into system prompt under a `<preconditions>` section

---

## System Prompt Analysis

### Structure & Organization

✅ **Excellent** - Uses clear XML-like sections with semantic tags:
- `<role>`, `<context>`, `<inputs>`, `<task>`, `<implementation>`, etc.
- Logical flow from role definition → context → inputs → implementation steps → validation → output format
- Progressive disclosure: high-level overview followed by detailed implementation guidance

The prompt structure is one of the strongest aspects of this subagent.

### Specificity & Focus

✅ **Excellent** - Highly task-specific, not a generic helper:
- Clear, narrow role: "implement ALL parameters from parameter-spec.md and create APVTS shell"
- Explicit scope boundaries: "You modify source files and return JSON report. You do NOT compile or verify builds."
- Lists specific files to modify (PluginProcessor.h/cpp, PluginEditor.cpp)
- Provides exact code examples for JUCE 8 API usage

**Notable strengths:**
- Includes anti-patterns: "What you DON'T do" section prevents scope creep
- JUCE 8 specificity with migration guidance (ParameterID format, APVTS initialization)
- Real-time safety notes for audio processing context

### Completeness

✅ **Excellent** - Comprehensive coverage:

**Workflow:**
1. Parse parameter-spec.md
2. Create APVTS in PluginProcessor.h
3. Implement createParameterLayout()
4. Initialize APVTS in constructor
5. Implement state management
6. Update processBlock() (pass-through)
7. Update editor placeholder
8. Self-validation
9. Return JSON report

**Constraints:**
- Contract enforcement (blocks if parameter-spec.md missing)
- Zero-drift guarantee (spec IDs must match code IDs exactly)
- JUCE 8 API requirements with migration examples
- Real-time safety guidelines

**Verification:**
- Self-validation checklist (code verification, contract compliance)
- Regex pattern for parameter extraction
- Failure handling with specific error messages

**Output Format:**
- Detailed JSON schema reference (`.claude/schemas/subagent-report.json`)
- Success, validation failure, and build failure examples
- Required fields documented

**Missing elements:**
- No explicit guidance on error recovery (what to do when self-validation fails)
- No mention of logging or debugging output during implementation

### PFS Integration

✅ **Excellent** - Strong integration with Plugin Freedom System:

**Contract File References:**
- Explicitly references `parameter-spec.md` (CRITICAL input)
- References `creative-brief.md` and `architecture.md` for context
- Blocks execution if parameter-spec.md is missing (contract enforcement)

**Required Reading Injection:**
- Includes "CRITICAL: Required Reading" section
- Points to `troubleshooting/patterns/juce8-critical-patterns.md`
- Lists specific patterns relevant to Stage 3 (APVTS, parameter IDs, processBlock safety)

**Dispatcher Pattern Understanding:**
- Acknowledges fresh context: "You run in a fresh context with complete specifications provided"
- Clear handoff protocol: "Build verification handled by plugin-workflow → build-automation skill after you complete"
- JSON report format enables clean orchestration

**Stage-Specific Context:**
- Understands Stage 3 position in workflow (after foundation, before DSP)
- References previous stage outputs (foundation files)
- References next stage requirements (DSP will use parameters)

---

## Best Practices Compliance

| Practice | Status | Notes |
|----------|--------|-------|
| Lowercase-with-hyphens naming | ✅ | `shell-agent` follows convention |
| Descriptive name | ✅ | Clear indication of purpose |
| Description includes invocation triggers | ❌ | Missing "Use when...", "Invoke when..." context |
| Tools field (comma-separated) | ❌ | Uses `allowed-tools` instead of `tools`, but correct format otherwise |
| Model field (optional) | ⚠️ | Not specified (uses default) - acceptable but undocumented |
| Task-specific (not generic) | ✅ | Highly specific to Stage 3 parameter implementation |
| XML-structured system prompt | ✅ | Excellent use of semantic XML sections |
| Clear role definition | ✅ | Role, context, and scope clearly defined |
| Explicit workflow steps | ✅ | 9-step implementation workflow documented |
| Constraints and anti-patterns | ✅ | "What you DON'T do" section prevents scope creep |
| Success criteria | ✅ | Detailed success/failure conditions |
| Output format guidance | ✅ | JSON schema with examples |
| Appropriate tool access | ✅ | Read/Edit/Write + MCP tools for JUCE docs - appropriate for code modification |
| Avoid generic "helper" language | ✅ | No generic language, all specific to parameter implementation |

---

## Recommendations

### Critical (Must Fix)

1. **Fix YAML field names to match Claude Code conventions**
   - Current: `allowed-tools`
   - Required: `tools`
   - Impact: Claude Code may not recognize `allowed-tools` field, potentially granting all tools instead of restricted set

2. **Remove non-standard YAML fields**
   - Remove: `type: agent` (not recognized by Claude Code)
   - Remove: `preconditions:` (valuable info but should be in system prompt)
   - Impact: Prevents confusion about what configuration does; makes subagent portable

### High Priority (Should Fix)

3. **Add invocation triggers to description field**
   - Current: "Implement plugin parameters and create APVTS shell (Stage 3)"
   - Improved: "Implements plugin parameters and creates APVTS shell (Stage 3). Use when plugin-workflow orchestrator invokes Stage 3 after foundation completion. Handles parameter system implementation from parameter-spec.md contract."
   - Impact: Helps Claude automatically detect when to use this subagent; critical for automatic invocation pattern

4. **Explicitly specify model choice**
   - Add: `model: sonnet` (or `inherit` if meant to match main conversation)
   - Rationale: Stage 3 implementation requires careful JUCE 8 API usage - sonnet is appropriate
   - Impact: Makes model selection explicit and intentional

5. **Move preconditions into system prompt**
   - Add `<preconditions>` section at the beginning of system prompt
   - Move current YAML `preconditions` content there
   - Impact: Ensures preconditions are actually visible to the subagent during execution

### Medium Priority (Nice to Have)

6. **Add error recovery guidance**
   - Under "Self-Validation" section, add explicit recovery steps when validation fails
   - Example: "If parameter count mismatch detected, re-read parameter-spec.md and identify missing parameters, then implement them before re-running validation"
   - Impact: Reduces need for human intervention when self-validation detects issues

7. **Add debugging output guidance**
   - Suggest logging parameter extraction results before implementation
   - Example: "Before implementing parameters, output extracted parameter list for verification"
   - Impact: Easier to diagnose issues when implementation fails

### Low Priority (Polish)

8. **Consolidate JUCE 8 requirements**
   - Currently scattered throughout prompt
   - Consider creating a dedicated `<juce8_requirements>` section near the top
   - Impact: Easier to reference JUCE version-specific patterns

9. **Add examples of common parameter-spec.md formats**
   - Show 2-3 different parameter specification formats that might appear in parameter-spec.md
   - Impact: Handles variability in contract format more robustly

---

## Strengths

- **Exceptional documentation quality**: The system prompt is tutorial-grade, with clear examples, anti-patterns, and migration guidance
- **Strong contract enforcement**: Blocks execution if parameter-spec.md is missing, preventing wasted work
- **JUCE 8 specificity**: Explicitly calls out JUCE 8 API changes with before/after examples (ParameterID format, APVTS initialization)
- **Real-time safety awareness**: Understands audio thread safety constraints and provides correct atomic parameter access patterns
- **Zero-drift guarantee**: Self-validation ensures parameter spec exactly matches implementation
- **Clear scope boundaries**: "What you DON'T do" prevents scope creep and clarifies orchestration boundaries
- **Comprehensive examples**: Includes Float/Bool/Choice parameter examples with correct JUCE API usage
- **Required Reading integration**: References juce8-critical-patterns.md before implementation
- **Structured JSON output**: Enables clean orchestration with plugin-workflow skill

---

## Example Improvements

### Before (YAML frontmatter):
```yaml
---
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
---
```

### After (YAML frontmatter):
```yaml
---
name: shell-agent
description: Implements plugin parameters and creates APVTS shell (Stage 3). Use when plugin-workflow orchestrator invokes Stage 3 after foundation completion. Handles parameter system implementation from parameter-spec.md contract.
tools: Read, Edit, Write, mcp__context7__resolve-library-id, mcp__context7__get-library-docs
model: sonnet
---
```

### Before (System Prompt - Opening):
```markdown
# Shell Agent - Stage 3 Parameter Implementation

**Role:** Autonomous subagent responsible for implementing ALL parameters from parameter-spec.md and creating the APVTS (AudioProcessorValueTreeState) shell.

**Context:** You are invoked by the plugin-workflow skill after Stage 2 (foundation) completes. You run in a fresh context with complete specifications provided.
```

### After (System Prompt - Opening):
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

### Before (Missing Error Recovery):
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

### After (With Error Recovery):
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

---

## Conclusion

**Production-ready:** Yes, with minor fixes

The shell-agent is fundamentally sound and demonstrates excellent understanding of PFS patterns, JUCE 8 API requirements, and subagent orchestration. The system prompt is comprehensive, well-structured, and provides exceptional implementation guidance.

**Priority level for improvements:** Medium

The critical issues are primarily configuration metadata (YAML field names) rather than functional problems. The subagent will work correctly in most cases, but fixing the YAML fields ensures proper tool restriction and makes invocation behavior more predictable.

**Recommended action plan:**

1. **Immediate (before next use):**
   - Fix `allowed-tools` → `tools`
   - Remove `type` and `preconditions` from YAML
   - Add invocation triggers to `description`

2. **Short-term (this week):**
   - Move preconditions into system prompt
   - Add explicit `model: sonnet`
   - Add error recovery guidance to validation section

3. **Long-term (future enhancement):**
   - Consolidate JUCE 8 requirements into dedicated section
   - Add debugging output guidance
   - Add examples of parameter-spec.md format variations

**Overall assessment:** This subagent represents strong subagent authoring with PFS-specific expertise. With configuration metadata fixes, it will be an exemplary reference implementation for other PFS subagents.
