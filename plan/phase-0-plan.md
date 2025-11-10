# Phase 0 Implementation Plan

## Phase Overview

**Phase 0: Foundation & Contracts** establishes the directory structure and contract system that all subsequent phases reference. This phase creates the organizational skeleton of the PLUGIN FREEDOM SYSTEM, enabling discovery-first workflows where users can explore capabilities through play rather than documentation.

**Why this phase is critical:**
- Every other phase depends on this directory structure
- Contract templates define the "single source of truth" pattern used throughout
- CLAUDE.md provides navigation so Claude knows WHERE to find components
- PLUGINS.md establishes the state machine that prevents concurrent modification bugs
- Slash commands enable immediate feature discovery via autocomplete
- Without this foundation, no workflows can function

**Duration:** ~30 minutes

---

## Sub-Phase Breakdown

This phase does NOT require sub-phases. All deliverables are foundational and must be completed together as they form an interdependent system.

---

## Required Documentation

### Architecture Files (MUST READ IN ENTIRETY)

- **@architecture/02-core-abstractions.md** - Lines 5-47: CLAUDE.md specification (navigation index, ~200 words, NOT orchestrator). Lines 111-183: Contract enforcement rules (Stage 1 blocks if contracts missing). Lines 185-252: Validation system overview.

- **@architecture/09-file-system-design.md** - Complete directory structure, naming conventions, git-tracked vs not-tracked files, special file purposes (.continue-here.md, .ideas/, PLUGINS.md).

- **@architecture/06-state-architecture.md** - Lines 1-61: Plugin lifecycle state machine (💡→🚧→✅→📦). Lines 62-154: .continue-here.md handoff file format. State machine protection rules (only ONE plugin can be 🚧).

- **@architecture/05-routing-architecture.md** - Instructed routing pattern (commands expand to prompts, NOT programmatic dispatch). How slash commands tell Claude which skill to invoke.

- **@architecture/00-PHILOSOPHY.md** - Discovery through play principle, progressive disclosure, risk-free exploration via Claude Code checkpointing.

### Procedure Files (MUST READ IN ENTIRETY)

- **@procedures/core/interactive-decision-system.md** - Inline numbered list format (NOT AskUserQuestion), option ordering strategy, recommendation markers, progressive disclosure patterns.

- **@procedures/core/checkpoint-system.md** - Three checkpoint types (stage boundaries, phase boundaries, decision checkpoints), Claude Code Esc+Esc integration, state preservation in git commits and handoff files.

- **@procedures/commands/dream.md** - Slash command template structure, frontmatter format, precondition checking patterns.

- **@procedures/commands/implement.md** - Precondition enforcement (checks PLUGINS.md status, creative-brief existence), rejection messages for wrong workflow states.

- **@procedures/commands/improve.md** - State validation (blocks if 🚧, requires ✅ or 📦), versioning integration.

- **@procedures/commands/continue.md** - Handoff file parsing, context resumption, "resume automation" command.

- **@procedures/commands/test.md** - Testing preconditions, pluginval integration overview.

---

## Implementation Sequence

### Task 1: Create Directory Structure

**Description**: Create the complete `.claude/` hierarchy and supporting directories that form the system's organizational skeleton. This structure enables component isolation (commands/skills/agents/hooks as separate concerns) and establishes conventions used throughout all phases.

**Required Reading**:
- @architecture/09-file-system-design.md (sections: all)
- @architecture/00-PHILOSOPHY.md (sections: discovery principles)

**Dependencies**: None (first task)

**Implementation Steps**:

1. Create `.claude/` with all subdirectories:
   ```bash
   mkdir -p .claude/{commands,skills,agents,hooks,hooks/validators}
   ```

2. Create skill directories with assets/ for templates:
   ```bash
   mkdir -p .claude/skills/{plugin-ideation,plugin-workflow,ui-mockup}/{assets,references}
   ```

3. Create `plugins/` workspace (example structure will be created by workflows):
   ```bash
   mkdir -p plugins
   ```

