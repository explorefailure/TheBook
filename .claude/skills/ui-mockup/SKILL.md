---
name: ui-mockup
description: Generate production-ready WebView UI mockups in two phases - design iteration (2 files) then implementation scaffolding (5 files after approval)
allowed-tools:
  - Read
  - Write
preconditions:
  - None (can work standalone or with creative brief)
---

# ui-mockup Skill

**Purpose:** Generate production-ready WebView UIs in two phases. The HTML generated IS the plugin UI, not a throwaway prototype.

## Workflow Overview

<critical_sequence phases="A,B" enforcement="gate">
<phase id="A" name="Design Iteration">
**Purpose:** Generate 2 design files for rapid iteration.

**Outputs:**
1. **v[N]-ui.yaml** - Machine-readable design specification
2. **v[N]-ui-test.html** - Browser-testable mockup (no JUCE required)

<decision_gate id="design_approval" required="true">
**STOP:** Do NOT proceed to Phase B until user approves design via Phase 5.5 decision menu.

**Gate criteria:**
- User selected option 2 from Phase 5.5 menu ("Finalize")
- Design validated against WebView constraints (Phase 5.3)
- Design validated against creative brief (Phase 5.6 automatic validation)
</decision_gate>
</phase>

<phase id="B" name="Implementation Scaffolding" requires_gate="design_approval">
**Purpose:** Generate 5 implementation files ONLY after Phase A approval.

**Outputs:**
3. **v[N]-ui.html** - Production HTML (copy-paste to plugin)
4. **v[N]-PluginEditor.h** - C++ header boilerplate
5. **v[N]-PluginEditor.cpp** - C++ implementation boilerplate
6. **v[N]-CMakeLists.txt** - WebView build configuration
7. **v[N]-integration-checklist.md** - Implementation steps
</phase>
</critical_sequence>

**Why two phases?** HTML mockups are cheap to iterate. C++ boilerplate is pointless if design isn't locked. This saves time by avoiding premature scaffolding generation.

All files saved to: `plugins/[PluginName]/.ideas/mockups/`

## Workflow Context Detection

This skill operates in two modes:

**Standalone Mode**:
- Invoked directly via natural language or /dream command
- No workflow state files present
- Generates mockups independently
- Skips state updates to .continue-here.md

**Workflow Mode**:
- Invoked by plugin-workflow skill during Stage 0
- File exists: `plugins/[PluginName]/.continue-here.md`
- File contains: `current_stage` field
- Updates workflow state after each phase

**Detection Logic**:
```bash
if [ -f "plugins/[PluginName]/.continue-here.md" ]; then
    # Workflow mode: update state files
    WORKFLOW_MODE=true
else
    # Standalone mode: skip state updates
    WORKFLOW_MODE=false
fi
```

## Phase 0: Check for Aesthetic Library

**Before starting design, check if saved aesthetics exist.**

```bash
if [ -f .claude/aesthetics/manifest.json ]; then
    AESTHETIC_COUNT=$(jq '.aesthetics | length' .claude/aesthetics/manifest.json)
    if [ $AESTHETIC_COUNT -gt 0 ]; then
        echo "Found $AESTHETIC_COUNT saved aesthetics"
    fi
fi
```

**If aesthetics exist, present decision menu:**

```
Found $AESTHETIC_COUNT saved aesthetics in library.

How would you like to start the UI design?
1. Start from aesthetic template - Apply saved visual system
2. Start from scratch - Create custom design
3. List all aesthetics - Browse library before deciding

Choose (1-3): _
```

**Option handling:**

- **Option 1: Start from aesthetic template**
  - Read manifest: `.claude/aesthetics/manifest.json`
  - Display available aesthetics with metadata:
    ```
    Available aesthetics:

    1. Vintage Hardware (vintage-hardware-001)
       Vibe: Vintage analog
       Colors: Orange/cream/brown with paper texture
       From: TapeAge mockup v2

    2. Modern Minimal Blue (modern-minimal-002)
       Vibe: Clean, modern
       Colors: Blue/gray/white with subtle shadows
       From: EQ4Band mockup v1

    4. None (start from scratch)

    Choose aesthetic: _
    ```
  - If user selects aesthetic: Invoke ui-template-library skill with "apply" operation
  - Skip to Phase 4 with generated mockup from aesthetic

- **Option 2: Start from scratch**
  - Continue to Phase 1 (load context)

- **Option 3: List all aesthetics**
  - Invoke ui-template-library skill with "list" operation
  - Show preview paths
  - Return to option menu

**If no aesthetics exist:**
- Skip Phase 0
- Continue directly to Phase 1

**See:** `references/aesthetic-integration.md` for complete integration details

---

## Phase 1: Load Context from Creative Brief

**CRITICAL: Read creative-brief.md if it exists:**

```bash
if [ -f "plugins/$PLUGIN_NAME/.ideas/creative-brief.md" ]; then
    # Extract context (see references/context-extraction.md)
    # Continue to Phase 1.5 with design-informed prompt
else
    # Skip to Phase 1.5 with generic prompt (standalone mode)
fi
```

**Note:** preconditions="None" means skill can work standalone without creative-brief.md, but MUST read it when present.

