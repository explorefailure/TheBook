---
name: system-setup
description: Validate and configure all dependencies required for Plugin Freedom System
allowed-tools:
  - Bash # For dependency checks and installation
  - Read # For checking existing config
  - Write # For creating system-config.json
  - Edit # For updating config
preconditions:
  - None - this is the entry point for new users
---

# system-setup Skill

**Purpose:** Validate and configure all dependencies required for JUCE plugin development in the Plugin Freedom System.

## Overview

This skill ensures new users can get started without friction by:
- Detecting the current platform (macOS, Linux, Windows)
- Checking for required dependencies (Python, build tools, CMake, JUCE, pluginval)
- Offering automated installation where possible
- Guiding manual installation when automation isn't available
- Validating that all tools are functional
- Saving validated configuration for build scripts

**Target platform:** macOS (extensible to Windows/Linux later)

**User experience:** Interactive, with clear choices between automated and guided setup

---

## Required Dependencies

### Python 3.8+
- **Purpose:** Build scripts and automation
- **Check:** `python3 --version`
- **Minimum version:** 3.8
- **Auto-install (macOS):** Homebrew (`brew install python3`) or download from python.org

### Build Tools
- **macOS:** Xcode Command Line Tools
  - **Check:** `xcode-select -p`
  - **Auto-install:** `xcode-select --install`
- **Linux:** GCC/Clang, build-essential
- **Windows:** Visual Studio 2019+

### CMake 3.15+
- **Purpose:** Build system for JUCE projects
- **Check:** `cmake --version`
- **Minimum version:** 3.15
- **Auto-install (macOS):** Homebrew (`brew install cmake`)

### JUCE 8.0.0+
- **Purpose:** Audio plugin framework
- **Check:** Search standard locations, validate version in `modules/juce_core/juce_core.h`
- **Standard locations:**
  - `~/JUCE`
  - `/Applications/JUCE`
  - `/usr/local/JUCE`
- **Auto-install:** Download from juce.com, extract to `~/JUCE`

### pluginval
- **Purpose:** Plugin validation tool
- **Check:** `pluginval --version` or check standard locations
- **Standard locations:**
  - `/usr/local/bin/pluginval`
  - `~/bin/pluginval`
- **Auto-install:** Download from GitHub releases, install to `/usr/local/bin`

---

## Skill Entry Point

When invoked via `/setup` command:

**Check for test mode first:**
- If user provided `--test=SCENARIO` argument, set TEST_MODE variable
- Pass test mode to all system-check.sh invocations via `--test=$SCENARIO`
- Show test mode banner if active:
  ```
  [TEST MODE: $SCENARIO]
  Using mock data - no actual system changes will be made
  ```

1. **Welcome message:**
   ```
   System Setup - Plugin Freedom System

   This will validate and configure all dependencies needed for JUCE plugin development.

   How would you like to proceed?
   1. Automated setup (install missing dependencies automatically)
   2. Guided setup (step-by-step instructions for manual installation)
   3. Check only (detect what's installed, no changes)
   4. Exit

   Choose (1-4): _
   ```

2. **Store user choice and proceed to platform detection**

---

## Platform Detection

**Step 1: Detect platform**

```bash
# Run system check script (append --test=$SCENARIO if in test mode)
bash .claude/skills/system-setup/assets/system-check.sh --detect-platform ${TEST_MODE:+--test=$TEST_MODE}
```

**Note:** If TEST_MODE is set, append `--test=$TEST_MODE` to ALL system-check.sh invocations throughout this skill.

The script returns JSON:
```json
{
  "platform": "darwin",
  "platform_version": "14.0",
  "arch": "arm64"
}
```

**Step 2: Confirm with user**

```
Detected platform: macOS 14.0 (arm64)

Is this correct?
1. Yes, continue
2. No, let me specify

Choose (1-2): _
```

---

## Dependency Validation Workflow

For each dependency (in order):

1. **Check if already installed and functional**
2. **If found:**
   - Display version and path
   - Validate it meets minimum requirements
   - Save to config
   - Continue to next dependency