4. Create `logs/` for build output (not git-tracked):
   ```bash
   mkdir -p logs
   ```

5. Create `backups/` for version snapshots (not git-tracked):
   ```bash
   mkdir -p backups
   ```

6. Create `scripts/` for automation (Phase 4 will populate):
   ```bash
   mkdir -p scripts
   ```

**Expected Output**:
```
plugin-freedom-system/
├── .claude/
│   ├── commands/
│   ├── skills/
│   │   ├── plugin-ideation/
│   │   │   ├── assets/
│   │   │   └── references/
│   │   ├── plugin-workflow/
│   │   │   ├── assets/
│   │   │   └── references/
│   │   └── ui-mockup/
│   │       ├── assets/
│   │       └── references/
│   ├── agents/
│   ├── hooks/
│   └── hooks/validators/
├── plugins/
├── logs/
├── backups/
└── scripts/
```

**Verification**:
- Automated:
  ```bash
  # Verify all directories exist
  test -d .claude/commands && test -d .claude/skills && test -d .claude/agents && test -d .claude/hooks && test -d .claude/hooks/validators && test -d .claude/skills/plugin-ideation/assets && test -d .claude/skills/plugin-workflow/assets && test -d .claude/skills/ui-mockup/assets && test -d plugins && test -d logs && test -d backups && test -d scripts && echo "✓ All directories exist" || echo "✗ Missing directories"
  ```
- Manual: "Lex, please verify that typing `ls -la .claude` shows commands/, skills/, agents/, and hooks/ subdirectories."

---

### Task 2: Create CLAUDE.md Navigation Index

**Description**: Write the ~200-word navigation index that tells Claude WHERE to find system components. This is explicitly NOT an orchestrator or detailed instruction manual - it's a map. Per architecture/02-core-abstractions.md lines 5-47, this file must be concise and focused on topology.

**Required Reading**:
- @architecture/02-core-abstractions.md (sections: lines 5-47, CLAUDE.md specification)
- @architecture/05-routing-architecture.md (sections: component locations)

**Dependencies**: Task 1 (directory structure must exist)

**Implementation Steps**:

1. Create `.claude/CLAUDE.md` with navigation structure:
   - System components section (WHERE things live)
   - Contracts section (single source of truth pattern)
   - Key principles (contracts immutable, dispatcher pattern)
   - NO detailed workflow instructions (those live in skills)
   - NO command behavior definitions (those live in slash commands)
   - NO validation logic (that lives in hooks)

2. Target length: ~200 words (readability limit)

3. Use clear headings: "System Components", "Contracts (Single Source of Truth)", "Key Principles"

4. Reference directory paths explicitly: `.claude/skills/`, `plugins/[Name]/.ideas/`

**Expected Output**: `.claude/CLAUDE.md` file containing:
```markdown
# PLUGIN FREEDOM SYSTEM - Plugin Development System

## System Components
- Skills: `.claude/skills/` - plugin-workflow, plugin-ideation, plugin-improve, ui-mockup, context-resume, plugin-testing, plugin-lifecycle
- Subagents: `.claude/agents/` - foundation-agent, shell-agent, dsp-agent, gui-agent, validator, troubleshooter
- Commands: `.claude/commands/` - /dream, /implement, /improve, /continue, /test, /install-plugin, /show-standalone, /troubleshoot-juce, /doc-fix
- Hooks: `.claude/hooks/` - Validation gates (PostToolUse, SubagentStop, UserPromptSubmit, Stop, PreCompact, SessionStart)

## Contracts (Single Source of Truth)
- `plugins/[Name]/.ideas/` - creative-brief.md (vision), parameter-spec.md (parameters), architecture.md (DSP design), plan.md (implementation strategy)
- State: PLUGINS.md (all plugins), .continue-here.md (active workflow)
- Templates: Contract templates stored in skill assets/
  - `.claude/skills/plugin-ideation/assets/creative-brief-template.md`
  - `.claude/skills/plugin-workflow/assets/{plan,architecture}-template.md`
  - `.claude/skills/ui-mockup/assets/parameter-spec-template.md`

## Key Principles
1. **Contracts are immutable during implementation** - All stages reference the same specs (zero drift)
2. **Dispatcher pattern** - Each subagent runs in fresh context (no accumulation)
3. **State machine protection** - Only ONE plugin can be 🚧 at a time
4. **Discovery through play** - Features found via slash command autocomplete and decision menus
5. **Instructed routing** - Commands expand to prompts, Claude invokes skills

## Workflow Entry Points
- New plugin: `/dream` → `/implement`
- Resume work: `/continue [PluginName]`
- Modify existing: `/improve [PluginName]`
- Test plugin: `/test [PluginName]`
```

