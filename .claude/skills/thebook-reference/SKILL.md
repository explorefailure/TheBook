# THE BOOK Reference Guide

> **Comprehensive reference for THE BOOK architecture, workflow, and terminology.**

## Trigger Conditions
- User asks "how does THE BOOK work", "what is [term]", "explain the system"
- User says "help me understand", "reference guide", "documentation"
- User runs `/thebook` command
- Questions about workflow stages, terminology, or best practices

---

## 1. SYSTEM OVERVIEW

**THE BOOK** is a multi-agent orchestration platform for developing JUCE audio plugins. It treats plugin development as **creative conversation** rather than coding task—AI handles technical complexity (DSP, parameters, UI, build, deployment) while creators focus on sonic goals and UX design.

### Core Philosophy
- **Contract-Based Development:** Immutable specification files guide implementation
- **Staged Workflow:** Discrete stages with validation gates prevent regression
- **Subagent Delegation:** Specialized agents handle each implementation stage
- **State Persistence:** Resume from any checkpoint via handoff files
- **Knowledge Capture:** Solved problems become searchable documentation

### Key Numbers
- **18 plugins** in various lifecycle states
- **8 subagents** for implementation stages
- **17 skills** for workflow orchestration
- **22 slash commands** for quick actions
- **7 workflow stages** (Ideation → Stage 0-3 → Validation → Deployment)

---

## 2. DIRECTORY STRUCTURE

```
/Users/grot/thebook/
├── .claude/                    # System orchestration
│   ├── agents/                 # 8 subagent definitions
│   ├── skills/                 # 17 workflow orchestrators
│   ├── commands/               # 22 slash command prompts
│   ├── hooks/                  # Lifecycle automation
│   ├── aesthetics/             # UI template library
│   ├── cache/                  # Build/validation cache
│   ├── schemas/                # JSON schema definitions
│   ├── utils/                  # Utility scripts
│   ├── settings.json           # Hook configuration
│   ├── system-config.json      # Validated dependencies
│   └── preferences.json        # Workflow mode settings
│
├── plugins/                    # All plugin projects
│   └── [PluginName]/
│       ├── .ideas/             # CONTRACT FILES (immutable during impl)
│       │   ├── creative-brief.md
│       │   ├── parameter-spec.md
│       │   ├── architecture.md
│       │   ├── plan.md
│       │   └── mockups/
│       │       ├── v[N]-ui.yaml
│       │       └── v[N]-ui-test.html
│       ├── Source/             # C++ implementation
│       │   ├── PluginProcessor.h/cpp
│       │   ├── PluginEditor.h/cpp
│       │   └── ui/public/      # WebView UI files
│       ├── Presets/            # Factory presets (XML)
│       ├── CMakeLists.txt      # Build configuration
│       ├── NOTES.md            # Lifecycle timeline
│       ├── CHANGELOG.md        # Version history
│       └── .continue-here.md   # Handoff/checkpoint file
│
├── scripts/                    # Build automation
│   └── build-and-install.sh    # 7-phase build pipeline
│
├── troubleshooting/            # Knowledge base (8 categories)
│   ├── build-failures/
│   ├── runtime-issues/
│   ├── gui-issues/
│   ├── dsp-issues/
│   ├── parameter-issues/
│   ├── api-usage/
│   ├── validation-problems/
│   └── patterns/
│       └── juce8-critical-patterns.md  # REQUIRED READING
│
├── backups/                    # Safety storage
├── PLUGINS.md                  # Master registry
├── README.md                   # System documentation
└── CMakeLists.txt              # Root build config
```

---

## 3. WORKFLOW STAGES

### Stage Overview

| Stage | Name | Purpose | Output |
|-------|------|---------|--------|
| **Ideation** | Dream | Brainstorm plugin concept | creative-brief.md, parameter-draft, mockups |
| **Stage 0** | Plan | Research & architecture | architecture.md, plan.md |
| **Stage 1** | Foundation | Build system + parameters | CMakeLists.txt, PluginProcessor skeleton |
| **Stage 2** | DSP | Audio processing | processBlock implementation |
| **Stage 3** | GUI | UI integration | WebView UI + parameter bindings |
| **Final** | Validation | Comprehensive testing | ✅ Working status |
| **Deploy** | Install | System deployment | 📦 Installed status |