**Context extraction checklist:**
- Plugin type (compressor, EQ, reverb, synth, utility)
- Parameter count and types (sliders, toggles, combos)
- Visual style mentions (vintage, modern, minimal, skeuomorphic)
- Layout preferences (horizontal, vertical, grid, custom)
- Special elements (meters, waveforms, visualizers, animations)
- Color/theme references (dark, light, specific colors)

**See:** `references/context-extraction.md#example-extracting-from-creative-brief` for detailed extraction examples and guidelines

**Extract UI context from creative-brief.md:**

- **UI Concept section:** Layout preferences, visual style mentions
- **Parameters:** Count and types (determines control layout)
- **Plugin type:** Effect/synth/utility (affects typical layouts)
- **Vision section:** Any visual references or inspirations

## Phase 1.5: Context-Aware Initial Prompt

**Adapt the prompt based on what's in the creative brief:**

**If rich UI details exist:**
```
I see you want [extracted description from UI Concept] for [PluginName]. Let's refine that vision. Tell me more about the layout, control arrangement, and visual elements you're imagining.
```

**If minimal UI details:**
```
Let's design the UI for [PluginName]. You mentioned it's a [type] with [X] parameters. What layout and style are you envisioning?
```

**If zero UI context:**
```
Let's design the UI for [PluginName]. What do you envision? (layout, style, controls, visual elements)
```

**Why context-aware:** Don't ask the user to repeat information they already provided in the creative brief. Build on what they said.

**Listen for:**

- Layout preferences ("controls on left, visualizer on right")
- Visual references ("like FabFilter Pro-C", "vintage analog gear")
- Mood/feel ("minimal and clean", "skeuomorphic wood panels")
- Special requests ("animated VU meter", "resizable window")

**Capture verbatim notes before moving to targeted questions.**

## Phase 2: Gap Analysis and Question Prioritization

**Question Priority Tiers:**

- **Tier 1 (Critical):** Layout structure, control types
- **Tier 2 (Visual):** Visual style, key visual elements (meters, waveforms, displays)
- **Tier 3 (Polish):** Colors, typography, animations, interactive features

**Extract from Phase 1.5 response and creative brief, then identify gaps:**

1. Parse user's UI description
2. Check which tiers are covered
3. Identify missing critical/visual information
4. Never ask about already-provided information

**Example of context-aware extraction:**

```
Creative brief UI Concept:
"Vintage aesthetic with three knobs"

Phase 1.5 user response:
"I want the knobs arranged horizontally, with a tape reel animation in the background"

Extracted:
- Layout: horizontal arrangement ✓
- Control type: knobs ✓
- Visual style: vintage ✓
- Special element: tape reel animation ✓

Gaps identified:
- Window size? (Tier 1)
- Vintage style details? (Tier 2)
- Knob style (large vintage vs small precise)? (Tier 2)
- VU meters or other feedback? (Tier 2)
```

## Phase 3: Question Batch Generation

**Generate exactly 4 questions using AskUserQuestion based on identified gaps.**

**Rules:**
- If 4+ gaps exist: ask top 4 by tier priority
- If fewer gaps exist: pad with "nice to have" tier 3 questions
- Provide meaningful options (not just open text prompts)
- Always include "Other" option for custom input
- Users can skip questions via "Other" option and typing "skip"

**Note:** Internal question routing uses AskUserQuestion tool, but final decision menus (Phase 5.5, Phase 10.7) MUST use inline numbered format per checkpoint protocol.

**Example question batch (via AskUserQuestion):**

```
Question 1:
  question: "Layout structure for the three knobs?"
  header: "Layout"
  options:
    - label: "Horizontal row of knobs", description: "Classic single-row layout"
    - label: "Vertical stack", description: "Narrow, tall layout"
    - label: "Triangle arrangement", description: "Two bottom, one top"
    - label: "Other", description: "Custom arrangement"

Question 2:
  question: "Vintage style details?"
  header: "Aesthetic"
  options:
    - label: "Brushed metal", description: "Industrial, professional"
    - label: "Wood panel", description: "Warm, classic studio"
    - label: "Reel-to-reel theme", description: "Tape machine aesthetic"
    - label: "Other", description: "Different vintage style"

Question 3:
  question: "Visual feedback elements?"
  header: "Meters/displays"
  options:
    - label: "VU meters", description: "Needle-style level indicators"
    - label: "Tape reel animation", description: "Spinning reel visual"
    - label: "Both VU and reels", description: "Full tape machine feel"
    - label: "None (controls only)", description: "Clean, minimal"

Question 4:
  question: "Knob style?"
  header: "Controls"
  options:
    - label: "Large vintage knobs", description: "60s-70s style, prominent"
    - label: "Small precise knobs", description: "Modern, technical"
    - label: "Chicken-head style", description: "Classic pointer knobs"
    - label: "Other", description: "Different knob style"
```

**After receiving answers:**
1. Accumulate context with previous responses
2. Re-analyze gaps
3. Proceed to decision gate

**Question batch generation:**

Generate 4 questions using AskUserQuestion based on identified gaps.