**Verification**:
- Automated:
  ```bash
  # Check file exists and is under 300 words
  test -f .claude/CLAUDE.md && word_count=$(wc -w < .claude/CLAUDE.md) && [ "$word_count" -lt 300 ] && echo "✓ CLAUDE.md exists and is concise ($word_count words)" || echo "✗ CLAUDE.md missing or too long"
  ```
- Manual: "Lex, please read `.claude/CLAUDE.md` and confirm it provides clear navigation without duplicating content that belongs in skills, commands, or hooks."

---

### Task 3: Create PLUGINS.md Registry

**Description**: Create the plugin registry file that serves as the single source of truth for all plugin lifecycle states. This file implements the state machine defined in architecture/06-state-architecture.md and prevents concurrent modification bugs through status tracking.

**Required Reading**:
- @architecture/06-state-architecture.md (sections: lines 1-61, plugin lifecycle state machine)
- @architecture/09-file-system-design.md (sections: PLUGINS.md format)

**Dependencies**: Task 1 (root directory structure must exist)

**Implementation Steps**:

1. Create `PLUGINS.md` in repository root

2. Include comprehensive state legend with all emoji states:
   - 💡 Ideated (creative brief exists)
   - 🚧 Stage N (in development)
   - ✅ Working (validated, not installed)
   - 📦 Installed (in system folders)
   - 🐛 Has Issues (combines with other states)
   - 🗑️ Archived (deprecated)

3. Include empty registry table with clear headers

4. Include complete entry template showing expected format for plugin tracking

5. Document state machine rules in comments (only ONE plugin 🚧, plugin-improve blocks if 🚧)

**Expected Output**: `PLUGINS.md` file in repository root

**Verification**:
- Automated:
  ```bash
  # Verify file exists and contains state legend
  test -f PLUGINS.md && grep -q "💡 Ideated" PLUGINS.md && grep -q "🚧 Stage" PLUGINS.md && grep -q "✅ Working" PLUGINS.md && grep -q "📦 Installed" PLUGINS.md && echo "✓ PLUGINS.md exists with complete state legend" || echo "✗ PLUGINS.md missing or incomplete"
  ```
- Manual: "Lex, please verify that `PLUGINS.md` includes the state legend, an empty registry table, and a complete entry template showing how plugins will be tracked."

---

### Task 4: Create Contract Templates

**Description**: Create the four contract template files in skill assets/ directories that skills will use to generate actual contract files for plugins. These templates define the "single source of truth" pattern - parameter-spec.md and architecture.md are required for Stage 1 Planning to proceed (enforcement in Phase 2).

**Required Reading**:
- @architecture/02-core-abstractions.md (sections: lines 111-183, contract enforcement rules)
- @procedures/skills/plugin-ideation.md (sections: creative-brief.md generation)
- @procedures/skills/plugin-workflow.md (sections: Stage 0 research.md, Stage 1 plan.md)

**Dependencies**: Task 1 (skill directories with assets/ must exist)

**Implementation Steps**:

1. Create `.claude/skills/plugin-ideation/assets/creative-brief-template.md` with structure for plugin vision:
   - Plugin concept (type, created date)
   - Sonic goals (primary/secondary/tertiary)
   - Use cases
   - Inspirations (reference plugins)
   - Technical notes (requirements, challenges)