### Detailed Stage Breakdown

#### IDEATION (💡)
**Command:** `/dream [PluginName]`
**Skill:** plugin-ideation

Creates the foundational vision through adaptive questioning:
- 8-phase workflow for new plugins
- 4-tier gap analysis (Critical → Important → Nice → Consider)
- Generates creative-brief.md with sonic goals, UX principles
- Optional: parameter-spec.md draft, UI mockup

**Output Files:**
- `plugins/[Name]/.ideas/creative-brief.md`
- `plugins/[Name]/.ideas/parameter-spec.md` (optional)
- `plugins/[Name]/.ideas/mockups/` (optional)

---

#### STAGE 0: Research & Planning (🚧 Stage 0)
**Command:** `/plan [PluginName]`
**Skill:** plugin-planning
**Agent:** research-planning-agent

Analyzes creative brief, researches DSP techniques, creates implementation strategy:
- Calculates complexity score (0-5+ scale)
- Defines phase breakdown for complex plugins
- Maps JUCE APIs to requirements

**Output Files:**
- `plugins/[Name]/.ideas/architecture.md` - DSP design specification
- `plugins/[Name]/.ideas/plan.md` - Implementation strategy with phases

**Preconditions:**
- creative-brief.md must exist
- parameter-spec.md should exist (blocks Stage 1 if missing)

---

#### STAGE 1: Foundation + Shell (🚧 Stage 1)
**Command:** `/implement [PluginName]`
**Skill:** plugin-workflow
**Agent:** foundation-shell-agent

Creates compilable plugin project with all parameters:
- CMakeLists.txt with JUCE 8 configuration (VST3, AU, Standalone)
- PluginProcessor with APVTS and all parameters from spec
- PluginEditor skeleton
- State management (save/load)

**Output Files:**
- `plugins/[Name]/CMakeLists.txt`
- `plugins/[Name]/Source/PluginProcessor.h/cpp`
- `plugins/[Name]/Source/PluginEditor.h/cpp`
- `plugins/[Name]/Source/PluginIDs.h`

**Validation:**
- Semantic: JUCE 8 patterns, parameter matching
- Runtime: pluginval smoke test (~10s)

---

#### STAGE 2: DSP Implementation (🚧 Stage 2)
**Skill:** plugin-workflow
**Agent:** dsp-agent

Implements audio processing algorithms:
- processBlock() with DSP algorithms
- prepareToPlay() initialization
- Parameter-to-DSP connections
- Real-time safety (no allocations in audio thread)

**For Complex Plugins (complexity ≥3):**
- Phases defined in plan.md (Phase 2.1, 2.2, 2.3, etc.)
- dsp-agent invoked once per phase sequentially
- Checkpoint after each phase

**Validation:**
- Semantic: Real-time safety, DSP matches architecture
- Runtime: pluginval functional test (~2-3 min)

**GUI Decision Gate:**
After Stage 2, user chooses:
1. **Custom UI** → Proceed to Stage 3
2. **Headless** → Generate minimal editor, skip to Final

---

#### STAGE 3: GUI Integration (🚧 Stage 3)
**Skill:** plugin-workflow
**Agent:** gui-agent

Integrates WebView UI with parameter bindings:
- Creates ui/public/ directory structure
- Embeds HTML/CSS/JS into plugin
- Creates parameter bindings (relay + attachment pairs)
- WebView initialization in PluginEditor

**CRITICAL:** Member declaration order must be:
1. Relays (ParameterRelay objects)
2. WebView (WebBrowserComponent)
3. Attachments (SliderAttachment, etc.)

**Validation:**
- Semantic: Member order, parameter bindings complete
- Runtime: pluginval full GUI test (~5-10 min)

---

#### FINAL VALIDATION (✅ Working)
Comprehensive testing before release:
- All pluginval tests passed
- CHANGELOG.md verified (Keep a Changelog format)
- Version set to 1.0.0
- Presets verified (3+ files)
- PLUGINS.md status updated

---

#### DEPLOYMENT (📦 Installed)
**Command:** `/install-plugin [PluginName]`
**Skill:** plugin-lifecycle

