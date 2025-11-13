# Fix Plan: foundation-agent

**Source audit:** /Users/lexchristopherson/Developer/plugin-freedom-system/audits/foundation-agent-audit.md
**Target file:** /Users/lexchristopherson/Developer/plugin-freedom-system/.claude/agents/foundation-agent.md
**Current grade:** D
**Target grade:** A-

---

## Executive Summary

The foundation-agent has an exemplary system prompt but critical YAML frontmatter configuration errors prevent proper tool restriction and automatic invocation. Non-standard fields (`type`, `allowed-tools`, `preconditions`) are not recognized by Claude Code, causing the agent to inherit all tools instead of the restricted set and lacking invocation triggers for the orchestrator.

**Estimated effort:** 10 minutes
**Risk level:** Low (configuration-only changes, system prompt preserved)
**Blocking issues:** 2 critical (tool restriction failure, missing invocation triggers)

---

## Fixes by Priority

### Critical (Must Fix) - 2 issues

#### Fix #1: Replace non-standard `allowed-tools` with standard `tools` field

**Problem:**
Current configuration uses `allowed-tools` (YAML array) which Claude Code doesn't recognize. The standard field is `tools` (comma-separated string). This causes the agent to inherit ALL tools instead of the restricted set (Read, Write, Context7 MCP tools), creating security and focus boundary violations.

**Current state:**
```yaml
allowed-tools:
  - Read # Read contract files
  - Write # Create CMakeLists.txt and skeleton files
  - mcp__context7__resolve-library-id # Find JUCE library
  - mcp__context7__get-library-docs # JUCE documentation
```

**Target state:**
```yaml
tools: Read, Write, mcp__context7__resolve-library-id, mcp__context7__get-library-docs
```

**Verification:**
- [ ] YAML frontmatter contains `tools:` field (not `allowed-tools:`)
- [ ] Tools are comma-separated string format (not YAML array)
- [ ] Only 4 tools specified (Read, Write, 2x context7 MCP)
- [ ] Tool names match exact Claude Code names

---

#### Fix #2: Expand description to include invocation triggers

**Problem:**
Current description is too terse and lacks keywords that help plugin-workflow orchestrator determine WHEN to invoke this subagent. Missing "Use when", "Invoke after", or workflow stage context. This prevents automatic discovery and routing in the dispatcher pattern.

**Current state:**
```yaml
description: Create JUCE plugin project structure (Stage 2)
```

**Target state:**
```yaml
description: Creates JUCE plugin project structure (Stage 2 - Foundation). Use when plugin-workflow orchestrator needs to generate initial CMakeLists.txt and skeleton source files after Stage 1 planning completes. Invoked automatically during /implement workflow after contracts are validated.
```

**Verification:**
- [ ] Description includes "Use when" invocation trigger
- [ ] Description mentions plugin-workflow orchestrator (caller context)
- [ ] Description specifies what files are generated (CMakeLists.txt, skeleton source)
- [ ] Description clarifies Stage 1 → Stage 2 transition
- [ ] Description mentions contract validation precondition

---

### High Priority (Should Fix) - 3 issues

#### Fix #3: Remove non-standard `type` field

**Problem:**
Claude Code doesn't use a `type` field in agent configuration. File location (`.claude/agents/`) already indicates this is a subagent. This field creates confusion and suggests Claude Code has configuration features it doesn't support.

**Current state:**
```yaml
type: agent
```

**Target state:**
```yaml
# (Field removed entirely)
```

**Verification:**
- [ ] YAML frontmatter does NOT contain `type:` field
- [ ] Agent name and description fields remain intact
- [ ] No other frontmatter fields affected by removal

---

#### Fix #4: Remove non-standard `preconditions` field

**Problem:**
Claude Code doesn't parse or enforce `preconditions` arrays in YAML frontmatter. This creates false expectations that preconditions are automatically validated. Precondition checks are already correctly implemented in the system prompt under "Contract Enforcement" section.

