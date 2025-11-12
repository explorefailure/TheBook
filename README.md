# Plugin Freedom System

An AI-assisted JUCE plugin development system that enables conversational creation of professional VST3 and AU audio plugins for macOS. Design and build custom audio processors through natural dialogue with Claude Code—no programming experience required.

**Created by [TÂCHES](https://youtube.com/tachesteaches)**

[![Watch the Demo](https://img.shields.io/badge/▶_Watch_Demo-1.45hr_walkthrough-red?style=for-the-badge&logo=youtube)](https://youtu.be/RsZB1K8oH0c)

## Why This Exists

Traditional plugin development demands deep expertise in C++, DSP algorithms, and the JUCE framework. This barrier keeps plugin creation restricted to experienced programmers, leaving musicians, producers, and sound designers dependent on commercial tools.

The Plugin Freedom System removes that barrier entirely.

By enabling conversational plugin development, this system:

- **Democratizes creation**: Anyone with an idea can build it, regardless of technical background
- **Prioritizes creativity**: Focus on sonic goals and UX, not implementation details
- **Accelerates iteration**: Ideas become working plugins in hours, not weeks
- **Removes gatekeeping**: Opens audio software development to the people who actually use these tools

## What You Can Build

- **Effects**: Reverbs, delays, distortion, modulation, filters, dynamics processors
- **Synthesizers**: Subtractive, FM, wavetable, granular, additive
- **Utilities**: Analyzers, meters, routing tools, MIDI processors
- **Experimental**: Custom DSP algorithms, hybrid processors, generative tools

All plugins compile to native VST3/AU formats compatible with any DAW (Ableton, Logic, Reaper, etc.).

## How It Works

### 1. Dream (`/dream`)

Brainstorm plugin concepts through conversation. Describe what you want—a vintage tape delay, a granular synthesizer, a spectral freeze effect—and refine the idea into a detailed creative brief.

### 2. Plan (`/plan`)

Design the plugin's parameters, architecture, and UI mockups. Create visual representations before writing any code. Iterate on the design until it feels right.

### 3. Implement (`/implement`)

Transform your specifications into a fully functional plugin through an automated 7-stage workflow:

- **Stage 0-1**: Research and planning (contracts)
- **Stage 2**: Foundation (project structure, CMake, basic scaffolding)
- **Stage 3**: Shell (APVTS, parameter management, state)
- **Stage 4**: DSP (audio processing algorithms)
- **Stage 5**: GUI (WebView-based interface with parameter bindings)
- **Stage 6**: Validation (automated testing, pluginval)

### 4. Deploy & Iterate

- `/install-plugin` - Install to system folders for DAW use
- `/test` - Run automated validation suite
- `/improve` - Add features or fix bugs (with regression testing)
- `/sync-design` - Validate UI consistency with creative brief
- `/reconcile` - Reconcile state between planning and implementation

## Modern Interface Design

Plugins use web-based interfaces (HTML/CSS/JS) rendered via JUCE's WebView instead of traditional GUI frameworks. This enables:

- **Rapid prototyping**: See design changes instantly without rebuilding
- **Modern aesthetics**: Leverage contemporary web design patterns and animations
- **Interactive mockups**: Test and refine interfaces before implementation
- **Familiar tools**: Use web technologies many creators already understand
- **Responsive layouts**: Easily adapt UIs to different sizes and contexts

## Key Features

### Automated Build Pipeline

7-phase build system (`scripts/build-and-install.sh`) handles validation, compilation, installation, and verification. No manual CMake commands or Xcode configuration required.

### Quality Assurance

- Automated pluginval testing (VST3/AU validation)
- Regression testing on modifications
- Backup verification before destructive operations
- Build failure detection and troubleshooting

### Knowledge Base

Dual-indexed troubleshooting database (`troubleshooting/`) captures solutions to build failures, runtime issues, GUI problems, and API misuse. The system learns from every problem encountered.

**Required Reading** (`juce8-critical-patterns.md`) automatically prevents repeat mistakes by injecting proven patterns into all subagent contexts.

### Graduated Research Protocol

3-level investigation system (`/research`) for complex problems:

- **Quick**: Single-agent targeted investigation (1-2 min)
- **Moderate**: Multi-agent parallel search (3-5 min)
- **Deep**: Comprehensive multi-level analysis (5-10 min)

### Version Management

- Semantic versioning on improvements
- Git-based state tracking
- Safe rollback capabilities
- Backup verification before destructive operations

### Lifecycle Management

- `/install-plugin` - Deploy to system folders
- `/uninstall` - Remove binaries (keep source)
- `/reset-to-ideation` - Roll back to concept stage
- `/destroy` - Complete removal with verified backup
- `/clean` - Interactive cleanup menu

## System Architecture

### Contracts (Single Source of Truth)

Every plugin has immutable contracts in `plugins/[Name]/.ideas/`:

- `creative-brief.md` - Vision, sonic goals, UX principles
- `parameter-spec.md` - Complete parameter definitions
- `architecture.md` - DSP design and signal flow
- `plan.md` - Implementation strategy
- `ui-mockups/` - Visual design references

**Zero drift**: All stages reference the same specs. No "telephone game" across workflows.

### Dispatcher Pattern

Each implementation stage runs in a fresh subagent context:

- `foundation-agent` (Stage 2) - Project structure
- `shell-agent` (Stage 3) - Parameter management
- `dsp-agent` (Stage 4) - Audio processing
- `gui-agent` (Stage 5) - WebView UI

**No context accumulation**: Clean separation prevents cross-contamination and keeps token usage minimal.

### Discovery Through Play

All features discoverable via:

- Slash command autocomplete (type `/` in Claude Code)
- Numbered decision menus at checkpoints
- Interactive skill prompts

**No manual required**: Learn by exploring, not reading docs.

### Checkpoint Protocol

At every completion point:

1. Auto-commit changes
2. Update state files (`.continue-here.md`, `PLUGINS.md`)
3. Present numbered decision menu
4. Wait for user response
5. Execute chosen action

**Never auto-proceeds**: You stay in control.

## Quick Start

### Prerequisites

- macOS (Sonoma or later recommended)
- Claude Code CLI

All other dependencies (Xcode Command Line Tools, JUCE, CMake, Python, pluginval) can be validated and installed via `/setup`.

### First-Time Setup

```bash
# Validate and configure your system dependencies
/setup

# The setup wizard will:
# - Detect your platform and installed tools
# - Offer to install missing dependencies automatically or guide manual installation
# - Save configuration for build scripts
# - Generate a system report
```

### Create Your First Plugin

```bash
# 1. Start with an idea
/dream

# Describe your plugin concept through conversation
# Example: "A tape delay with wow/flutter modulation and analog saturation"

# 2. Plan the implementation
/plan

# Design parameters, architecture, and UI mockups
# Iterate until the design feels right

# 3. Build the plugin
/implement

# Automated 7-stage workflow handles all technical implementation
# Takes 5-15 minutes depending on complexity

# 4. Install and test
/install-plugin MyPlugin

# Plugin now available in your DAW
```

### Improve an Existing Plugin

```bash
# Fix bugs or add features
/improve MyPlugin

# Describe the change
# System handles versioning, testing, and rollback safety
```

### Resume Interrupted Work

```bash
# Pick up where you left off
/continue MyPlugin

# System loads checkpoint and presents next steps
```

## Complete Command Reference

### Setup

- `/setup` - Validate and configure system dependencies (first-time setup)
  - Detects platform, checks for required tools
  - Offers automated installation or guided manual setup
  - Saves configuration to `.claude/system-config.json`

### Development Workflow

- `/dream` - Ideate new plugin concept
- `/plan` - Research and design (Stages 0-1)
- `/implement [Name]` - Build plugin (Stages 2-6)
- `/continue [Name]` - Resume paused workflow
- `/improve [Name]` - Modify completed plugin (with regression testing)

### Quality Assurance

- `/test [Name]` - Run automated validation suite
- `/sync-design [Name]` - Validate mockup ↔ brief consistency
- `/research [topic]` - Deep investigation (3-level protocol)
- `/doc-fix` - Document solved problems for knowledge base
- `/add-critical-pattern` - Add current problem to Required Reading

### Deployment

- `/install-plugin [Name]` - Install to system folders
- `/uninstall [Name]` - Remove binaries (keep source)
- `/show-standalone [Name]` - Preview UI in standalone mode

### Lifecycle

- `/clean` - Interactive cleanup menu (uninstall/reset/destroy)
- `/reset-to-ideation [Name]` - Remove implementation, keep idea/mockups
- `/destroy [Name]` - Complete removal (with verified backup)

## Project Structure

```
plugin-freedom-system/
├── plugins/                          # Plugin source code
│   └── [PluginName]/
│       ├── .ideas/                   # Contracts (immutable during impl)
│       │   ├── creative-brief.md
│       │   ├── parameter-spec.md
│       │   ├── architecture.md
│       │   ├── plan.md
│       │   └── ui-mockups/
│       ├── Source/                   # C++ implementation
│       ├── WebUI/                    # HTML/CSS/JS interface
│       └── CMakeLists.txt
├── .claude/
│   ├── skills/                       # Specialized workflows
│   │   ├── plugin-workflow/          # Orchestrator (Stages 2-6)
│   │   ├── plugin-planning/          # Research & design (Stages 0-1)
│   │   ├── plugin-ideation/          # Concept brainstorming
│   │   ├── plugin-improve/           # Versioned modifications
│   │   ├── ui-mockup/                # Visual design system
│   │   ├── plugin-testing/           # Validation suite
│   │   ├── plugin-lifecycle/         # Install/uninstall/destroy
│   │   ├── design-sync/              # Drift detection
│   │   ├── deep-research/            # 3-level investigation
│   │   └── troubleshooting-docs/     # Knowledge capture
│   ├── agents/                       # Implementation subagents
│   │   ├── foundation-agent/         # Stage 2
│   │   ├── shell-agent/              # Stage 3
│   │   ├── dsp-agent/                # Stage 4
│   │   ├── gui-agent/                # Stage 5
│   │   ├── validator/                # Stage 6
│   │   └── troubleshooter/           # Build failures
│   ├── commands/                     # Slash command prompts
│   └── hooks/                        # Validation gates
├── scripts/
│   ├── build-and-install.sh          # 7-phase build pipeline
│   └── verify-backup.sh              # Backup integrity checks
├── troubleshooting/                  # Dual-indexed knowledge base
│   ├── build-failures/
│   ├── runtime-issues/
│   ├── gui-issues/
│   ├── dsp-issues/
│   └── patterns/
│       └── juce8-critical-patterns.md  # Required Reading
├── PLUGINS.md                        # Plugin registry
└── .continue-here.md                 # Active workflow state
```

## Philosophy

This system treats plugin development as a **creative conversation**, not a coding task.

You describe the sound, behavior, and appearance you want. The system handles the technical complexity—DSP implementation, parameter management, UI rendering, build configuration, validation, deployment.

**Focus on what matters**: Creating tools that serve your music.

## Feedback Loop

The complete improvement cycle:

```
Build → Test → Find Issue → Research → Improve → Document → Validate → Deploy
    ↑                                                                      ↓
    └──────────────────────────────────────────────────────────────────────┘
```

- **deep-research** finds solutions (graduated 3-level protocol)
- **plugin-improve** applies changes (with regression testing)
- **troubleshooting-docs** captures knowledge (dual-indexed for fast lookup)
- **design-sync** prevents drift (validates contracts)
- **plugin-lifecycle** manages deployment (cache clearing, verification)
- **Required Reading** prevents repeat mistakes (auto-injected into subagents)

Every problem encountered becomes institutional knowledge. The system learns and improves over time.

## Implementation Status

- ✓ **Phase 0**: Foundation & Contracts
- ✓ **Phase 1**: Discovery System
- ✓ **Phase 2**: Workflow Engine
- ✓ **Phase 3**: Implementation Subagents
- ✓ **Phase 4**: Build & Troubleshooting System
- ✓ **Phase 5**: Validation System
- ✓ **Phase 6**: WebView UI System
- ✓ **Phase 7**: Polish & Enhancement

**System status**: Production ready.

## Requirements

### Software

**Required:**
- macOS 13+ (Sonoma recommended)
- Claude Code CLI

**Dependencies (validated/installed via `/setup`):**
- Xcode Command Line Tools (`xcode-select --install`)
- JUCE 8.0.0+ (audio plugin framework)
- Python 3.8+ (build scripts)
- CMake 3.15+ (build system)
- pluginval (plugin validation tool)
- Git

### Hardware

- Apple Silicon or Intel Mac
- 8GB RAM minimum (16GB recommended)
- 2GB free disk space per plugin

### Knowledge

- **Zero programming required**
- Basic understanding of audio plugin concepts (parameters, presets, DAW usage)
- Ability to describe sonic goals and UX preferences

## Contributing

This system is designed for personal use and learning. If you build something interesting with it, share your creative briefs and mockups—the process is the valuable part, not the compiled binaries.

## License

MIT - Use freely, modify as needed, share what you learn.

## Acknowledgments

Built with:

- [JUCE](https://juce.com/) - Cross-platform audio application framework
- [Claude Code](https://claude.com/claude-code) - AI-assisted development environment
- [Anthropic](https://anthropic.com/) - Claude AI models

---

**Start building**: `/dream`
