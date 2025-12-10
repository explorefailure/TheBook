# cross-platform-build Skill

**Purpose:** Build plugins for Windows and macOS via GitHub Actions, then push to trigger CI/CD builds.

**Invoked by:** `/build-cross-platform [PluginName?]` command or natural language like "build for Windows", "cross-platform build", "I need a Windows version"

---

## Overview

This skill:
1. Ensures GitHub Actions workflow exists
2. Adds/updates plugin in workflow matrix
3. Commits and pushes to GitHub
4. Provides instructions to download artifacts

---

## Preconditions

1. **Git remote configured:** `git remote -v` shows GitHub origin
2. **Plugin exists and builds locally:** Verify with `cmake --build build --target [PluginName]_VST3`
3. **GitHub Actions workflow exists:** `.github/workflows/build-plugins.yml`

---

## Workflow File

**Location:** `.github/workflows/build-plugins.yml`

**Template:**
```yaml
name: Build Plugins

on:
  push:
    branches: [main]
    paths:
      - 'plugins/**'
      - '.github/workflows/build-plugins.yml'
  workflow_dispatch:  # Allow manual trigger

jobs:
  build:
    strategy:
      matrix:
        os: [windows-latest, macos-latest]
        plugin: [PluginName1, PluginName2]  # Update this list

    runs-on: ${{ matrix.os }}

    steps:
      - name: Checkout code
        uses: actions/checkout@v4

      - name: Setup JUCE
        run: |
          git clone --depth 1 --branch 8.0.3 https://github.com/juce-framework/JUCE.git ${{ github.workspace }}/JUCE

      - name: Configure CMake
        run: |
          cmake -B build -DCMAKE_BUILD_TYPE=Release -DJUCE_PATH=${{ github.workspace }}/JUCE

      - name: Build ${{ matrix.plugin }}
        run: |
          cmake --build build --target ${{ matrix.plugin }}_VST3 --config Release -j4

      - name: Upload VST3 artifact (Windows)
        if: runner.os == 'Windows'
        uses: actions/upload-artifact@v4
        with:
          name: ${{ matrix.plugin }}-Windows-VST3
          path: build/plugins/${{ matrix.plugin }}/${{ matrix.plugin }}_artefacts/Release/VST3/*.vst3

      - name: Upload VST3 artifact (macOS)
        if: runner.os == 'macOS'
        uses: actions/upload-artifact@v4
        with:
          name: ${{ matrix.plugin }}-macOS-VST3
          path: build/plugins/${{ matrix.plugin }}/${{ matrix.plugin }}_artefacts/Release/VST3/*.vst3
```

---

## Execution Steps

### Step 1: Verify Preconditions

```bash
# Check git remote
git remote -v | grep origin

# Check plugin builds locally
cmake --build build --target ${PLUGIN_NAME}_VST3 --config Release
```

### Step 2: Ensure Workflow Exists

Check if `.github/workflows/build-plugins.yml` exists. If not, create it from template above.

### Step 3: Update Plugin Matrix

Read current workflow file and check if plugin is in the matrix:

```yaml
plugin: [Bitcrusher, ChopShop, NewPlugin]  # Add new plugin here
```

If plugin not in list, add it.

### Step 4: Commit and Push

```bash
git add -A
git commit -m "chore: trigger cross-platform build for [PluginName]"
git push origin main
```

### Step 5: Provide Download Instructions

After push, display:

```
✓ Pushed to GitHub. Cross-platform build triggered.

Build progress: https://github.com/[owner]/[repo]/actions

When complete (~5-10 min), download artifacts:
  - [PluginName]-Windows-VST3
  - [PluginName]-macOS-VST3

From: https://github.com/[owner]/[repo]/actions/runs/latest
```

---

## Adding a New Plugin

When user wants to add a plugin to cross-platform builds:

1. Read `.github/workflows/build-plugins.yml`
2. Find the `plugin:` line in matrix
3. Add new plugin name to the array
4. Commit and push

**Example edit:**
```yaml
# Before
plugin: [Bitcrusher, ChopShop]

# After
plugin: [Bitcrusher, ChopShop, NewPlugin]
```

---

## Manual Trigger

Users can also trigger builds manually without pushing:

1. Go to GitHub repo → Actions tab
2. Select "Build Plugins" workflow
3. Click "Run workflow"
4. Select branch and run

---

## Troubleshooting

**Build fails on Windows:**
- Check CMakeLists.txt uses `${JUCE_PATH}` not hardcoded path
- Ensure no macOS-specific code without `#ifdef __APPLE__`

**Build fails on macOS:**
- Check code signing settings (ad-hoc is fine for artifacts)

**Artifacts not uploading:**
- Check path in upload-artifact step matches build output location
- VST3 path pattern: `build/plugins/[Name]/[Name]_artefacts/Release/VST3/*.vst3`

---

## CMakeLists.txt Requirement

Root CMakeLists.txt must support configurable JUCE path:

```cmake
# JUCE path - configurable via -DJUCE_PATH or defaults to local paths
if(NOT DEFINED JUCE_PATH)
    if(EXISTS "/Users/grot/JUCE")
        set(JUCE_PATH "/Users/grot/JUCE")
    elseif(EXISTS "${CMAKE_SOURCE_DIR}/JUCE")
        set(JUCE_PATH "${CMAKE_SOURCE_DIR}/JUCE")
    else()
        message(FATAL_ERROR "JUCE not found. Set -DJUCE_PATH=/path/to/JUCE")
    endif()
endif()

add_subdirectory(${JUCE_PATH} JUCE)
```

This allows GitHub Actions to pass `-DJUCE_PATH=${{ github.workspace }}/JUCE`.
