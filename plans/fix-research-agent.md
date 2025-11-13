# Fix Plan: research-agent

**Source audit:** ./audits/research-agent-audit.md
**Target file:** .claude/agents/research-agent.md
**Current grade:** C
**Target grade:** A

---

## Executive Summary

The research-agent has an outstanding 661-line system prompt with exemplary structure, graduated complexity protocol, and deep PFS integration. However, the YAML frontmatter contains critical configuration errors that prevent proper tool restriction and create false expectations through non-standard fields. **A 2-minute configuration fix changes this from C to A grade.**

**Estimated effort:** 2 minutes
**Risk level:** Low (config-only changes, system prompt untouched)
**Blocking issues:** 2 (tool access violation, non-standard fields)

---

## Fixes by Priority

### Critical (Must Fix) - 2 issues

#### Fix #1: Tool Configuration Field Name

**Problem:**
Security boundary violation. Uses `allowed-tools` (non-standard field) instead of `tools` (standard field). This causes the subagent to inherit ALL tools instead of the restricted research-only set, defeating security boundaries and wasting context.

**Current state:**
```yaml
allowed-tools:
  - Read # Read contract files and critical patterns
  - Write # Create architecture.md
  - Edit # Update state files
  - Bash # Git operations
  - WebSearch # Professional plugin research
  - Grep # Search existing implementations
  - Glob # Find reference files
  - mcp__context7__resolve-library-id # Find JUCE library
  - mcp__context7__get-library-docs # JUCE 8 documentation (NOT WebSearch)
  - mcp__sequential-thinking__sequentialthinking # Extended thinking for deep analysis
```

**Target state:**
```yaml
# Stage 0 research requires diverse tools:
# - Read/Grep/Glob: Contract files, critical patterns, reference plugins
# - Write/Edit: Create architecture.md, update .continue-here.md and PLUGINS.md
# - Bash: Git operations for checkpoint commits
# - WebSearch: Professional plugin research (FabFilter, Waves, UAD, Valhalla)
# - Context7-MCP: JUCE 8 API documentation (authoritative, NOT WebSearch)
# - Sequential-thinking: Deep reasoning for complexity detection and feasibility
tools: Read, Write, Edit, Bash, WebSearch, Grep, Glob, mcp__context7__resolve-library-id, mcp__context7__get-library-docs, mcp__sequential-thinking__sequentialthinking
```

**Verification:**
- [ ] Field name changed from `allowed-tools:` to `tools:`
- [ ] Format changed from list to comma-separated string
- [ ] Inline comment added explaining tool justification
- [ ] Subagent loads without errors
- [ ] Tool restriction properly enforced (check at runtime)

---

#### Fix #2: Remove Non-Standard Fields

**Problem:**
The configuration includes `type: agent` and `preconditions` fields which Claude Code does not recognize. These are silently ignored, creating false expectations about functionality. Precondition logic already exists in system prompt (lines 616-641), making the YAML field redundant.

**Current state:**
```yaml
---
name: research-agent
description: Deep DSP architecture research for JUCE plugins (Stage 0). Analyzes creative brief, researches professional plugins, maps JUCE APIs, assesses feasibility, creates architecture.md. Use when starting Stage 0 research or when plugin-planning invokes Stage 0.
type: agent
allowed-tools:
  [...]
preconditions:
  - creative-brief.md must exist in plugins/[Name]/.ideas/
  - Plugin must not be past Stage 1
model: sonnet
---
```

**Target state:**
```yaml
---
name: research-agent
description: Stage 0 DSP architecture research for JUCE plugins. Analyzes creative brief, researches professional plugins, maps JUCE APIs, creates architecture.md. Invoked by plugin-planning for Stage 0.
# Stage 0 research requires diverse tools:
# [tool justification comments...]
tools: Read, Write, Edit, Bash, WebSearch, Grep, Glob, mcp__context7__resolve-library-id, mcp__context7__get-library-docs, mcp__sequential-thinking__sequentialthinking
# Sonnet: Research tasks require balanced capability and speed (5-30 min sessions)
# Opus too slow for iteration, Haiku insufficient for complex JUCE API mapping
model: sonnet
---
```

**Verification:**
- [ ] `type: agent` line deleted
- [ ] `preconditions:` section (3 lines) deleted
- [ ] System prompt precondition logic unchanged (lines 616-641)
- [ ] No functionality lost (preconditions enforced in prompt)

---

### High Priority (Should Fix) - 2 issues

#### Fix #3: Condense Description Field