2. Create `.claude/skills/ui-mockup/assets/parameter-spec-template.md` with CRITICAL parameter contract:
   - Per-parameter format: ID, type (float/choice/bool), range, default, skew, UI control, DSP usage
   - Total parameter count
   - Contract immutability note (Stage 1 blocks if missing)
   - Generated by ui-mockup finalization

3. Create `.claude/skills/plugin-workflow/assets/architecture-template.md` with DSP component design:
   - Core components section (juce::dsp:: classes)
   - Processing chain diagram
   - Parameter mapping table (which parameters affect which components)
   - Special considerations (thread safety, performance, denormals)
   - Contract immutability note (Stage 1 blocks if missing)
   - Generated by Stage 0 research

4. Create `.claude/skills/plugin-workflow/assets/plan-template.md` with implementation strategy:
   - Complexity assessment (1-5 scale)
   - Prerequisites verified (contracts exist)
   - Stage breakdown (Stages 2-6 with duration estimates)
   - Phase breakdown for complex plugins (≥3 complexity)
   - Total estimated duration
   - Generated by Stage 1 planning

**Expected Output**: Four template files in skill assets/ directories:
- `.claude/skills/plugin-ideation/assets/creative-brief-template.md`
- `.claude/skills/ui-mockup/assets/parameter-spec-template.md`
- `.claude/skills/plugin-workflow/assets/architecture-template.md`
- `.claude/skills/plugin-workflow/assets/plan-template.md`

**Verification**:
- Automated:
  ```bash
  # Verify all template files exist
  test -f .claude/skills/plugin-ideation/assets/creative-brief-template.md && test -f .claude/skills/ui-mockup/assets/parameter-spec-template.md && test -f .claude/skills/plugin-workflow/assets/architecture-template.md && test -f .claude/skills/plugin-workflow/assets/plan-template.md && echo "✓ All 4 contract templates exist" || echo "✗ Missing contract templates"
  ```
- Manual: "Lex, please verify that parameter-spec-template.md and architecture-template.md both contain notes about being required for Stage 1 Planning to proceed (contract enforcement)."

---

### Task 5: Create Slash Command Files

**Description**: Create the 5 core slash command files that enable immediate feature discovery via autocomplete. These commands use instructed routing (expand to prompts that tell Claude which skill to invoke) rather than programmatic dispatch, per architecture/05-routing-architecture.md.

**Required Reading**:
- @architecture/05-routing-architecture.md (sections: all, instructed routing pattern)
- @procedures/commands/dream.md (sections: all, command structure)
- @procedures/commands/implement.md (sections: all, precondition enforcement)
- @procedures/commands/improve.md (sections: all, state validation)
- @procedures/commands/continue.md (sections: all, handoff parsing)
- @procedures/commands/test.md (sections: all, testing preconditions)

**Dependencies**: Task 1 (.claude/commands/ directory must exist)

**Implementation Steps**:

1. Create `.claude/commands/dream.md`:
   - Frontmatter: name, description
   - Instructions: "When user runs `/dream [concept]`, invoke the plugin-ideation skill"
   - Preconditions: None (brainstorming always available)
   - Behavior: Check if new plugin or improvement, invoke plugin-ideation appropriately