Installs to system folders:
- `~/Library/Audio/Plug-Ins/VST3/`
- `~/Library/Audio/Plug-Ins/Components/` (AU)
- Clears DAW caches (Ableton, Logic)
- Verifies permissions

**Optional:** `/package [PluginName]` creates PKG installer

---

## 4. SKILLS, AGENTS, AND COMMANDS

### Skills (Orchestrators)

| Skill | Command | Purpose |
|-------|---------|---------|
| plugin-ideation | `/dream` | Brainstorm new plugins |
| plugin-planning | `/plan` | Stage 0 research & planning |
| plugin-workflow | `/implement` | Stages 1-3 implementation |
| plugin-testing | `/test` | Manual validation |
| plugin-improve | `/improve` | Versioned modifications |
| plugin-lifecycle | `/install-plugin`, `/uninstall`, `/destroy` | Lifecycle management |
| plugin-packaging | `/package` | PKG installer creation |
| context-resume | `/continue` | Resume from checkpoint |
| deep-research | `/research` | Complex problem investigation |
| ui-mockup | - | WebView UI design |
| ui-template-library | - | Aesthetic template management |
| workflow-reconciliation | `/reconcile` | State file repair |
| troubleshooting-docs | `/doc-fix` | Knowledge capture |
| system-setup | `/setup` | Dependency validation |

### Agents (Implementers)

| Agent | Stage | Purpose |
|-------|-------|---------|
| research-planning-agent | 0 | Create architecture.md, plan.md |
| foundation-shell-agent | 1 | CMakeLists + APVTS implementation |
| dsp-agent | 2 | Audio processing algorithms |
| gui-agent | 3 | WebView UI integration |
| validation-agent | All | Semantic + runtime validation |
| ui-design-agent | Mockup | Generate UI mockups |
| ui-finalization-agent | Mockup | Implementation scaffolding |
| troubleshoot-agent | Any | Build failure diagnosis |

### Agent Execution Model

**Agents run SEQUENTIALLY, not simultaneously.**

```
Stage 0 ──→ Stage 1 ──→ Stage 2 ──→ Stage 3
   │           │           │           │
   ▼           ▼           ▼           ▼
 plan       foundation    dsp        gui
 agent        agent      agent      agent
               │           │           │
               ▼           ▼           ▼
            validate    validate    validate
```

#### Why Sequential?

Each stage **depends on the previous stage's output**:
- Stage 1 needs `architecture.md` from Stage 0
- Stage 2 needs `PluginProcessor.h` from Stage 1
- Stage 3 needs DSP code from Stage 2

#### Why 8 Separate Agents Instead of One?

1. **Fresh Context** - Each agent starts clean with no accumulated confusion or context pollution
2. **Specialization** - DSP agent knows audio algorithms; GUI agent knows WebView patterns
3. **Token Efficiency** - Only load relevant knowledge per stage (not entire codebase context)
4. **Isolation** - A bug in Stage 2 doesn't corrupt Stage 3's reasoning
5. **Validation Gates** - validation-agent runs between each stage, catching issues early

#### The Assembly Line Mental Model

Think of it like a factory assembly line:
- **Specialists at each station** - Each agent has deep expertise in one area
- **Work passes forward** - Output from one stage becomes input for the next
- **Quality control between steps** - validation-agent inspects work before it moves on
- **No backtracking** - If validation fails, fix issues before proceeding

#### When Agents Run

| Trigger | Agent Invoked |
|---------|---------------|
| `/plan` | research-planning-agent |
| `/implement` (Stage 1) | foundation-shell-agent → validation-agent |
| `/implement` (Stage 2) | dsp-agent → validation-agent |
| `/implement` (Stage 3) | gui-agent → validation-agent |
| UI mockup workflow | ui-design-agent → ui-finalization-agent |
| Build errors | troubleshoot-agent |

### Key Commands

