# Subagent Audit Report: research-agent

**File:** .claude/agents/research-agent.md
**Audited:** 2025-11-12
**Auditor:** Claude Code (Subagent Best Practices)

---

## Executive Summary

The research-agent subagent is a comprehensive, well-structured Stage 0 research specialist with exceptional documentation and clear protocols. The system prompt is highly detailed with XML structure, graduated complexity detection, and clear success criteria. However, the configuration contains critical issues: it uses non-standard field names (`type`, `allowed-tools`, `preconditions`) that Claude Code does not recognize, preventing proper tool access and automatic invocation.

**Overall Grade:** C

**Critical Issue:** Non-standard configuration fields will cause this subagent to fail at runtime. The `allowed-tools` field should be `tools`, and `type` and `preconditions` are not recognized fields.

---

## Configuration Analysis

### Required Fields

#### name: `research-agent`
- ✅ Status: **COMPLIANT**
- Finding: Correct format (lowercase-with-hyphens), unique, descriptive
- Recommendation: None

#### description: `Deep DSP architecture research for JUCE plugins (Stage 0). Analyzes creative brief, researches professional plugins, maps JUCE APIs, assesses feasibility, creates architecture.md. Use when starting Stage 0 research or when plugin-planning invokes Stage 0.`
- ⚠️ Status: **PARTIAL COMPLIANCE**
- Finding: Comprehensive description clearly states purpose and invocation context. However, it's quite long (50+ words) which may reduce matching effectiveness for automatic invocation.
- Recommendation: Consider condensing to 1-2 sentences (30-40 words) for the description field, and move detailed context to the system prompt. Example:
  ```yaml
  description: Stage 0 DSP architecture research for JUCE plugins. Analyzes creative brief, researches professional plugins, maps JUCE APIs, documents architecture.md. Use when plugin-planning invokes Stage 0.
  ```

### Optional Fields

#### tools: MISSING (uses invalid `allowed-tools` instead)
- ❌ Status: **NON-COMPLIANT - CRITICAL**
- Finding: The configuration uses `allowed-tools` which is NOT a recognized field in Claude Code. The correct field name is `tools`. This means the subagent will inherit ALL tools instead of the restricted set intended.
- Current configuration lists: Read, Write, Edit, Bash, WebSearch, Grep, Glob, mcp__context7__resolve-library-id, mcp__context7__get-library-docs, mcp__sequential-thinking__sequentialthinking
- Recommendation: **CRITICAL FIX REQUIRED**
  ```yaml
  tools: Read, Write, Edit, Bash, WebSearch, Grep, Glob, mcp__context7__resolve-library-id, mcp__context7__get-library-docs, mcp__sequential-thinking__sequentialthinking
  ```

#### model: `sonnet`
- ✅ Status: **COMPLIANT**
- Finding: Appropriate model choice for complex research tasks requiring balance of capability and speed
- Recommendation: None

#### Non-standard Fields: `type`, `preconditions`
- ❌ Status: **NON-COMPLIANT**
- Finding: The configuration includes `type: agent` and `preconditions` fields which are NOT recognized by Claude Code. These will be silently ignored and provide no functionality.
- Impact:
  - `type: agent` - Redundant (all files in `.claude/agents/` are agents)
  - `preconditions` - Not enforced by Claude Code (these checks must be in system prompt)
- Recommendation: **Remove these fields**. Move precondition logic into the system prompt's contract enforcement section (which already exists at lines 616-641).

---

## System Prompt Analysis

### Structure & Organization
✅ **EXCELLENT**

The system prompt demonstrates exemplary structure:
- Clear XML tags for major sections: `<role>`, `<context>`, `<inputs>`, `<research_protocol>`, `<outputs>`, `<tools_guidance>`, `<success_criteria>`
- Progressive disclosure with numbered sections and subsections
- Visual separators between major sections (horizontal rules)
- Logical flow: role → context → inputs → protocol → outputs → tools → success
- Nested structure for complex protocols (3.0 → 3.1 → 3.2.1-3.2.6 → 3.3 → 3.4)

