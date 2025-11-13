# Fix Plan: troubleshooter

**Source audit:** ./audits/troubleshooter-audit.md
**Target file:** .claude/agents/troubleshooter.md
**Current grade:** B+
**Target grade:** A

---

## Executive Summary

The troubleshooter subagent has excellent system prompt design with sophisticated graduated depth protocol and clear boundaries. However, it uses deprecated YAML field names (`allowed-tools` → `tools`, unnecessary `type` field) that prevent proper tool restriction and may cause configuration errors. The description field lacks invocation triggers, and the system prompt is missing PFS contract file integration.

**Estimated effort:** 12 minutes
**Risk level:** Low
**Blocking issues:** 2

---

## Fixes by Priority

### Critical (Must Fix) - 2 issues

#### Fix #1: Rename `allowed-tools` to `tools`

**Problem:**
Claude Code expects `tools` field, not `allowed-tools`. Current configuration may not properly restrict tool access, allowing the research-only subagent to potentially access execution tools like Edit/Write/Task.

**Current state:**
```yaml
allowed-tools:
  - Read # Read source files and logs
  - Grep # Search for error patterns
  - Glob # Find related files
  - Bash # Run diagnostic commands
  - WebSearch # Search for error solutions
  - WebFetch # Fetch documentation
  - mcp__context7__search_juce_docs # JUCE docs
```

**Target state:**
```yaml
tools: Read, Grep, Glob, Bash, WebSearch, WebFetch, mcp__context7__search_juce_docs
```

**Verification:**
- [ ] YAML field renamed from `allowed-tools` to `tools`
- [ ] Tools list formatted as single-line comma-separated (matches Claude Code examples)
- [ ] All 7 tools preserved (Read, Grep, Glob, Bash, WebSearch, WebFetch, mcp__context7__search_juce_docs)
- [ ] Inline comments removed (not needed in comma-separated format)

---

#### Fix #2: Remove `type: agent` field

**Problem:**
The `type` field is not part of Claude Code subagent configuration spec. This non-standard field may cause configuration parsing errors or confusion. Subagents are identified by their location in `.claude/agents/` directory, not by a type field.

**Current state:**
```yaml
---
name: troubleshooter
type: agent
description: Deep research agent for build failures and JUCE problems
model: claude-opus-4-1-20250805
allowed-tools:
  - Read
  ...
---
```

**Target state:**
```yaml
---
name: troubleshooter
description: Deep research agent for build failures and JUCE problems. Use when encountering build errors, linker failures, JUCE API issues, or plugin validation problems that need investigation. Invoke for troubleshooting that requires graduated research depth.
model: opus
tools: Read, Grep, Glob, Bash, WebSearch, WebFetch, mcp__context7__search_juce_docs
---
```

**Verification:**
- [ ] `type: agent` line completely removed
- [ ] YAML frontmatter only contains standard fields (name, description, model, tools)
- [ ] YAML syntax remains valid after removal

---

### High Priority (Should Fix) - 2 issues

#### Fix #3: Enhance description with invocation triggers

**Problem:**
Current description doesn't include "use when" keywords that help Claude automatically invoke this subagent during workflow execution. Missing triggers reduce discoverability and require manual invocation instead of automatic routing.

**Current state:**
```yaml
description: Deep research agent for build failures and JUCE problems
```

**Target state:**
```yaml
description: Deep research agent for build failures and JUCE problems. Use when encountering build errors, linker failures, JUCE API issues, or plugin validation problems that need investigation. Invoke for troubleshooting that requires graduated research depth.
```

**Verification:**
- [ ] Description includes "Use when" trigger phrase
- [ ] Specific scenarios listed (build errors, linker failures, JUCE API issues, validation problems)
- [ ] Invocation context clarified (graduated research depth)
- [ ] Description remains concise (under 250 chars)

---

#### Fix #4: Add PFS context section to system prompt

**Problem:**
Subagent doesn't know about contract files (.ideas/*.md) or Required Reading (juce8-critical-patterns.md), missing valuable context for plugin-specific investigations. Also doesn't explicitly state it runs in fresh context per dispatcher pattern.

**Current state:**
```markdown
## Purpose

You are the **troubleshooter agent** - a specialized research agent that investigates build failures, JUCE API issues, and plugin development problems using a graduated depth protocol.

**Your mission:** Find the root cause and solution as efficiently as possible using a 4-level research strategy. Stop as soon as you have a confident answer.
```

**Target state:**
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

**Verification:**
- [ ] `<role>` XML tag wraps existing Purpose section
- [ ] New "PFS Context" section added after Purpose
- [ ] `<pfs_integration>` XML tag contains context information
- [ ] Contract files documented with paths
- [ ] Required Reading reference added
- [ ] Dispatcher pattern (fresh context) explicitly stated
- [ ] Section placed before "Responsibilities" section

---

### Medium Priority (Nice to Have) - 3 issues

#### Fix #5: Use model alias instead of specific ID