**Current state:**
```yaml
preconditions:
  - creative-brief.md exists
  - architecture.md exists (from Stage 0)
  - plan.md exists (from Stage 1)
```

**Target state:**
```yaml
# (Field removed entirely - validation already in system prompt)
```

**Verification:**
- [ ] YAML frontmatter does NOT contain `preconditions:` field
- [ ] System prompt "Contract Enforcement" section still exists
- [ ] System prompt still validates contract file existence
- [ ] No loss of precondition validation functionality

---

#### Fix #5: Add explicit model selection

**Problem:**
No model specified means the agent defaults to system subagent model (typically sonnet). For Stage 2 foundation work, sonnet is appropriate, but this should be explicit and documented. Prevents future confusion about intended model choice.

**Current state:**
```yaml
# (No model field present)
```

**Target state:**
```yaml
model: sonnet
```

**Verification:**
- [ ] YAML frontmatter contains `model: sonnet`
- [ ] Model choice is documented in implementation notes
- [ ] Rationale: Stage 2 needs JUCE code generation, sonnet provides speed + capability balance

---

### Medium Priority (Nice to Have) - 2 issues

#### Fix #6: Add explicit verification step for Required Reading

**Problem:**
System prompt references juce8-critical-patterns.md as critical reading but doesn't verify the agent actually reads it before proceeding. Adding a verification checkpoint ensures patterns are internalized before code generation.

**Current state:**
```markdown
## CRITICAL: Required Reading

**Before ANY implementation, read:**

`troubleshooting/patterns/juce8-critical-patterns.md`

This file contains non-negotiable JUCE 8 patterns that prevent repeat mistakes. Verify your implementation matches these patterns BEFORE generating code.

**Key patterns to internalize:**
1. `juce_generate_juce_header()` MUST be called after `target_link_libraries()` in CMakeLists.txt
2. Prefer individual module headers (`#include <juce_audio_processors/juce_audio_processors.h>`) over `<JuceHeader.h>`
3. WebView requires `juce::juce_gui_extra` module + `JUCE_WEB_BROWSER=1` flag
4. Effects need input+output buses, instruments need output-only bus
```

**Target state:**
```markdown
## CRITICAL: Required Reading

**Before ANY implementation, read:**

`troubleshooting/patterns/juce8-critical-patterns.md`

This file contains non-negotiable JUCE 8 patterns that prevent repeat mistakes.

**Verify you understand these patterns before proceeding:**
1. `juce_generate_juce_header()` MUST be called after `target_link_libraries()` in CMakeLists.txt
2. Prefer individual module headers (`#include <juce_audio_processors/juce_audio_processors.h>`) over `<JuceHeader.h>`
3. WebView requires `juce::juce_gui_extra` module + `JUCE_WEB_BROWSER=1` flag
4. Effects need input+output buses, instruments need output-only bus

**Checkpoint:** After reading, confirm you understand these patterns. If any are unclear, reference the troubleshooting doc for detailed explanations before generating code.
```

**Verification:**
- [ ] Required Reading section includes explicit verification checkpoint
- [ ] Language changed from "verify implementation matches" to "verify you understand"
- [ ] Checkpoint added: "confirm you understand these patterns"
- [ ] Reference to troubleshooting doc for clarification

---

#### Fix #7: Strengthen JSON schema validation language

**Problem:**
System prompt references subagent-report schema but doesn't mandate validation. Adding explicit validation requirement prevents malformed reports that break orchestrator parsing.

**Current state:**
```markdown
## JSON Report Format

**Schema:** `.claude/schemas/subagent-report.json`

All reports MUST conform to the unified subagent report schema. This ensures consistent parsing by plugin-workflow orchestrator.
```

**Target state:**
```markdown
## JSON Report Format

**Schema:** `.claude/schemas/subagent-report.json`

All reports MUST conform to the unified subagent report schema. This ensures consistent parsing by plugin-workflow orchestrator.