### Specificity & Focus
✅ **EXCELLENT**

Highly task-specific with laser focus on Stage 0 DSP architecture research:
- Clear role definition: "DSP architecture research specialist"
- Explicit about what it does AND doesn't do (lines 33-50)
- Graduated complexity protocol with 6 tiers (lines 110-125)
- Per-feature iteration pattern (3.2.1 through 3.2.6)
- Specific tool usage patterns for WebSearch vs Context7-MCP (lines 477-522)

**Strength:** The "YOUR ROLE (READ THIS FIRST)" section immediately establishes boundaries:
- Does: Research, document, analyze
- Doesn't: Implement code, run builds, create CMakeLists

### Completeness
✅ **EXCELLENT**

Covers all critical aspects:
- **Workflow**: Complete 5-section research protocol (lines 82-357)
- **Constraints**: Clear about research-only role, no implementation
- **Verification**: Comprehensive success criteria (lines 524-550)
- **Output format**: Detailed JSON schema with success/failure examples (lines 552-641)
- **Error handling**: Contract enforcement with specific error messages
- **Tool guidance**: When to use each tool (lines 477-522)
- **Integration**: References critical patterns, contract files, state updates

**Notable completeness features:**
- Extended thinking triggers with token budgets
- Graduated complexity detection (6 tiers)
- Per-feature documentation pattern (prevents information loss)
- Design sync integration (lines 331-356)
- Atomic PLUGINS.md update protocol (lines 411-451)

### PFS Integration
✅ **EXCELLENT**

Deep integration with Plugin Freedom System architecture:
- **Contract files**: References creative-brief.md, architecture.md, mockups (lines 69-80)
- **Required Reading**: Mandatory juce8-critical-patterns.md injection (lines 55-66)
- **Dispatcher pattern**: Understands fresh context invocation (line 25)
- **State management**: Complete handoff protocol (.continue-here.md, PLUGINS.md updates)
- **Checkpoint protocol**: Git commit with proper formatting (lines 453-474)
- **Stage awareness**: Knows it's Stage 0, hands off to Stage 1 planning (lines 651-661)

---

## Best Practices Compliance

| Practice | Status | Notes |
|----------|--------|-------|
| Lowercase-hyphenated name | ✅ | `research-agent` follows convention |
| Clear, concise description | ⚠️ | Comprehensive but lengthy (50+ words) |
| Proactive trigger words | ✅ | "Use when starting Stage 0" and "when plugin-planning invokes" |
| Tools field (not allowed-tools) | ❌ | Uses invalid `allowed-tools` field name |
| Appropriate model selection | ✅ | Sonnet for balanced capability/speed |
| Task-specific (not generic) | ✅ | Highly specialized Stage 0 research role |
| XML-structured system prompt | ✅ | Excellent use of XML tags throughout |
| Clear role definition | ✅ | Lines 23-31 establish precise role |
| Workflow steps included | ✅ | 5-section research protocol with subsections |
| Constraints documented | ✅ | Lines 33-50 clearly state what NOT to do |
| Output format specified | ✅ | JSON schema with examples (lines 552-641) |
| Success criteria defined | ✅ | Comprehensive list (lines 524-550) |
| Focused tool access | ❌ | Intended restriction blocked by config error |
| No unauthorized fields | ❌ | Uses `type` and `preconditions` (non-standard) |

---

## Recommendations

### Critical (Must Fix)

1. **Fix tool configuration field name**
   - **Issue**: Uses `allowed-tools` instead of `tools`
   - **Impact**: Subagent will inherit ALL tools, defeating intended restriction
   - **Fix**: Replace `allowed-tools:` with `tools:` on line 5
   ```yaml
   tools: Read, Write, Edit, Bash, WebSearch, Grep, Glob, mcp__context7__resolve-library-id, mcp__context7__get-library-docs, mcp__sequential-thinking__sequentialthinking
   ```