**Question structure pattern:**
- question: Clear, specific question about the gap
- header: Short category label (max 12 chars)
- options: 2-4 distinct choices + "Other" (automatically added)

**See:** `references/design-questions.md#example-question-batches` for question templates and tiering examples.

**Tier priority:**
1. Critical gaps (layout, control types) - ask first
2. Visual gaps (style, key elements) - ask second
3. Polish gaps (colors, animations) - ask if needed

## Phase 3.5: Decision Gate

**Use AskUserQuestion with 3 options after each question batch:**

```
Question:
  question: "Ready to finalize the mockup design?"
  header: "Next step"
  options:
    - label: "Yes, finalize it", description: "Generate YAML and test HTML"
    - label: "Ask me 4 more questions", description: "Continue refining"
    - label: "Let me add more context first", description: "Provide additional details"

Route based on answer:
- Option 1 → Proceed to Phase 4 (generate YAML and test HTML)
- Option 2 → Return to Phase 2 (re-analyze gaps, generate next 4 questions)
- Option 3 → Collect free-form text, merge with context, return to Phase 2
```

## Phase 4: Generate Hierarchical YAML

**Create:** `plugins/[Name]/.ideas/mockups/v[N]-ui.yaml`

**Purpose:** Machine-readable design spec that guides HTML generation and C++ implementation.

**YAML structure:** See `assets/ui-yaml-template.yaml` for complete template with all control types.

**Required sections:**
- window: Dimensions, resizability
- controls: Array of control definitions (id, type, position, range)
- styling: Colors, fonts, theme tokens

**Example YAML structure:**
```yaml
window:
  width: 600
  height: 400
  resizable: false

controls:
  - id: threshold
    type: slider
    position: { x: 50, y: 100 }
    range: [-60.0, 0.0]
    default: -20.0
    label: "Threshold"

  - id: ratio
    type: slider
    position: { x: 200, y: 100 }
    range: [1.0, 20.0]
    default: 4.0
    label: "Ratio"

styling:
  colors:
    background: "#2a2a2a"
    foreground: "#ffffff"
    accent: "#ff6b35"
  fonts:
    primary: "Arial, sans-serif"
```

**See:** `assets/ui-yaml-template.yaml` for complete template with all control types.

## Phase 5: Generate Browser Test HTML

**Create:** `plugins/[Name]/.ideas/mockups/v[N]-ui-test.html`

**Purpose:** Test UI in browser for rapid design iteration.

**Features:**

- Standalone HTML file (open directly in browser)
- Mock parameter state (simulates C++ backend)
- Interactive controls (test bindings)
- Console logging (verify parameter messages)
- Same visual as production will be
- No JUCE/WebView required

**Critical WebView Constraints** (enforced in Phase 5.3):
- ❌ NO viewport units: `100vh`, `100vw`, `100dvh`, `100svh` (causes blank screens)
- ✅ REQUIRED: `html, body { height: 100%; }` for full-screen layouts
- ✅ REQUIRED: `user-select: none` for native plugin feel
- ✅ REQUIRED: Context menu disabled via JavaScript `contextmenu` event

**Why these matter**: Viewport units cause blank screens in JUCE WebView (JUCE 8 limitation). Missing user-select allows text selection (breaks plugin feel). Without context menu disable, right-click shows browser menu (not plugin-appropriate).

**See:** `references/ui-design-rules.md` for complete constraint list and validation patterns, and `references/browser-testing.md` for testing guidelines

## Phase 5.3: Validate WebView Constraints (Before Decision Menu)

<requirement type="validation" blocking="true">
<validation_checklist>
**Execute validation before Phase 5.5:**

```bash
# Store validation results
VALIDATION_PASSED=true

# Check 1: No viewport units
if grep -q "100vh\|100vw\|100dvh\|100svh" v[N]-ui-test.html; then
    echo "❌ FAIL: Forbidden viewport units found"
    VALIDATION_PASSED=false
fi

# Check 2: Required html/body height
if ! grep -q "html, body.*height: 100%" v[N]-ui-test.html; then
    echo "❌ FAIL: Missing required html/body height: 100%"
    VALIDATION_PASSED=false
fi

# Check 3: Native feel CSS
if ! grep -q "user-select: none" v[N]-ui-test.html; then
    echo "❌ FAIL: Missing user-select: none"
    VALIDATION_PASSED=false
fi

# Check 4: Context menu disabled
if ! grep -q 'contextmenu.*preventDefault' v[N]-ui-test.html; then
    echo "❌ FAIL: Context menu not disabled"
    VALIDATION_PASSED=false
fi

# Gate decision
if [ "$VALIDATION_PASSED" = false ]; then
    echo "Regenerating mockup with corrections..."
    # Return to Phase 4 with fixes
else
    echo "✅ All WebView constraints validated"
    # Proceed to Phase 5.5
fi
```

**Failure handling:**
IF any check fails THEN:
  1. Log specific violation
  2. Regenerate mockup with corrections
  3. Re-run validation
  4. Do NOT proceed to Phase 5.5 until ALL checks pass
</validation_checklist>
</requirement>

**See:** `references/ui-design-rules.md` for complete validation rules

