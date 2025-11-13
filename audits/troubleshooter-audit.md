# Subagent Audit Report: troubleshooter

**File:** .claude/agents/troubleshooter.md
**Audited:** 2025-11-12
**Auditor:** Claude Code (Subagent Best Practices)

---

## Executive Summary

The troubleshooter subagent is a well-structured, comprehensive research agent with clear purpose and sophisticated graduated depth protocol. The configuration uses deprecated field names that may cause compatibility issues, and the description field could better trigger automatic invocation.

**Overall Grade:** B+

- Strong system prompt with detailed protocols and examples
- Clear role definition and research methodology
- Excellent use of XML structure and comprehensive workflow guidance
- Configuration uses deprecated field names (`type`, `allowed-tools`)
- Description field lacks proactive trigger keywords
- Missing PFS contract file integration in system prompt

---

## Configuration Analysis

### Required Fields

#### name: `troubleshooter`
- ✅ Status: **Compliant**
- Finding: Lowercase, descriptive, unique identifier
- Recommendation: None

#### description: `Deep research agent for build failures and JUCE problems`
- ⚠️ Status: **Partial**
- Finding: Clear purpose but lacks automatic invocation triggers. Does not include "use proactively" or "MUST BE USED" keywords that encourage Claude to invoke automatically.
- Recommendation: Enhance to include trigger scenarios:
  ```yaml
  description: Deep research agent for build failures and JUCE problems. Use when encountering build errors, linker failures, JUCE API issues, or plugin validation problems that need investigation.
  ```

### Optional Fields

#### type: `agent`
- ❌ Status: **Non-compliant (Deprecated)**
- Finding: The `type` field is not recognized in Claude Code subagent configuration. This appears to be legacy/custom metadata.
- Recommendation: Remove this field entirely. The `.md` file in `.claude/agents/` directory is sufficient for Claude Code to recognize it as a subagent.

#### model: `claude-opus-4-1-20250805`
- ⚠️ Status: **Partial**
- Finding: Uses specific model ID instead of alias. While this works, it's less maintainable than using aliases (`opus`, `sonnet`, `haiku`, `inherit`).
- Recommendation: Change to `opus` alias for better maintainability:
  ```yaml
  model: opus
  ```
  This is appropriate for deep research tasks requiring extended thinking and complex analysis.

#### allowed-tools: [list]
- ❌ Status: **Non-compliant (Deprecated)**
- Finding: The correct field name is `tools`, not `allowed-tools`. This field may not be recognized by Claude Code.
- Recommendation: Rename to `tools`:
  ```yaml
  tools: Read, Grep, Glob, Bash, WebSearch, WebFetch, mcp__context7__search_juce_docs
  ```
  Tool selection is appropriate for research tasks (read-only, search, diagnostics).

---

## System Prompt Analysis

### Structure & Organization

**✅ Excellent XML Structure**

The system prompt is exceptionally well-organized with clear XML tags:
- `<role>` equivalent sections (Purpose, Responsibilities)
- `<workflow>` equivalent (Graduated Depth Protocol with Levels 0-3)
- `<constraints>` (Research Rules)
- `<output_format>` (Report Format with detailed template)
- `<examples>` (Example Scenarios for each level)

**Strengths:**
- Hierarchical organization makes complex protocol easy to navigate
- Clear section boundaries with descriptive headers
- Logical flow from purpose → protocol → rules → output → examples

**Minor improvements:**
- Could benefit from explicit `<role>`, `<workflow>`, `<constraints>` XML tags for consistency with best practices examples

### Specificity & Focus

**✅ Highly Task-Specific**

The subagent has a tightly defined role as a research specialist:
- Clear mission: "Find root cause and solution as efficiently as possible"
- Explicit negatives: "You are NOT a code writer/executor/decision maker"
- Specific expertise: Build failures, JUCE API issues, plugin development problems

**NOT a generic helper** - This is exactly the right approach per best practices.