**Problem:**
Description is 50+ words, reducing effectiveness for automatic invocation matching. Claude Code favors concise descriptions (30-40 words) for better pattern matching.

**Current state:**
```yaml
description: Deep DSP architecture research for JUCE plugins (Stage 0). Analyzes creative brief, researches professional plugins, maps JUCE APIs, assesses feasibility, creates architecture.md. Use when starting Stage 0 research or when plugin-planning invokes Stage 0.
```

**Target state:**
```yaml
description: Stage 0 DSP architecture research for JUCE plugins. Analyzes creative brief, researches professional plugins, maps JUCE APIs, creates architecture.md. Invoked by plugin-planning for Stage 0.
```

**Verification:**
- [ ] Word count reduced from 50 to 38 words
- [ ] All essential information preserved
- [ ] Invocation triggers still present ("Stage 0", "plugin-planning")
- [ ] Detailed context remains in system prompt

---

#### Fix #4: Add Tool Justification Comment

**Problem:**
Large tool list (10 tools) without inline rationale makes future maintenance difficult. No explanation for why research needs this many tools.

**Current state:**
```yaml
tools: Read, Write, Edit, Bash, WebSearch, Grep, Glob, mcp__context7__resolve-library-id, mcp__context7__get-library-docs, mcp__sequential-thinking__sequentialthinking
```

**Target state:**
```yaml
# Stage 0 research requires diverse tools:
# - Read/Grep/Glob: Contract files, critical patterns, reference plugins
# - Write/Edit: Create architecture.md, update .continue-here.md and PLUGINS.md
# - Bash: Git operations for checkpoint commits
# - WebSearch: Professional plugin research (FabFilter, Waves, UAD, Valhalla)
# - Context7-MCP: JUCE 8 API documentation (authoritative, NOT WebSearch)
# - Sequential-thinking: Deep reasoning for complexity detection and feasibility
tools: Read, Write, Edit, Bash, WebSearch, Grep, Glob, mcp__context7__resolve-library-id, mcp__context7__get-library-docs, mcp__sequential-thinking__sequentialthinking
```

**Verification:**
- [ ] Comment block added above `tools:` field
- [ ] All 10 tools explained with clear rationale
- [ ] Future maintainers understand why each tool needed

---

### Medium Priority (Nice to Have) - 1 issue

#### Fix #5: Add Model Justification Comment

**Problem:**
No explanation for why Sonnet vs Opus/Haiku. Future changes might use wrong model without understanding the tradeoffs.

**Current state:**
```yaml
model: sonnet
```

**Target state:**
```yaml
# Sonnet: Research tasks require balanced capability and speed (5-30 min sessions)
# Opus too slow for iteration, Haiku insufficient for complex JUCE API mapping
model: sonnet
```

**Verification:**
- [ ] Comment explains why Sonnet chosen
- [ ] Tradeoffs documented (Opus too slow, Haiku insufficient)
- [ ] Rationale tied to use case (5-30 min research sessions)

---

## Strengths to Preserve

**DO NOT change these elements** (identified as strengths in audit):

- **661-line comprehensive research protocol** - Exemplary structure with XML tags, progressive disclosure, numbered sections
- **Graduated complexity system** - 6-tier detection (Tier 1-6), research depth scales appropriately (5-30 minutes)
- **Progressive documentation** - Per-feature write pattern (3.2.6) prevents information loss during iteration
- **Tool clarity** - Explicit guidance on WebSearch vs Context7-MCP (professional plugins vs JUCE 8 docs)
- **Extended thinking integration** - 10000 token budget for deep reasoning at strategic points
- **Deep PFS integration** - Contract files, dispatcher pattern, checkpoint protocol, state management
- **Success criteria** - Comprehensive 9-item checklist with failure conditions
- **JSON report format** - Well-defined schema with success/failure examples
- **Contract enforcement** - Clear preconditions with specific error messages (lines 616-641)
- **Per-feature iteration** - Complete research → document → next feature (prevents information loss)

**CRITICAL WARNING:** The system prompt is a reference implementation. DO NOT modify it. Only touch YAML frontmatter (lines 1-20).

---

## Implementation Sequence

1. **Fix #1 - Tool field name** - 1 min - CRITICAL: Security boundary violation must be fixed first
2. **Fix #2 - Remove non-standard fields** - 30 sec - CRITICAL: Delete `type` and `preconditions` lines
3. **Fix #3 - Condense description** - 30 sec - HIGH: Improve invocation matching
4. **Fix #4 - Add tool justification** - 1 min - HIGH: Document rationale for 10-tool list
5. **Fix #5 - Add model justification** - 30 sec - MEDIUM: Explain Sonnet choice

