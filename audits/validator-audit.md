# Subagent Audit Report: validator

**File:** .claude/agents/validator.md
**Audited:** 2025-11-12
**Auditor:** Claude Code (Subagent Best Practices)

---

## Executive Summary

The validator subagent is well-structured with comprehensive stage-specific validation logic and clear documentation. However, it suffers from significant configuration issues: incorrect YAML field names (`allowed-tools` should be `tools`), non-standard fields (`extended_thinking`, `preconditions`), and uses Opus model without justification. The system prompt is excellent but violates the "expect pure JSON output" constraint by instructing responses without markdown.

**Overall Grade:** C

- A: Exemplary, follows all best practices
- B: Good, minor improvements needed
- **C: Acceptable, several improvements recommended**
- D: Poor, major issues require attention
- F: Non-functional, critical problems

---

## Configuration Analysis

### Required Fields

#### name: `validator`
- ✅ Status: **Compliant**
- Finding: Lowercase with hyphens, unique, descriptive
- Recommendation: None

#### description: `Independent semantic validation. Use after deterministic hooks pass to verify quality, design decisions, and best practices. Invoke for complexity 4+ or when thorough review needed.`
- ⚠️ Status: **Partial Compliance**
- Finding: Clear purpose and invocation conditions specified. Good use of multi-line YAML string. However, "Use after deterministic hooks pass" is somewhat technical - could be clearer about proactive triggers.
- Recommendation: Add more explicit trigger phrases like "MUST BE USED" or "Automatically invoke" to encourage proactive use. Consider: "Independent semantic validation. MUST BE USED after each stage completion to verify quality, design decisions, and best practices. Invoke for complexity 4+ or when thorough review needed."

### Optional Fields

#### tools: Not present (uses `allowed-tools` instead)
- ❌ Status: **Non-compliant**
- Finding: Configuration uses `allowed-tools` instead of the standard `tools` field. According to Claude Code documentation, the field should be `tools`, not `allowed-tools`. While functionally it may work, this violates the standard configuration schema.
- Recommendation: **CRITICAL FIX** - Change `allowed-tools:` to `tools:` to match Claude Code specification. Also change format from YAML array to comma-separated string: `tools: Read, Grep, Bash`

#### model: `claude-opus-4-1-20250805`
- ⚠️ Status: **Partial Compliance**
- Finding: Uses Opus model (most capable, slowest, most expensive). No justification provided for this choice. Validator performs semantic analysis which benefits from reasoning capability, but unclear if Opus is necessary vs. Sonnet. Best practice recommends using model aliases (`opus`, `sonnet`, `haiku`) rather than specific model IDs for forward compatibility.
- Recommendation:
  1. Change to model alias: `model: opus` (not the full ID)
  2. Add comment in system prompt explaining why Opus is needed: "Uses Opus model for deep semantic analysis and nuanced design intent evaluation"
  3. Consider testing with Sonnet to see if cheaper model suffices for most validations

#### Non-standard fields: `extended_thinking`, `preconditions`
- ❌ Status: **Non-compliant**
- Finding: Configuration includes `extended_thinking: false` and `preconditions` block. These are NOT documented fields in Claude Code subagent specification. These fields have no effect and clutter the configuration.
- Recommendation: **Remove these fields entirely**. They are not part of the subagent specification and serve no purpose.

---

## System Prompt Analysis

### Structure & Organization

**Excellent.** The system prompt uses clear XML-like markdown headers and is logically organized:
- Role definition at the top
- Process workflow
- Mandatory cross-contract validation
- Stage-specific validation (0-6) with detailed checklists
- JSON report format with schema reference
- Override mechanism for false positives
- Best practices
- Invocation pattern examples

The stage-specific sections are comprehensive with concrete examples of both checks and JSON responses. This is a best-in-class structure.

### Specificity & Focus

**Excellent.** This is a highly task-specific subagent, not a generic helper. Role is crystal clear: "You are an independent validator performing semantic review of plugin implementation stages."

Clear distinction from deterministic hooks: "You are NOT checking if files exist or patterns match - hooks did that. You ARE checking: [semantic concerns]"

Each stage has specific, actionable checks relevant to that stage's outputs. This is exactly what the best practices recommend.

### Completeness

**Excellent.** The prompt is extremely thorough:
- ✅ Clear workflow steps (read contracts → validate → read implementation → evaluate → return JSON)
- ✅ Explicit constraints (be advisory not blocking, focus on semantics, return valid JSON)
- ✅ Success criteria defined per stage
- ✅ Output format specified with schema reference
- ✅ Examples provided for each stage
- ✅ Error handling guidance (override mechanism)
- ✅ Severity levels defined

**However**, there's one critical issue: The system prompt states "Your response should be pure JSON (no markdown, no code blocks, just JSON)" at the end. This contradicts Claude Code's general instruction style where you typically want subagents to explain their findings in natural language, not just dump JSON. While structured output is valuable, requiring *only* JSON without explanation reduces transparency and makes debugging harder.