**Example of excellent specificity:**
```markdown
**You are NOT:**
- A code writer (no Write/Edit tools)
- An executor (no Task tool)
- A decision maker (research only, user decides)

**You ARE:**
- A researcher (deep investigation)
- An analyst (root cause identification)
- A technical advisor (solution recommendations with confidence levels)
```

### Completeness

**✅ Comprehensive Coverage**

The system prompt addresses:
- ✅ Clear role and capabilities (Purpose section)
- ✅ Explicit workflow steps (Graduated Depth Protocol with 4 levels)
- ✅ Success criteria (10-point checklist)
- ✅ Output format (Detailed markdown template with examples)
- ✅ Verification requirements (Confidence assessment, version compatibility)
- ✅ What to avoid (Research Rules: "STOP When Confident", "Never Propose Uncertain Solutions")
- ✅ Integration points (Invoked by, Returns to, Does NOT)

**Exceptional features:**
- Graduated depth protocol prevents over-research
- Time budgets for each level (30 sec → 2 min → 5 min → 15 min)
- Source credibility tier system (Tier 1-4)
- Confidence level framework (HIGH/MEDIUM/LOW with criteria)
- Prevention guidance (not just fixes)

### PFS Integration

**⚠️ Partial Integration**

**Present:**
- Integration points documented (invoked by build-automation, deep-research)
- Return message format specified
- Clear boundaries (no code changes, no builds, no subagent invocation)

