# Fix Plan: validator

**Source audit:** ./audits/validator-audit.md
**Target file:** .claude/agents/validator.md
**Current grade:** C
**Target grade:** A-

---

## Executive Summary

The validator subagent has an excellent system prompt with comprehensive stage-specific validation logic, but suffers from critical configuration errors: incorrect YAML field name (`allowed-tools` vs `tools`), non-standard fields that have no effect (`extended_thinking`, `preconditions`), and uses full Opus model ID instead of alias. These issues prevent proper tool restriction and waste cost/forward-compatibility. Additionally, missing Required Reading integration prevents the validator from leveraging documented anti-patterns.

**Estimated effort:** 15 minutes
**Risk level:** Low (configuration-only changes, no logic modifications)
**Blocking issues:** 2 (tool field name, non-standard fields)
**Cost impact:** No direct savings (Opus justified for semantic analysis), but forward-compatibility improved

---

## Fixes by Priority

### Critical (Must Fix) - 2 issues

#### Fix #1: Correct YAML tool field name

**Problem:**
Uses `allowed-tools:` instead of the standard `tools:` field. This violates Claude Code specification and may prevent the subagent from receiving intended tools (Read, Grep, Bash). Also uses YAML array format instead of comma-separated string.

**Current state:**
```yaml
allowed-tools:
  - Read # Read source files and contracts
  - Grep # Search for patterns
  - Bash # Run validation scripts
```

**Target state:**
```yaml
tools: Read, Grep, Bash
```

**Verification:**
- [ ] YAML frontmatter uses `tools` field (not `allowed-tools`)
- [ ] Value is comma-separated string (not YAML array)
- [ ] Subagent loads without errors
- [ ] Tool access works correctly when invoked

---

#### Fix #2: Remove non-standard YAML fields

**Problem:**
Configuration includes `extended_thinking: false` and `preconditions:` block. These are NOT documented fields in Claude Code subagent specification and have no effect. They clutter the configuration and violate best practices.

**Current state:**
```yaml
extended_thinking: false
model: claude-opus-4-1-20250805
allowed-tools:
  - Read
  - Grep
  - Bash
preconditions:
  - stage_complete: true
  - contracts_exist: true
```

**Target state:**
```yaml
tools: Read, Grep, Bash
model: opus
```

**Verification:**
- [ ] `extended_thinking` field removed entirely
- [ ] `preconditions` block removed entirely
- [ ] Only standard Claude Code fields remain (name, description, tools, model)
- [ ] YAML syntax remains valid

---

### High Priority (Should Fix) - 3 issues

#### Fix #3: Use model alias instead of full ID

**Problem:**
Uses full model ID `claude-opus-4-1-20250805` instead of alias `opus`. Best practice recommends using model aliases for forward compatibility. When new Opus versions release, alias automatically uses latest.

**Current state:**
```yaml
model: claude-opus-4-1-20250805
```

**Target state:**
```yaml
model: opus
```

**Verification:**
- [ ] Model field uses `opus` (not full ID)
- [ ] Subagent still receives Opus-level capabilities
- [ ] Forward-compatible with future Opus versions

---

#### Fix #4: Add Required Reading integration

**Problem:**
Validator checks for best practices but doesn't explicitly reference `juce8-critical-patterns.md` (Required Reading). This means it may miss documented anti-patterns that other subagents are required to avoid. The validator should check implementations against known patterns like silent failures, member order issues, JUCE 8 migration problems.

**Current state:**
System prompt has "Contract Validation (MANDATORY)" section but no reference to Required Reading.

**Target state:**
Add new section immediately after "Contract Validation (MANDATORY)":

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

**Verification:**
- [ ] Section added after "Contract Validation (MANDATORY)"
- [ ] References `juce8-critical-patterns.md` explicitly
- [ ] Lists key pattern categories (silent failures, member order, etc.)
- [ ] Shows example of pattern-referencing in JSON check message
- [ ] Consistent with Required Reading usage in other subagents

---

#### Fix #5: Add Opus model justification

**Problem:**
Validator uses Opus (most expensive model) without explaining why. Best practice is to justify model selection in system prompt so future maintainers understand the rationale.

**Current state:**
System prompt starts with "You are an independent validator..." with no model justification.

**Target state:**
Add new section at the top of system prompt, right after role definition:

```markdown
## Configuration Notes

This subagent uses the Opus model for superior reasoning capabilities in evaluating:
- Semantic alignment between implementation and creative intent
- Nuanced design decisions that go beyond pattern matching
- Architectural soundness requiring deep contextual understanding
- Quality judgments that benefit from human-like evaluation

Sonnet handles deterministic checks in hooks; Opus provides the judgment layer for semantic correctness.
```

**Verification:**
- [ ] Justification section added near top of system prompt
- [ ] Explains why Opus needed (not just "better" - specific capabilities)
- [ ] Distinguishes from Sonnet's role (hooks = deterministic, Opus = semantic)
- [ ] Clear rationale for cost/benefit tradeoff