## Phase 5.4: Auto-Open in Browser

**After validation passes, automatically open the test HTML in browser.**

```bash
open plugins/[PluginName]/.ideas/mockups/v[N]-ui-test.html
```

This allows immediate visual inspection without requiring user to manually navigate and open the file.

**Note:** Uses `open` command (macOS). On other platforms, adjust command accordingly (e.g., `xdg-open` on Linux, `start` on Windows).

## Phase 5.45: Version Control Checkpoint

**CRITICAL: Commit each UI version immediately after generation.**

**After Phase 5.4 completes (YAML + test HTML generated and validated):**

```bash
cd plugins/[PluginName]/.ideas/mockups
git add v[N]-ui.yaml v[N]-ui-test.html
git commit -m "feat([PluginName]): UI mockup v[N] (design iteration)"
```

**Why commit at this point:**
- Preserves design history between iterations
- Each version is recoverable
- Enables A/B comparison of different designs
- Atomic commits per iteration (not batched)

**Update workflow state (if in workflow context):**

```bash
if [ -f "plugins/[PluginName]/.continue-here.md" ]; then
    # Update version tracking
    sed -i '' "s/latest_mockup_version: .*/latest_mockup_version: [N]/" plugins/[PluginName]/.continue-here.md
    # Keep mockup_finalized: false until user chooses "finalize"
    git add plugins/[PluginName]/.continue-here.md
    git commit --amend --no-edit
fi
```

**State tracking in `.continue-here.md`:**

```markdown
current_stage: 0
stage_0_status: ui_design_in_progress
latest_mockup_version: 2
mockup_finalized: false
```

**Only proceed to Phase 5.5 after successful commit.**

---

<phase_gate_enforcement id="design-approval-gate" enforcement_level="STRICT">
**Purpose:** Prevent generating implementation scaffolding (Phase B: files 3-7) before design is finalized (Phase A: files 1-2).

**Why critical:** C++ boilerplate generation is expensive. If design changes after Phase B runs, all 5 implementation files must be regenerated. The two-phase approach saves time by deferring scaffolding until design is locked.

**Gate Trigger:** After Phase A completes (v[N]-ui.yaml + v[N]-ui-test.html generated and committed)

## Phase A Completion Detection

Before presenting decision menu, verify Phase A artifacts exist:

```bash
# Check for latest design iteration files
MOCKUP_DIR="plugins/${PLUGIN_NAME}/.ideas/mockups"
LATEST_VERSION=$(ls -1 "$MOCKUP_DIR"/v*-ui.yaml 2>/dev/null | \
                 sed 's/.*v\([0-9]*\)-.*/\1/' | sort -n | tail -1)

if [ -n "$LATEST_VERSION" ]; then
  YAML_FILE="$MOCKUP_DIR/v${LATEST_VERSION}-ui.yaml"
  TEST_HTML="$MOCKUP_DIR/v${LATEST_VERSION}-ui-test.html"

  # Phase A complete if both files exist
  if [ -f "$YAML_FILE" ] && [ -f "$TEST_HTML" ]; then
    PHASE_A_COMPLETE=true
  else
    echo "✗ ERROR: Phase A incomplete (missing YAML or test HTML)"
    exit 1
  fi
else
  echo "✗ ERROR: No mockup versions found"
  exit 1
fi
```

## Decision Menu (Required - Phase 5.5)

When Phase A completes, MUST present decision menu and WAIT for user choice.

**Menu format and option routing:** See `references/decision-menus.md#phase-5-5-design-decision-menu`

**Gate enforcement:** Phases 6-10 are CONDITIONALLY EXECUTED based on user choice. Only option 2 (Finalize) proceeds to Phase B.

## State Tracking (Finalization Marker)

When user selects option 2 (Finalize), mark design as finalized in YAML:

```bash
# Append finalization metadata to YAML
cat >> "$MOCKUP_DIR/v${LATEST_VERSION}-ui.yaml" << EOF

# Finalization metadata
finalized: true
finalized_at: $(date -u +%Y-%m-%dT%H:%M:%SZ)
finalized_by_phase: 5.5
EOF
```

**Purpose:**
- Prevents accidental regeneration of finalized designs
- Tracks which version was approved for implementation
- Enables version history queries (which designs were finalized vs exploratory)

## Phase B Guard Protocol

Before generating any Phase B file (Phases 6-10), verify design finalization.

**See:** `references/phase-b-enforcement.md` for complete guard implementation.

## Anti-Patterns to Avoid

**Critical rule: NEVER generate Phase B files (3-7) without Phase 5.5 menu approval.**

**See:** `references/common-pitfalls.md` for detailed anti-patterns and why they matter.

**Quick checklist:**
- ✓ Present Phase 5.5 menu after Phase A
- ✓ Wait for option 2 (Finalize) before Phase B
- ✓ Check finalization marker in YAML
- ✓ Validate WebView constraints (Phase 5.3)
- ✓ Read creative-brief.md if it exists

</phase_gate_enforcement>

---

<decision_gate id="phase_5_5_approval" blocking="true">
## Phase 5.5: Design Decision Menu

