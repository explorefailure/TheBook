# Subagent Audit Report: foundation-agent

**File:** .claude/agents/foundation-agent.md
**Audited:** 2025-11-12
**Auditor:** Claude Code (Subagent Best Practices)

---

## Executive Summary

The foundation-agent demonstrates excellent structure and documentation but suffers from several critical configuration issues that prevent it from functioning as a proper Claude Code subagent. The system prompt is comprehensive and well-organized, but the YAML frontmatter uses non-standard fields that Claude Code doesn't recognize, rendering the configuration ineffective.

**Overall Grade:** D
- Critical configuration issues block effective operation
- Excellent system prompt structure and PFS integration
- Custom fields not recognized by Claude Code
- Missing standard optional fields that would enhance functionality

---

## Configuration Analysis

### Required Fields

#### name: `foundation-agent`
- ✅ **Status:** Compliant
- **Finding:** Follows lowercase-with-hyphens convention and is descriptive.
- **Recommendation:** None needed.

#### description: `Create JUCE plugin project structure (Stage 2)`
- ⚠️ **Status:** Partial Compliance
- **Finding:** Description is too terse and lacks invocation triggers. It states WHAT the agent does but not WHEN Claude should invoke it. Missing proactive trigger keywords like "Use when" or "Invoke after".
- **Recommendation:** Expand to include invocation context and trigger phrases:
  ```yaml
  description: Creates JUCE plugin project structure (Stage 2 - Foundation). Use when plugin-workflow orchestrator needs to generate initial CMakeLists.txt and skeleton source files after Stage 1 planning completes. Invoked automatically during /implement workflow after contracts are validated.
  ```

### Optional Fields

#### tools: (uses non-standard `allowed-tools`)
- ❌ **Status:** Non-compliant
- **Finding:** Uses custom field name `allowed-tools` which Claude Code doesn't recognize. The standard field is `tools` (comma-separated string, not YAML array). Current configuration likely results in the agent inheriting ALL tools instead of the restricted set intended.
- **Recommendation:** Replace with standard `tools` field:
  ```yaml
  tools: Read, Write, mcp__context7__resolve-library-id, mcp__context7__get-library-docs
  ```

#### model: (missing)
- ⚠️ **Status:** Missing Optional Field
- **Finding:** No model specified. Will default to system's subagent model (usually sonnet). For Stage 2 foundation work, this is acceptable but should be explicit.
- **Recommendation:** Add explicit model choice:
  ```yaml
  model: sonnet
  ```
  Rationale: Stage 2 requires code generation and JUCE knowledge but doesn't need Opus-level reasoning. Sonnet provides good balance of speed and capability.

### Non-Standard Fields

#### type: `agent`
- ❌ **Status:** Non-standard
- **Finding:** Claude Code doesn't use a `type` field. This is redundant (file location already indicates it's an agent).
- **Recommendation:** Remove this field.

#### preconditions: (array)
- ❌ **Status:** Non-standard
- **Finding:** Claude Code doesn't parse or enforce `preconditions` fields. These are documentation-only and create false expectations. Precondition validation must be implemented in the system prompt logic.
- **Recommendation:** Remove from frontmatter and incorporate into system prompt's workflow section:
  ```markdown
  ## Workflow

  ### Step 1: Verify Preconditions

  Before proceeding, verify these contract files exist:
  - `plugins/[PluginName]/.ideas/creative-brief.md`
  - `plugins/[PluginName]/.ideas/architecture.md`
  - `plugins/[PluginName]/.ideas/plan.md`

  If any are missing, return failure status immediately.
  ```

---

## System Prompt Analysis

### Structure & Organization
**Grade: A**

Exceptional use of XML-style headers for organization. The prompt is divided into clear sections:
- Role definition (concise and specific)
- Inputs/Outputs (contract files clearly identified)
- Critical reading requirements (juce8-critical-patterns.md)
- Implementation steps (numbered, actionable)
- Validation criteria
- Error handling

The use of code examples with template placeholders (`[PluginName]`) is excellent for clarity.

### Specificity & Focus
**Grade: A**

