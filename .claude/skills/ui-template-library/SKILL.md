---
name: ui-template-library
description: Manage aesthetic templates - save visual systems from completed mockups, apply to new plugins with adaptive layouts
allowed-tools:
  - Read
  - Write
  - Bash
preconditions:
  - .claude/aesthetics/ directory exists
---

# ui-template-library Skill

**Purpose:** Capture and apply aesthetic "vibes" across plugins using structured prose descriptions rather than rigid specifications.

## Core Philosophy: Conceptual Aesthetics

**Aesthetic = Transferable design language captured as interpretable prose**

This system captures the **conceptual essence** of a design - the "vibe," color philosophy, control styling approach, spacing feel - in structured markdown that Claude can reliably interpret and apply to plugins with different parameter counts.

**Why prose instead of specs:**
- Aesthetics are conceptual, not rigid pixel-perfect templates
- Descriptions adapt naturally to different layouts and parameter counts
- Human-editable and machine-interpretable without browser APIs
- Works entirely with Read/Write/Bash tools (no DOM parsing required)

**Example:**
- **Save aesthetic** from 3-knob TapeAge: "Warm vintage aesthetic with brass rotary knobs, subtle paper texture, and generous spacing"
- **Apply aesthetic** to 8-parameter reverb: Same brass knobs, same paper texture, same spacing philosophy, but in a 3x3 grid layout appropriate for 8 parameters

The aesthetic system separates **visual design language** (colors, control styling, spacing philosophy) from **layout structure** (arrangement, grid, parameter count).

---

## Operations

**You MUST determine which operation the user is requesting, then execute ONLY that operation.**

1. **Save aesthetic** - Analyze mockup HTML and generate structured prose aesthetic.md
2. **Apply aesthetic** - Interpret aesthetic.md to generate new mockup with appropriate layout
3. **List aesthetics** - Show all saved aesthetics with prose summaries
4. **Delete aesthetic** - Remove aesthetic from library
5. **Update aesthetic** - Refine aesthetic.md from improved mockup

---

## Directory Structure

```
.claude/aesthetics/
├── manifest.json                    # Registry of all aesthetics
└── [aesthetic-id]/
    ├── aesthetic.md                 # Structured prose description (THE SOURCE OF TRUTH)
    ├── preview.html                 # Visual reference (original mockup)
    └── metadata.json                # Name, description, source plugin, tags
```

**Key change from previous system:**
- **aesthetic.yaml → aesthetic.md**: YAML specs replaced with structured prose descriptions
- Format is consistent and parseable, but content is interpretable concepts, not rigid values

---

<operation name="save_aesthetic">
  <purpose>Capture visual design from mockup as structured prose aesthetic.md</purpose>

  <critical_sequence>
    <step id="1" required="true">
      Read mockup HTML file into memory (Read tool)
    </step>
    <step id="2" required="true" depends_on="1">
      Extract visual patterns using pattern-extraction.md strategies
    </step>
    <step id="3" required="true" depends_on="2">
      Generate prose descriptions using prose-generation.md guidelines
    </step>
    <step id="4" required="true" depends_on="3">
      Write aesthetic.md following aesthetic-template.md structure exactly
    </step>
    <step id="5" required="true" depends_on="4">
      Copy preview.html to aesthetic directory
    </step>
    <step id="6" required="true" depends_on="5">
      Generate metadata.json with inferred tags
    </step>
    <step id="7" required="true" depends_on="6">
      Update manifest.json using Read → modify → Write pattern
    </step>
    <step id="7.5" required="true" depends_on="7">
      Commit aesthetic to git using conventional format
    </step>
    <step id="8" required="true" depends_on="7.5">
      Present confirmation + decision menu
    </step>
  </critical_sequence>

  <state_requirement>
    <before_completion>
      ALL steps 1-7.5 must complete successfully
      IF any step fails: halt, report error, DO NOT proceed to step 8
    </before_completion>

    <verification>
      Before step 8:
      - VERIFY aesthetic.md exists and is not empty
      - VERIFY preview.html exists
      - VERIFY metadata.json is valid JSON
      - VERIFY manifest.json updated successfully
      - VERIFY git commit completed successfully
    </verification>
  </state_requirement>