2. **Remove non-standard fields**
   - **Issue**: `type: agent` and `preconditions:` are not recognized by Claude Code
   - **Impact**: Silently ignored, create false expectations
   - **Fix**: Delete lines 4 and 16-18
   ```yaml
   # DELETE these lines:
   type: agent
   preconditions:
     - creative-brief.md must exist in plugins/[Name]/.ideas/
     - Plugin must not be past Stage 1
   ```
   - **Note**: Precondition logic already exists in system prompt (lines 616-641), no functionality lost

### High Priority (Should Fix)

1. **Condense description field**
   - **Issue**: 50+ word description may reduce automatic invocation effectiveness
   - **Impact**: Longer descriptions are harder for Claude to match against user requests
   - **Fix**: Shorten to 30-40 words, move detail to system prompt
   ```yaml
   description: Stage 0 DSP architecture research for JUCE plugins. Analyzes creative brief, researches professional plugins, maps JUCE APIs, creates architecture.md. Invoked by plugin-planning for Stage 0.
   ```

2. **Add explicit tool justification comment**
   - **Issue**: Large tool list (10 tools) without inline rationale
   - **Impact**: Future maintainers may question why so many tools needed
   - **Fix**: Add comment above tools field
   ```yaml
   # Stage 0 research requires:
   # - Read/Grep/Glob: Contract files, critical patterns, references
   # - Write/Edit: Create architecture.md, update state files
   # - Bash: Git operations
   # - WebSearch: Professional plugin research (NOT JUCE docs)
   # - Context7-MCP: JUCE 8 API documentation (authoritative)
   # - Sequential-thinking: Deep reasoning (10000 token budget)
   tools: Read, Write, Edit, Bash, WebSearch, Grep, Glob, mcp__context7__resolve-library-id, mcp__context7__get-library-docs, mcp__sequential-thinking__sequentialthinking
   ```

### Medium Priority (Nice to Have)

1. **Add model justification comment**
   - **Issue**: No explanation for why Sonnet vs Opus/Haiku
   - **Impact**: Future changes might use wrong model
   - **Fix**: Add comment
   ```yaml
   # Sonnet: Research tasks require balance of deep analysis capability
   # and reasonable speed (5-30 min sessions). Opus too slow for iteration,
   # Haiku insufficient for complex API mapping and feasibility assessment.
   model: sonnet
   ```

2. **Extract research protocol to reference file**
   - **Issue**: System prompt is 661 lines (very long, though well-organized)
   - **Impact**: Harder to maintain, large context consumption
   - **Benefit**: Main benefits already achieved (clear structure, complete protocol)
   - **Fix**: Consider extracting detailed research protocol (lines 82-357) to `.claude/skills/plugin-planning/references/stage-0-protocol.md` and reference it
   ```markdown
   <research_protocol>
   Execute the complete Stage 0 research protocol documented in:
   `.claude/skills/plugin-planning/references/stage-0-protocol.md`

   **High-level steps:**
   1. Read creative brief and identify features
   2. Detect complexity tier (1-6) using sequential-thinking
   3. For each feature: research algorithms, JUCE APIs, professional plugins
   4. Document findings in architecture.md progressively
   5. Create integration analysis
   6. Update state files and commit

   See protocol reference for detailed sub-steps and validation requirements.
   </research_protocol>
   ```

3. **Add examples section to system prompt**
   - **Issue**: No concrete examples of research process or architecture.md output
   - **Impact**: Subagent might miss nuances on first execution
   - **Fix**: Add abbreviated example showing research flow for simple feature
   ```markdown
   <example>
   ## Example: Researching Simple Reverb (Tier 2)

   **Input:** creative-brief.md mentions "lush reverb with decay control"

   **3.1 Feature Identification:** Reverb engine

   **3.2.1 Algorithmic Understanding:**
   Reverb = multiple delayed reflections with feedback and filtering

   **3.2.4 JUCE API Mapping:**
   - Context7-MCP query: "JUCE reverb DSP"
   - Found: juce::dsp::Reverb (module: juce_dsp)
   - Parameters: roomSize, damping, wetLevel, dryLevel, width

   **3.2.5 Validation:**
   - Check critical patterns: No gotchas found for juce::dsp::Reverb
   - Feasibility: LOW complexity, built-in JUCE class

   **3.2.6 Documentation:**
   Write to architecture.md immediately:
   ```
   ### Reverb Engine
   **JUCE Class:** juce::dsp::Reverb
   **Module:** juce_dsp
   **Complexity:** LOW
   **Approach:** Use built-in JUCE reverb with standard parameters
   ```
   </example>
   ```