**Gate enforcement:** Phases 6-10 are CONDITIONALLY EXECUTED based on user choice.

<menu_presentation>
Present this decision menu:

```
✓ Mockup v[N] design created (2 files)

Files generated:
- v[N]-ui.yaml (design specification)
- v[N]-ui-test.html (browser-testable mockup)

What would you like to do?

1. Iterate - Refine design, adjust layout
2. Finalize - Validate alignment and complete mockup
3. Save as template - Add to aesthetic library for reuse
4. Other

Choose (1-4): _
```
</menu_presentation>

<conditional_execution requires="user_choice">
**Execution routing:**
- IF user chose option 2 (finalize) THEN proceed to Phase 5.6 (automatic validation)
- IF user chose option 1 (iterate) THEN return to Phase 2 with v[N+1]
- IF user chose option 3 (save template) THEN invoke ui-template-library skill
- ELSE handle custom options

**DO NOT proceed to Phase 6 unless user explicitly chose finalization option.**

**Option handling:**

- **Option 1: Iterate** → User gives feedback → Return to Phase 2 with new version number (v2, v3, etc.)

- **Option 2: Finalize** → Proceed to Phase 5.6 (automatic validation gate)
  - MANDATORY: Automatically invoke design-sync skill to validate mockup ↔ creative brief consistency
  - Detects drift (parameter mismatches, missing features, style divergence)
  - User resolves any issues within design-sync
  - **design-sync routes back to Phase 5.5 decision menu if issues found**
  - Only proceed to Phase 6-10 if validation passes or user overrides
  - See Phase 5.6 for validation protocol

- **Option 3: Save as template** → Invoke ui-template-library skill with "save" operation
  ```
  Invoke Skill tool:
  - skill: "ui-template-library"
  - prompt: "Save aesthetic from plugins/[PluginName]/.ideas/mockups/v[N]-ui-test.html"
  ```
  After saving, return to Phase 5.5 decision menu

- **Option 4: Other** → Collect free-form text, handle custom request (test in browser, validate WebView constraints, etc.)
</conditional_execution>
</decision_gate>

---

<validation_gate id="phase_5_6_automatic_validation" blocking="true" requires_gate="phase_5_5_approval">
## Phase 5.6: Automatic Design Validation (Finalize Gate)

**Purpose:** Mandatory validation before generating implementation files. Prevents drift and ensures alignment.

**Trigger:** User selected "Finalize" option in Phase 5.5

**Protocol:**

1. **Invoke design-sync skill automatically:**
   ```
   Invoke Skill tool:
   - skill: "design-sync"
   - context: "Automatic validation before finalization"
   ```

2. **Handle validation results:**

   **If validation passes (no drift):**
   - Proceed directly to Phase 6-10 (generate implementation files)

   **If drift detected:**
   - design-sync skill presents drift findings and resolution options
   - User chooses resolution (update mockup, update brief, override, review)
   - **If user updates mockup:** Return to Phase 5.5 with updated design
   - **If user updates brief:** Continue to Phase 6-10 with updated contracts
   - **If user overrides:** Log override decision, continue to Phase 6-10
   - **If user reviews:** Present findings, then return to Phase 5.5

3. **Skip validation only if:**
   - No creative-brief.md exists (standalone mockup mode)
   - In that case, proceed directly to Phase 6-10

**Why mandatory:** Catches misalignment before generating 5 implementation files. Prevents wasted work and ensures contracts match design.

**Fallback:** If design-sync skill unavailable, warn user and proceed with manual confirmation.
</validation_gate>

---

<conditional_execution requires_gate="phase_5_5_approval">
<critical_sequence phases="6,7,8,9,10" enforcement="sequential">

## Phase B: Implementation Scaffolding (Phases 6-10)

**Prerequisites for ALL Phase B phases:**
- User confirmed design in Phase 5.5 decision menu (selected option 2: Finalize)
- Execute Phase B guard from `references/phase-b-enforcement.md` before each phase
- Phase A files (v[N]-ui.yaml and v[N]-ui-test.html) exist and validated

These prerequisites apply to Phases 6, 7, 8, 9, and 10. Verify guard before proceeding to any Phase B phase.

---

## Phase 6: Generate Production HTML (After Finalization Only)

**Create:** `plugins/[Name]/.ideas/mockups/v[N]-ui.html`

**This HTML IS the plugin UI.** It will be copied to `Source/ui/public/index.html` during Stage 5 (GUI).

### Generation Strategy

**Base template:** `assets/webview-templates/index-template.html`

**Key replacements:**

1. **{{PLUGIN_NAME}}** → Plugin name from creative brief
2. **{{CONTROL_HTML}}** → Generate controls from finalized YAML/HTML
3. **{{PARAMETER_BINDINGS}}** → Generate JavaScript bindings for each parameter

### Parameter ID Extraction

Parse finalized HTML from Phase 5 for JUCE parameter bindings:

```javascript
// Extract parameter IDs from JavaScript code patterns
const parameterIds = [];

// Pattern 1: Juce.getSliderState("PARAM_ID")
const sliderMatches = html.matchAll(/Juce\.getSliderState\("([^"]+)"\)/g);
for (const match of sliderMatches) {
    parameterIds.push({ id: match[1], type: "slider" });
}

// Pattern 2: Juce.getToggleButtonState("PARAM_ID")
const toggleMatches = html.matchAll(/Juce\.getToggleButtonState\("([^"]+)"\)/g);
for (const match of toggleMatches) {
    parameterIds.push({ id: match[1], type: "toggle" });
}

// Pattern 3: Juce.getComboBoxState("PARAM_ID")
const comboMatches = html.matchAll(/Juce\.getComboBoxState\("([^"]+)"\)/g);
for (const match of comboMatches) {
    parameterIds.push({ id: match[1], type: "combo" });
}
```

**Use extracted IDs to generate matching relay/attachment code in C++.**

### Critical Constraints

**Enforce from `references/ui-design-rules.md`:**

- ❌ NO viewport units: `100vh`, `100vw`, `100dvh`, `100svh`
- ✅ REQUIRED: `html, body { height: 100%; }`
- ✅ REQUIRED: `user-select: none` (native feel)
- ✅ REQUIRED: Context menu disabled in JavaScript

**See:** `references/ui-design-rules.md` for complete constraints

## Phase 7: Generate C++ Boilerplate (After Finalization Only)

**Create:**
- `plugins/[Name]/.ideas/mockups/v[N]-PluginEditor-TEMPLATE.h`
- `plugins/[Name]/.ideas/mockups/v[N]-PluginEditor-TEMPLATE.cpp`

**Note**: These files are REFERENCE TEMPLATES for gui-agent, not copy-paste files. gui-agent uses these as starting point during Stage 5.

**Purpose:** WebView integration boilerplate for Stage 5 (GUI).

**Generation strategy:** Use template replacement from `references/cpp-boilerplate-generation.md`.

**Required inputs:**
- Plugin name (PascalCase and UPPERCASE)
- Current version number [N]
- Window dimensions from YAML
- Parameter IDs from Phase 6 extraction

**Key templates:**
- Header: `assets/webview-templates/PluginEditor-webview.h`
- Implementation: `assets/webview-templates/PluginEditor-webview.cpp`

**Critical patterns:**
- Generate relay declarations for each parameter (slider/toggle/combo)
- Generate matching attachment declarations
- **⚠️ CRITICAL:** Enforce member order (relays → webView → attachments) to prevent release build crashes
- Generate relay creation, WebView options, and attachment creation code
- Generate resource provider mappings for all UI files
- Extract window size from YAML

**See:** `references/cpp-boilerplate-generation.md` for complete template details and variable mapping

## Phase 8: Generate Build Configuration (After Finalization Only)

**Create:** `plugins/[Name]/.ideas/mockups/v[N]-CMakeLists-SNIPPET.txt`

**Note**: This snippet is appended to CMakeLists.txt by gui-agent, not used standalone.

**Purpose:** CMake snippet to enable WebView support in JUCE.

**IMPORTANT:** This is a SNIPPET to append to existing plugin CMakeLists.txt, NOT a complete CMakeLists.txt file.

**Generation strategy:** Use template from `references/cmake-generation.md`.

**Required inputs:**
- Plugin name (PascalCase)
- Current version number [N]

**Base template:** `assets/webview-templates/CMakeLists-webview-snippet.cmake`

**See:** `references/cmake-generation.md` for complete template structure and integration instructions

## Phase 9: Generate Integration Checklist (After Finalization Only)

**Create:** `plugins/[Name]/.ideas/mockups/v[N]-integration-checklist.md`

**Purpose:** Step-by-step guide to integrate UI into plugin during Stage 5.

### Checklist Structure

**Base template:** `assets/integration-checklist-template.md`

**WebView-specific steps:**

```markdown
## Stage 5 (GUI) Integration Steps

### 1. Copy UI Files
- [ ] Copy v[N]-ui.html to Source/ui/public/index.html
- [ ] Copy JUCE frontend library to Source/ui/public/js/juce/index.js
- [ ] Copy any additional resources (CSS, images, etc.)

### 2. Update PluginEditor Files
- [ ] Replace PluginEditor.h with v[N]-PluginEditor.h
- [ ] Verify member order: relays → webView → attachments
- [ ] Replace PluginEditor.cpp with v[N]-PluginEditor.cpp
- [ ] Verify initialization order matches member order

### 3. Update CMakeLists.txt
- [ ] Add juce_add_binary_data for UI resources
- [ ] Link ${PLUGIN_NAME}_UIResources to plugin
- [ ] Add JUCE_WEB_BROWSER=1 definition
- [ ] Add platform-specific config (if cross-platform)

### 4. Build and Test (Debug)
- [ ] Build succeeds without warnings
- [ ] Standalone loads WebView (not blank)
- [ ] Right-click → Inspect works
- [ ] Console shows no JavaScript errors
- [ ] window.__JUCE__ object exists
- [ ] Parameter state objects accessible

### 5. Build and Test (Release)
- [ ] Release build succeeds without warnings
- [ ] Release build runs (tests member order logic)
- [ ] No crashes on plugin reload (test 10 times)

### 6. Test Parameter Binding
- [ ] Moving UI slider changes audio (verify in DAW)
- [ ] Changing parameter in DAW updates UI
- [ ] Parameter values persist after reload
- [ ] Multiple parameters sync independently

### 7. WebView-Specific Validation
- [ ] Verify member order in PluginEditor.h (relays → webView → attachments)
- [ ] Test resource provider returns all files (no 404 in console)
- [ ] Verify parameter binding (automation/preset recall)
- [ ] Test in Debug and Release builds
- [ ] Check for crashes on plugin close (reload 10 times)
- [ ] CSS does NOT use viewport units (100vh, 100vw)
- [ ] Native feel CSS present (user-select: none)
```

