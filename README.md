```
████████╗██╗  ██╗███████╗    ██████╗  ██████╗  ██████╗ ██╗  ██╗
╚══██╔══╝██║  ██║██╔════╝    ██╔══██╗██╔═══██╗██╔═══██╗██║ ██╔╝
   ██║   ███████║█████╗      ██████╔╝██║   ██║██║   ██║█████╔╝
   ██║   ██╔══██║██╔══╝      ██╔══██╗██║   ██║██║   ██║██╔═██╗
   ██║   ██║  ██║███████╗    ██████╔╝╚██████╔╝╚██████╔╝██║  ██╗
   ╚═╝   ╚═╝  ╚═╝╚══════╝    ╚═════╝  ╚═════╝  ╚═════╝ ╚═╝  ╚═╝
        A CHOOSE YOUR OWN AU/VST PLUGIN ADVENTURE
```

# THE BOOK

> **This is an expirimental modified fork of [glittercowboy/plugin-freedom-system](https://github.com/glittercowboy/plugin-freedom-system)**
>
> Original system created by [TÂCHES](https://youtube.com/tachesteaches). This fork adds cross-platform builds, headless GUI workflow, and other enhancements.(Experimental Build Use at own risk)

An AI-assisted JUCE plugin development system that enables conversational creation of professional VST3 and AU audio plugins for macOS. Design and build custom audio processors through natural dialogue with Claude Code—no programming experience required.

---

## What's New in This Fork

### Cross-Platform Builds
Build Windows and macOS VST3 plugins via GitHub Actions—no need for a Windows machine or JUCE installed on CI runners.

```bash
/publish WaveFolder
```

- Pushes to GitHub, triggers automated builds
- Downloads Windows + macOS VST3 artifacts from Actions tab
- Configurable plugin matrix in `.github/workflows/build-plugins.yml`

### Dual-Repo Architecture
Plugins are developed in a separate repo with GitHub Actions for CI/CD:

```
/Users/grot/thebook/     <- THE BOOK framework (skills, agents, workflows)
    └── plugins/         <- Symlink to Plugins repo

/Users/grot/Plugins/     <- Your plugins (GitHub: explorefailure/Plugins)
    ├── plugins/         <- All plugin source code
    └── .github/         <- GitHub Actions for multi-OS builds
```

**Benefits:**
- One source of truth for plugin code
- Push triggers Windows + macOS builds automatically
- Framework updates separate from plugin code

### Headless GUI Workflow
Skip custom UI design and test your DSP immediately using native DAW controls.

```bash
# During /implement, choose "headless" when prompted for GUI
# Plugin uses native JUCE sliders/buttons instead of WebView
```

**Benefits:**
- Test audio processing without waiting for UI implementation
- All parameters exposed to DAW automation
- Smaller binary, faster compile times
- Add custom WebView UI later via `/improve`

### Wireframe + External Assets Workflow
Design your own graphics in Photoshop/Figma while the system handles layout and integration.

```bash
# During /dream UI design phase:
# 1. Choose "Wireframe + My Assets" (default option)
# 2. System generates wireframe with labeled dimensions
# 3. You design PNG assets externally
# 4. Place assets in plugins/[Name]/.ideas/assets/
# 5. System integrates into functional WebView UI
```

**Benefits:**
- Full creative control over visual design
- Use professional design tools you already know
- System handles the technical HTML/CSS/JS integration
- Iterate on assets without rebuilding

### Configurable JUCE Path
CMakeLists.txt now supports `-DJUCE_PATH` for CI/CD environments:

```cmake
cmake -B build -DJUCE_PATH=/path/to/JUCE
```

Falls back to local paths if not specified.

---

## Why This Exists

Traditional plugin development demands deep expertise in C++, DSP algorithms, and the JUCE framework. This barrier keeps plugin creation restricted to experienced programmers, leaving musicians, producers, and sound designers dependent on commercial tools.

THE BOOK removes that barrier entirely.

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

Brainstorm your plugin concept through conversation:
- **Creative brief** - Vision, sonic goals, UX principles
- **Parameter specification** - All controls, ranges, and mappings
- **UI mockups** - Visual design and layout (or skip for headless)

### 2. Plan (`/plan`)

Research and design the technical architecture:
- **DSP architecture** - Signal flow and processing strategy
- **Implementation plan** - Technical approach and complexity analysis

### 3. Implement (`/implement`)

Transform your specifications into a fully functional plugin through an automated workflow:

- **Build System Ready** (Stage 1): Project structure, CMake configuration, and all parameters implemented - validated automatically
- **Audio Engine Working** (Stage 2): DSP algorithms and audio processing complete - validated automatically
- **UI Integrated** (Stage 3): WebView interface connected to audio engine, OR headless native controls - validated automatically with runtime tests
- After Stage 3 validation passes: Plugin complete, ready to install

### 4. Deploy & Iterate

- `/install-plugin` - Install to system folders for DAW use
- `/test` - Run automated validation suite
- `/improve` - Add features or fix bugs (with regression testing)
- `/build-cross-platform` - Build for Windows + macOS via GitHub Actions
- `/reconcile` - Reconcile state between planning and implementation

## GUI Options

### WebView UI (Full Auto)
AI generates complete modern web-based interfaces (HTML/CSS/JS) rendered via JUCE's WebView:

- **Rapid prototyping**: See design changes instantly without rebuilding
- **Modern aesthetics**: Leverage contemporary web design patterns and animations
- **Interactive mockups**: Test and refine interfaces before implementation
- **Familiar tools**: Use web technologies many creators already understand
- **Responsive layouts**: Easily adapt UIs to different sizes and contexts

### WebView UI (Wireframe + Your Assets)
You design the graphics, system handles the code. **(Default mode)**

1. System generates wireframe with labeled dimensions and positions
2. You create PNG assets in Photoshop, Figma, or any design tool
3. Place transparent PNGs in `plugins/[Name]/.ideas/assets/`
4. System integrates assets into functional WebView UI

**Benefits:**
- Full creative control over every visual element
- Use professional tools you already know
- No CSS/HTML knowledge required
- Iterate on graphics without touching code

### Headless UI (Native Controls)
Skip custom UI entirely and use native JUCE/DAW controls:

- **Fastest iteration**: Test DSP immediately after Stage 2
- **Zero design overhead**: No mockups, no HTML/CSS, no WebView
- **Full DAW integration**: All parameters exposed for automation
- **Smaller footprint**: Reduced binary size and faster compilation
- **Progressive enhancement**: Add custom WebView UI later via `/improve`

**When to use headless:**
- Prototyping new DSP ideas
- Simple utility plugins (gain, clipper, filter)
- Testing before committing to visual design
- Plugins where DAW controls are sufficient

## Cross-Platform Distribution

### GitHub Actions Workflow

Build plugins for Windows and macOS without local cross-compilation:

```bash
# Publish and trigger builds
/publish MyPlugin

# Or manually: push changes to Plugins repo
cd /Users/grot/Plugins && git push origin main
```

**What happens:**
1. GitHub Actions clones JUCE 8.0.3
2. Builds VST3 for Windows and macOS in parallel
3. Uploads artifacts for download

**Download artifacts:**
- Go to https://github.com/explorefailure/Plugins/actions
- Select completed workflow run
- Download `MyPlugin-Windows-VST3` and `MyPlugin-macOS-VST3`

### Adding Plugins to Build Matrix

The `/publish` command automatically adds plugins to the build matrix. Manual edit:

```yaml
# /Users/grot/Plugins/.github/workflows/build-plugins.yml
matrix:
  os: [windows-latest, macos-latest]
  plugin: [Bitcrusher, WaveFolder, MyNewPlugin]
```

## Key Features

### Automated Build Pipeline

7-phase build system (`scripts/build-and-install.sh`) handles validation, compilation, installation, and verification. No manual CMake commands or Xcode configuration required.

### Quality Assurance

- Automatic validation after each stage (compile-time + runtime tests)
- validation-agent runs pluginval automatically (VST3/AU validation)
- Validation is blocking - errors must be fixed before progressing
- Regression testing on modifications
- Backup verification before destructive operations
- Build failure detection and troubleshooting

### Knowledge Base

Dual-indexed troubleshooting database (`troubleshooting/`) captures solutions to build failures, runtime issues, GUI problems, and API misuse. The system learns from every problem encountered.

**Required Reading** (`juce8-critical-patterns.md`) automatically prevents repeat mistakes by injecting proven patterns into all subagent contexts.

### DSP Patterns Library

Reusable DSP research automatically extracted from successful plugins:

```bash
# Research a DSP topic
/research-dsp compression

# View saved patterns
ls .claude/skills/dsp-patterns/patterns/
```

**Auto-extraction:** After pluginval passes, new DSP techniques are automatically saved to the patterns library. Future plugins can reference this knowledge.

**Current patterns:** oversampling, wave-folding, dc-blocking, dry-wet-mixing

**Manual control:** Say "remove the [name] pattern" to delete unwanted patterns.

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

### Workflow Modes

- **Manual mode** (default): Present decision menus at each checkpoint for full control
- **Express mode**: Auto-progress through implementation stages without intermediate menus
- **Configurable**: Set preferences in `.claude/preferences.json` or use `--express`/`--manual` flags
- **Safe**: Express mode drops to manual on any error, ensuring oversight when needed

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

- `foundation-shell-agent` (Stage 1) - Project structure and parameter management
- `dsp-agent` (Stage 2) - Audio processing
- `gui-agent` (Stage 3) - WebView UI or headless native controls
- `validation-agent` (after each stage) - Automatic validation with runtime tests

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
# 1. Dream the concept
/dream

# Brainstorm your plugin idea through conversation
# Creates: creative brief, parameter spec, UI mockups (optional)

# 2. Plan the architecture
/plan

# Research and design the technical implementation
# Creates: DSP architecture, implementation plan

# 3. Build it
/implement

# Automated workflow builds the plugin
# Choose "headless" at Stage 3 for fastest iteration

# 4. Install and test
/install-plugin YourPluginName

# Plugin now available in your DAW
```

### Build for Windows

```bash
# After plugin works locally
/publish YourPluginName

# Downloads available from GitHub Actions when complete
# https://github.com/explorefailure/Plugins/actions
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

- `/dream` - Brainstorm concept, create creative brief, parameter spec, and UI mockups
- `/plan` - Research and design DSP architecture and implementation strategy
- `/implement [Name]` - Build plugin through automated 3-stage workflow with continuous validation
- `/continue [Name]` - Resume paused workflow
- `/improve [Name]` - Modify completed plugin (with regression testing)

### Quality Assurance

- `/test [Name]` - Run automated validation suite
- `/research [topic]` - Deep investigation (3-level protocol)
- `/research-dsp [topic]` - Research DSP techniques and save to patterns library
- `/doc-fix` - Document solved problems for knowledge base
- `/add-critical-pattern` - Add current problem to Required Reading

### Deployment

- `/install-plugin [Name]` - Install to system folders
- `/uninstall [Name]` - Remove binaries (keep source)
- `/show-standalone [Name]` - Preview UI in standalone mode
- `/publish [Name]` - Push to GitHub and trigger Windows + macOS builds

### Lifecycle

- `/clean` - Interactive cleanup menu (uninstall/reset/destroy)
- `/reconcile [Name]` - Reconcile state between planning and implementation
- `/clear-cache [Name]` - Clear validation cache
- `/reset-to-ideation [Name]` - Remove implementation, keep idea/mockups
- `/destroy [Name]` - Complete removal (with verified backup)

## Project Structure

```
thebook/
├── plugins/                          # Symlink → /Users/grot/Plugins/plugins
│   └── [PluginName]/
│       ├── .ideas/                   # Contracts (immutable during impl)
│       │   ├── creative-brief.md
│       │   ├── parameter-spec.md
│       │   ├── architecture.md
│       │   ├── plan.md
│       │   └── ui-mockups/
│       ├── Source/                   # C++ implementation
│       ├── WebUI/                    # HTML/CSS/JS interface (if not headless)
│       └── CMakeLists.txt
├── .claude/
│   ├── skills/                       # Specialized workflows
│   │   ├── plugin-workflow/          # Orchestrator (Build → DSP → GUI → Validation)
│   │   ├── plugin-planning/          # Research & design (Research Complete)
│   │   ├── plugin-ideation/          # Concept brainstorming
│   │   ├── plugin-improve/           # Versioned modifications
│   │   ├── ui-mockup/                # Visual design system
│   │   ├── plugin-testing/           # Validation suite
│   │   ├── plugin-lifecycle/         # Install/uninstall/destroy
│   │   ├── cross-platform-build/     # GitHub Actions builds
│   │   ├── dsp-patterns/             # Reusable DSP research library
│   │   ├── deep-research/            # 3-level investigation
│   │   ├── troubleshooting-docs/     # Knowledge capture
│   │   └── workflow-reconciliation/  # State consistency checks
│   ├── agents/                       # Implementation subagents
│   │   ├── research-planning-agent/  # Research Complete (Stage 0)
│   │   ├── foundation-shell-agent/   # Build System Ready (Stage 1)
│   │   ├── dsp-agent/                # Audio Engine Working (Stage 2)
│   │   ├── gui-agent/                # UI Integrated (Stage 3)
│   │   ├── validation-agent/         # Automatic validation (after each stage)
│   │   ├── ui-design-agent/          # UI mockup design
│   │   ├── ui-finalization-agent/    # UI implementation scaffolding
│   │   └── troubleshoot-agent/       # Build failures
│   ├── commands/                     # Slash command prompts
│   └── hooks/                        # Validation gates
├── .github/
│   └── workflows/
│       └── build-plugins.yml         # Cross-platform CI/CD
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
- **ui-mockup finalization** auto-updates brief (treats mockup as source of truth)
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
- ✓ **Phase 8**: Cross-Platform Builds (this fork)
- ✓ **Phase 9**: Headless GUI Workflow (this fork)
- ✓ **Phase 10**: Wireframe + External Assets Workflow (this fork)

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

**For cross-platform builds:**
- GitHub account
- Repository with GitHub Actions enabled

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

Original system by [TÂCHES](https://youtube.com/tachesteaches) - [plugin-freedom-system](https://github.com/glittercowboy/plugin-freedom-system)

---

**Start building**: `/dream`