3. **If not found:**
   - **Automated mode:** Offer to install automatically
   - **Guided mode:** Show manual installation instructions
   - **Check-only mode:** Report as missing, continue

### 1. Python Validation

**Check command:**
```bash
bash .claude/skills/system-setup/assets/system-check.sh --check-python
```

**Returns:**
```json
{
  "found": true,
  "version": "3.11.5",
  "path": "/usr/local/bin/python3",
  "meets_minimum": true
}
```

**If found and valid:**
```
✓ Python 3.11.5 found at /usr/local/bin/python3
```

**If not found (automated mode):**
```
✗ Python 3.8+ not found

Would you like me to install Python 3 via Homebrew?
1. Yes, install automatically
2. No, show me manual instructions
3. Skip Python (not recommended)

Choose (1-3): _
```

**If user chooses automated install:**
```bash
# Check if Homebrew is installed first
which brew || /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install Python
brew install python3

# Verify installation
python3 --version
```

**If user chooses manual instructions:**
Display content from `references/platform-requirements.md` section on Python installation.

**If not found (guided mode):**
Display manual installation instructions immediately, no automation offered.

### 2. Build Tools Validation

**macOS:**
```bash
bash .claude/skills/system-setup/assets/system-check.sh --check-xcode
```

**Returns:**
```json
{
  "found": true,
  "path": "/Library/Developer/CommandLineTools",
  "version": "15.0"
}
```

**If found:**
```
✓ Xcode Command Line Tools found (version 15.0)
```

**If not found (automated mode):**
```
✗ Xcode Command Line Tools not found

These are required for compiling C++ code on macOS.

Would you like me to install them?
1. Yes, install automatically
2. No, show me manual instructions
3. Skip (build will fail)

Choose (1-3): _
```

**Automated install:**
```bash
xcode-select --install
# Wait for installation dialog
# User must complete the GUI installation
echo "⏳ Please complete the Xcode Command Line Tools installation dialog."
echo "Press Enter when installation is complete..."
read
```

### 3. CMake Validation

**Check command:**
```bash
bash .claude/skills/system-setup/assets/system-check.sh --check-cmake
```

**Returns:**
```json
{
  "found": true,
  "version": "3.27.4",
  "path": "/usr/local/bin/cmake",
  "meets_minimum": true
}
```

**If found and valid:**
```
✓ CMake 3.27.4 found at /usr/local/bin/cmake
```

**If not found (automated mode):**
```
✗ CMake 3.15+ not found

CMake is required for building JUCE projects.

Would you like me to install CMake via Homebrew?
1. Yes, install automatically
2. No, download manually from cmake.org
3. Skip (build will fail)

Choose (1-3): _
```

**Automated install:**
```bash
brew install cmake
cmake --version
```

### 4. JUCE Validation

**This is the most complex check due to multiple possible locations.**

**Step 1: Search standard locations**
```bash
bash .claude/skills/system-setup/assets/system-check.sh --check-juce
```

**Returns:**
```json
{
  "found": true,
  "path": "/Users/lex/JUCE",
  "version": "8.0.3",
  "meets_minimum": true
}
```

OR if not found in standard locations:
```json
{
  "found": false,
  "checked_paths": [
    "/Users/lex/JUCE",
    "/Applications/JUCE",
    "/usr/local/JUCE"
  ]
}
```

**If found in standard location:**
```
✓ JUCE 8.0.3 found at /Users/lex/JUCE
```

**If not found, ask about custom location:**
```
✗ JUCE not found in standard locations

JUCE is required for plugin development.

Do you have JUCE 8.0+ installed in a custom location?
1. Yes, let me provide the path
2. No, install it for me (automated mode only)
3. No, show me how to install it manually

Choose (1-3): _
```

**If user provides custom path:**
```
Enter the full path to your JUCE installation:
(e.g., /Users/lex/Development/JUCE)

Path: _
```

Validate the provided path:
```bash
bash .claude/skills/system-setup/assets/system-check.sh --validate-juce-path "$USER_PROVIDED_PATH"
```

**If path is valid:**
```
✓ JUCE 8.0.3 found at /Users/lex/Development/JUCE
```