## Operation 1: Save Aesthetic

**High-level summary:**
Read mockup HTML, extract visual patterns, generate prose descriptions, write aesthetic.md following template structure, copy preview HTML, generate metadata, update manifest, present decision menu.

**See:** `references/save-operation.md` for complete 8-step workflow.
---

<operation name="apply_aesthetic">
  <purpose>Generate new mockup by interpreting aesthetic prose and adapting to target plugin</purpose>

  <critical_sequence>
    <step id="1" required="true">
      Load aesthetic.md and metadata.json (Read tool)
    </step>
    <step id="2" required="true">
      Read target plugin parameter-spec.md or creative-brief.md
    </step>
    <step id="3" required="true" depends_on="1,2">
      Interpret aesthetic prose using aesthetic-interpretation.md strategies
    </step>
    <step id="4" required="true" depends_on="3">
      Choose layout based on parameter count (layout-generation.md)
    </step>
    <step id="5" required="true" depends_on="3,4">
      Generate HTML/CSS applying aesthetic to chosen layout
    </step>
    <step id="6" required="true" depends_on="5">
      Save generated mockup to target plugin mockups directory
    </step>
    <step id="7" required="true" depends_on="6">
      Update aesthetic metadata.json usedInPlugins array
    </step>
    <step id="8" required="true" depends_on="7">
      Present decision menu (inline numbered, NOT AskUserQuestion)
    </step>
  </critical_sequence>

  <decision_gate wait_required="true">
    Present decision menu with preview/continue options
    MUST wait for user response
  </decision_gate>


## Operation 2: Apply Aesthetic

**High-level summary:**
Load aesthetic prose and plugin requirements, interpret aesthetic into specific CSS values, choose layout based on parameter count, generate HTML applying aesthetic to layout, save to plugin mockups directory, update aesthetic metadata, present decision menu.

**See:** `references/apply-operation.md` for complete 8-step workflow.
---

<operation name="list_aesthetics">
  <critical_sequence>
    <step id="1" required="true">Read manifest.json</step>
    <step id="2" required="true">Format as markdown table</step>
    <step id="3" required="true">Show preview paths</step>
    <step id="4" required="true">Present decision menu</step>
  </critical_sequence>

  <decision_gate wait_required="true">
    Wait for user to select action from menu
  </decision_gate>


## Operation 3: List Aesthetics

**Input:** None (or optional filter by tag)
**Output:** Table of all aesthetics with prose summaries

### Step 1: Read Manifest

```bash
MANIFEST=".claude/aesthetics/manifest.json"
test -f "$MANIFEST" || { echo "No aesthetics saved"; exit 0; }
```

**Use Read tool to load manifest.json.**

### Step 2: Display Table

**Format as markdown table:**

```markdown
# Aesthetic Library

| ID | Name | Vibe | Source Plugin | Created | Used In |
|----|------|------|---------------|---------|---------|
| vintage-hardware-001 | Vintage Hardware | Warm retro analog | TapeAge | 2025-11-10 | TapeAge, CompCo |
| modern-minimal-002 | Modern Minimal | Clean flat blue/gray | EQPro | 2025-11-11 | EQPro |
```

**For each aesthetic:**
- ID
- Name
- Vibe (extract from aesthetic.md "Visual Identity" or metadata description)
- Source plugin
- Created date
- Used in plugins (comma-separated)

### Step 3: Show Preview Paths

```
Preview files:
- vintage-hardware-001: .claude/aesthetics/vintage-hardware-001/preview.html
- modern-minimal-002: .claude/aesthetics/modern-minimal-002/preview.html

Open in browser:
  open .claude/aesthetics/vintage-hardware-001/preview.html
```

### Step 4: Decision Menu