**Problem:**
Hardcoded model ID `claude-opus-4-1-20250805` is less maintainable than using the `opus` alias. When Claude updates model versions, the alias automatically maps to the latest version, while specific IDs become outdated.

**Current state:**
```yaml
model: claude-opus-4-1-20250805
```

**Target state:**
```yaml
model: opus
```

**Verification:**
- [ ] Model field changed from specific ID to `opus` alias
- [ ] Alias appropriate for deep research tasks (Opus = complex analysis)

---

#### Fix #6: Add explicit XML workflow/constraints tags

**Problem:**
While the system prompt structure is excellent, it could benefit from explicit `<workflow>` and `<constraints>` XML tags to match Claude Code reference examples exactly. This improves consistency across all subagents.

**Current state:**
```markdown
## Graduated Depth Protocol

Use this 4-level protocol to research efficiently...

## Research Rules

### STOP When Confident
```

**Target state:**
```markdown
<workflow>
## Graduated Depth Protocol

Use this 4-level protocol to research efficiently...
</workflow>

<constraints>
## Research Rules

### STOP When Confident
```

**Verification:**
- [ ] `<workflow>` tag wraps "Graduated Depth Protocol" section
- [ ] `</workflow>` closing tag placed before "Research Rules" section
- [ ] `<constraints>` tag wraps "Research Rules" section
- [ ] `</constraints>` closing tag placed before "Report Format" section
- [ ] No content changes, only XML tag additions

---

#### Fix #7: Clarify deep-research relationship in Level 4

**Problem:**
Level 4 section mentions invoking deep-research skill but doesn't clearly explain the relationship between troubleshooter (fast 15-min path) and deep-research (extended 45+ min path). This causes confusion about when to escalate.

**Current state:**
```markdown
### Level 4: Deep Research (Invoke deep-research skill)

**Only reaches this level if:**

- Levels 0-3 exhausted without confident answer
- Complex algorithm question requiring multiple approaches
...
```

**Target state:**
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

---

**Only reaches this level if:**

- Levels 0-3 exhausted without confident answer
- Complex algorithm question requiring multiple approaches
...
```

**Verification:**
- [ ] Section retitled from "Deep Research" to "Invoke deep-research skill"
- [ ] Clarification added about troubleshooter role (15-min fast path)
- [ ] Escalation criteria clearly listed
- [ ] Explicit statement that troubleshooter cannot invoke (no Task tool)
- [ ] Original content preserved after clarification

---

### Low Priority (Polish) - 1 issue

#### Fix #8: Add output format XML tags

**Problem:**
The "Report Format" section could use explicit `<output_format>` XML tags for consistency with other well-structured sections. This is purely cosmetic but improves structure consistency.

**Current state:**
```markdown
## Report Format

Return all findings in this structured markdown format:
```

**Target state:**
```markdown
<output_format>
## Report Format