### PFS Integration

**Excellent.** The validator shows deep understanding of PFS architecture:
- ✅ References all contract files (creative-brief.md, parameter-spec.md, architecture.md, plan.md)
- ✅ Includes **mandatory** cross-contract validation using Python validator script
- ✅ Understands dispatcher pattern (fresh context per invocation)
- ❌ **MISSING**: No reference to juce8-critical-patterns.md (Required Reading). While validator checks for best practices, it should explicitly reference this file as part of its validation criteria.

---

## Best Practices Compliance

| Practice | Status | Notes |
|----------|--------|-------|
| Lowercase-hyphen naming | ✅ | `validator` follows convention |
| Clear description field | ⚠️ | Good but could emphasize proactive triggers more |
| Task-specific (not generic) | ✅ | Highly focused on semantic validation |
| Appropriate tool access | ⚠️ | Correct tools but wrong field name (`allowed-tools` vs `tools`) |
| Model selection justified | ⚠️ | Uses Opus but no justification; should use alias not ID |
| Structured system prompt (XML tags) | ✅ | Excellent use of markdown headers and structure |
| Specific workflow steps | ✅ | Clear 5-step process defined |
| Output format guidance | ⚠️ | Clear but "pure JSON only" is problematic |
| Constraints defined | ✅ | Best practices section at end |
| Examples provided | ✅ | Comprehensive examples for each stage |
| Contract references | ✅ | All PFS contracts referenced |
| Required Reading injection | ❌ | Missing reference to juce8-critical-patterns.md |
| No standard field violations | ❌ | Uses non-standard fields (`extended_thinking`, `preconditions`) |

---

## Recommendations

### Critical (Must Fix)

1. **Fix YAML field name**: Change `allowed-tools:` to `tools:` to match Claude Code specification
   ```yaml
   # Before:
   allowed-tools:
     - Read
     - Grep
     - Bash

   # After:
   tools: Read, Grep, Bash
   ```

2. **Remove non-standard fields**: Delete `extended_thinking: false` and the entire `preconditions:` block - these have no effect in Claude Code

### High Priority (Should Fix)

1. **Use model alias instead of ID**: Change `model: claude-opus-4-1-20250805` to `model: opus` for forward compatibility

2. **Add Required Reading reference**: In the system prompt, add a section after "Contract Validation (MANDATORY)" that states:
   ```markdown
   ## Required Reading Integration

   Before performing semantic validation, you MUST review patterns from:
   - `troubleshooting/patterns/juce8-critical-patterns.md`

   Check implementations against these known anti-patterns:
   - Silent failures (processBlock returns without error but doesn't process)
   - Member initialization order issues (WebView crashes)
   - JUCE 8 migration issues (old API usage)

   Reference specific patterns when flagging issues in your JSON report.
   ```

3. **Reconsider "pure JSON only" output constraint**: The instruction "Your response should be pure JSON (no markdown, no code blocks, just JSON)" reduces transparency. Consider allowing natural language summary followed by JSON, or at minimum explain *why* pure JSON is required. If the orchestrator needs to parse it, that's fine, but consider: does the validator ever communicate directly with the user? If so, pure JSON is user-hostile.

   Suggested revision:
   ```markdown
   ## Response Format

   Return a structured JSON report (see schema above) that the plugin-workflow orchestrator will parse. You may include a brief natural language summary before the JSON if helpful for debugging, but ensure the JSON block is clearly delimited and parseable.
   ```

4. **Add Opus model justification**: In the system prompt's opening, add:
   ```markdown
   ## Configuration Notes

   This subagent uses the Opus model for its superior reasoning capabilities in evaluating:
   - Semantic alignment with creative intent
   - Nuanced design decisions
   - Architectural soundness beyond pattern matching

   Sonnet handles the deterministic checks in hooks; Opus provides the human-like judgment layer.
   ```

### Medium Priority (Nice to Have)

1. **Improve description field for proactive invocation**: Current description is good but could be stronger. Suggested revision:
   ```yaml
   description: |
     Independent semantic validator. MUST BE USED after each stage completion
     (0-6) to verify quality, design decisions, and JUCE best practices.
     Automatically invoke for complexity 4+ or when thorough review needed.
     Provides advisory feedback on creative intent alignment.
   ```

2. **Add stage complexity guidance**: Since the description mentions "complexity 4+", add a brief note in the system prompt explaining what complexity levels mean (this assumes PFS calculates complexity - confirm this)

3. **Cross-reference with hooks**: Since validator explicitly states "hooks already validated patterns", consider adding a reference to which hooks run before validator:
   ```markdown
   ## Relationship to Hooks

   Validator runs AFTER these deterministic checks pass:
   - PostToolUse hook (contract immutability, processBlock safety)
   - SubagentStop hook (contract checksums, stage-specific patterns)

   Your job is semantic evaluation, not pattern detection.
   ```

### Low Priority (Polish)