---

#### Fix #6: Improve description for proactive invocation

**Problem:**
Current description says "Use after deterministic hooks pass" which is passive. Could be stronger about when/how to invoke automatically. Also "complexity 4+" reference may be unclear without context.

**Current state:**
```yaml
description: |
  Independent semantic validation. Use after deterministic hooks pass
  to verify quality, design decisions, and best practices.
  Invoke for complexity 4+ or when thorough review needed.
```

**Target state:**
```yaml
description: |
  Independent semantic validator. MUST BE USED after each stage completion
  (0-6) to verify quality, design decisions, and JUCE best practices.
  Automatically invoke for complexity 4+ or when thorough review needed.
  Provides advisory feedback on creative intent alignment.
```

**Verification:**
- [ ] Uses imperative "MUST BE USED" to encourage proactive invocation
- [ ] Specifies stage range (0-6) for clarity
- [ ] Changes "Use" to "Automatically invoke" (more proactive)
- [ ] Adds "JUCE best practices" (more specific)
- [ ] Adds "Provides advisory feedback" (clarifies output type)
- [ ] Still fits within reasonable description length

---

### Medium Priority (Optional Enhancement) - 1 issue

#### Fix #7: Clarify "pure JSON only" output constraint

**Problem:**
System prompt states "Your response should be pure JSON (no markdown, no code blocks, just JSON)". This reduces transparency and makes debugging harder. If orchestrator parses JSON, that's fine, but consider whether validator ever communicates directly with user. If so, pure JSON is user-hostile.

**Analysis needed:** Does plugin-workflow always parse validator output programmatically, or does user ever see raw validator response?

**Current state:**
```markdown
Your response should be pure JSON (no markdown, no code blocks, just JSON).
```

**Option A - If always parsed by orchestrator (recommended):**
```markdown
## Response Format

Return a structured JSON validation report conforming to `.claude/schemas/validator-report.json`.

The plugin-workflow orchestrator parses this JSON programmatically, so ensure it's valid and complete.
Format as a standard JSON code block for readability:

```json
{
  "agent": "validator",
  "stage": 0,
  ...
}
```

Pure JSON without markdown is acceptable but less readable in logs.
```

**Option B - If user sees output directly:**
```markdown
## Response Format

Return a structured JSON report conforming to `.claude/schemas/validator-report.json`.

You may include a brief natural language summary before the JSON if helpful for debugging, but ensure the JSON block is clearly delimited and parseable:

Brief summary for humans (optional)

```json
{
  "agent": "validator",
  ...
}
```

In automated workflows, only the JSON block is parsed. Keep prose minimal.
```

**Verification:**
- [ ] Response format expectations clarified
- [ ] Explains WHY format matters (orchestrator parsing)
- [ ] Balances machine-readability with human-readability
- [ ] References schema file for validation

**Decision needed:** Test how plugin-workflow invokes validator to determine Option A vs Option B.

---

## Strengths to Preserve

**DO NOT change these elements** (identified as strengths in audit):

- Exceptional stage-specific documentation (Stages 0-6 with detailed checklists)
- Clear role definition distinguishing validator from deterministic hooks
- Structured JSON output with schema reference
- Override mechanism (.validator-overrides.yaml support)
- Advisory approach ("Be advisory, not blocking")
- Cross-contract validation with Python script
- Comprehensive examples for each stage
- Best practices section at end
- False positive handling workflow
- Severity levels (error/warning/info)
- Contract references (creative-brief.md, parameter-spec.md, architecture.md, plan.md)

---

## Implementation Sequence

1. **Fix #1: Correct tool field name** - 2 min - Blocking issue, must fix first
2. **Fix #2: Remove non-standard fields** - 1 min - Blocking issue, clean configuration
3. **Fix #3: Use model alias** - 1 min - Quick win for forward compatibility
4. **Fix #5: Add Opus justification** - 3 min - Context for model choice
5. **Fix #4: Add Required Reading** - 5 min - Integration with knowledge base
6. **Fix #6: Improve description** - 2 min - Better discoverability
7. **Fix #7: Clarify output format** - 3 min - Optional, requires decision