```bash
# Ideation & Planning
/dream [PluginName]        # Brainstorm new plugin
/plan [PluginName]         # Stage 0 planning

# Implementation
/implement [PluginName]    # Build through Stages 1-3
/continue [PluginName]     # Resume from checkpoint

# Testing & Validation
/test [PluginName]         # Manual validation
/research [topic]          # Deep investigation

# Lifecycle
/install-plugin [Name]     # Deploy to system
/uninstall [Name]          # Remove binaries
/reset-to-ideation [Name]  # Keep ideas, remove code
/destroy [Name]            # Complete removal
/clean [Name]              # Interactive cleanup menu
/package [Name]            # Create PKG installer

# Improvement
/improve [Name] [desc]     # Versioned modifications
/doc-fix [context]         # Document solved problem

# System
/setup                     # Validate dependencies
/reconcile [Name]          # Repair state files
/pfs                       # Load this reference guide
```

---

## 5. TERMINOLOGY & GLOSSARY

### Core Concepts

| Term | Definition |
|------|------------|
| **Contract** | Immutable specification file (.ideas/) that guides implementation. Cannot be modified during Stages 1-3. |
| **Handoff File** | `.continue-here.md` - YAML frontmatter + context for resuming workflow |
| **Checkpoint** | Decision point after stage/phase completion with validation |
| **Phase** | Sub-division of a stage for complex plugins (e.g., Phase 2.1, 2.2) |
| **Complexity Score** | 0-5+ rating determining if phased implementation needed (≥3 = phased) |
| **Required Reading** | juce8-critical-patterns.md - injected into all subagent contexts |
| **Express Mode** | Auto-progress through stages without decision menus |
| **Manual Mode** | Present decision menu at every checkpoint |

### Status Emojis

| Emoji | Status | Description |
|-------|--------|-------------|
| 💡 | Ideated | Creative brief exists, not implemented |
| 🚧 | Stage N | Currently in implementation stage N |
| ✅ | Working | All stages complete, passes validation |
| 📦 | Installed | Deployed to ~/Library/Audio/Plug-Ins/ |
| 🐛 | Has Issues | Known bugs or problems |
| 🗑️ | Archived | No longer maintained |

### File Types

| File | Purpose |
|------|---------|
| `creative-brief.md` | Plugin vision, sonic goals, UX principles |
| `parameter-spec.md` | Parameter definitions with IDs, ranges, defaults |
| `architecture.md` | DSP components, processing chain, signal flow |
| `plan.md` | Implementation strategy with phases and complexity score |
| `v[N]-ui.yaml` | UI design specification (YAML format) |
| `v[N]-ui-test.html` | Browser-testable UI mockup |
| `.continue-here.md` | Workflow checkpoint/handoff file |
| `NOTES.md` | Lifecycle timeline, known issues |
| `CHANGELOG.md` | Version history (Keep a Changelog format) |
| `PLUGINS.md` | Master registry of all plugins |

### Technical Terms

| Term | Definition |
|------|------------|
| **APVTS** | AudioProcessorValueTreeState - JUCE's thread-safe parameter system |
| **WebView** | HTML/CSS/JS UI rendered via WebBrowserComponent |
| **Relay** | ParameterRelay object connecting APVTS to WebView |
| **Attachment** | SliderAttachment/ButtonAttachment binding UI control to parameter |
| **pluginval** | Industry-standard plugin validation tool |
| **Smoke Test** | Quick validation that plugin loads (~10s) |
| **Functional Test** | Parameter automation, state, thread safety (~2-3 min) |
| **Full GUI Test** | Complete validation including UI (~5-10 min) |

### Orchestration Terms

| Term | Definition |
|------|------------|
| **Skill** | Orchestrator script that manages workflow and invokes agents |
| **Agent/Subagent** | Autonomous implementer running in fresh context |
| **Dispatcher** | Pattern where skill loops through phases, invoking agent per phase |
| **Checkpoint Protocol** | 6-step verification after every subagent completion |
| **State Delegation** | Subagents update state files; orchestrator verifies |
| **Fallback** | Orchestrator updates state if subagent didn't |

---

## 6. CONTRACT FILES

### Immutability Rule
Contract files in `.ideas/` are **READ-ONLY** during Stages 1-3:
- `creative-brief.md`
- `parameter-spec.md`
- `architecture.md`
- `plan.md`
- `mockups/v[N]-ui.yaml`

**When Contracts CAN Be Modified:**
- During Stage 0 (planning) - architecture.md, plan.md created
- After Stage 3 complete (via `/improve`)
- During ideation updates (via `/dream`)

