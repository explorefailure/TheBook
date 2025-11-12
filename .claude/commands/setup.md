---
name: setup
description: Validate and configure system dependencies for the Plugin Freedom System
---

# /setup

Validates system dependencies required for JUCE plugin development and configures the environment.

## Behavior

When user runs `/setup` or `/setup --test=SCENARIO`, invoke the system-setup skill.

**Test Mode:**
If user provides `--test=SCENARIO`, pass the scenario to the skill:
- Available scenarios: fresh-system, missing-juce, old-versions, custom-paths, partial-python
- In test mode, the skill uses mock data and makes no actual system changes
- Useful for validating the setup flow without modifying the environment

The skill will:
1. Detect current platform (macOS, Linux, Windows)
2. Check for required dependencies (Python, build tools, CMake, JUCE, pluginval)
3. Offer automated installation where possible or guide manual setup
4. Validate all installations are functional
5. Save configuration to `.claude/system-config.json`
6. Generate system report showing what was validated

## Preconditions

None - this is the first command new users should run.

## Output

Creates `.claude/system-config.json` with validated dependency paths:
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

This file is gitignored and used by build scripts to locate dependencies.