2. Create `.claude/commands/implement.md`:
   - Frontmatter: name, description
   - Instructions: "When user runs `/implement [PluginName]`, invoke the plugin-workflow skill"
   - Preconditions: Check PLUGINS.md (plugin doesn't exist OR status is 💡), check for creative-brief.md
   - Behavior: Verify preconditions, reject if status is 🚧 (use `/continue` instead), invoke plugin-workflow at Stage 0

3. Create `.claude/commands/improve.md`:
   - Frontmatter: name, description
   - Instructions: "When user runs `/improve [PluginName] [description]`, invoke the plugin-improve skill"
   - Preconditions: Plugin exists, status is ✅ or 📦 (NOT 🚧 or 💡)
   - Behavior: Verify preconditions, check for existing improvements, invoke plugin-improve with backup creation

4. Create `.claude/commands/continue.md`:
   - Frontmatter: name, description
   - Instructions: "When user runs `/continue [PluginName?]`, load context and resume workflow"
   - Preconditions: Check for `.continue-here.md` files (specific plugin or any plugin)
   - Behavior: Read handoff file, identify current stage/phase, load contracts, present context summary, resume at continuation point

5. Create `.claude/commands/test.md`:
   - Frontmatter: name, description
   - Instructions: "When user runs `/test [PluginName]`, invoke the plugin-testing skill"
   - Preconditions: Plugin exists, status is NOT 💡 (must have implementation)
   - Behavior: Verify build success, run pluginval, report results, present failure options if needed

**Expected Output**: Five slash command files in `.claude/commands/`:
- `.claude/commands/dream.md`
- `.claude/commands/implement.md`
- `.claude/commands/improve.md`
- `.claude/commands/continue.md`
- `.claude/commands/test.md`

**Verification**:
- Automated:
  ```bash
  # Verify all command files exist
  test -f .claude/commands/dream.md && test -f .claude/commands/implement.md && test -f .claude/commands/improve.md && test -f .claude/commands/continue.md && test -f .claude/commands/test.md && echo "✓ All 5 slash command files exist" || echo "✗ Missing slash command files"

  # Verify each has frontmatter
  for cmd in dream implement improve continue test; do
    grep -q "^---$" .claude/commands/$cmd.md && echo "✓ $cmd.md has frontmatter" || echo "✗ $cmd.md missing frontmatter"
  done
  ```
- Manual: "Lex, please type `/` in Claude Code and verify that autocomplete shows all 5 commands: /dream, /implement, /improve, /continue, /test with their descriptions."

---

### Task 6: Create .gitignore

**Description**: Create .gitignore to exclude session state, build artifacts, logs, and backups from version control. This ensures git only tracks the source of truth (contracts, code) and not ephemeral state (.continue-here.md, build outputs).

**Required Reading**:
- @architecture/09-file-system-design.md (sections: git-tracked vs not-tracked files)

**Dependencies**: None (can run in parallel with other tasks)

**Implementation Steps**:

1. Create `.gitignore` in repository root

2. Exclude session state:
   - `.continue-here.md` (temporary workflow state)

3. Exclude build artifacts:
   - `build/` (CMake build directories)
   - `logs/` (build logs)
   - `backups/` (version snapshots)

4. Exclude OS files:
   - `.DS_Store` (macOS)

5. Include comment explaining each exclusion category

**Expected Output**: `.gitignore` file in repository root

**Verification**:
- Automated:
  ```bash
  # Verify .gitignore exists and contains key patterns
  test -f .gitignore && grep -q ".continue-here.md" .gitignore && grep -q "build/" .gitignore && grep -q "logs/" .gitignore && grep -q "backups/" .gitignore && echo "✓ .gitignore complete" || echo "✗ .gitignore missing or incomplete"
  ```
- Manual: "Lex, please verify that `.gitignore` excludes `.continue-here.md`, `build/`, `logs/`, `backups/`, and `.DS_Store`."

---

### Task 7: Update CLAUDE.md with Phase 0 Completion Status

**Description**: Add a brief note to CLAUDE.md indicating Phase 0 is complete and the foundation is ready for Phase 1 (Discovery System). This provides a checkpoint marker for system evolution.

**Required Reading**:
- @architecture/02-core-abstractions.md (sections: lines 5-47, CLAUDE.md specification)

**Dependencies**: Task 2 (CLAUDE.md must exist)

**Implementation Steps**:

1. Open `.claude/CLAUDE.md`

2. Add footer section:
   ```markdown

   ## Implementation Status
   - ✓ Phase 0: Foundation & Contracts (complete)
   - Phase 1: Discovery System (next)
   ```

3. Keep file under 250 words total

**Expected Output**: Updated `.claude/CLAUDE.md` with status section

**Verification**:
- Automated:
  ```bash
  # Verify status section exists
  grep -q "Phase 0: Foundation & Contracts" .claude/CLAUDE.md && echo "✓ CLAUDE.md updated with Phase 0 status" || echo "✗ CLAUDE.md missing status update"
  ```
- Manual: "Lex, please confirm that `.claude/CLAUDE.md` now includes an Implementation Status section showing Phase 0 as complete."

---

## Comprehensive Verification Criteria

### Automated Tests

```bash
# Test 1: Verify directory structure
echo "=== Directory Structure Verification ==="
for dir in .claude/commands .claude/skills .claude/agents .claude/hooks .claude/hooks/validators .claude/skills/plugin-ideation/assets .claude/skills/plugin-workflow/assets .claude/skills/ui-mockup/assets plugins logs backups scripts; do
  if [ -d "$dir" ]; then
    echo "✓ $dir exists"
  else
    echo "✗ $dir missing"
  fi
done

# Test 2: Verify core files
echo -e "\n=== Core Files Verification ==="
for file in .claude/CLAUDE.md PLUGINS.md .gitignore; do
  if [ -f "$file" ]; then
    echo "✓ $file exists"
  else
    echo "✗ $file missing"
  fi
done

# Test 3: Verify contract templates
echo -e "\n=== Contract Templates Verification ==="
test -f .claude/skills/plugin-ideation/assets/creative-brief-template.md && echo "✓ creative-brief-template.md exists" || echo "✗ creative-brief-template.md missing"
test -f .claude/skills/ui-mockup/assets/parameter-spec-template.md && echo "✓ parameter-spec-template.md exists" || echo "✗ parameter-spec-template.md missing"
test -f .claude/skills/plugin-workflow/assets/architecture-template.md && echo "✓ architecture-template.md exists" || echo "✗ architecture-template.md missing"
test -f .claude/skills/plugin-workflow/assets/plan-template.md && echo "✓ plan-template.md exists" || echo "✗ plan-template.md missing"

# Test 4: Verify slash commands
echo -e "\n=== Slash Commands Verification ==="
for cmd in dream implement improve continue test; do
  if [ -f ".claude/commands/$cmd.md" ]; then
    if grep -q "^---$" ".claude/commands/$cmd.md"; then
      echo "✓ .claude/commands/$cmd.md exists with frontmatter"
    else
      echo "⚠ .claude/commands/$cmd.md exists but missing frontmatter"
    fi
  else
    echo "✗ .claude/commands/$cmd.md missing"
  fi
done

# Test 5: Verify CLAUDE.md conciseness
echo -e "\n=== CLAUDE.md Conciseness Check ==="
if [ -f ".claude/CLAUDE.md" ]; then
  word_count=$(wc -w < .claude/CLAUDE.md)
  if [ "$word_count" -lt 300 ]; then
    echo "✓ CLAUDE.md is concise ($word_count words)"
  else
    echo "⚠ CLAUDE.md is verbose ($word_count words, target <300)"
  fi
fi

# Test 6: Verify PLUGINS.md state legend
echo -e "\n=== PLUGINS.md State Legend Check ==="
if [ -f "PLUGINS.md" ]; then
  states_found=0
  for state in "💡 Ideated" "🚧 Stage" "✅ Working" "📦 Installed"; do
    if grep -q "$state" PLUGINS.md; then
      ((states_found++))
    fi
  done
  if [ "$states_found" -eq 4 ]; then
    echo "✓ PLUGINS.md contains all 4 core states"
  else
    echo "✗ PLUGINS.md missing states ($states_found/4 found)"
  fi
fi

# Test 7: Verify .gitignore patterns
echo -e "\n=== .gitignore Patterns Check ==="
if [ -f ".gitignore" ]; then
  patterns_found=0
  for pattern in ".continue-here.md" "build/" "logs/" "backups/"; do
    if grep -q "$pattern" .gitignore; then
      ((patterns_found++))
    fi
  done
  if [ "$patterns_found" -eq 4 ]; then
    echo "✓ .gitignore contains all required patterns"
  else
    echo "✗ .gitignore missing patterns ($patterns_found/4 found)"
  fi
fi

echo -e "\n=== Phase 0 Verification Complete ==="
```

### Manual Verification Checklist

- [ ] When I type `/` in Claude Code, does autocomplete show `/dream`, `/implement`, `/improve`, `/continue`, and `/test`?
- [ ] Can you read `.claude/CLAUDE.md` and confirm it's under 300 words and focused on navigation (not detailed instructions)?
- [ ] Does `PLUGINS.md` include the complete state legend with all emoji states and an entry template?
- [ ] Do all 4 contract templates in skill assets/ directories have clear structure and contract immutability notes?
- [ ] Does `parameter-spec-template.md` indicate it's required for Stage 1 Planning?
- [ ] Does `architecture-template.md` indicate it's required for Stage 1 Planning?
- [ ] Does `.gitignore` exclude `.continue-here.md`, `build/`, `logs/`, and `backups/`?
- [ ] Can you run `ls -la .claude/skills/` and see plugin-ideation/, plugin-workflow/, and ui-mockup/ subdirectories with assets/?

### File Existence Verification

```bash
# Verify all expected files exist
echo "=== Complete File Structure Verification ==="

# Check directories
dirs=(".claude" ".claude/commands" ".claude/skills" ".claude/agents" ".claude/hooks" ".claude/hooks/validators" "templates" "plugins" "logs" "backups" "scripts")
for dir in "${dirs[@]}"; do
  ls -la "$dir" 2>/dev/null || echo "✗ $dir missing"
done

# Check core files
files=(".claude/CLAUDE.md" "PLUGINS.md" ".gitignore")
for file in "${files[@]}"; do
  ls -la "$file" 2>/dev/null || echo "✗ $file missing"
done

# Check contract templates
templates=(
  ".claude/skills/plugin-ideation/assets/creative-brief-template.md"
  ".claude/skills/ui-mockup/assets/parameter-spec-template.md"
  ".claude/skills/plugin-workflow/assets/architecture-template.md"
  ".claude/skills/plugin-workflow/assets/plan-template.md"
)
for template in "${templates[@]}"; do
  ls -la "$template" 2>/dev/null || echo "✗ $template missing"
done

# Check slash commands
commands=(".claude/commands/dream.md" ".claude/commands/implement.md" ".claude/commands/improve.md" ".claude/commands/continue.md" ".claude/commands/test.md")
for cmd in "${commands[@]}"; do
  ls -la "$cmd" 2>/dev/null || echo "✗ $cmd missing"
done
```

### Integration Tests

Phase 0 has no integration tests (no workflows exist yet). Integration testing begins in Phase 1 when slash commands can invoke skills.

### Success Criteria

Phase 0 is COMPLETE when:

1. All directories exist (verified via automated test)
2. `.claude/CLAUDE.md` exists with navigation under 300 words
3. All 4 contract templates exist in skill assets/ directories
4. `PLUGINS.md` exists with complete state legend and entry template
5. All 5 slash command files exist in `.claude/commands/` with frontmatter
6. `.gitignore` excludes session state, build artifacts, logs, and backups
7. All automated tests pass without errors
8. Typing `/` in Claude Code shows all 5 commands in autocomplete
9. No git-tracked files contain session state or build artifacts
10. Directory structure matches architecture/09-file-system-design.md specification exactly

---

## Potential Issues & Mitigations

### Issue 1: CLAUDE.md becomes too verbose

**Symptom**: File exceeds 300 words, contains detailed workflow instructions

**Root Cause**: Confusion between navigation index and orchestrator role

**Mitigation**:
- Keep ONLY topology (WHERE things live)
- Move detailed instructions to skills, commands, or hooks
- Verify word count: `wc -w .claude/CLAUDE.md` (must be <300)

### Issue 2: Slash commands don't appear in autocomplete

**Symptom**: Typing `/` in Claude Code doesn't show commands

**Root Cause**: Missing frontmatter or incorrect file location

**Mitigation**:
- Verify all command files in `.claude/commands/` (not `.claude/command/` or other typo)
- Verify each file has YAML frontmatter with `name:` and `description:` fields
- Restart Claude Code session if needed

### Issue 3: Contract templates too prescriptive

**Symptom**: Templates contain plugin-specific content instead of boilerplate structure

**Root Cause**: Misunderstanding template purpose (structure, not content)

**Mitigation**:
- Templates show STRUCTURE and field names only
- Use placeholder text: `[PluginName]`, `[Description]`, `[Value]`
- Skills will fill in actual content when generating real contracts

### Issue 4: .gitignore too aggressive or too lenient

**Symptom**: Important files excluded OR junk files tracked

**Root Cause**: Incorrect pattern matching

**Mitigation**:
- Test with `git status` - should NOT show `build/`, `logs/`, `backups/`, `.continue-here.md`
- Test with `git status` - SHOULD show `.claude/`, `PLUGINS.md`
- Use `git check-ignore -v [file]` to debug exclusions

### Issue 5: Directory structure doesn't match later phase expectations

**Symptom**: Phase 1 fails because it expects different structure

**Root Cause**: Architecture docs updated but Phase 0 plan not synced

**Mitigation**:
- Cross-reference with architecture/09-file-system-design.md
- Verify subdirectory structure: `.claude/hooks/validators/` (not `.claude/validators/`)
- Create all subdirectories even if empty (skills/, agents/, hooks/)

---

## Notes for Next Phase

### Phase 1 Prerequisites

Phase 1 (Discovery System) requires:
- ✓ All Phase 0 directories exist (create skills/, agents/ structure)
- ✓ CLAUDE.md provides component navigation
- ✓ Slash commands exist with frontmatter (Phase 1 will test invocation)
- ✓ Contract templates exist (Phase 1 skills will use them)
- ✓ PLUGINS.md exists (Phase 1 workflows will update it)

### Phase 1 Expectations

Phase 1 will:
- Create all 7 skill files in `.claude/skills/` (plugin-ideation, plugin-workflow, plugin-improve, context-resume, ui-mockup, plugin-testing, plugin-lifecycle)
- Test slash command invocation (verify instructed routing works)
- Implement interactive decision menus using inline numbered lists
- Create `.continue-here.md` handoff file system
- Update PLUGINS.md with first plugin entries

### Handoff Context

After Phase 0 completion:
- Directory structure is complete and immutable (no structural changes in later phases)
- Contract templates are defined and ready for skill usage
- Slash commands are discoverable but not yet functional (Phase 1 creates the skills they invoke)
- CLAUDE.md navigation is ready for Phase 1 to add skill/agent implementations
- PLUGINS.md registry is ready to track first plugin

### Testing Strategy for Phase 1

When Phase 1 begins:
1. Test `/dream TestIdea` → Should invoke plugin-ideation skill (Phase 1 deliverable)
2. Verify `.claude/CLAUDE.md` was read by Claude (navigation should guide skill location)
3. Verify PLUGINS.md updated with first 💡 Ideated entry
4. Verify contract template usage (plugin-ideation should generate creative-brief.md)

---

## Meta

**Phase:** 0 (Foundation & Contracts)
**Duration Estimate:** 30 minutes
**Dependencies:** None (first phase)
**Deliverables:** 7 tasks producing 17 files + complete directory structure
**Architecture References:** 5 files (02-core-abstractions.md, 09-file-system-design.md, 06-state-architecture.md, 05-routing-architecture.md, 00-PHILOSOPHY.md)
**Procedure References:** 8 files (interactive-decision-system.md, checkpoint-system.md, 5 command files)
**Next Phase:** Phase 1 (Discovery System) - All commands + core skills + interactive menus

**Critical Success Factor**: Directory structure and contract templates form the foundation ALL other phases depend on. Any structural errors here will cascade through the entire system. Verification is non-negotiable.