### Contract Checksums
`.continue-here.md` stores SHA256 checksums:
```yaml
contract_checksums:
  creative_brief: sha256:abc123...
  parameter_spec: sha256:def456...
  architecture: sha256:ghi789...
  plan: sha256:jkl012...
```

Validation detects tampering by comparing checksums.

---

## 7. STATE MANAGEMENT

### Per-Plugin State Files

#### .continue-here.md (Handoff)
```yaml
---
plugin: PluginName
stage: 2
status: in_progress
last_updated: 2025-12-05
complexity_score: 3.5
phased_implementation: true
workflow_mode: express
gui_type: webview
current_phase: "2.2"
next_action: "Implement Phase 2.3"
contract_checksums:
  creative_brief: sha256:...
  parameter_spec: sha256:...
---

## Completed So Far
- **Stage 0:** Architecture & planning ✅
- **Stage 1:** Foundation shell ✅
- **Phase 2.1:** Core processing ✅
- **Phase 2.2:** Parameter modulation (in progress)
```

#### NOTES.md (Timeline)
```markdown
## Status: 🚧 Stage 2 (v0.0.0)

## Lifecycle Timeline
- 2025-12-01: Creative brief completed
- 2025-12-02 (Stage 0): Architecture & planning
- 2025-12-03 (Stage 1): Foundation (Build System Ready)
- 2025-12-04 (Phase 2.1): Core DSP implemented
- 2025-12-05 (Phase 2.2): Parameter modulation in progress

## Known Issues
- None currently
```

### Global State

#### PLUGINS.md (Registry)
```markdown
| Plugin Name | Status | Version | Type | Last Updated |
|-------------|--------|---------|------|--------------|
| GainKnob | 📦 Installed | 1.2.3 | Effect | 2025-11-10 |
| ChopShop | 💡 Ideated | - | Effect | 2025-12-05 |
| MinimalKick | 🚧 Stage 5 | - | Synth | 2025-11-13 |
```

---

## 8. BEST PRACTICES FOR MODIFYING THE SYSTEM

### Adding a New Skill

1. **Create skill directory:**
   ```
   .claude/skills/[skill-name]/SKILL.md
   ```

2. **Define trigger conditions** at top of SKILL.md:
   ```markdown
   ## Trigger Conditions
   - User says "keyword1", "keyword2"
   - User runs `/command` command
   - Automatic: [condition]
   ```

3. **Define skill sections:**
   - Overview/purpose
   - Preconditions
   - Process steps
   - Output files
   - Success criteria
   - Error handling

4. **If skill invokes subagents:**
   - Create agent file in `.claude/agents/[agent-name].md`
   - Define agent's scope, inputs, outputs
   - Specify what agent should NOT do

5. **Add slash command** (optional):
   - Create `.claude/commands/[command].md`
   - Reference skill in command prompt

### Adding a New Agent

1. **Create agent file:**
   ```
   .claude/agents/[agent-name].md
   ```

2. **Define agent specification:**
   - Model (sonnet/opus) and color
   - Task description
   - Input contracts required
   - Output files created
   - What agent should NOT do
   - Precondition checks

3. **Include Required Reading injection:**
   ```markdown
   ## Context Injection
   Before starting, read:
   - troubleshooting/patterns/juce8-critical-patterns.md
   ```

4. **Define JSON return format:**
   ```json
   {
     "agent": "agent-name",
     "status": "success|error",
     "outputs": { ... },
     "stateUpdated": true
   }
   ```

### Adding a New Command

1. **Create command file:**
   ```
   .claude/commands/[command].md
   ```

2. **Write prompt that invokes skill:**
   ```markdown
   Invoke the [skill-name] skill for [PluginName].

   Mode: [mode if applicable]
   Arguments: $ARGUMENTS
   ```

3. **Document in README.md** commands section

### Modifying Workflow Stages

**CAUTION:** Stage modifications affect all plugins. Test thoroughly.

1. **Update stage agent** in `.claude/agents/`
2. **Update orchestrator skill** in `.claude/skills/plugin-workflow/`
3. **Update validation-agent** checks if adding new requirements
4. **Update checkpoint protocol** if changing stage boundaries
5. **Test with simple plugin** before using on complex projects