```
What would you like to do?
1. View aesthetic details - Show full aesthetic.md prose
2. Apply aesthetic to plugin - Choose aesthetic and target
3. Delete aesthetic - Remove from library
4. Exit
```

---

## Operation 4: Delete Aesthetic

**Input:** Aesthetic ID
**Output:** Aesthetic removed from library

### Step 1: Confirm Deletion

**Show usage information:**
```
⚠️  Delete aesthetic "Vintage Hardware" (vintage-hardware-001)?

Currently used in 2 plugins:
- TapeAge
- CompCo

Deleting will NOT affect existing plugins (they have copies of the UI).
This only removes the template from the library.

Are you sure? (yes/no)
```

### Step 2: Remove Files

```bash
if confirmed; then
    rm -rf ".claude/aesthetics/$AESTHETIC_ID"
fi
```

Removes:
- aesthetic.md
- preview.html
- metadata.json
- Directory itself

### Step 3: Update Manifest

**Remove entry from manifest.json:**
```bash
# Read manifest.json
# Filter out deleted aesthetic from "aesthetics" array
# Write updated manifest.json
```

### Step 4: Confirmation

```
✅ Aesthetic "Vintage Hardware" deleted from library.

Plugins using this aesthetic are unaffected (they have independent copies).

Remaining aesthetics: 1
```

---

## Operation 5: Update Aesthetic

**Input:**
- Aesthetic ID
- Path to improved mockup HTML

**Output:** Updated aesthetic.md with refined prose

**Process:**
1. Run same analysis as Save workflow (Step 2-3)
2. Generate new prose descriptions
3. Merge with existing aesthetic.md (preserve user edits where possible)
4. Update metadata.json (increment version, update modified date)
5. Optionally replace preview.html

**Strategy for preserving user edits:**
- Parse existing aesthetic.md section by section
- Regenerate each section from new mockup
- Compare old vs new prose
- If user has manually edited a section (detected by non-template language), prompt:
  ```
  Section "Knob Style" has manual edits. How to handle?
  1. Keep existing (preserve manual edits)
  2. Replace with regenerated (lose edits)
  3. Show diff and let me merge
  ```

---

## Integration with ui-mockup Skill

<handoff_protocol>
  <from_skill>ui-mockup</from_skill>
  <to_skill>ui-template-library</to_skill>

  <trigger_conditions>
    <condition when="Phase 0" if="aesthetics exist in library">
      ui-mockup presents option "Start from aesthetic template"
      User selects option → invoke ui-template-library "apply" operation
    </condition>
    <condition when="Phase 5.5 decision menu">
      ui-mockup presents option "Save as aesthetic template"
      User selects option → invoke ui-template-library "save" operation
    </condition>
  </trigger_conditions>

  <data_contract>
    <input name="mockup_path" type="absolute_path" required="true">
      Path to finalized mockup HTML file
    </input>
    <input name="plugin_name" type="string" required="true">
      Name of plugin for aesthetic source tracking
    </input>
    <input name="aesthetic_name" type="string" required="false">
      Optional custom name for aesthetic (defaults to plugin name)
    </input>
  </data_contract>

  <return_values>
    <output name="aesthetic_id" type="string">
      Generated aesthetic ID (e.g., "vintage-hardware-001")
    </output>
    <output name="aesthetic_path" type="absolute_path">
      Path to aesthetic directory
    </output>
  </return_values>
</handoff_protocol>

<delegation_rule>
  <from>ui-mockup skill</from>
  <to>ui-template-library skill</to>
  <method>inline_invocation</method>
  <rationale>
    Both skills are lightweight and stateless.
    No need for separate agent context.
    ui-mockup simply expands to include ui-template-library operations.
  </rationale>
</delegation_rule>
## Success Criteria

**Save operation successful when:**
- ✅ aesthetic.md generated following exact template structure
- ✅ All sections filled with interpretable prose (no placeholders)
- ✅ Example color codes provided as concrete reference
- ✅ preview.html copied to aesthetic directory
- ✅ metadata.json created with tags and source info
- ✅ manifest.json updated with new aesthetic entry
- ✅ Format is idempotent (same mockup → same structure every time)

