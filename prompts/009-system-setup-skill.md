# Create System Setup Skill and Command

<objective>
Create a comprehensive system setup skill that validates and configures all dependencies required for the Plugin Freedom System to work. This ensures new users can get started without friction by detecting missing dependencies and either installing them automatically or guiding manual installation.
</objective>

<context>
The Plugin Freedom System requires several dependencies to function:
- JUCE 8.0.0+ (audio plugin framework)
- CMake (build system)
- Build tools (Xcode on macOS, Visual Studio on Windows)
- Python 3.x (for build scripts)
- pluginval (plugin validation tool)

Currently, users cloning the repository have no automated way to verify their system is ready. This skill will be the first thing new users run via `/setup` to get their environment configured.

Target platform: macOS (extensible to Windows/Linux later)
User experience: Interactive, with clear choices between automated and guided setup
</context>

<requirements>
1. **Create slash command**: `.claude/commands/setup.md`
   - Entry point that invokes the system-setup skill
   - Should be simple, just delegates to the skill

2. **Create skill structure**: `.claude/skills/system-setup/`
   - `SKILL.md` - Main skill logic
   - `references/platform-requirements.md` - Platform-specific requirements documentation
   - `references/juce-setup-guide.md` - Detailed JUCE installation guide
   - `assets/system-check.sh` - Bash script for automated validation

3. **Skill behavior**:
   - Present initial choice: "Do you want me to automate setup where possible, or guide you through manual steps?"
   - Detect current platform (macOS, Linux, Windows)
   - Check for each required dependency in order
   - For missing dependencies:
     - Automated mode: Install automatically (with confirmation)
     - Guided mode: Provide step-by-step instructions
   - Support custom paths for existing installations
   - Save validated configuration to `.claude/system-config.json`
   - Generate system report showing what was found/installed

4. **Dependency checks** (in order):

   **Python**:
   - Check for Python 3.x (`python3 --version`)
   - Minimum version: 3.8
   - Auto-install: Guide to python.org or use Homebrew on macOS

   **Build Tools**:
   - macOS: Check for Xcode Command Line Tools (`xcode-select -p`)
   - Auto-install: `xcode-select --install`

   **CMake**:
   - Check for CMake (`cmake --version`)
   - Minimum version: 3.15
   - Auto-install: Homebrew (`brew install cmake`) or direct download

   **JUCE**:
   - Check standard locations first:
     - `~/JUCE`
     - `/Applications/JUCE`
     - `/usr/local/JUCE`
   - Validate version is 8.0.0 or higher (check modules/juce_core/juce_core.h for version)
   - If not found, ask: "Do you have JUCE installed in a custom location?"
     - Yes → Prompt for path, validate, save to config
     - No → Offer to download/install
   - Auto-install: Download from juce.com, extract to `~/JUCE`

   **pluginval**:
   - Check for pluginval binary in PATH
   - Check standard locations:
     - `/usr/local/bin/pluginval`
     - `~/bin/pluginval`
   - Auto-install: Download from GitHub releases, install to `/usr/local/bin`

5. **Configuration persistence**:
   - Save validated paths to `.claude/system-config.json`:
     ```json
     {
       "platform": "darwin",
       "juce_path": "/Users/username/JUCE",
       "cmake_path": "/usr/local/bin/cmake",
       "pluginval_path": "/usr/local/bin/pluginval",
       "python_version": "3.11.5",
       "validated_at": "2025-11-11T10:30:00Z"
     }
     ```
   - This file is used by build scripts to locate dependencies

6. **System report**:
   - After validation completes, display summary:
     ```
     ✓ System Setup Complete

     Dependencies validated:
     ✓ Python 3.11.5
     ✓ Xcode Command Line Tools 15.0
     ✓ CMake 3.27.4
     ✓ JUCE 8.0.3 (/Users/lex/JUCE)
     ✓ pluginval 1.0.3

     Configuration saved to: .claude/system-config.json

     What's next?
     1. Create your first plugin (/dream)
     2. View available commands (type /?)
     3. Read the documentation (@README.md)
     4. Exit

     Choose (1-4): _
     ```

7. **Error handling**:
   - If automated installation fails, fall back to guided mode
   - Provide troubleshooting links for common issues
   - Allow skipping optional dependencies (with warnings)
   - Never proceed if critical dependencies are missing

8. **Follow checkpoint protocol**:
   - After each major step (dependency validated), show numbered menu
   - Wait for user confirmation before proceeding
   - Allow user to skip, retry, or customize at any point
</requirements>

<implementation>
**Skill structure follows existing patterns**:
- Study `.claude/skills/plugin-workflow/` for skill architecture
- Follow the dispatcher pattern - skill orchestrates, doesn't implement directly
- Use inline numbered menus (NOT AskUserQuestion tool) for decisions
- Follow CLAUDE.md guidelines (no sycophancy, direct communication)

**Detection before action**:
- Always check if dependency exists before attempting install
- Validate versions meet minimums
- Test executables are actually functional (not just present)

**Path resolution**:
- Check multiple standard locations before asking user
- Validate user-provided paths exist and contain expected files
- Convert relative paths to absolute paths in config

**Automated installation approach**:
- macOS: Prefer Homebrew where applicable (cmake, python)
- Direct downloads for JUCE and pluginval (no package manager)
- Always show what will be installed and get confirmation first
- Provide fallback instructions if automation fails

**Why these constraints matter**:
- Version minimums ensure compatibility with JUCE 8 and modern C++
- Path persistence eliminates repeated "where is JUCE?" questions
- Automated mode reduces friction for users who trust the system
- Guided mode gives control to users who prefer manual setup
- Validation ensures we don't proceed with broken dependencies
</implementation>

<output>
Create these files:

**Command:**
- `.claude/commands/setup.md` - Slash command entry point

**Skill:**
- `.claude/skills/system-setup/SKILL.md` - Main skill logic with detection and validation
- `.claude/skills/system-setup/references/platform-requirements.md` - Platform-specific dependency documentation
- `.claude/skills/system-setup/references/juce-setup-guide.md` - Detailed JUCE installation guide
- `.claude/skills/system-setup/assets/system-check.sh` - Bash validation script (checks all dependencies, returns JSON)

**Generated at runtime:**
- `.claude/system-config.json` - Validated dependency paths (gitignored)
</output>

<verification>
Before declaring complete, verify:

1. **Command works**: Test that `/setup` successfully invokes the skill
2. **Detection logic**: Verify each dependency check works correctly
3. **Path validation**: Test custom JUCE path detection
4. **Config persistence**: Confirm system-config.json is created with valid paths
5. **Error handling**: Test behavior when dependencies are missing
6. **Menu flow**: Ensure numbered decision menus appear at correct points
7. **Follows patterns**: Skill structure matches existing skills in `.claude/skills/`

Test the skill by:
- Running `/setup` in a fresh environment
- Testing both automated and guided modes
- Providing custom paths for JUCE
- Verifying generated system-config.json is valid
</verification>

<success_criteria>
- `/setup` command exists and invokes system-setup skill
- Skill detects all required dependencies (Python, CMake, build tools, JUCE, pluginval)
- Automated mode can install missing dependencies where possible
- Guided mode provides clear instructions for manual installation
- Custom JUCE paths are supported and validated
- System configuration is persisted to `.claude/system-config.json`
- System report shows clear summary of what was validated
- Checkpoint protocol is followed (numbered menus, wait for user)
- Error messages are helpful and provide troubleshooting guidance
- Skill follows existing architectural patterns in the codebase
</success_criteria>