This is a model example of task-specific (not generic) subagent design:
- **Specific role:** "Create JUCE plugin project structure" (not "help with plugins")
- **Clear boundaries:** Explicitly states what it does AND doesn't do (no compilation, no build verification)
- **Domain expertise:** Deep JUCE 8 knowledge baked into templates
- **Stage-specific:** Tied to Stage 2 of a defined workflow

The "YOUR ROLE (READ THIS FIRST)" section with explicit "What you do / What you DON'T do" lists is exceptional clarity.

### Completeness
**Grade: A**

The prompt covers all essential elements:
- **Workflow:** 8-step implementation process with validation
- **Constraints:** No compilation, no build verification (delegated to build-automation)
- **Verification criteria:** File existence checks, JSON schema conformance
- **Output format:** Detailed JSON schema with success/failure examples
- **Error handling:** Contract violations, file creation failures

The inclusion of "Real-Time Safety" considerations even in Stage 2 demonstrates thoroughness.

### PFS Integration
**Grade: A**

Excellent integration with Plugin Freedom System patterns:
- **Contract references:** Explicitly reads creative-brief.md, architecture.md, plan.md
- **Required Reading injection:** Mandatory reading of juce8-critical-patterns.md before implementation
- **Dispatcher pattern awareness:** Understands it runs in fresh context, returns to orchestrator
- **State management:** Returns structured JSON conforming to `.claude/schemas/subagent-report.json`
- **Separation of concerns:** Knows build verification is handled by plugin-workflow → build-automation

---

## Best Practices Compliance

| Practice | Status | Notes |
|----------|--------|-------|
| Lowercase-with-hyphens naming | ✅ | `foundation-agent` follows convention |
| Clear invocation triggers in description | ⚠️ | Too terse, missing "Use when" context |
| Task-specific (not generic) | ✅ | Highly specific to Stage 2 foundation work |
| XML structure in system prompt | ✅ | Excellent organization with headers |
| Specific workflow steps | ✅ | 8 numbered steps with validation |
| Error handling defined | ✅ | Contract violations, file errors covered |
| Success criteria specified | ✅ | File creation + JSON report format |
| Standard `tools` field (comma-separated) | ❌ | Uses non-standard `allowed-tools` array |
| Explicit model selection | ⚠️ | Missing, defaults to system setting |
| Avoid non-standard fields | ❌ | Uses `type` and `preconditions` fields |
| Focus on capabilities AND constraints | ✅ | Explicit "What you DON'T do" section |

---

## Recommendations

### Critical (Must Fix)

1. **Replace `allowed-tools` with standard `tools` field**

   **Issue:** Current configuration uses non-standard field name that Claude Code doesn't recognize, likely resulting in unintended tool access.

   **Fix:**
   ```yaml
   tools: Read, Write, mcp__context7__resolve-library-id, mcp__context7__get-library-docs
   ```

2. **Expand description to include invocation triggers**

   **Issue:** Description doesn't help Claude determine WHEN to invoke this subagent.

   **Fix:**
   ```yaml
   description: Creates JUCE plugin project structure (Stage 2 - Foundation). Use when plugin-workflow orchestrator needs to generate initial CMakeLists.txt and skeleton source files after Stage 1 planning completes. Invoked automatically during /implement workflow after contracts are validated.
   ```

### High Priority (Should Fix)

3. **Remove non-standard `type` field**

   **Issue:** Claude Code doesn't use this field; creates confusion.

   **Fix:** Delete the `type: agent` line from frontmatter.

4. **Remove non-standard `preconditions` field**

   **Issue:** Claude Code doesn't parse or enforce this; creates false expectations of automatic validation.

   **Fix:** Delete the `preconditions` section from frontmatter. Precondition checks are already correctly implemented in the system prompt under "Contract Enforcement" section.

5. **Add explicit model selection**

   **Issue:** Model choice should be intentional and documented.

   **Fix:**
   ```yaml
   model: sonnet
   ```

   **Rationale:** Stage 2 requires JUCE code generation knowledge but not Opus-level reasoning. Sonnet balances speed and capability appropriately.

### Medium Priority (Nice to Have)