**Before returning your report:**
1. Read `.claude/schemas/subagent-report.json` to verify field requirements
2. Validate your JSON structure includes all required fields
3. Confirm field types match schema (strings, arrays, booleans)

**Required fields:**
- `agent`: string (must be "foundation-agent")
- `status`: string (must be "success" or "failure")
- `outputs`: object (plugin_name, source_files_created or error details)
- `issues`: array (empty on success, error messages on failure)
- `ready_for_next_stage`: boolean
```

**Verification:**
- [ ] JSON Report Format section includes validation steps
- [ ] Explicit 3-step validation process added
- [ ] Required fields list added with type information
- [ ] Schema path reference preserved

---

### Low Priority (Polish) - 0 issues

*No low-priority issues identified in audit.*

---

## Strengths to Preserve

**DO NOT change these elements** (identified as strengths in audit):

- **Exceptional system prompt clarity:** "YOUR ROLE (READ THIS FIRST)" section with explicit do/don't lists
- **Strong separation of concerns:** Clearly delegates build verification to orchestrator
- **Comprehensive code templates:** Complete JUCE 8 code with placeholder substitution
- **PFS pattern adherence:** Dispatcher pattern awareness, contract system integration, Required Reading injection
- **Detailed error handling:** Multiple failure modes with specific JSON report formats
- **Real-time safety awareness:** Enforces JUCE best practices (ScopedNoDenormals) even in skeleton code
- **Schema conformance:** Adheres to unified subagent report schema
- **XML structure organization:** Clear section headers and logical flow

---

## Implementation Sequence

1. **Remove `type: agent` field** - 1 min - Eliminates non-standard field causing confusion
2. **Remove `preconditions:` array** - 1 min - Removes false expectation of automatic validation
3. **Replace `allowed-tools:` with `tools:` field** - 2 min - CRITICAL: Fixes tool restriction to prevent tool access violations
4. **Expand `description:` with invocation triggers** - 3 min - CRITICAL: Enables automatic discovery and routing
5. **Add `model: sonnet` field** - 1 min - Makes model choice explicit and documented
6. **Add Required Reading verification checkpoint** - 1 min - Ensures patterns internalized before code generation
7. **Strengthen JSON schema validation language** - 1 min - Prevents malformed reports breaking orchestrator

**Total estimated time:** 10 minutes

---

## Verification Checklist

After all fixes applied:

**Configuration (YAML frontmatter):**
- [ ] YAML frontmatter uses only standard Claude Code fields
- [ ] `tools` field (not `allowed-tools`) contains comma-separated list
- [ ] `description` includes invocation triggers ("Use when...", "Invoked automatically...")
- [ ] `model` field explicitly set to `sonnet`
- [ ] All non-standard fields removed (`type`, `preconditions`)
- [ ] YAML syntax valid (no array notation for tools)

**System Prompt:**
- [ ] Required Reading section includes verification checkpoint
- [ ] JSON Report Format includes validation steps
- [ ] All strengths preserved (do/don't lists, separation of concerns, code templates)
- [ ] Contract Enforcement section unchanged
- [ ] Error handling formats unchanged
- [ ] XML structure organization preserved

**Integration:**
- [ ] Agent name unchanged (`foundation-agent`)
- [ ] File structure unchanged (frontmatter + system prompt)
- [ ] No breaking changes to JSON report schema
- [ ] No changes to contract file references

---

## Risk Assessment

**What could go wrong:**

1. **Tool list typo** → Agent fails to load
   - *Mitigation:* Double-check tool names match exact Claude Code conventions (Read, Write, mcp__context7__resolve-library-id, mcp__context7__get-library-docs)
   - *Test:* Attempt to invoke agent after changes

2. **Description too verbose** → Clutters agent listing
   - *Mitigation:* Current expanded description (3 sentences) is within reasonable bounds per audit examples
   - *Acceptable:* Better verbose than missing invocation triggers

3. **YAML syntax error** → Agent fails to parse
   - *Mitigation:* Validate YAML syntax before saving (no tabs, proper indentation, comma-separated tools)
   - *Test:* Load agent configuration in Claude Code

4. **System prompt changes break orchestrator expectations** → Stage 2 failures
   - *Mitigation:* System prompt changes are minimal (add verification checkpoints only, no structural changes)
   - *Rollback:* Git commit before changes preserves original

**Rollback plan:**
- Git commit before changes: `git add .claude/agents/foundation-agent.md && git commit -m "chore: snapshot foundation-agent before audit fixes"`
- Keep copy in backups: `cp .claude/agents/foundation-agent.md .claude/agents/backups/foundation-agent-pre-audit.md`
- Rollback command: `git checkout HEAD~1 -- .claude/agents/foundation-agent.md`

---

## Post-Fix Testing

**How to validate the fixes:**

1. **Configuration loading test**
   - Action: Restart Claude Code session or reload agent configuration
   - Expected: No YAML parsing errors
   - Expected: Agent appears in available subagents list

2. **Tool restriction test**
   - Action: Invoke foundation-agent via plugin-workflow orchestrator
   - Expected: Agent can only access Read, Write, and Context7 MCP tools
   - Expected: Agent cannot access Bash, Edit, Grep, Glob, etc.

3. **Invocation trigger test**
   - Action: Run `/implement [PluginName]` workflow to Stage 2 transition
   - Expected: plugin-workflow automatically invokes foundation-agent
   - Expected: Description helps orchestrator determine correct invocation context

4. **Required Reading verification test**
   - Action: Monitor agent execution during Stage 2
   - Expected: Agent reads juce8-critical-patterns.md before generating code
   - Expected: Generated CMakeLists.txt follows critical patterns (juce_generate_juce_header after target_link_libraries)

5. **JSON report validation test**
   - Action: Complete Stage 2 and inspect agent return report
   - Expected: Report conforms to subagent-report.json schema
   - Expected: All required fields present (agent, status, outputs, issues, ready_for_next_stage)

**Success criteria:**
- [ ] Agent loads without errors
- [ ] Tools properly restricted to specified set (4 tools only)
- [ ] Description triggers automatic invocation in Stage 2 context
- [ ] System prompt functionality unchanged (code generation, error handling, validation)
- [ ] JSON reports conform to schema
- [ ] Critical patterns enforced in generated code

---

## Notes

**Why Grade D despite excellent system prompt:**

The audit graded this D because configuration errors (non-standard fields) make the agent non-functional despite the system prompt quality. This is Claude Code's equivalent of having perfect application logic but invalid syntax - it doesn't matter how good the code is if it doesn't compile.

**Configuration vs. Content priority:**

Configuration fixes (1-5) must be applied first because they affect how Claude Code loads and invokes the agent. Content improvements (6-7) enhance behavior but don't block basic functionality.

**Model choice rationale:**

Sonnet chosen for Stage 2 because:
- Foundation work requires JUCE code generation knowledge (sonnet trained on JUCE)
- No complex reasoning required (boilerplate generation from contracts)
- Speed matters (Stage 2 runs frequently in workflow)
- Cost efficiency (sonnet cheaper than opus for routine tasks)

Opus would be overkill for Stage 2. Reserve opus for Stage 4 (DSP algorithm design) or troubleshooting (complex debugging).

**Post-fix follow-up:**

After foundation-agent fixes verified:
1. Apply same audit process to shell-agent, dsp-agent, gui-agent
2. Create standardized subagent configuration template
3. Add pre-commit hook to validate subagent YAML frontmatter
4. Document standard fields in `.claude/agents/README.md`

**System prompt quality:**

The audit rated system prompt A grade across all categories:
- Structure & Organization: A
- Specificity & Focus: A
- Completeness: A
- PFS Integration: A

This is rare. Most subagents need prompt improvements. Foundation-agent only needs configuration fixes.