### Low Priority (Polish)

1. **Consistent comment style**
   - **Issue**: Mix of inline comments and section headers
   - **Impact**: Minor readability inconsistency
   - **Fix**: Use consistent XML comment style `<!-- comment -->` or markdown headers, not both

2. **Add versioning to JSON schema**
   - **Issue**: JSON report format has no version field
   - **Impact**: Hard to detect schema changes if format evolves
   - **Fix**: Add `schema_version` field
   ```json
   {
     "schema_version": "1.0",
     "agent": "research-agent",
     "status": "success",
     ...
   }
   ```

---

## Strengths

- **Graduated complexity protocol**: 6-tier system scales research depth appropriately (5-30 minutes)
- **Progressive documentation**: Per-feature write pattern prevents information loss during iteration
- **Tool usage clarity**: Explicit guidance on WebSearch vs Context7-MCP (JUCE 6 vs JUCE 8 docs)
- **Extended thinking integration**: 10000 token budget for deep reasoning at strategic points
- **Contract enforcement**: Clear preconditions and error messages for missing inputs
- **State management**: Atomic PLUGINS.md updates (registry table + full entry)
- **PFS integration**: Deep understanding of dispatcher pattern, contract files, checkpoint protocol
- **Success criteria**: Comprehensive list of what must be true for success (9 items)
- **JSON report format**: Well-defined schema with success/failure examples
- **Required Reading injection**: Mandatory juce8-critical-patterns.md before research

---

## Example Improvements

### Before (Current Configuration - BROKEN):
```yaml
---
name: research-agent
description: Deep DSP architecture research for JUCE plugins (Stage 0). Analyzes creative brief, researches professional plugins, maps JUCE APIs, assesses feasibility, creates architecture.md. Use when starting Stage 0 research or when plugin-planning invokes Stage 0.
type: agent
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
preconditions:
  - creative-brief.md must exist in plugins/[Name]/.ideas/
  - Plugin must not be past Stage 1
model: sonnet
---
```

### After (Fixed Configuration):
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

**Changes:**
1. ✅ Fixed `allowed-tools` → `tools` (critical)
2. ✅ Removed non-standard `type` field
3. ✅ Removed non-standard `preconditions` field (logic already in system prompt)
4. ✅ Condensed description from 50 to 38 words
5. ✅ Added inline comments justifying tool selection
6. ✅ Added inline comment justifying model choice

**Note**: Precondition enforcement already exists in system prompt at lines 616-641, so no functionality lost by removing YAML `preconditions` field.

---

## Conclusion

The research-agent subagent has an **outstanding system prompt** with exceptional structure, clarity, and completeness. The graduated complexity protocol, progressive documentation pattern, and PFS integration demonstrate expert-level subagent design.

However, the configuration contains **critical errors** that will prevent it from functioning as intended:
1. `allowed-tools` is not a valid field (should be `tools`)
2. Non-standard fields (`type`, `preconditions`) create false expectations

**Priority:** FIX IMMEDIATELY before any Stage 0 research attempts. Without the configuration fix, the subagent will inherit all tools (defeating intended restrictions) and the other fields will be silently ignored.

**Production Ready:** NO - Configuration must be fixed first.

**After fixing configuration:** This will be an A-grade subagent demonstrating best-in-class practices for complex, multi-step research workflows with PFS integration.

**Estimated fix time:** 2 minutes (change 3 field names, delete 4 lines)