1. **Consistent terminology**: The prompt uses both "Stage N" and "stage completion" - ensure consistent capitalization (probably "Stage" since it's a formal PFS concept)

2. **Add version metadata**: Consider adding a version or last-updated date to the system prompt so you can track changes over time

3. **Consolidate examples**: The stage-specific sections are excellent but quite verbose. Consider moving detailed JSON examples to a separate reference file and linking to it, keeping only minimal examples inline

---

## Strengths

- **Exceptional documentation**: The system prompt is one of the most comprehensive and well-organized I've seen. Stage-specific checklists with concrete examples are exemplary.

- **Clear role definition**: Validator's distinction from deterministic hooks is crystal clear - it handles semantic evaluation while hooks handle pattern matching.

- **Structured output**: JSON schema reference and consistent report format enable reliable orchestration.

- **Override mechanism**: The `.validator-overrides.yaml` support shows thoughtful design for handling false positives without code changes.

- **Advisory approach**: "Be advisory, not blocking" and "User makes final decisions" demonstrate appropriate autonomy boundaries.

- **Stage specialization**: Each stage (0-6) has tailored validation criteria matching its specific outputs and concerns.

- **Cross-contract validation**: Mandatory consistency checks prevent drift between contracts - critical for PFS architecture.

---

## Example Improvements

### Configuration Block

**Before:**
```yaml
---
name: validator
description: |
  Independent semantic validation. Use after deterministic hooks pass
  to verify quality, design decisions, and best practices.
  Invoke for complexity 4+ or when thorough review needed.
extended_thinking: false
model: claude-opus-4-1-20250805
allowed-tools:
  - Read
  - Grep
  - Bash
preconditions:
  - stage_complete: true
  - contracts_exist: true
---
```

**After:**
```yaml
---
name: validator
description: |
  Independent semantic validator. MUST BE USED after each stage completion
  (0-6) to verify quality, design decisions, and JUCE best practices.
  Automatically invoke for complexity 4+ or when thorough review needed.
  Provides advisory feedback on creative intent alignment.
tools: Read, Grep, Bash
model: opus
---
```

**Changes:**
- ✅ Removed `extended_thinking` (non-standard field)
- ✅ Removed `preconditions` (non-standard field)
- ✅ Changed `allowed-tools` to `tools` (standard field)
- ✅ Changed array format to comma-separated string
- ✅ Changed full model ID to alias (`opus`)
- ✅ Enhanced description with proactive trigger language

---

### System Prompt Addition (Required Reading)

**Insert after "Contract Validation (MANDATORY)" section:**

```markdown
## Required Reading Integration

Before performing semantic validation, review critical patterns from:

**File:** `troubleshooting/patterns/juce8-critical-patterns.md`

Cross-check implementations against documented anti-patterns:
- **Silent failures**: processBlock returns without error but doesn't process audio
- **Member order issues**: WebView/APVTS initialization order causes crashes
- **JUCE 8 migration**: Usage of deprecated APIs (AudioProcessorValueTreeState constructor, old ParameterID format)
- **Real-time safety**: Allocations in processBlock, missing ScopedNoDenormals
- **Buffer safety**: Not checking for zero-length buffers or channel mismatches

When flagging issues, reference specific pattern names from Required Reading in your check messages:
```json
{
  "name": "realtime_safety",
  "passed": false,
  "message": "Violates pattern 'RT-ALLOC-01' from Required Reading: allocation found in processBlock line 47",
  "severity": "error"
}
```

This provides context and links findings to the knowledge base.
```

---

### Response Format Clarification

**Before:**
```markdown
Your response should be pure JSON (no markdown, no code blocks, just JSON).
```

**After:**
```markdown
## Response Format

Return a structured JSON validation report conforming to `.claude/schemas/validator-report.json`.

**For orchestration:** The plugin-workflow skill parses this JSON programmatically, so ensure it's valid and complete.

**For debugging:** You may optionally include a brief natural language summary before the JSON block if it aids troubleshooting, but the JSON must be clearly delimited:

```
Brief summary for humans (optional)

```json
{
  "agent": "validator",
  ...
}
```

**Production usage:** In automated workflows, only the JSON block is parsed. Keep any prose minimal.
```

---

## Conclusion

The validator subagent is **functionally strong but technically non-compliant** in its configuration. The system prompt is exemplary - comprehensive, well-structured, and task-specific. However, the YAML frontmatter violates Claude Code standards with incorrect field names and unsupported fields.

**Production readiness:** Not production-ready until configuration issues are fixed (Critical priority items). The tool access misconfiguration (`allowed-tools` vs `tools`) may cause the subagent to not receive the intended tools, potentially breaking validation workflows.

**Priority level:** **HIGH** - Fix configuration issues immediately (10-minute fix). The system prompt requires only minor enhancements (Required Reading integration) and can proceed to testing after config fixes.

**Recommended action:** Apply Critical fixes from recommendations section, test invocation to confirm tool access works correctly, then proceed with High Priority enhancements before deploying to production workflows.