**Apply operation successful when:**
- ✅ aesthetic.md parsed and interpreted correctly
- ✅ Appropriate layout chosen for parameter count
- ✅ Generated mockup reflects aesthetic visual language
- ✅ Control styling matches prose descriptions
- ✅ Colors, typography, spacing consistent with aesthetic
- ✅ WebView constraints enforced (no viewport units, etc.)
- ✅ Result is recognizably the same aesthetic despite different layout

**List operation successful when:**
- ✅ All aesthetics shown in readable table
- ✅ Vibe summaries extracted from prose
- ✅ Preview paths provided for visual reference
- ✅ Decision menu presented for next action

---

## Implementation Notes

### Why This Approach Works

**1. No browser APIs required:**
- Pattern matching with grep/sed extracts colors, fonts, values
- String analysis detects control types and styles
- Prose generation transforms patterns into descriptions
- Only requires Read/Write/Bash tools

**2. Aesthetics are conceptual:**
- "Brass knobs with tick marks" is more transferable than "knob diameter: 70px, border: 2px solid #c8a882"
- Descriptions capture intent and feel, not rigid specifications
- Claude can interpret prose and make appropriate decisions for different contexts

**3. Format is idempotent:**
- Template structure is always the same (same sections, same order)
- Parsing is predictable and reliable
- Sections are clearly delimited with markdown headers
- Human-editable while remaining machine-parseable

**4. Flexible adaptation:**
- Apply workflow interprets prose to make context-appropriate decisions
- Same aesthetic works for 3-parameter or 12-parameter plugins
- Layout adapts while visual language remains consistent

### Constraints Satisfied

- ✅ NO browser-based operations (getComputedStyle, DOM parsing)
- ✅ NO complex JavaScript execution requirements
- ✅ ONLY uses Read, Write, Bash tools
- ✅ Produces consistent, parseable aesthetic.md format
- ✅ Maintains compatibility with ui-mockup skill invocation patterns

### Parsing Strategy

**Extracting from aesthetic.md:**

1. **Section detection:**
   ```bash
   # Extract "Knob Style" section
   sed -n '/^### Knob Style$/,/^### /p' aesthetic.md | head -n -1
   ```

2. **Value extraction:**
   ```bash
   # Extract color codes from "Example Color Codes" section
   sed -n '/^## Example Color Codes$/,/^## /p' aesthetic.md | \
       grep -oE '#[0-9a-fA-F]{6}'
   ```

3. **Prose interpretation:**
   - Read section text into variable
   - Use Claude's reasoning to interpret descriptions
   - Extract key phrases: "generous spacing" → gap: 32px
   - Map descriptive terms: "warm palette" → orange/brown hues

### Quality Control

**Before saving aesthetic:**
- ✅ Verify all template sections are present
- ✅ Check for placeholder text not replaced
- ✅ Validate color codes are valid hex/rgb
- ✅ Ensure prose is descriptive (not just values)

**Before applying aesthetic:**
- ✅ Verify aesthetic.md exists and is readable
- ✅ Check required sections are present
- ✅ Validate parameter spec is available
- ✅ Ensure generated HTML follows WebView constraints

---

## Reference Documentation

- **Pattern extraction:** `references/pattern-extraction.md` - String/regex strategies for extracting visual patterns from HTML
- **Prose generation:** `references/prose-generation.md` - Guidelines for transforming patterns into interpretable prose
- **Aesthetic interpretation:** `references/aesthetic-interpretation.md` - Strategies for parsing prose and generating CSS/HTML
- **Layout generation:** `references/layout-generation.md` - Layout decision trees and control placement strategies

---

## Template Assets

- **Aesthetic template:** `assets/aesthetic-template.md` - Complete structured prose template (THE FORMAT SPEC)
- **Metadata template:** `assets/metadata-template.json` - Metadata JSON structure
- **Manifest init:** `assets/manifest-init.json` - Empty manifest structure for initialization