**Total estimated time:** 15 minutes (without Fix #7), 18 minutes (with Fix #7)

---

## Verification Checklist

After all fixes applied:

**Configuration (YAML frontmatter):**
- [ ] `name: validator` (unchanged)
- [ ] `description` uses imperative language and specifies stage range
- [ ] `tools: Read, Grep, Bash` (comma-separated, not array)
- [ ] `model: opus` (alias, not full ID)
- [ ] No `allowed-tools` field
- [ ] No `extended_thinking` field
- [ ] No `preconditions` field
- [ ] YAML syntax valid (test with `yq` or Python YAML parser)

**System Prompt:**
- [ ] Configuration Notes section added with Opus justification
- [ ] Required Reading Integration section added after Contract Validation
- [ ] References `juce8-critical-patterns.md` explicitly
- [ ] Example JSON includes pattern reference format
- [ ] Response Format section clarified (if Fix #7 applied)
- [ ] All existing strengths preserved (stage checklists, examples, etc.)

**Functional:**
- [ ] File loads in Claude Code without errors
- [ ] Subagent can be invoked by plugin-workflow
- [ ] Tools (Read, Grep, Bash) accessible when invoked
- [ ] JSON output format unchanged (schema compliance maintained)
- [ ] Validation quality unchanged or improved

---

## Risk Assessment

**What could go wrong:**

1. **Tool access breaks**: If `tools` field doesn't work, validator can't read files
   - **Mitigation**: Test invocation immediately after fix; `tools` is documented standard field
   - **Rollback**: Revert to `allowed-tools` temporarily, file bug with Claude Code team

2. **Model downgrade**: If `opus` alias doesn't map to Opus-level model
   - **Mitigation**: Verify with Claude Code docs that `opus` alias exists and works
   - **Rollback**: Use full model ID if alias fails (sacrifice forward-compatibility)

3. **YAML syntax error**: Removing fields could introduce parse errors
   - **Mitigation**: Validate YAML syntax after each change using `yq` or Python parser
   - **Rollback**: Git commit before changes, revert if parse fails

4. **Output format breaks orchestrator**: If changing "pure JSON" breaks plugin-workflow parsing
   - **Mitigation**: Keep JSON format identical, only change explanation/guidance
   - **Rollback**: Revert response format section if orchestrator fails to parse

**Rollback plan:**
- Git commit before any changes: `git commit -am "backup: validator before fixes"`
- Keep copy of original: `cp .claude/agents/validator.md .claude/agents/backups/validator-pre-audit.md`
- If any functional issue: `git checkout .claude/agents/validator.md`

---

## Post-Fix Testing

**How to validate the fixes:**

1. **Syntax validation:**
   ```bash
   # Test YAML frontmatter parsing
   python3 -c "
   import yaml
   with open('.claude/agents/validator.md') as f:
       content = f.read()
       frontmatter = content.split('---')[1]
       config = yaml.safe_load(frontmatter)
       assert 'tools' in config
       assert 'allowed-tools' not in config
       assert 'extended_thinking' not in config
       assert 'preconditions' not in config
       assert config['model'] == 'opus'
       print('✓ YAML configuration valid')
   "
   ```

2. **Subagent invocation test:**
   - Invoke validator via plugin-workflow on any completed stage
   - Verify no tool access errors
   - Verify JSON output parses correctly
   - Verify validation quality unchanged

3. **Required Reading integration:**
   - Check if validator now references juce8-critical-patterns.md
   - Verify pattern names appear in check messages (if anti-patterns found)

4. **Model verification:**
   - Confirm subagent receives Opus-level capabilities
   - Check Claude Code logs/metadata to verify model alias resolution

**Success criteria:**
- [ ] Agent loads without YAML parse errors
- [ ] Tools properly restricted to Read, Grep, Bash
- [ ] Model selection uses alias, still gets Opus capabilities
- [ ] Required Reading referenced in system prompt
- [ ] Validation reports include pattern references when applicable
- [ ] JSON output format unchanged (schema compliance)
- [ ] All existing validation quality maintained
- [ ] No regression in orchestrator integration

---

## Notes

### Model Selection Rationale

The audit questioned whether Opus is necessary or if Sonnet would suffice. Analysis:

**Why Opus is justified for validator:**
- **Semantic alignment**: Validator checks if implementation matches creative *intent*, not just technical correctness. This requires nuanced interpretation of brief language.
- **Design decision evaluation**: Judging architectural soundness benefits from Opus's superior reasoning.
- **Complementary to hooks**: Sonnet already handles deterministic checks in PostToolUse/SubagentStop hooks. Validator provides the judgment layer.
- **Cost/benefit**: Validator runs once per stage (7 times per plugin max). Not high-frequency like hooks.

**Recommendation:** Keep Opus, but add justification to system prompt (Fix #5) so future maintainers understand the tradeoff.

### Required Reading Integration

Unlike implementation subagents (foundation, shell, dsp, gui) which have Required Reading injected via SubagentStop hook, validator is invoked ad-hoc by plugin-workflow. It needs explicit instruction to reference juce8-critical-patterns.md. Fix #4 addresses this by adding the reference directly to the system prompt.

### Output Format Decision

Fix #7 is marked optional because it requires understanding the orchestrator's parsing behavior. Current "pure JSON only" instruction may be intentional for reliable parsing. Recommend:

1. Test current plugin-workflow behavior
2. If it robustly extracts JSON from mixed prose, apply Option B (allows summary)
3. If it requires pure JSON, apply Option A (clarify but keep pure JSON)
4. If unsure, skip Fix #7 for now (low impact on functionality)

### Forward Compatibility

Using `model: opus` instead of full ID means when Claude releases claude-opus-4-2 or claude-opus-5, the subagent automatically gets the upgrade without configuration changes. This is especially valuable for long-lived systems like PFS.