**See:** `assets/integration-checklist-template.md` for full template

## Phase 10: Finalize parameter-spec.md (After Finalization Only)

**Prerequisites:**
- This is the first mockup version (v1 only)

**Version check:**
```bash
# Only generate parameter-spec.md for v1
if [ "$LATEST_VERSION" != "1" ]; then
  echo "ℹ Skipping parameter-spec.md (already exists from v1)"
  exit 0
fi
```

**If this is the first UI mockup (v1):**

<draft_validation_gate>
**CRITICAL: Check for existing parameter-spec-draft.md before generating full spec.**

```bash
DRAFT_PATH="plugins/${PLUGIN_NAME}/.ideas/parameter-spec-draft.md"

if [ -f "$DRAFT_PATH" ]; then
  echo "ℹ Draft parameters found - validating consistency..."
  # Proceed to consistency validation
else
  echo "ℹ No draft found - generating full spec from mockup"
  # Skip to generation
fi
```

**If draft exists, validate consistency:**

1. Parse draft parameters (IDs, types, ranges)
2. Parse mockup parameters (from YAML controls)
3. Compare parameter lists:

```bash
# Pseudocode for consistency check
DRAFT_PARAMS=$(parse_draft_parameters "$DRAFT_PATH")
MOCKUP_PARAMS=$(parse_mockup_parameters "v1-ui.yaml")

MISSING_FROM_MOCKUP=()
EXTRA_IN_MOCKUP=()

for param in $DRAFT_PARAMS; do
  if ! contains "$MOCKUP_PARAMS" "$param"; then
    MISSING_FROM_MOCKUP+=("$param")
  fi
done

for param in $MOCKUP_PARAMS; do
  if ! contains "$DRAFT_PARAMS" "$param"; then
    EXTRA_IN_MOCKUP+=("$param")
  fi
done

if [ ${#MISSING_FROM_MOCKUP[@]} -gt 0 ] || [ ${#EXTRA_IN_MOCKUP[@]} -gt 0 ]; then
  # Mismatch detected - present resolution menu
  CONFLICT_DETECTED=true
else
  # No mismatch - proceed to generation
  CONFLICT_DETECTED=false
fi
```

**If mismatch detected, present resolution menu:**

```
⚠️ Parameter mismatch between draft and mockup

Draft specified but missing from mockup:
- filterCutoff (Float, 20-20000 Hz)
- resonance (Float, 0-1)

Mockup includes but not in draft:
- outputGain (Float, -60 to 12 dB)

Resolution options:

1. Update mockup - Add missing parameters to UI design (return to Phase 4)
2. Update draft - Remove obsolete parameters from draft (regenerate draft)
3. Merge both - Include all parameters from both sources
4. Manual resolution - I'll fix this myself (pause workflow)
5. Other

Choose (1-5): _
```

**Handle resolution choices:**

- **Option 1:** Return to Phase 4 with instructions to add missing parameters
- **Option 2:** Regenerate draft with only parameters present in mockup, then continue
- **Option 3:** Generate full spec with union of both parameter sets (warn about possible duplication)
- **Option 4:** Pause workflow, wait for manual fix
- **Option 5:** Collect custom input

**After resolution (or if no conflict), proceed to generation:**
</draft_validation_gate>

**Create:** `plugins/[Name]/.ideas/parameter-spec.md`

**Purpose:** Lock parameter specification for implementation. This becomes the **immutable contract** for all subsequent stages.

**Extract from YAML (and merge with draft if exists):**

```markdown
## Total Parameter Count

**Total:** 5 parameters

## Parameter Definitions

### threshold
- **Type:** Float
- **Range:** -60.0 to 0.0 dB
- **Default:** -20.0
- **Skew Factor:** linear
- **UI Control:** Rotary knob, center-top position
- **DSP Usage:** Compressor threshold level
```

**See:** `assets/parameter-spec-template.md`

### Parameter ID Naming Convention

Parameter IDs generated in YAML must follow these rules:
- **Lowercase only**: `threshold` not `Threshold`
- **snake_case for multi-word**: `attack_time` not `attackTime` or `AttackTime`
- **Alphanumeric + underscore**: No spaces, hyphens, or special chars
- **Max 32 characters**: Keep concise for readability
- **Valid C++ identifier**: Must compile in PluginProcessor.cpp

**Examples**:
- ✓ `threshold`, `ratio`, `attack_time`, `release_ms`
- ✗ `Threshold`, `attack-time`, `release (ms)`, `really_long_parameter_name_over_32_chars`

