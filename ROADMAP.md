# PLUGIN FREEDOM SYSTEM Implementation Roadmap

**Version:** 2.0.0
**Last Updated:** 2025-11-10
**Philosophy:** Discovery first, implementation second, polish last

---

## Overview

This roadmap implements the PLUGIN FREEDOM SYSTEM through 7 phases organized around **working, explorable functionality first**. Users can discover and navigate the system from Phase 1, with implementation capabilities added progressively.

**Core Principle:** Get the discovery system working immediately so users can explore through play, not documentation.

---

## Phase Summary

| Phase | Name                     | Core Deliverables                                  | Why This Order                         |
| ----- | ------------------------ | -------------------------------------------------- | -------------------------------------- |
| 0     | Foundation & Contracts   | Directory structure, CLAUDE.md, contract templates | Everything else references these       |
| 1     | Discovery System         | All commands + core skills + interactive menus     | Users can explore from session 1       |
| 2     | Workflow Engine          | plugin-workflow Stages 0,1,6 + state management    | Core development workflow operational  |
| 3     | Implementation Subagents | foundation/shell/dsp/gui agents + dispatch         | Actual code generation capability      |
| 4     | Build & Troubleshooting  | build-and-install.sh + troubleshooter              | Compile and debug what's built         |
| 5     | Validation System        | Hooks + validator subagent + Python scripts        | Quality assurance and drift prevention |
| 6     | WebView UI System        | Complete ui-mockup + WebView patterns              | Beautiful plugin interfaces            |
| 7     | Polish & Enhancement     | Lifecycle management + advanced features           | Professional finish                    |

---

## Phase 0: Foundation & Contracts

**Objective:** Establish directory structure and contract system that all other phases reference.

**Prerequisites:**

- JUCE 8 installed
- CMake, Ninja available
- pluginval installed
- macOS development system

**Core Deliverables:**

1. **Directory Structure:**

```
plugin-freedom-system/
├── .claude/
│   ├── commands/          # Slash command definitions
│   ├── skills/            # Workflow coordination
│   ├── agents/            # Subagent implementations
│   ├── hooks/             # Validation hooks
│   └── CLAUDE.md          # Navigation index (~200 words)
├── plugins/               # Plugin projects
├── logs/                  # Build logs
├── backups/               # Version backups
├── scripts/               # Automation scripts
├── PLUGINS.md             # Plugin registry with state machine
└── .gitignore
```

2. **CLAUDE.md Navigation Index:**

- System topology (WHERE components live)
- Key principles (contracts immutable, dispatcher pattern)
- Component locations (skills, agents, commands)
- NOT an orchestrator - just navigation

3. **Contract Templates:**

Templates distributed to skill-specific `assets/` directories:
- `.claude/skills/plugin-ideation/assets/creative-brief-template.md` - High-level plugin vision
- `.claude/skills/ui-mockup/assets/parameter-spec-template.md` - Audio parameters specification
- `.claude/skills/plugin-workflow/assets/architecture-template.md` - DSP component design
- `.claude/skills/plugin-workflow/assets/plan-template.md` - Stage breakdown template

4. **PLUGINS.md Registry:**

```markdown
# Plugin Registry

| Plugin | Status | Version | Last Updated |
| ------ | ------ | ------- | ------------ |

## Status Legend

- 💡 Ideated (creative brief exists)
- 🚧 Stage N (in development)
- ✅ Working (validated, not installed)
- 📦 Installed (in system folders)
```

**Architecture References:**

- `architecture/02-core-abstractions.md` (lines 5-47) - CLAUDE.md specification
- `architecture/09-file-system-design.md` - Directory structure
- `architecture/06-state-architecture.md` - PLUGINS.md state machine

**Verification:**

- [ ] All directories exist
- [ ] CLAUDE.md exists with navigation (~200 words)
- [ ] All 4 contract templates exist in skill assets/ directories
- [ ] PLUGINS.md exists with status legend
- [ ] .gitignore excludes logs/, backups/, build/

---

## Phase 1: Discovery System

**Objective:** Implement complete navigation and discovery layer so users can explore from session 1.

**Prerequisites:**

- Phase 0 complete

**Core Deliverables:**

### 1a. All Slash Commands

Create all 9 command files in `.claude/commands/`:

- `dream.md` - Routes to plugin-ideation skill
- `implement.md` - Routes to plugin-workflow skill
- `improve.md` - Routes to plugin-improve skill
- `continue.md` - Routes to context-resume skill
- `test.md` - Routes to plugin-testing skill
- `install-plugin.md` - Routes to plugin-lifecycle skill
- `show-standalone.md` - Routes to ui-mockup skill
- `troubleshoot-juce.md` - Routes to troubleshooter agent
- `doc-fix.md` - Documentation maintenance

Each command contains:

- Clear invocation instructions (instructed routing, not automatic)
- Precondition checks (state validation)
- User-facing description

### 1b. Core Skills (Basic Implementation)

Create skill structure in `.claude/skills/` with frontmatter and basic logic:

**plugin-ideation:** Generates creative-brief.md

- Adaptive brainstorming workflow
- Interactive decision menus for exploration
- Outputs to `plugins/[Name]/.ideas/creative-brief.md`

**plugin-workflow:** 7-stage orchestration (stub Stages 2-5 for now)

- Stage dispatch logic
- State machine updates
- Git commit integration
- Decision menus at each stage boundary

**plugin-improve:** Version management

- Backup creation before changes
- CHANGELOG.md updates (Keep a Changelog format)
- State machine protection (blocks if 🚧)

**context-resume:** Parse and resume from `.continue-here.md`

- YAML frontmatter parsing
- State reconstruction
- Resumption options menu

**ui-mockup:** Two-phase UI design workflow

- Design phase (Phases 3-4): Generate YAML spec + browser test HTML (2 files)
- Phase 4.5: Present decision menu and WAIT for user approval
- Implementation phase (Phases 5-8, only after approval): Generate 5 additional files (production HTML + C++ boilerplate + build config + checklist)
- Total per finalized version: 7 files

**plugin-testing:** Validation suite

- Build verification
- pluginval integration
- DAW loading tests

**plugin-lifecycle:** Installation management

- System folder operations
- Installation verification
- Uninstallation support

### 1c. Interactive Decision System

Build decision menus INTO all skills:

- Inline numbered lists (NOT AskUserQuestion)
- Context-aware option generation
- Recommendation markers (`(recommended)`)
- Discovery hints (`← User discovers [feature]`)
- Always include "Other" option

### 1d. Handoff File System

Implement `.continue-here.md` creation:

- YAML frontmatter (stage, plugin, status)
- Prose context (what happened, what's next)
- Created after Stage 0
- Updated after each stage

**Architecture References:**

- `architecture/05-routing-architecture.md` - Instructed routing pattern
- `procedures/core/interactive-decision-system.md` - Decision menu patterns
- `procedures/skills/plugin-ideation.md` - Creative workflow
- `procedures/skills/context-resume.md` - Resumption logic
- `architecture/06-state-architecture.md` (lines 62-154) - Handoff format

**Critical Patterns:**

- Commands contain instructions, Claude invokes skills (no automatic routing)
- Interactive menus built into skill logic, not bolted on
- context-resume available from start (checkpoint philosophy)

**Verification:**

- [ ] Type "/" shows all 9 commands
- [ ] Each command file has clear invocation text
- [ ] All 7 skill files exist with frontmatter
- [ ] `/dream TestIdea` generates creative-brief.md with menus
- [ ] context-resume can parse handoff files
- [ ] Interactive menus present options contextually
- [ ] CLAUDE.md updated with command/skill locations

---

## Phase 2: Workflow Engine

**Objective:** Implement core plugin-workflow for Stages 0, 1, 6 (main context stages) with full state management.

**Prerequisites:**

- Phase 0-1 complete

**Core Deliverables:**

### Stage 0: Research

- Plugin type analysis
- Context7 JUCE documentation lookup
- Professional plugin research
- Output: `plugins/[Name]/.ideas/research.md`
- Decision menu: Continue to planning / Review research / Pause

### Stage 1: Planning

- **Contract prerequisite enforcement:** BLOCKS if parameter-spec.md or architecture.md missing
- Complexity scoring: `min(params/5, 2) + algorithms + features` (cap at 5)
- Phase breakdown for complexity ≥3
- Output: `plugins/[Name]/.ideas/plan.md`
- Decision menu: Continue / Review plan / Adjust complexity / Pause

### Stage 6: Validation & Presets

- Factory preset creation
- pluginval invocation
- CHANGELOG.md generation
- PLUGINS.md status update (🚧 → ✅ Working)
- Decision menu: Install / Test in DAW / Improve / Pause

### State Management

- Git commits after each stage: `feat: [Plugin] Stage N - description`
- PLUGINS.md transitions: 💡 → 🚧 Stage N → ✅ Working
- `.continue-here.md` created and updated after each stage

### Stages 2-5 Stubs

Placeholder dispatch logic:

- Stage 2: Foundation (TODO: dispatch foundation-agent)
- Stage 3: Shell (TODO: dispatch shell-agent)
- Stage 4: DSP (TODO: dispatch dsp-agent)
- Stage 5: GUI (TODO: dispatch gui-agent)

**Architecture References:**

- `procedures/skills/plugin-workflow.md` - Complete workflow spec
- `architecture/02-core-abstractions.md` (lines 111-183) - Contract enforcement
- `architecture/06-state-architecture.md` - State machine rules

**Critical Patterns:**

- **Complexity scoring prevents over-ambition**
- **Contract enforcement is non-negotiable** (blocks Stage 1)
- **State machine prevents concurrent modification**

**Verification:**

- [ ] `/implement TestPlugin` creates plugin directory
- [ ] Stage 0 generates research.md with Context7 lookup
- [ ] Stage 1 BLOCKS if contracts missing
- [ ] Stage 1 generates plan.md with complexity score
- [ ] Stage 6 runs pluginval and updates status
- [ ] Git commits created after each stage
- [ ] `.continue-here.md` created and updated correctly
- [ ] PLUGINS.md tracks status transitions
- [ ] Decision menus present at all stage boundaries
- [ ] `/continue` can resume from any checkpoint

---

## Phase 3: Implementation Subagents

**Objective:** Create the 4 implementation subagents and integrate dispatcher pattern into plugin-workflow.

**Prerequisites:**

- Phase 0-2 complete

**Core Deliverables:**

### foundation-agent (Stage 2)

Located: `.claude/agents/foundation-agent.md`

- Creates CMakeLists.txt (JUCE 8 configuration)
- Creates empty PluginProcessor.{h,cpp}
- Creates empty PluginEditor.{h,cpp}
- Verifies compilation with `--no-install` flag
- Returns JSON report:

```json
{
  "agent": "foundation-agent",
  "status": "success",
  "outputs": {
    "cmake_path": "...",
    "processor_path": "...",
    "editor_path": "..."
  },
  "issues": [],
  "ready_for_next_stage": true
}
```

### shell-agent (Stage 3)

Located: `.claude/agents/shell-agent.md`

- Implements parameters from parameter-spec.md
- Creates APVTS (AudioProcessorValueTreeState)
- Basic processBlock stub
- Empty editor with placeholder
- Returns JSON with parameter IDs

### dsp-agent (Stage 4)

Located: `.claude/agents/dsp-agent.md`

- Implements components from architecture.md
- Phases implementation if complexity ≥3
- Real audio processing in processBlock
- Uses Opus model (complex algorithms)
- Returns JSON with components implemented

### gui-agent (Stage 5)

Located: `.claude/agents/gui-agent.md`

- Creates editor layout (FlexBox/Grid, never manual setBounds)
- Parameter bindings via attachments
- Visual styling
- Basic WebView support (full patterns in Phase 6)
- Returns JSON with control types

### Dispatcher Integration

Update plugin-workflow Stages 2-5:

- Dispatch subagent via Task tool with complete specs
- Pass all contracts (parameter-spec, architecture, plan)
- Parse JSON reports
- Handle subagent failures with decision menus
- Each subagent runs in fresh context (no accumulation)

**Architecture References:**

- `architecture/02-core-abstractions.md` (lines 74-109) - Subagent abstraction
- `architecture/07-communication-architecture.md` (lines 82-101) - Report parsing
- `procedures/skills/juce-foundation.md` - JUCE 8 patterns
- `architecture/03-model-selection-extended-thinking-strategy.md` - dsp-agent uses Opus

**Critical Patterns:**

- **Fresh context per subagent** (dispatcher pattern core)
- **Complete specs passed** (parameter-spec + architecture + plan)
- **Self-validation before return** (subagent checks own work)
- **JSON reports always returned** (even on failure)

**Verification:**

- [ ] All 4 subagent files exist in `.claude/agents/`
- [ ] Each has frontmatter (model, tools, preconditions)
- [ ] plugin-workflow dispatches via Task tool
- [ ] JSON reports parsed correctly
- [ ] `/implement SimpleGain` completes all 7 stages
- [ ] Git commits after Stages 2, 3, 4, 5
- [ ] Each stage runs in fresh context
- [ ] Decision menus present after each subagent

---

## Phase 4: Build & Troubleshooting System

**Objective:** Centralized build automation and diagnostic capabilities.

**Prerequisites:**

- Phase 0-3 complete

**Core Deliverables:**

### build-and-install.sh Script

Located: `scripts/build-and-install.sh`

7-phase pipeline:

1. Pre-flight validation (directory, CMakeLists.txt, PRODUCT_NAME)
2. Build (parallel VST3 + AU with Ninja: `-G Ninja --parallel`)
3. Extract PRODUCT_NAME (handles spaces)
4. Remove old versions from system folders
5. Install new versions to `~/Library/Audio/Plug-Ins/`
6. Clear DAW caches (Ableton, AudioUnitCache)
7. Verification (file existence, sizes, timestamps <60s)

Flags:

- `--dry-run` - Show commands without executing
- `--no-install` - Build only, don't install

Logging:

- `logs/[Plugin]/build_TIMESTAMP.log`
- Color-coded output (success, warning, error)
- Exit on error (`set -e`)

### build-automation Skill

Located: `.claude/skills/build-automation/SKILL.md`

- Wraps build-and-install.sh
- 4-option failure protocol:
  1. Investigate (analyze build log via troubleshooter)
  2. Show build output (display errors)
  3. Wait for manual fix
  4. Other
- Retry logic
- Success confirmation with decision menu

### troubleshooter Agent

Located: `.claude/agents/troubleshooter.md`

Multi-level investigation:

- **Level 1:** Basic error analysis
- **Level 2:** Context7 JUCE docs lookup
- **Level 3:** Forum/GitHub issue search
- **Level 4:** Deep research with extended thinking

Outputs structured diagnostic reports:

- Symptoms observed
- Root cause analysis
- Proposed solutions
- References

### Integration

- Stage 2 (foundation): `build-automation --no-install`
- Stages 3-6: `build-automation` (full build + install)
- Build failure → decision menu → Investigate option → troubleshooter

**Architecture References:**

- `architecture/11-build-automation-architecture.md` - Complete script spec
- `procedures/scripts/build-and-install.md` - Implementation details
- `procedures/skills/build-automation.md` - Failure protocol
- `procedures/agents/troubleshooter.md` - Investigation protocol

**Critical Patterns:**

- **Ninja for speed** (`-G Ninja`)
- **Parallel targets** (VST3 + AU simultaneously)
- **Cache clearing** (critical for DAW detection)
- **Timestamp verification** (<60 seconds = fresh build)

**Verification:**

- [ ] build-and-install.sh executable
- [ ] Builds VST3 + AU in parallel
- [ ] Clears DAW caches
- [ ] `--dry-run` shows commands
- [ ] `--no-install` builds without installing
- [ ] Logs written to `logs/[Plugin]/`
- [ ] 4-option menu on failure
- [ ] Investigate option invokes troubleshooter
- [ ] troubleshooter generates diagnostic report
- [ ] plugin-workflow uses build-automation at correct stages

---

## Phase 5: Validation System

**Objective:** Implement hybrid validation (deterministic hooks + semantic validator subagent).

**Prerequisites:**

- Phase 0-4 complete

**Core Deliverables:**

### Python Validation Scripts

Located: `.claude/hooks/validators/`

- `validate-parameters.py` - Stage 3: parameter-spec.md ↔ APVTS
- `validate-dsp-components.py` - Stage 4: architecture.md ↔ processBlock
- `validate-gui-bindings.py` - Stage 5: parameters ↔ attachments

Each script:

- Takes plugin directory as argument
- Parses contracts and implementation
- Returns exit code 0 (pass) or 1 (fail)
- Outputs specific error messages

### Hook Scripts

Located: `.claude/hooks/`

**PostToolUse Hook:**

- Code quality validation (real-time safety, allocations)
- Pattern matching for JUCE best practices
- Fast (<2s), blocking on failure

**UserPromptSubmit Hook:**

- Auto-inject context (handoff files, PLUGINS.md)
- Load relevant contracts

**Stop Hook:**

- Stage enforcement (prevents skipping)
- Verifies workflow integrity

**SubagentStop Hook:**

- Contract validation after each subagent
- Calls validation scripts
- BLOCKS if drift detected

**PreCompact Hook:**

- Preserves contracts before context compaction

**SessionStart Hook:**

- Environment validation
- Setup verification

**Hook Pattern (ALL hooks must follow):**

```bash
#!/bin/bash

# Check relevance FIRST
if [[ ! condition_makes_hook_relevant ]]; then
  echo "Hook not relevant, skipping gracefully"
  exit 0  # Success, not failure
fi

# Execute hook logic
# ...

exit 0
```

### validator Subagent

Located: `.claude/agents/validator.md`

- Semantic validation (quality, design, best practices)
- Invoked explicitly for complexity ≥4 plugins
- Compares implementation to creative intent
- Uses Sonnet model (quality > speed)
- Returns recommendations (advisory, not blocking)
- User decides whether to accept/ignore via decision menu

### Integration

- Hooks run automatically (SubagentStop after each subagent)
- validator invoked by plugin-workflow for complexity ≥4
- Decision menu presents findings: Accept / Make changes / Other

**Architecture References:**

- `architecture/02-core-abstractions.md` (lines 185-252) - Hybrid validation
- `architecture/10-extension-architecture.md` (lines 96-165) - Hook system
- `architecture/13-error-handling-recovery.md` (lines 182-461) - Hook patterns

**Critical Patterns:**

- **Layer 1 (Hooks):** Fast (2s), deterministic, blocking
- **Layer 2 (validator):** Slow (60s), semantic, advisory
- **Conditional execution:** ALL hooks check relevance first
- **Graceful skip:** `exit 0` when not applicable, NEVER `exit 1`

**Verification:**

- [ ] All 6 hooks exist in `.claude/hooks/`
- [ ] Hooks are executable (`chmod +x`)
- [ ] Each hook checks relevance before executing
- [ ] Python validation scripts exist in `.claude/hooks/validators/`
- [ ] Scripts can parse contracts and implementation
- [ ] validator subagent exists in `.claude/agents/`
- [ ] SubagentStop runs after each subagent
- [ ] validator invoked for complexity ≥4
- [ ] False positive: Hook catches parameter drift
- [ ] True negative: validator provides semantic recommendations
- [ ] Decision menu presents findings clearly

---

## Phase 6: WebView UI System

**Objective:** Complete WebView integration with two-phase design workflow and critical safety patterns.

**Prerequisites:**

- Phase 0-5 complete

**Core Deliverables:**

### ui-mockup Skill (Complete)

Located: `.claude/skills/ui-mockup/SKILL.md`

**Design Phase (Phases 3-4):**

- YAML design definition → `test.html` mockup (2 files)
- Browser testing support
- Iteration loop with decision menus
- Phase 4.5: STOP and present decision menu

**Implementation Phase (Phases 5-8, only after user confirms finalization):**

- Generate 5 additional files:
  - `ui/index.html`
  - `ui/ui.css`
  - `ui/ui.js`
  - Relay setup in PluginEditor.h
  - Parameter attachments
  - Resource provider integration
  - CMake binary data configuration

### WebView Code Templates

Located: `.claude/skills/ui-mockup/assets/webview-templates/`

- `PluginEditor-webview.h` - Header with CORRECT member order
- `PluginEditor-webview.cpp` - Implementation with resource provider
- Relay pattern boilerplate (WebSliderRelay, WebToggleButtonRelay, WebComboBoxRelay)
- Parameter attachment patterns

**CRITICAL: Member Declaration Order**

```cpp
// ✅ CORRECT ORDER (destroyed in reverse)
private:
    juce::WebSliderRelay gainRelay;              // 1. Relays first
    juce::WebBrowserComponent webView;           // 2. WebView second
    juce::WebSliderParameterAttachment attachment; // 3. Attachments last
```

### UI Design Rules

Located: `.claude/skills/ui-mockup/references/ui-design-rules.md`

- CSS constraints (use `100%`, NOT `100vh`)
- Native feel patterns (disable text selection, right-click)
- Resource provider requirements (JUCE 8, no data URLs)
- Sizing strategies (fixed vs resizable)

### UI Aesthetic Library

Located: `.claude/aesthetics/`

User-created aesthetic library managed by ui-template-library skill:
- Aesthetics saved from completed mockups
- Reusable across different plugins
- Includes visual system (colors, typography, controls) without layout specifics

### gui-agent WebView Integration

Update gui-agent to:

- Use finalized WebView files if exist
- Implement relay pattern correctly
- **ENFORCE member declaration order**
- Implement resource provider pattern
- Enable native integration
- Apply WebView best practices

### CMake Integration

Update foundation-agent CMakeLists.txt generation:

- `juce_add_binary_data` for ui/ directory
- Platform-specific WebView configuration
- WebView2 setup for Windows (future)

**Architecture References:**

- `architecture/12-webview-integration-design.md` - Complete architecture
- `procedures/webview/README.md` - ⚠️ START HERE
- `procedures/webview/best-practices.md` - ⚠️ CRITICAL: Member order
- `procedures/webview/04-parameter-binding.md` - Relay pattern
- `procedures/webview/03-communication-patterns.md` - C++ ↔ JavaScript
- `procedures/webview/common-problems.md` - Troubleshooting

**Critical Patterns:**

- **Member order:** Relays → WebView → Attachments (prevents 90% of crashes)
- **Resource provider:** JUCE 8 requirement (replaces JUCE 7 data URLs)
- **CSS constraints:** `100%` not `100vh` (initial render issue)
- **Native feel:** Disable text selection, right-click, use default cursor
- **Relay types:** WebSliderRelay, WebToggleButtonRelay, WebComboBoxRelay

**Verification:**

- [ ] ui-mockup skill has complete two-phase workflow
- [ ] ui-design-rules.md exists in skill references/
- [ ] WebView code templates exist in skill assets/
- [ ] WebView code templates exist with CORRECT member order
- [ ] gui-agent uses WebView templates
- [ ] Member order enforced in templates
- [ ] Resource provider implementation complete
- [ ] CMake includes `juce_add_binary_data`
- [ ] Test WebView plugin builds and runs
- [ ] Parameters bind correctly (C++ ↔ JavaScript)
- [ ] No release build crashes (member order verified)
- [ ] Decision menus guide mockup workflow

---

## Phase 7: Polish & Enhancement

**Objective:** Professional finish and advanced features for complete system.

**Prerequisites:**

- Phase 0-6 complete

**Core Deliverables:**

### plugin-lifecycle Enhancements

Located: `.claude/skills/plugin-lifecycle/SKILL.md`

- Advanced installation management
- System folder verification
- Uninstallation support
- Multi-format handling (VST3, AU, future formats)

### design-sync Skill

Located: `.claude/skills/design-sync/SKILL.md`

- Validate mockup ↔ creative brief consistency
- Catch design drift before implementation
- Visual comparison tools
- Recommendation generation

### deep-research Skill

Located: `.claude/skills/deep-research/SKILL.md`

- Multi-agent parallel investigation
- JUCE docs, forums, GitHub search
- Graduated protocol (simple → complex)
- Research report generation

### troubleshooting-docs Skill

Located: `.claude/skills/troubleshooting-docs/SKILL.md`

- Capture resolutions in `/troubleshooting/`
- Organize by plugin/symptom
- Build searchable knowledge base
- Pattern recognition across issues

### plugin-improve Enhancements

Add to existing skill:

- Advanced version management
- Regression testing suite
- Changelog generation improvements
- Backup verification

**Architecture References:**

- `procedures/skills/plugin-lifecycle.md`
- `procedures/skills/design-sync.md`
- `procedures/skills/deep-research.md`
- `procedures/skills/troubleshooting-docs.md`
- `procedures/skills/plugin-improve.md`

**Verification:**

- [ ] plugin-lifecycle handles complex installation scenarios
- [ ] design-sync validates mockup ↔ brief alignment
- [ ] deep-research performs parallel investigation
- [ ] troubleshooting-docs captures resolutions
- [ ] plugin-improve handles all version management scenarios
- [ ] `/improve TestPlugin` creates v1.1 with backup
- [ ] Regression tests catch breaking changes
- [ ] Knowledge base searchable and organized

---

## Implementation Strategy

### Quick Wins

1. **Phase 0 (30 minutes)**: Structure + templates
2. **Phase 1 (2-3 hours)**: Complete discovery system
3. **Test**: Type "/" and explore all workflows
4. **Phase 2 (2 hours)**: Core workflow operational
5. **Test**: `/implement TestPlugin` through planning
6. **Phase 3 (2-3 hours)**: Subagents for all stages
7. **Test**: Complete SimpleGain plugin end-to-end

### Dependency Chain

```
Foundation (Phase 0)
    ↓
Discovery Layer (Phase 1) ← Users can explore immediately
    ↓
Workflow Engine (Phase 2) ← Core development flow works
    ↓
Implementation (Phase 3) ← Can actually generate plugins
    ↓
Build + Debug (Phase 4) ← Can compile and troubleshoot
    ↓
Validation (Phase 5) ← Quality assurance
    ↓
WebView UI (Phase 6) ← Beautiful interfaces
    ↓
Polish (Phase 7) ← Professional finish
```

### Testing Progression

| Phase | Test Plugin     | What to Verify                                |
| ----- | --------------- | --------------------------------------------- |
| 0     | -               | Directory structure exists                    |
| 1     | -               | All commands discoverable, menus work         |
| 2     | TestPlugin      | Stages 0→1→6 complete                         |
| 3     | SimpleGain      | Full 7-stage workflow                         |
| 4     | MediumDelay     | Build failures handled, troubleshooting works |
| 5     | DriftTest       | Validation catches contract violations        |
| 6     | WebViewPlugin   | Beautiful UI, parameters bind correctly       |
| 7     | VersionedPlugin | v1.0 → v1.1 with all enhancements             |

---

## Critical Implementation Warnings

### WebView Member Order (Phase 6)

❌ **WRONG - Release build crashes:**

```cpp
juce::WebBrowserComponent webView;
juce::WebSliderRelay relay;
juce::WebSliderParameterAttachment attachment;
```

✅ **CORRECT - Destroyed in reverse:**

```cpp
juce::WebSliderRelay relay;           // 1. Relays first
juce::WebBrowserComponent webView;    // 2. WebView second
juce::WebSliderParameterAttachment attachment; // 3. Attachments last
```

### Contract Enforcement (Phase 2)

Stage 1 Planning BLOCKS if:

- `parameter-spec.md` missing
- `architecture.md` missing

This is non-negotiable. Prevents "code first, design later" anti-pattern.

### Hook Graceful Skip (Phase 5)

```bash
# ALWAYS check relevance first
if [[ ! relevant-condition ]]; then
  echo "Hook not relevant, skipping gracefully"
  exit 0  # Success, NOT failure
fi
```

### State Machine Protection (Phases 1-2)

If `PLUGINS.md` shows status 🚧:

- ONLY plugin-workflow can modify
- plugin-improve MUST NOT touch

Prevents concurrent modification corruption.

---

## Success Criteria

System is complete when:

- ✅ User types "/" in session 1 and discovers all workflows
- ✅ `/dream → /implement → installed plugin` works in single session
- ✅ Interactive menus guide user through all decisions
- ✅ Checkpointing and resume work from Phase 1 onward
- ✅ Build failures have investigation path (troubleshooter)
- ✅ Contracts enforce zero drift (validation catches mismatches)
- ✅ WebView plugins follow proven patterns (no crashes)
- ✅ All stages run in fresh contexts (no accumulation)
- ✅ Features discoverable through play (no docs needed)

---

## Meta

**Document Version:** 2.0.0
**Phases:** 7 (0-6)
**Philosophy:** Discovery first, implementation second, polish last
**Architecture Documents:** 18
**Procedure Files:** 42
**Based On:** `analyses/roadmap-optimization-audit.md`

**Last Updated:** 2025-11-10
**Next Steps:** Begin Phase 0 implementation