Return all findings in this structured markdown format:
...
</output_format>
```

**Verification:**
- [ ] `<output_format>` opening tag before "Report Format" header
- [ ] `</output_format>` closing tag after report template (before "Example Scenarios")
- [ ] No content changes, only XML wrapper added

---

## Strengths to Preserve

**DO NOT change these elements** (identified as strengths in audit):

1. **Graduated depth protocol** - 4-level research strategy (Level 0-3) with time budgets
2. **Source credibility framework** - Tier 1-4 system for evaluating web sources
3. **Confidence assessment** - Honest HIGH/MEDIUM/LOW framework with criteria
4. **Comprehensive examples** - Three detailed scenarios (Level 0, 2, 3 resolutions)
5. **Clear boundaries** - Explicit "You are NOT" / "You ARE" distinction
6. **Structured output template** - Detailed markdown report format
7. **Version awareness** - JUCE 8.0.9+ compatibility checks
8. **Prevention focus** - How to avoid problems in future
9. **Tool restrictions** - Read-only + search tools (no execution)
10. **Success criteria** - 10-point checklist
11. **"Stop when confident" rule** - Prevents over-research

---

## Implementation Sequence

1. **Fix #2: Remove type field** - 1 min - Remove blocking non-standard field first
2. **Fix #1: Rename allowed-tools → tools** - 2 min - Fix critical tool restriction config
3. **Fix #3: Enhance description** - 2 min - Add invocation triggers for discoverability
4. **Fix #5: Use model alias** - 1 min - Quick maintainability improvement
5. **Fix #4: Add PFS context** - 3 min - Integrate contract file and Required Reading awareness
6. **Fix #6: Add workflow/constraints XML tags** - 2 min - Structure consistency
7. **Fix #7: Clarify deep-research relationship** - 2 min - Improve escalation clarity
8. **Fix #8: Add output_format XML tags** - 1 min - Final structure polish

**Total estimated time:** 14 minutes

---

## Verification Checklist

After all fixes applied:
- [ ] YAML frontmatter uses only standard Claude Code fields (name, description, model, tools)
- [ ] `tools` field (not `allowed-tools`) contains comma-separated list of 7 tools
- [ ] `type: agent` field completely removed
- [ ] Description includes "Use when" with specific trigger scenarios
- [ ] All non-standard fields removed
- [ ] PFS context integration added after `<role>` section
- [ ] Contract files documented (creative-brief, parameter-spec, architecture)
- [ ] Required Reading reference included (juce8-critical-patterns.md)
- [ ] Dispatcher pattern (fresh context) explicitly stated
- [ ] Model changed from specific ID to `opus` alias
- [ ] `<role>`, `<workflow>`, `<constraints>`, `<output_format>` XML tags added
- [ ] Deep-research escalation criteria clarified
- [ ] File syntax valid (YAML + Markdown)
- [ ] All 11 identified strengths preserved (no changes to core protocols)

---

## Risk Assessment

**What could go wrong:**

1. **YAML syntax error** - Malformed tools list breaks configuration
   - **Mitigation:** Use exact format from audit (single-line comma-separated)
   - **Test:** Verify YAML parses with `python -c "import yaml; yaml.safe_load(open('.claude/agents/troubleshooter.md').read().split('---')[1])"`

2. **XML tag mismatch** - Unclosed or mismatched XML tags break parsing
   - **Mitigation:** Add opening and closing tags in pairs, verify all close
   - **Test:** Check each XML tag has matching close tag

3. **Content displacement** - Adding PFS context section displaces existing content
   - **Mitigation:** Insert after `</role>` tag, before "Responsibilities" header
   - **Test:** Verify "Responsibilities" section still present and unchanged

4. **Lost tool restrictions** - Incorrect tools list grants unintended access
   - **Mitigation:** Copy exact tool list from audit (7 tools), no additions
   - **Test:** Verify no Write, Edit, or Task tools in list

**Rollback plan:**
- Git commit before changes: `git commit -am "checkpoint: before troubleshooter fixes"`
- Keep copy of original: `cp .claude/agents/troubleshooter.md .claude/agents/backups/troubleshooter-b-plus.md`
- Rollback command: `git checkout HEAD -- .claude/agents/troubleshooter.md`

---

## Post-Fix Testing

**How to validate the fixes:**

1. **Configuration loads without errors**
   - Test: Parse YAML frontmatter with Python YAML parser
   - Expected: No syntax errors, all fields recognized

2. **Tool restrictions properly enforced**
   - Test: Check tools field contains only read/search tools
   - Expected: No Write, Edit, Task, NotebookEdit, or other execution tools

3. **Description triggers automatic invocation**
   - Test: Read description and verify "Use when" phrase present
   - Expected: Clear invocation scenarios listed (build errors, API issues, etc.)

4. **PFS context accessible during investigation**
   - Test: Search for "plugins/[PluginName]/.ideas/" in system prompt
   - Expected: Contract file paths documented in PFS Context section

5. **XML structure valid and complete**
   - Test: Verify all opening tags have matching closing tags
   - Expected: `<role></role>`, `<pfs_integration></pfs_integration>`, `<workflow></workflow>`, `<constraints></constraints>`, `<output_format></output_format>`

**Success criteria:**
- [ ] Agent loads without errors
- [ ] Tools properly restricted (7 read/search tools only)
- [ ] Description triggers automatic invocation
- [ ] PFS context accessible (contract files, Required Reading)
- [ ] Model alias used (opus)
- [ ] All 11 strengths preserved
- [ ] Graduated depth protocol unchanged
- [ ] Debugging capability identical to before

---

## Notes

**Key considerations:**

1. **Preserve all example scenarios** - The three detailed examples (Level 0, 2, 3) are critical for understanding the graduated protocol. Do not modify.

2. **Maintain time budgets** - Level 0 (30s), Level 1 (1-2m), Level 2 (3-5m), Level 3 (10-15m) are carefully calibrated. Do not change.

3. **Keep report template intact** - The structured markdown template with YAML front matter suggestion is essential for consistent output. Preserve completely.

4. **Don't add new tools** - The subagent is intentionally read-only (no execution). Do not add Write, Edit, Task, or other execution tools.

5. **Test with real workflow** - After fixes, invoke troubleshooter via build-automation failure protocol to verify real-world behavior.

**Post-implementation:**
- Update PLUGINS.md with note that troubleshooter has been upgraded to Grade A
- Consider running similar audits on other subagents (foundation-agent, shell-agent, dsp-agent, gui-agent, validator)
- Document this fix pattern for future subagent configuration updates

**Related files:**
- Audit source: `./audits/troubleshooter-audit.md`
- Target file: `.claude/agents/troubleshooter.md`
- Backup location: `.claude/agents/backups/troubleshooter-b-plus.md`
- Build automation: `.claude/skills/build-automation/SKILL.md` (invokes troubleshooter)
- Deep research: `.claude/skills/deep-research/SKILL.md` (escalation path)