**Total estimated time:** 2 minutes (config-only changes)

---

## Verification Checklist

After all fixes applied:

- [ ] YAML frontmatter uses only standard Claude Code fields (`name`, `description`, `tools`, `model`)
- [ ] `tools` field (not `allowed-tools`) contains comma-separated string
- [ ] `description` condensed to 38 words with invocation triggers intact
- [ ] `type` field removed
- [ ] `preconditions` field removed (logic preserved in system prompt lines 616-641)
- [ ] System prompt UNCHANGED (661 lines preserved exactly)
- [ ] File syntax valid (YAML frontmatter + Markdown body)
- [ ] Tool justification comment added (explains 10-tool requirement)
- [ ] Model justification comment added (explains Sonnet vs Opus/Haiku)

---

## Risk Assessment

**What could go wrong:**
- Minimal risk - only touching YAML frontmatter (lines 1-20), not system prompt (lines 21-661)
- System prompt contains all actual functionality - config just tells Claude Code how to load it
- Precondition removal safe - logic already exists in system prompt

**Rollback plan:**
- Git commit before changes: `git commit -m "checkpoint: research-agent before config fixes"`
- Keep backup: `cp .claude/agents/research-agent.md .claude/agents/backups/research-agent-original.md`
- If issues occur, restore from backup or git revert

---

## Post-Fix Testing

**How to validate the fixes:**

1. **Syntax validation:** Subagent loads without YAML parsing errors
2. **Tool restriction:** Verify only 10 tools available (not full tool set)
3. **Invocation matching:** Test Stage 0 trigger automatically selects research-agent
4. **Research quality:** Run one Stage 0 research task, verify output quality unchanged

**Success criteria:**

- [ ] Agent loads without errors
- [ ] Tools properly restricted to research-only set (Read, Write, Edit, Bash, WebSearch, Grep, Glob, Context7-MCP x2, Sequential-thinking)
- [ ] Description triggers automatic invocation in Stage 0 context
- [ ] Research output quality and depth unchanged from pre-fix baseline
- [ ] architecture.md generated with all 11 required sections
- [ ] JSON report format matches schema

---

## Notes

**From audit:** "2-minute configuration fix changes this from C to A grade"

The system prompt is a reference implementation - comprehensive, well-structured, and deeply integrated with PFS patterns. It demonstrates:

- Expert XML structure with clear role/context/inputs/protocol/outputs/tools/success sections
- Graduated complexity protocol (6 tiers, 5-30 minute research depth)
- Per-feature iteration preventing information loss
- Extended thinking at strategic points (complexity detection, feasibility assessment)
- Clear tool usage guidance (WebSearch for professional plugins, Context7-MCP for JUCE 8 docs)
- Complete state management (handoff files, PLUGINS.md updates, git commits)

**DO NOT modify the system prompt. Only fix YAML frontmatter configuration errors.**

---

## Final Configuration (After All Fixes)

```yaml
---
name: research-agent
description: Stage 0 DSP architecture research for JUCE plugins. Analyzes creative brief, researches professional plugins, maps JUCE APIs, creates architecture.md. Invoked by plugin-planning for Stage 0.
# Stage 0 research requires diverse tools:
# - Read/Grep/Glob: Contract files, critical patterns, reference plugins
# - Write/Edit: Create architecture.md, update .continue-here.md and PLUGINS.md
# - Bash: Git operations for checkpoint commits
# - WebSearch: Professional plugin research (FabFilter, Waves, UAD, Valhalla)
# - Context7-MCP: JUCE 8 API documentation (authoritative, NOT WebSearch)
# - Sequential-thinking: Deep reasoning for complexity detection and feasibility
tools: Read, Write, Edit, Bash, WebSearch, Grep, Glob, mcp__context7__resolve-library-id, mcp__context7__get-library-docs, mcp__sequential-thinking__sequentialthinking
# Sonnet: Research tasks require balanced capability and speed (5-30 min sessions)
# Opus too slow for iteration, Haiku insufficient for complex JUCE API mapping
model: sonnet
---
```

**Changes from original:**
1. ✅ Fixed `allowed-tools` → `tools` (critical security fix)
2. ✅ Changed tools from list format to comma-separated string
3. ✅ Removed non-standard `type: agent` field
4. ✅ Removed non-standard `preconditions:` field (3 lines)
5. ✅ Condensed description from 50 to 38 words
6. ✅ Added tool justification comment (6 lines)
7. ✅ Added model justification comment (2 lines)

**Grade improvement:** C → A (configuration now compliant with Claude Code standards)