**If path is invalid:**
```
✗ Invalid JUCE installation at provided path

The path must contain JUCE modules (modules/juce_core/juce_core.h).

Would you like to:
1. Try a different path
2. Install JUCE to default location (~/JUCE)
3. Show manual installation instructions

Choose (1-3): _
```

**Automated JUCE installation:**
```
Installing JUCE 8.0.3 to ~/JUCE...

⏳ Downloading JUCE from juce.com...
```

```bash
# Download latest JUCE 8.x
cd ~
curl -L -o JUCE.zip "https://github.com/juce-framework/JUCE/releases/download/8.0.3/juce-8.0.3-osx.zip"

# Extract
unzip -q JUCE.zip
mv juce-8.0.3-osx JUCE

# Cleanup
rm JUCE.zip

# Verify
bash .claude/skills/system-setup/assets/system-check.sh --validate-juce-path ~/JUCE
```

**If download fails:**
Fall back to manual instructions from `references/juce-setup-guide.md`.

### 5. pluginval Validation

**Check command:**
```bash
bash .claude/skills/system-setup/assets/system-check.sh --check-pluginval
```

**Returns:**
```json
{
  "found": true,
  "version": "1.0.3",
  "path": "/usr/local/bin/pluginval"
}
```

**If found:**
```
✓ pluginval 1.0.3 found at /usr/local/bin/pluginval
```

**If not found (automated mode):**
```
✗ pluginval not found

pluginval is used to validate plugin compliance.

Would you like me to install it?
1. Yes, install automatically
2. No, show manual instructions
3. Skip (validation will be limited)

Choose (1-3): _
```

**Automated install:**
```bash
# Download latest pluginval for macOS
cd /tmp
curl -L -o pluginval.zip "https://github.com/Tracktion/pluginval/releases/latest/download/pluginval_macOS.zip"

# Extract
unzip -q pluginval.zip

# Install to /usr/local/bin
sudo mv pluginval.app/Contents/MacOS/pluginval /usr/local/bin/
sudo chmod +x /usr/local/bin/pluginval

# Cleanup
rm -rf pluginval.zip pluginval.app

# Verify
pluginval --version
```

---

## Configuration Persistence

After all dependencies are validated, create `.claude/system-config.json`:

```bash
# Generate config file
cat > .claude/system-config.json <<EOF
{
  "platform": "darwin",
  "platform_version": "14.0",
  "arch": "arm64",
  "python_path": "/usr/local/bin/python3",
  "python_version": "3.11.5",
  "xcode_path": "/Library/Developer/CommandLineTools",
  "cmake_path": "/usr/local/bin/cmake",
  "cmake_version": "3.27.4",
  "juce_path": "/Users/lex/JUCE",
  "juce_version": "8.0.3",
  "pluginval_path": "/usr/local/bin/pluginval",
  "pluginval_version": "1.0.3",
  "validated_at": "$(date -u +%Y-%m-%dT%H:%M:%SZ)"
}
EOF
```

**Add to .gitignore if not already present:**
```bash
grep -q "system-config.json" .gitignore || echo ".claude/system-config.json" >> .gitignore
```

---

## System Report

After configuration is saved, display comprehensive summary:

```
✓ System Setup Complete

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

Platform: macOS 14.0 (arm64)

Dependencies validated:
✓ Python 3.11.5 (/usr/local/bin/python3)
✓ Xcode Command Line Tools 15.0
✓ CMake 3.27.4 (/usr/local/bin/cmake)
✓ JUCE 8.0.3 (/Users/lex/JUCE)
✓ pluginval 1.0.3 (/usr/local/bin/pluginval)

Configuration saved to:
.claude/system-config.json

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

What's next?
1. Create your first plugin (/dream)
2. View available commands (type /? or press Tab)
3. Read the documentation (@README.md)
4. Run system check again (/setup)
5. Exit

Choose (1-5): _
```

**Handle user choice:**
- Choice 1: Invoke plugin-ideation skill (same as `/dream`)
- Choice 2: Show command list via `ls .claude/commands/`
- Choice 3: Display README.md
- Choice 4: Re-run system-setup skill
- Choice 5: Exit with message