<state_requirement>
<commit_protocol phase="finalization">
## Phase 10.5: Finalization Commit

**MUST commit all implementation files and update workflow state.**

**Files to commit:**
- All 7 mockup files (v[N]-*.{html,yaml,h,cpp,txt,md})
- parameter-spec.md (if v1 only)
- .continue-here.md (workflow state)

**After Phase 10 completes (all 7 files generated):**

```bash
cd plugins/[PluginName]/.ideas/mockups
git add v[N]-ui.html v[N]-PluginEditor.h v[N]-PluginEditor.cpp v[N]-CMakeLists.txt v[N]-integration-checklist.md

# If parameter-spec.md was created (v1 only)
if [ -f "../parameter-spec.md" ]; then
    git add ../parameter-spec.md
fi

git commit -m "feat([PluginName]): UI mockup v[N] finalized (implementation files)"
```

**State updates required:**
```bash
# Update .continue-here.md
sed -i '' "s/mockup_finalized: .*/mockup_finalized: true/" .continue-here.md
sed -i '' "s/finalized_version: .*/finalized_version: [N]/" .continue-here.md
sed -i '' "s/stage_0_status: .*/stage_0_status: ui_design_complete/" .continue-here.md
```

**Verification:**
- [ ] Git commit succeeded
- [ ] .continue-here.md updated
- [ ] mockup_finalized: true
- [ ] stage_0_status: ui_design_complete

ONLY proceed to "After Completing All Phases" menu if verification passes.

**Updated state in `.continue-here.md`:**

```markdown
current_stage: 0
stage_0_status: ui_design_complete
latest_mockup_version: 2
mockup_finalized: true
finalized_version: 2
```

**Why this matters:**
- Marks design phase as complete
- Enables `/continue` to resume at Stage 1
- Records which version was finalized (if multiple exist)
- Atomic commit of all implementation files
</commit_protocol>
</state_requirement>

</critical_sequence>
</conditional_execution>

## After Completing All Phases

Present completion menu after all 7 files generated.

**Menu format and option routing:** See `references/decision-menus.md#completion-menu`

## Versioning Strategy

**Pattern:** v1, v2, v3... Each UI version is saved separately.

**Purpose:**
- Explore different layouts without losing previous work
- A/B test designs before committing
- Keep design history for rollback

**File naming:** All 7 files prefixed with version (e.g., `v2-ui.html`, `v2-PluginEditor.h`)

**Implementation:** Latest version used for Stage 5 unless user specifies different version.

**See:** `references/versioning.md` for file management details.

## Success Criteria

**Design phase successful when:**
- ✅ YAML spec generated matching user requirements
- ✅ Browser test HTML works (interactive controls, parameter messages)
- ✅ Design files committed to git (Phase 5.45)
- ✅ `.continue-here.md` updated with version number (if in workflow)
- ✅ User presented with Phase 5.5 decision menu
- ✅ Design approved OR user iterates with refinements

**Implementation phase successful when (after finalization):**
- ✅ All 7 files generated and saved to `.ideas/mockups/`
- ✅ Production HTML is complete (no placeholder content)
- ✅ C++ boilerplate matches YAML structure (correct parameter bindings)
- ✅ parameter-spec.md generated and locked (for v1 only)
- ✅ Implementation files committed to git (Phase 10.5)
- ✅ `.continue-here.md` updated with finalization status (if in workflow)

## Integration Points

**Invoked by:**

- `/dream` command → After creative brief, before parameter finalization
- `plugin-workflow` skill → During Stage 0 (UI design phase)
- `plugin-improve` skill → When redesigning existing plugin UI
- Natural language: "Design UI for [PluginName]", "Create mockup for compressor"

**Invokes:**

- (None - terminal skill that generates files only)

**Creates:**

- `plugins/[Name]/.ideas/mockups/v[N]-*.{yaml,html,h,cpp,txt,md}` (7 files)
- `plugins/[Name]/.ideas/parameter-spec.md` (if v1 and doesn't exist)

**Updates:**

- `PLUGINS.md` → Mark UI designed (if part of workflow)
- `.continue-here.md` → Update workflow state (if part of workflow)

**Blocks:**

- Stage 1 (Planning) → Cannot proceed without parameter-spec.md
- Stage 5 (GUI) → Cannot implement without approved UI mockup

## Reference Documentation

- **Context extraction:** `references/context-extraction.md` - Guidelines for loading plugin context
- **Design questions:** `references/design-questions.md` - Targeted question templates and defaults
- **HTML generation:** `references/html-generation.md` - Rules for production HTML generation
- **Browser testing:** `references/browser-testing.md` - Browser test workflow
- **CMake configuration:** `references/cmake-configuration.md` - WebView build settings

## Template Assets

- **UI YAML template:** `assets/ui-yaml-template.yaml` - Complete YAML structure
- **WebView boilerplate:** `assets/webview-boilerplate.md` - C++ integration templates
- **Integration checklist:** `assets/integration-checklist-template.md` - Step-by-step integration guide
- **Parameter spec template:** `assets/parameter-spec-template.md` - Parameter specification format