6. **Add explicit verification step for Required Reading**

   **Issue:** System prompt references juce8-critical-patterns.md but doesn't verify agent actually reads it.

   **Enhancement:** Add verification checkpoint in system prompt:
   ```markdown
   ### Step 0: Read Critical Patterns

   Before proceeding, you MUST read:
   `troubleshooting/patterns/juce8-critical-patterns.md`

   Verify you understand these non-negotiable patterns:
   1. juce_generate_juce_header() placement
   2. Individual module headers vs JuceHeader.h
   3. WebView module requirements
   4. Bus configuration for effects vs instruments

   Confirm understanding before proceeding to Step 1.
   ```

7. **Strengthen JSON schema validation language**

   **Issue:** System prompt references schema but doesn't mandate validation.

   **Enhancement:** Add explicit validation requirement:
   ```markdown
   Before returning your report, validate it against:
   `.claude/schemas/subagent-report.json`

   Ensure all required fields are present and correctly typed.
   ```

### Low Priority (Polish)

8. **Add versioning information**

   **Enhancement:** Consider adding version or last-updated metadata for tracking changes over time:
   ```yaml
   # Optional metadata (documentation only)
   version: 1.0
   updated: 2025-11-12
   ```

---

## Strengths

- **Exceptional system prompt clarity:** The "YOUR ROLE (READ THIS FIRST)" section with explicit do/don't lists is a model example
- **Strong separation of concerns:** Clearly delegates build verification to orchestrator, doesn't overstep boundaries
- **Comprehensive code templates:** Provides complete, copy-paste-ready JUCE 8 code with placeholder substitution
- **PFS pattern adherence:** Deep integration with dispatcher pattern, contract system, Required Reading
- **Detailed error handling:** Multiple failure modes documented with specific JSON report formats
- **Real-time safety awareness:** Enforces JUCE best practices (ScopedNoDenormals) even in skeleton code
- **Schema conformance:** Adheres to unified subagent report schema for consistent orchestrator parsing

---

## Example Improvements

### Before (Current Frontmatter):
```yaml
---
name: foundation-agent
type: agent
description: Create JUCE plugin project structure (Stage 2)
allowed-tools:
  - Read # Read contract files
  - Write # Create CMakeLists.txt and skeleton files
  - mcp__context7__resolve-library-id # Find JUCE library
  - mcp__context7__get-library-docs # JUCE documentation
preconditions:
  - creative-brief.md exists
  - architecture.md exists (from Stage 0)
  - plan.md exists (from Stage 1)
---
```

### After (Corrected Frontmatter):
```yaml
---
name: foundation-agent
description: Creates JUCE plugin project structure (Stage 2 - Foundation). Use when plugin-workflow orchestrator needs to generate initial CMakeLists.txt and skeleton source files after Stage 1 planning completes. Invoked automatically during /implement workflow after contracts are validated.
tools: Read, Write, mcp__context7__resolve-library-id, mcp__context7__get-library-docs
model: sonnet
---
```

**Changes:**
1. Removed non-standard `type` field
2. Expanded `description` with invocation triggers
3. Replaced `allowed-tools` array with standard `tools` comma-separated string
4. Removed `preconditions` (validation handled in system prompt)
5. Added explicit `model: sonnet` selection

---

## Conclusion

The foundation-agent has an **outstanding system prompt** that serves as a reference implementation for task-specific subagent design. The prompt demonstrates exceptional clarity, comprehensive workflow documentation, and deep integration with PFS patterns.

However, **critical configuration issues** in the YAML frontmatter prevent this subagent from functioning as intended within Claude Code's subagent system. The use of non-standard fields (`type`, `allowed-tools`, `preconditions`) means Claude Code won't parse the configuration correctly.

**Priority:** High - The configuration issues should be fixed immediately to ensure the subagent operates with the correct tool restrictions and invocation behavior.

**Production Ready:** No - Not until frontmatter configuration is corrected to use standard Claude Code fields.

**Estimated Fix Time:** 5 minutes - Simple find/replace of frontmatter fields. System prompt requires no changes.

**Recommendation:** After fixing the frontmatter configuration, this subagent can serve as a template for other PFS subagents. The system prompt structure is exemplary.