---

## Error Handling

### Automated Installation Failures

If any automated installation fails:

1. **Capture error output**
2. **Display error message**
3. **Fall back to guided mode**

Example:
```
✗ Failed to install CMake via Homebrew

Error: Homebrew is not installed and automatic installation failed.

Falling back to manual instructions...

[Display manual CMake installation steps from references/platform-requirements.md]

Press Enter to continue with manual installation...
```

### Missing Critical Dependencies

If critical dependencies are missing and user skips them:

```
⚠ Warning: Critical dependencies are missing

The following required dependencies were not installed:
- CMake 3.15+ (required for building)
- JUCE 8.0+ (required for plugin development)

You will not be able to build plugins until these are installed.

Would you like to:
1. Go back and install missing dependencies
2. Save current configuration anyway (not recommended)
3. Exit without saving

Choose (1-3): _
```

### Version Too Old

If a dependency is found but version is too old:

```
✗ CMake 3.10.2 found, but version 3.15+ is required

Would you like to:
1. Update CMake via Homebrew (automated)
2. Show manual update instructions
3. Continue anyway (build may fail)

Choose (1-3): _
```

### Permission Errors

If installation fails due to permissions:

```
✗ Failed to install pluginval: Permission denied

This usually means you need sudo access.

Would you like to:
1. Retry with sudo (will prompt for password)
2. Install to ~/bin instead (no sudo required)
3. Show manual instructions

Choose (1-3): _
```

---

## Checkpoint Protocol Integration

This skill follows the checkpoint protocol:

**After each major validation step:**
1. Display what was found/installed
2. Present numbered decision menu
3. Wait for user response
4. Execute chosen action

**Major checkpoint moments:**
- After platform detection
- After each dependency validation
- After configuration is saved
- At final system report

**Always use inline numbered menus, never AskUserQuestion tool.**

---

## Integration Points

**Invoked by:**
- `/setup` command (primary entry point)
- New user onboarding
- When build scripts detect missing dependencies

**Reads:**
- `.claude/system-config.json` (if exists, to show current config)
- `references/platform-requirements.md` (platform-specific installation guides)
- `references/juce-setup-guide.md` (detailed JUCE installation)

**Creates:**
- `.claude/system-config.json` (validated dependency paths)

**Uses:**
- `assets/system-check.sh` (bash validation script)

**May invoke:**
- `plugin-ideation` skill (if user chooses to create plugin after setup)

---

## Success Criteria

Setup is successful when:

- All required dependencies are detected or installed
- All versions meet minimum requirements
- All tools are validated as functional (not just present)
- Configuration is saved to `.claude/system-config.json`
- User receives clear system report
- Decision menus presented at appropriate points
- Errors are handled gracefully with fallback options

---

## Notes for Claude

**CRITICAL REQUIREMENTS:**

1. **ALWAYS check before installing** - Never install if dependency already exists
2. **ALWAYS validate versions** - Don't assume found dependency meets minimum
3. **ALWAYS test functionality** - Run version check to ensure executable works
4. **ALWAYS get confirmation** - Present menu before any automated installation
5. **ALWAYS provide fallback** - If automation fails, offer manual instructions

**Automated vs Guided Mode:**

- **Automated mode:** Attempt installation with confirmation, fall back to guided if fails
- **Guided mode:** Show manual instructions immediately, no automation
- **Check-only mode:** Report what's found, make no changes

**Path handling:**

- Convert all paths to absolute paths
- Validate paths exist before saving to config
- Expand `~` to actual home directory path
- Check that JUCE path contains expected files (modules/juce_core/juce_core.h)

**macOS-specific notes:**

- Xcode CLI Tools installation requires GUI interaction
- Homebrew installation may require user interaction
- Some installations may need sudo (get permission first)
- `/usr/local/bin` may need sudo for writing

**Common pitfalls to AVOID:**

- Installing without checking if already present
- Not validating versions meet minimums
- Proceeding when critical dependencies missing
- Using relative paths in configuration
- Not testing if executables actually run
- Auto-proceeding without user confirmation
- Using AskUserQuestion instead of inline menus