**Missing:**
- **No reference to contract files** (.ideas/*.md) - subagent doesn't know to check creative-brief.md, parameter-spec.md, architecture.md for context
- **No Required Reading injection** - juce8-critical-patterns.md should be referenced for preventable patterns
- **No dispatcher pattern awareness** - doesn't explicitly state it runs in fresh context

**Recommendation:** Add PFS context section:

```markdown
## PFS Integration

<pfs_context>
You operate within the Plugin Freedom System workflow:

**Contract files** (.ideas/ directory):
- creative-brief.md - Plugin vision and requirements
- parameter-spec.md - Parameter definitions
- architecture.md - DSP design decisions
- plan.md - Implementation strategy

Check these files for context when investigating plugin-specific issues.

**Required Reading:**
Review `troubleshooting/patterns/juce8-critical-patterns.md` when investigating to identify known preventable patterns.

**Dispatcher pattern:**
You run in fresh context per invocation. Previous conversation history is not available. All context comes from:
- Invoking skill's handoff message
- Contract files you read
- Code files you investigate
</pfs_context>
```

---

## Best Practices Compliance

| Practice | Status | Notes |
|----------|--------|-------|
| **Name format (lowercase-hyphens)** | ✅ | `troubleshooter` follows convention |
| **Clear description with triggers** | ⚠️ | Describes purpose but lacks "use when" triggers |
| **Task-specific (not generic helper)** | ✅ | Excellent specificity: research specialist only |
| **XML-structured system prompt** | ✅ | Well-organized with clear sections |
| **Tool access appropriate for task** | ✅ | Read-only + search tools (correct for research) |
| **Model selection justified** | ✅ | Opus appropriate for complex research |
| **Includes workflow steps** | ✅ | Graduated 4-level protocol clearly defined |
| **Defines constraints** | ✅ | Research Rules section with clear boundaries |
| **Output format specified** | ✅ | Detailed markdown template with examples |
| **Success criteria defined** | ✅ | 10-point checklist provided |
| **Use correct field names** | ❌ | Uses deprecated `allowed-tools` (should be `tools`) |
| **Remove unnecessary fields** | ❌ | Includes `type: agent` (not recognized field) |
| **Model aliases preferred** | ⚠️ | Uses specific model ID vs `opus` alias |

---

## Recommendations

### Critical (Must Fix)

1. **Rename `allowed-tools` to `tools`**

   **Issue:** Claude Code expects `tools`, not `allowed-tools`. Current configuration may not properly restrict tool access.

   **Fix:**
   ```yaml
   # Before
   allowed-tools:
     - Read
     - Grep
     ...

   # After
   tools: Read, Grep, Glob, Bash, WebSearch, WebFetch, mcp__context7__search_juce_docs
   ```

2. **Remove `type: agent` field**

   **Issue:** This field is not part of Claude Code subagent configuration spec. May cause confusion or errors.

   **Fix:**
   ```yaml
   # Remove this line entirely
   type: agent
   ```

### High Priority (Should Fix)

1. **Enhance description with invocation triggers**

   **Issue:** Current description doesn't include keywords that help Claude automatically invoke this subagent.

   **Fix:**
   ```yaml
   # Before
   description: Deep research agent for build failures and JUCE problems

   # After
   description: Deep research agent for build failures and JUCE problems. Use when encountering build errors, linker failures, JUCE API issues, or plugin validation problems that need investigation. Invoke for troubleshooting that requires graduated research depth.
   ```

2. **Add PFS context section to system prompt**

   **Issue:** Subagent doesn't know about contract files or Required Reading, missing valuable context.

   **Fix:** Add after "Purpose" section:
   ```markdown
   ## PFS Context

   <pfs_integration>
   You operate in the Plugin Freedom System. Check these for context:

   **Contract files** (when investigating plugin-specific issues):
   - plugins/[PluginName]/.ideas/creative-brief.md - Requirements
   - plugins/[PluginName]/.ideas/parameter-spec.md - Parameters
   - plugins/[PluginName]/.ideas/architecture.md - DSP design

   **Required Reading:**
   - troubleshooting/patterns/juce8-critical-patterns.md - Known preventable patterns

   **Fresh context:** You run in isolated context per invocation. All context comes from:
   - Handoff message from invoking skill
   - Files you read during investigation
   </pfs_integration>
   ```

### Medium Priority (Nice to Have)

1. **Use model alias instead of specific ID**

   **Issue:** Hardcoded model IDs are less maintainable than aliases.

   **Fix:**
   ```yaml
   # Before
   model: claude-opus-4-1-20250805

   # After
   model: opus
   ```

2. **Add explicit XML role tags**

   **Issue:** While structure is excellent, could benefit from explicit `<role>`, `<workflow>`, `<constraints>` tags to match reference examples exactly.

   **Fix:**
   ```markdown
   # Wrap existing sections in XML tags
   <role>
   You are the troubleshooter agent - a specialized research agent...
   </role>

   <workflow>
   ## Graduated Depth Protocol
   ...
   </workflow>

   <constraints>
   ## Research Rules
   ...
   </constraints>
   ```

3. **Add reference to deep-research skill**

   **Issue:** Level 4 mentions invoking deep-research but doesn't explain relationship clearly.

   **Fix:** Clarify in Level 4 section:
   ```markdown
   ### Level 4: Invoke deep-research skill

   **When troubleshooter reaches limits:**
   You are the FAST research path (15 min max). If problem requires:
   - Extended investigation (45+ minutes)
   - Parallel subagent research
   - Academic paper analysis
   - Comprehensive approach comparison

   → Return to invoker recommending deep-research skill invocation

   **Do NOT invoke deep-research yourself** (you lack Task tool by design)
   ```

### Low Priority (Polish)

1. **Consistency in YAML list formatting**

   **Issue:** Tools could be formatted as single-line comma-separated (matches examples) vs multi-line list.

   **Fix:**
   ```yaml
   # Current (multi-line)
   allowed-tools:
     - Read
     - Grep

   # Suggested (single-line, matches best practices examples)
   tools: Read, Grep, Glob, Bash, WebSearch, WebFetch, mcp__context7__search_juce_docs
   ```

---

## Strengths

**What this subagent does exceptionally well:**

1. **Graduated depth protocol** - Prevents over-research with clear time budgets and stop criteria per level
2. **Source credibility framework** - 4-tier system for evaluating web sources with cross-reference requirements
3. **Confidence assessment** - Honest HIGH/MEDIUM/LOW framework with specific criteria
4. **Comprehensive examples** - Three detailed scenarios showing Level 0, 2, and 3 resolutions
5. **Clear boundaries** - Explicit "You are NOT" section prevents scope creep
6. **Structured output** - Detailed markdown template ensures consistent, actionable reports
7. **Version awareness** - JUCE 8.0.9+ compatibility checks throughout
8. **Prevention focus** - Not just fixes, but "how to avoid this problem in future"

**Best practices exemplified:**

- Task-specific role definition (research only, not generic helper)
- Well-organized XML structure (clear sections, logical flow)
- Appropriate tool restrictions (read-only + search, no execution)
- Model selection justified (Opus for complex research)
- Success criteria defined (10-point checklist)

---

## Example Improvements

### Before: Configuration YAML

```yaml
---
name: troubleshooter
type: agent
description: Deep research agent for build failures and JUCE problems
model: claude-opus-4-1-20250805
allowed-tools:
  - Read
  - Grep
  - Glob
  - Bash
  - WebSearch
  - WebFetch
  - mcp__context7__search_juce_docs
---
```

### After: Configuration YAML

```yaml
---
name: troubleshooter
description: Deep research agent for build failures and JUCE problems. Use when encountering build errors, linker failures, JUCE API issues, or plugin validation problems that need investigation. Invoke for troubleshooting that requires graduated research depth.
model: opus
tools: Read, Grep, Glob, Bash, WebSearch, WebFetch, mcp__context7__search_juce_docs
---
```

**Changes:**
- ❌ Removed `type: agent` (not recognized field)
- ✅ Renamed `allowed-tools` → `tools` (correct field name)
- ✅ Enhanced description with invocation triggers
- ✅ Changed model ID to alias (`opus`)
- ✅ Simplified tool list formatting

---

### Before: System Prompt (partial)

```markdown
## Purpose

You are the **troubleshooter agent** - a specialized research agent that investigates build failures, JUCE API issues, and plugin development problems using a graduated depth protocol.

**Your mission:** Find the root cause and solution as efficiently as possible using a 4-level research strategy. Stop as soon as you have a confident answer.
```

### After: System Prompt (partial)

```markdown
<role>
You are the **troubleshooter agent** - a specialized research agent that investigates build failures, JUCE API issues, and plugin development problems using a graduated depth protocol.

**Your mission:** Find the root cause and solution as efficiently as possible using a 4-level research strategy. Stop as soon as you have a confident answer.
</role>

## PFS Context

<pfs_integration>
You operate in the Plugin Freedom System. Check these for context:

**Contract files** (when investigating plugin-specific issues):
- plugins/[PluginName]/.ideas/creative-brief.md - Requirements
- plugins/[PluginName]/.ideas/parameter-spec.md - Parameters
- plugins/[PluginName]/.ideas/architecture.md - DSP design

**Required Reading:**
- troubleshooting/patterns/juce8-critical-patterns.md - Known preventable patterns

**Fresh context:** You run in isolated context per invocation. All context comes from:
- Handoff message from invoking skill
- Files you read during investigation
</pfs_integration>
```

**Changes:**
- ✅ Wrapped role definition in `<role>` XML tag
- ✅ Added PFS integration section with contract file references
- ✅ Added Required Reading reference
- ✅ Clarified fresh context / dispatcher pattern

---

## Conclusion

**Production-ready:** Yes, with configuration fixes

The troubleshooter subagent demonstrates excellent system prompt design with sophisticated research protocols, clear boundaries, and comprehensive guidance. The graduated depth approach is a standout feature that prevents over-research while maintaining thoroughness.

**Critical issues** preventing optimal operation:
1. Deprecated `allowed-tools` field name (should be `tools`)
2. Unnecessary `type: agent` field (not recognized by Claude Code)

**Priority improvements** that enhance effectiveness:
1. Enhanced description for automatic invocation triggers
2. PFS context integration (contract files, Required Reading)

Once configuration fields are corrected, this subagent will operate as designed. The high-priority improvements would significantly enhance its integration with the Plugin Freedom System workflow.

**Estimated fix time:** 10 minutes for critical + high priority changes

**Recommended action:** Apply critical configuration fixes immediately, then add PFS context integration before next workflow invocation.