### Adding to Required Reading

**Method 1: Full documentation flow**
```bash
/doc-fix "description of the fix"
# Follow prompts, select "Promote to Required Reading"
```

**Method 2: Fast-track**
```bash
/add-critical-pattern [pattern name]
```

**Method 3: Manual edit**
- Edit `troubleshooting/patterns/juce8-critical-patterns.md`
- Add pattern with clear problem/solution format

### Modifying Validation Rules

1. **Edit validation-agent** in `.claude/agents/validation-agent.md`
2. **Add semantic check** with:
   - Check name
   - Pass/fail condition
   - Error message
   - Severity (error/warning/info)
3. **Update pluginval flags** if changing runtime tests

### Configuration Changes

#### Workflow Mode
Edit `.claude/preferences.json`:
```json
{
  "workflow": {
    "mode": "express",       // or "manual"
    "auto_install": true,    // auto-deploy after Stage 3
    "auto_package": false    // auto-create PKG
  }
}
```

#### Hook Behavior
Edit `.claude/settings.json` to modify:
- SessionStart validation
- PostToolUse code checks
- PreCompact contract preservation

---

## 9. COMMON PATTERNS

### Checkpoint Protocol (6 Steps)
After every subagent completion:
1. **Verify State Update** - Check subagent updated .continue-here.md
2. **Fallback State Update** - Orchestrator updates if subagent didn't
3. **Invoke Validation** - Run validation-agent (BLOCKING on failures)
4. **Commit Stage** - Git commit all changes
5. **Verify Checkpoint** - Validate completion
6. **Handle Checkpoint** - Menu (manual) or auto-progress (express)

### Phase-Aware Dispatch
For complexity ≥3:
```
FOR each phase in plan.md:
    1. Invoke agent with "Implement ONLY Phase [N.M]"
    2. Verify agent preserved previous phase code
    3. Run validation
    4. Checkpoint
    5. Continue to next phase
```

### State Delegation
```
Subagent:
    1. Implement stage/phase
    2. Update .continue-here.md
    3. Update PLUGINS.md
    4. Return JSON with stateUpdated: true

Orchestrator:
    1. Receive JSON
    2. Verify stateUpdated AND files actually changed
    3. If not: Trigger fallback (orchestrator updates)
    4. Proceed with validation
```

### Resume Pattern
```bash
/continue [PluginName]
```
1. Find .continue-here.md
2. Parse YAML frontmatter
3. Verify contract checksums
4. Route to appropriate skill
5. Resume at recorded stage/phase

---

## 10. TROUBLESHOOTING

### Common Issues

| Problem | Solution |
|---------|----------|
| "Contract not found" | Run `/plan` before `/implement` |
| "State mismatch" | Run `/reconcile [PluginName]` |
| "Build failed" | Check `build.log`, run `/research [error]` |
| "Validation failed" | Review validation output, fix issues, re-run |
| "Stage stuck" | Check .continue-here.md, verify status field |
| "Plugin won't load in DAW" | Clear caches, check permissions, run `/test` |

### State Recovery

**Minor drift:**
```bash
/reconcile [PluginName]
```

**Major corruption:**
1. Check git history: `git log plugins/[Name]/`
2. Restore from backup: `backups/[Name]-[timestamp]/`
3. Reset to ideation: `/reset-to-ideation [Name]`

### Debug Commands

```bash
# Check plugin status
cat plugins/[Name]/.continue-here.md

# Check global registry
grep "[Name]" PLUGINS.md

# View recent builds
ls -la plugins/[Name]/build.log

# Check validation logs
ls logs/[Name]/
```

---

## 11. QUICK REFERENCE CARD

### New Plugin Flow
```
/dream → /plan → /implement → /install-plugin
```

### Resume Flow
```
/continue [Name]
```

### Improvement Flow
```
/improve [Name] "description"
```

### Problem Investigation
```
/research "problem description"
```

### Cleanup Options
```
/uninstall [Name]        # Remove binaries only
/reset-to-ideation [Name] # Keep ideas, remove code
/destroy [Name]          # Complete removal
/clean [Name]            # Interactive menu
```

---

*This reference guide is part of the THE BOOK. Use `/pfs` to recall it at any time.*
