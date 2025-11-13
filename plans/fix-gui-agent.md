# Fix Plan: gui-agent

**Source audit:** ./audits/gui-agent-audit.md
**Target file:** .claude/agents/gui-agent.md
**Current grade:** C
**Target grade:** A-

---

## Executive Summary

The gui-agent requires configuration standardization and dramatic prompt condensation. Configuration issues (non-standard YAML fields, weak description) block proper Claude Code integration. The 900+ line prompt consumes excessive token budget, leaving insufficient context for contract files. Condensing to ~250 lines while moving implementation details to reference documentation will improve performance and reduce costs across all plugin builds.

**Estimated effort:** 3.5 hours
**Risk level:** Low (configuration fixes) to Medium (prompt condensation)
**Blocking issues:** 2 (YAML frontmatter, description field)

---

## Fixes by Priority

### Critical (Must Fix) - 2 issues

#### Fix #1: Standardize YAML frontmatter

**Problem:**
Uses non-standard fields (`type`, `allowed-tools`, `preconditions`) that Claude Code doesn't recognize. These fields are ignored by the system, causing:
- Tool restrictions to fail (`allowed-tools` vs `tools`)
- Precondition checks never executed (frontmatter checks unsupported)
- Configuration ambiguity (`type: agent` has no effect)

**Current state:**
```yaml
---
name: gui-agent
type: agent
description: Integrate WebView UI and bind parameters (Stage 5)
allowed-tools:
  - Read
  - Edit
  - Write
  - Bash
  - mcp__context7__resolve-library-id
  - mcp__context7__get-library-docs
preconditions:
  - Finalized UI mockup exists (v[N]-ui.html)
  - parameter-spec.md exists (from mockup finalization)
  - Stage 4 complete (DSP implemented and working)
  - Build system operational
---
```

**Target state:**
```yaml
---
name: gui-agent
description: Stage 5 GUI implementation specialist. Integrates finalized WebView UI mockups with JUCE C++ code, creates parameter bindings (relays and attachments), and configures CMake for WebView. Use during /implement workflow after Stage 4 (DSP) completes. MUST be invoked by plugin-workflow skill for Stage 5 execution.
tools: Read, Edit, Write, Bash, mcp__context7__resolve-library-id, mcp__context7__get-library-docs
model: sonnet
---
```

**Verification:**
- [ ] No `type` field present
- [ ] No `preconditions` array present
- [ ] Field named `tools` (not `allowed-tools`)
- [ ] Tools list is comma-separated string (not YAML array)
- [ ] `model: sonnet` explicitly set
- [ ] Description expanded to 2-3 sentences

---

#### Fix #2: Expand description field for automatic invocation

**Problem:**
Description "Integrate WebView UI and bind parameters (Stage 5)" is too terse. Claude cannot determine WHEN to invoke this agent automatically. Missing trigger keywords prevent discovery. Lacks invocation context (after which stage, during which workflow).

**Current state:**
```yaml
description: Integrate WebView UI and bind parameters (Stage 5)
```

**Target state:**
```yaml
description: Stage 5 GUI implementation specialist. Integrates finalized WebView UI mockups with JUCE C++ code, creates parameter bindings (relays and attachments), and configures CMake for WebView. Use during /implement workflow after Stage 4 (DSP) completes. MUST be invoked by plugin-workflow skill for Stage 5 execution.
```

**Verification:**
- [ ] Includes "Stage 5" (workflow position)
- [ ] Includes "WebView" (technology trigger)
- [ ] Includes "parameter bindings" (task trigger)
- [ ] Includes "after Stage 4" (invocation timing)
- [ ] Includes "plugin-workflow skill" (invoker context)
- [ ] 2-3 sentences explaining WHAT, WHEN, WHY

---

### High Priority (Should Fix) - 3 issues

#### Fix #3: Reduce system prompt length by ~70%

**Problem:**
Current prompt is 900+ lines, consuming excessive token budget. Leaves insufficient context for contract files (v[N]-ui.html, parameter-spec.md, creative-brief.md). Detailed implementation pseudocode, step-by-step instructions, and troubleshooting belong in reference documentation, not the core prompt.

**Current state:**
- **Total lines:** 903
- **Excessive sections:**
  - Lines 176-207: Python validation pseudocode (31 lines)
  - Lines 352-464: Detailed code generation pseudocode (112 lines)
  - Lines 518-564: Resource provider implementation (46 lines)
  - Lines 574-606: Step-by-step CMake configuration (32 lines)
  - Lines 793-809: Relay pattern reference table (16 lines)
  - Lines 810-862: Extended troubleshooting (52 lines)
  - **Subtotal for removal:** ~289 lines

**Target state:**
- **Total lines:** ~250 (70% reduction)
- **Keep in prompt:**
  - Role and boundaries (lines 20-44): ~25 lines
  - High-level workflow (Phase 0, Steps 1-12 headers only): ~30 lines
  - Critical patterns (member order warning): ~20 lines
  - Validation checklist (conceptual, not code): ~30 lines
  - JSON report format and schema reference: ~40 lines
  - Success/failure criteria: ~20 lines
  - Required Reading reference: ~10 lines
  - Context and responsibilities: ~25 lines
  - Quick reference summary: ~30 lines
  - **Subtotal:** ~230 lines + XML tags = ~250 lines

- **Move to reference:** `.claude/skills/plugin-workflow/references/stage5-webview-guide.md`
  - Detailed code generation pseudocode (step-by-step relay creation)
  - Resource provider implementation with examples
  - CMake configuration details
  - Relay pattern reference table
  - Extended troubleshooting (5 common issues with debugging)
  - Parameter consistency validation logic
  - Member order deep-dive explanation

**Condensed structure:**
```markdown
<role>
Stage 5 GUI implementation specialist for Plugin Freedom System. Integrate finalized WebView UI mockups with JUCE C++ code by creating parameter bindings and configuring the build system.
</role>

<context>
Invoked by plugin-workflow skill after Stage 4 (DSP) completes. Run in fresh context with contracts provided. Build verification and testing handled by other skills after completion.
</context>

<responsibilities>
**You DO:**
- Read finalized UI mockup (v[N]-ui.html) and parameter-spec.md
- Copy UI files to Source/ui/public/
- Generate PluginEditor.h/cpp with relays and attachments
- Update CMakeLists.txt for WebView (binary data, juce_gui_extra, JUCE_WEB_BROWSER=1)
- Validate parameter ID consistency (mockup ↔ spec)
- Return JSON report (schema: .claude/schemas/subagent-report.json)

**You DON'T:**
- Run cmake, build scripts, or compilation
- Install or test the plugin
- Verify builds succeed
</responsibilities>

<required_reading>
Read before implementation:
- `troubleshooting/patterns/juce8-critical-patterns.md` (patterns #7: WebView bindings, member order)
</required_reading>

<critical_patterns>
1. **Member declaration order (prevents 90% of crashes):**
   - Relays first (no dependencies)
   - WebView second (depends on relays via .withOptionsFrom())
   - Attachments last (depend on both relays and webView)
   - Members destroyed in REVERSE order - wrong order = crashes on reload

2. **Parameter ID consistency:**
   - HTML element IDs must match APVTS parameter IDs exactly (case-sensitive)
   - All spec parameters need relay + attachment + .withOptionsFrom() registration

3. **CMake WebView requirements:**
   - juce_add_binary_data for UI files
   - Link juce::juce_gui_extra
   - Define JUCE_WEB_BROWSER=1
</critical_patterns>

<workflow>
1. **Validate contracts:** Check for finalized mockup (v[N]-ui.html) and parameter-spec.md. BLOCK if missing.
2. **Verify parameter consistency:** Extract IDs from mockup and spec, ensure they match
3. **Copy UI files:** mockup → Source/ui/public/ (index.html, CSS, JS, assets)
4. **Generate PluginEditor.h:** Create relay/attachment members in correct order from parameter-spec.md
5. **Generate PluginEditor.cpp:** Initialize relays, register with .withOptionsFrom(), create attachments
6. **Implement resource provider:** Map URLs to BinaryData resources with correct MIME types
7. **Update CMakeLists.txt:** Add juce_add_binary_data, link juce_gui_extra, enable JUCE_WEB_BROWSER
8. **Self-validate:** Check member order, parameter completeness, file structure
9. **Return JSON report:** Status, outputs, issues, ready_for_next_stage
</workflow>

<validation_checklist>
- [ ] Finalized UI mockup exists (v[N]-ui.html)
- [ ] All parameters from spec have matching HTML elements
- [ ] Member order: relays → webView → attachments
- [ ] All relays registered with .withOptionsFrom()
- [ ] All parameters have attachments
- [ ] CMakeLists.txt includes juce_add_binary_data
- [ ] CMakeLists.txt links juce::juce_gui_extra
- [ ] CMakeLists.txt defines JUCE_WEB_BROWSER=1
- [ ] Resource provider implemented with MIME types
</validation_checklist>

<output_format>
Return JSON report conforming to `.claude/schemas/subagent-report.json`:

**Success:**
```json
{
  "agent": "gui-agent",
  "status": "success",
  "outputs": {
    "plugin_name": "[PluginName]",
    "webview_integrated": true,
    "ui_files_created": ["Source/ui/public/index.html", ...],
    "relays_created": N,
    "attachments_created": N
  },
  "issues": [],
  "ready_for_next_stage": true
}
```

**Failure:**
```json
{
  "agent": "gui-agent",
  "status": "failure",
  "outputs": {
    "plugin_name": "[PluginName]",
    "error_type": "missing_mockup|binding_mismatch|compilation_error",
    ...
  },
  "issues": ["Detailed error messages"],
  "ready_for_next_stage": false
}
```
</output_format>

<detailed_implementation_guide>
For step-by-step code generation, relay patterns, CMake configuration, and troubleshooting:
See `.claude/skills/plugin-workflow/references/stage5-webview-guide.md`
</detailed_implementation_guide>

<success_criteria>
Stage 5 succeeds when:
1. UI mockup integrated (HTML/CSS/JS in Source/ui/public/)
2. All parameters have relay + attachment
3. Member order correct (relays → webView → attachments)
4. CMake configured for WebView
5. JSON report returned with status="success"

Stage 5 fails when:
- No finalized mockup found
- Parameter ID mismatch (mockup ↔ spec)
- Missing bindings (parameters without relay/attachment)
- Wrong member order
</success_criteria>
```

**Verification:**
- [ ] Prompt reduced from 903 to ~250 lines
- [ ] Reference file created: `.claude/skills/plugin-workflow/references/stage5-webview-guide.md`
- [ ] All implementation details moved to reference
- [ ] Critical patterns preserved in prompt
- [ ] Workflow remains clear (high-level only)
- [ ] JSON report format preserved
- [ ] Success criteria intact

---

#### Fix #4: Replace Python pseudocode with Bash validation

**Problem:**
Lines 176-207, 352-465, 649-680 contain Python pseudocode that Claude Code subagents cannot execute. Provides intent but not actionable validation logic. Bash/jq/grep commands are executable and provide practical validation.

**Current state (lines 176-207):**
```python
# Extract parameter IDs from HTML (simplified example)
import re
from pathlib import Path

html_content = Path(f"plugins/{PLUGIN_NAME}/.ideas/mockups/v{VERSION}-ui.html").read_text()
html_param_ids = set(re.findall(r'data-param-id=["\'](\w+)["\']', html_content))
html_param_ids.update(re.findall(r'<(?:input|select)[^>]+id=["\'](\w+)["\']', html_content))

spec_content = Path(f"plugins/{PLUGIN_NAME}/.ideas/parameter-spec.md").read_text()
spec_param_ids = set(re.findall(r'\*\*ID:\*\*\s+(\w+)', spec_content))

missing_in_html = spec_param_ids - html_param_ids
extra_in_html = html_param_ids - spec_param_ids

if missing_in_html:
    print(f"ERROR: Parameters in spec but missing in HTML: {missing_in_html}")
    exit(1)
```

**Target state:**
```markdown
<parameter_consistency_validation>
Verify all parameters from spec appear in mockup:

1. Extract parameter IDs from parameter-spec.md:
   ```bash
   grep '^\*\*ID:\*\*' parameter-spec.md | sed 's/\*\*ID:\*\* //' | sort > /tmp/spec-params.txt
   ```

2. Extract parameter IDs from HTML mockup:
   ```bash
   grep -oE 'data-param-id="[^"]+"|id="[^"]+"' v*-ui.html | \
     sed 's/.*="\([^"]*\)"/\1/' | sort | uniq > /tmp/html-params.txt
   ```

3. Check for missing parameters:
   ```bash
   comm -23 /tmp/spec-params.txt /tmp/html-params.txt > /tmp/missing.txt
   if [ -s /tmp/missing.txt ]; then
     echo "ERROR: Parameters in spec missing from HTML:"
     cat /tmp/missing.txt
     # Return failure report with error_type: "parameter_mismatch"
   fi
   ```

If validation fails, return JSON report with:
- `status: "failure"`
- `error_type: "parameter_mismatch"`
- `missing_parameters: [list from missing.txt]`
</parameter_consistency_validation>
```

**Verification:**
- [ ] Python pseudocode removed entirely
- [ ] Bash validation commands provided
- [ ] Commands are executable (use grep, sed, comm)
- [ ] Failure handling specified (JSON report format)
- [ ] Same validation logic preserved (parameter ID consistency)

---

#### Fix #5: Remove or implement Native JUCE fallback

**Problem:**
Lines 144-151 reference unimplemented Native JUCE workflow, creating ambiguity. Section promises functionality that doesn't exist. Either implement it or remove the placeholder to avoid confusion.

**Current state (lines 142-151):**
```markdown
### Workflow B: Native JUCE Implementation (fallback)

**[TO BE IMPLEMENTED]** Generate basic native JUCE UI:

- Create PluginEditor with juce::Slider/ToggleButton/ComboBox components
- Layout components vertically with labels
- Use juce::AudioProcessorValueTreeState::SliderAttachment for bindings
- Minimal styling (default JUCE look and feel)

**Note:** Native JUCE workflow is a fallback for quick iteration. For production plugins, always use WebView mockup workflow for better visual design.
```

**Target state (Option 1 - Remove entirely):**
```markdown
### Workflow Routing

**WebView Implementation (mandatory):**

Stage 5 requires a finalized WebView mockup (v[N]-ui.html). If no mockup exists, return failure report with:
- `status: "failure"`
- `error_type: "missing_mockup"`
- Resolution: Complete UI mockup workflow (/mockup) and finalize a design version

See Contract Enforcement section (line 62) for blocking behavior.
```

**Target state (Option 2 - Clarify as future enhancement):**
```markdown
### Workflow B: Native JUCE Implementation (future)

**FUTURE ENHANCEMENT:** Native JUCE fallback not yet implemented.

For now, finalized WebView mockup is required. If missing, gui-agent returns failure report and blocks Stage 5.

When implemented, Native JUCE workflow will generate basic juce::Slider/ToggleButton/ComboBox components for quick iteration without mockup. Production plugins should always use WebView for better visual design.
```

**Recommended approach:** Option 1 (remove entirely)
- Simplifies agent scope (WebView only)
- Removes ambiguity about available workflows
- Contract Enforcement section already handles missing mockup correctly

**Verification:**
- [ ] Unimplemented workflow section removed OR clearly marked as future enhancement
- [ ] No "[TO BE IMPLEMENTED]" placeholders in prompt
- [ ] Contract Enforcement section clarifies WebView is mandatory
- [ ] No confusion about available implementation paths

---

### Medium Priority (Nice to Have) - 3 issues

#### Fix #6: Consolidate redundant member order warnings

**Problem:**
Member declaration order is mentioned in 4 separate locations (lines 94, 349-350, 393-429, 636), creating redundancy and consuming prompt space. Single comprehensive warning with brief references elsewhere would be more efficient.

**Current locations:**
- Line 94: Brief mention in "Key patterns for Stage 5"
- Lines 349-350: Critical warning header before code generation
- Lines 393-429: Detailed 36-line explanation with examples
- Line 636: Validation checklist item

**Target state:**
Consolidate into `<critical_patterns>` section (keep in condensed prompt):

```markdown
<critical_patterns>
1. **Member declaration order (prevents 90% of crashes):**
   - Relays first (no dependencies)
   - WebView second (depends on relays via .withOptionsFrom())
   - Attachments last (depend on both relays and webView)
   - **Why:** Members destroyed in REVERSE order of declaration
   - **Wrong order = crash:** Attachments try to use already-destroyed webView on plugin reload

2. **Parameter ID consistency:** [as before]
3. **CMake WebView requirements:** [as before]
</critical_patterns>
```

Reference elsewhere:
- Validation checklist: "Member order: relays → webView → attachments"
- Code generation section (in reference doc): Link to pattern with "See critical pattern #1"

**Verification:**
- [ ] Single comprehensive member order explanation in `<critical_patterns>`
- [ ] Other sections reference pattern briefly ("relays → webView → attachments")
- [ ] Detailed 36-line explanation moved to reference doc
- [ ] Pattern emphasizes crash prevention
- [ ] Validation checklist includes order check

---

#### Fix #7: Add explicit handoff instruction

**Problem:**
Prompt implies but doesn't explicitly state that gui-agent returns control to plugin-workflow. Unclear what happens after JSON report is returned. Subagent might attempt build verification itself.

**Current state (line 685 - section header only):**
```markdown
### 12. Return Report

## JSON Report Format
[Schema details...]
```

**Target state:**
```markdown
### 12. Return Report

After implementation completes, return JSON report to plugin-workflow orchestrator.

**What happens next:**
1. plugin-workflow parses your report
2. If status="failure": Presents errors to user, stops workflow
3. If status="success":
   - Invokes build-automation skill to compile and verify
   - Runs automated tests via plugin-testing skill
   - Presents checkpoint menu to user

**You do NOT:**
- Run cmake or build commands
- Verify compilation succeeds
- Test the plugin in DAW
- Install binaries

Build verification handled by build-automation skill after you complete.

## JSON Report Format
[Schema details...]
```

**Verification:**
- [ ] Explicit statement: "return JSON report to plugin-workflow orchestrator"
- [ ] Next steps enumerated (parse, build, test, checkpoint)
- [ ] Clarifies what gui-agent doesn't do (build/test)
- [ ] Prevents subagent from attempting build verification

---

#### Fix #8: Make schema reference more prominent

**Problem:**
Schema reference (`.claude/schemas/subagent-report.json`) mentioned on line 688 but buried in text. Easy to miss. Should be prominently displayed at start of JSON Report Format section.

**Current state (line 688):**
```markdown
## JSON Report Format

**Schema:** `.claude/schemas/subagent-report.json`

All reports MUST conform to the unified subagent report schema...
```

**Target state:**
```markdown
## JSON Report Format

**CRITICAL:** All reports MUST validate against unified schema.

**Schema location:** `.claude/schemas/subagent-report.json`
**Validation details:** `.claude/schemas/README.md`

This ensures consistent parsing by plugin-workflow orchestrator. Non-compliant reports will cause workflow failures.

[Rest of section with success/failure examples...]
```

**Verification:**
- [ ] "CRITICAL:" label added
- [ ] Schema location on separate line (prominent)
- [ ] README.md reference added for validation details
- [ ] Explains WHY schema compliance matters (orchestrator parsing)
- [ ] Positioned at top of section (before examples)

---

### Low Priority (Polish) - 2 issues

#### Fix #9: Consistent terminology for PFS components

**Problem:**
Inconsistent references to PFS components (sometimes "skill", sometimes omitted). Creates ambiguity about component types.

**Current inconsistencies:**
- Line 22: "plugin-workflow skill" ✓
- Line 43: "build-automation skill" ✓
- Other locations: "plugin-workflow", "build-automation" (missing "skill")

**Target state:**
- **Skills:** Always "X skill" (plugin-workflow skill, build-automation skill, ui-mockup skill)
- **Subagents:** Always "X subagent" or just "X" if context clear (gui-agent, dsp-agent, foundation-agent)
- **Commands:** Always "/X" (slash prefix) when referring to command (/implement, /test, /improve)

**Pattern:**
```markdown
✓ "Invoked by plugin-workflow skill after Stage 4 completes"
✓ "Build verification handled by build-automation skill"
✓ "Complete UI mockup workflow via /mockup command"
✓ "gui-agent generates PluginEditor files"
✗ "Invoked by plugin-workflow after Stage 4"
✗ "Build verification handled by build-automation"
```

**Verification:**
- [ ] All skill references include "skill" suffix
- [ ] All subagent references include "subagent" suffix or are contextually clear
- [ ] All command references include "/" prefix
- [ ] Terminology consistent throughout prompt

---

#### Fix #10: Add quick reference summary at top

**Problem:**
Critical information scattered throughout 900-line prompt. Quick reference at top would improve subagent efficiency (scan before deep-diving into workflow).

**Target location:** After `</responsibilities>` tag, before `<required_reading>`

**Content:**
```markdown
<quick_reference>
**Input contracts:**
- `.ideas/mockups/v[N]-ui.html` (finalized WebView mockup)
- `.ideas/parameter-spec.md` (parameter definitions)
- `.ideas/creative-brief.md` (plugin name, aesthetic)

**Output artifacts:**
- `Source/ui/public/index.html` (copied from mockup)
- `Source/ui/public/js/` (JavaScript files)
- `Source/PluginEditor.h` (generated with relays/attachments)
- `Source/PluginEditor.cpp` (generated with bindings)
- `CMakeLists.txt` (updated with WebView config)

**Critical patterns:**
- Member order: relays → webView → attachments (prevents crashes)
- All parameters need: relay + attachment + .withOptionsFrom()
- Parameter IDs must match exactly (case-sensitive)

**Output:** JSON report conforming to `.claude/schemas/subagent-report.json`
</quick_reference>
```

**Verification:**
- [ ] Quick reference added after `</responsibilities>` tag
- [ ] Lists input contracts (3 files)
- [ ] Lists output artifacts (5 files)
- [ ] Summarizes critical patterns (3 patterns)
- [ ] References JSON schema
- [ ] Uses XML tag structure (`<quick_reference>`)
- [ ] Scannable format (bullet points, bold labels)

---

## Strengths to Preserve

**DO NOT change these elements** (identified as strengths in audit):

- **XML-style tags for structure:** `<role>`, `<workflow>`, `<constraints>`, `<critical_patterns>`, etc.
- **Clear single responsibility:** Stage 5 WebView UI Integration specialist only
- **Explicit boundaries:** "What you DON'T do" section (no build/test verification)
- **12-step WebView implementation workflow:** Logical progression from validation to report
- **Success/failure criteria:** Clear definitions of when Stage 5 succeeds/fails
- **JSON report format:** Schema-based output with success/failure examples
- **Comprehensive validation checklist:** 9-item checklist for self-validation
- **PFS integration awareness:** Contract references, dispatcher pattern, Required Reading injection
- **Member order emphasis:** Critical pattern preventing 90% of crashes
- **Parameter consistency validation:** HTML ↔ spec ID matching
- **Error handling specificity:** Detailed failure report examples with error types

---

## Implementation Sequence

1. **Standardize YAML frontmatter** - 15 min - Blocking issue, quick fix
2. **Expand description field** - 10 min - Blocking issue, enables automatic invocation
3. **Create reference document** - 30 min - Extract detailed content before condensing prompt
4. **Condense system prompt** - 90 min - Major refactor, preserve critical patterns
5. **Replace Python with Bash** - 30 min - Requires understanding validation logic
6. **Remove Native JUCE placeholder** - 10 min - Simple deletion, update Contract Enforcement
7. **Consolidate member order warnings** - 20 min - Merge 4 locations into 1 comprehensive section
8. **Add explicit handoff instruction** - 15 min - Clarify orchestrator relationship
9. **Make schema reference prominent** - 10 min - Move to top of section, add emphasis
10. **Consistent terminology** - 20 min - Search-replace with verification
11. **Add quick reference summary** - 20 min - Create scannable overview section

**Total estimated time:** 4 hours 30 minutes

---

## Special Focus: Prompt Condensation Strategy

**Current:** 903 lines
**Target:** ~250 lines (72% reduction)

### Phase 1: Extract to Reference Document

**Create:** `.claude/skills/plugin-workflow/references/stage5-webview-guide.md`

**Move these sections (total ~450 lines):**

1. **Detailed code generation pseudocode** (lines 352-464, ~112 lines)
   - Python-style relay generation loops
   - Step-by-step attachment creation
   - Member initialization patterns

2. **Resource provider implementation** (lines 518-564, ~46 lines)
   - Complete getResource() function with examples
   - MIME type mapping table
   - BinaryData lookup logic

3. **CMake configuration details** (lines 574-606, ~32 lines)
   - juce_add_binary_data syntax with file lists
   - target_link_libraries examples
   - target_compile_definitions patterns

4. **Relay pattern reference table** (lines 793-809, ~16 lines)
   - Parameter type → Relay class → JavaScript function mapping
   - Full table with 3 parameter types

5. **Extended troubleshooting** (lines 810-862, ~52 lines)
   - 5 common issues with detailed debugging steps
   - Symptom → Resolution → Impact for each issue
   - Blank WebView, sync failures, crashes on reload, etc.

6. **Python validation pseudocode** (lines 176-207, ~31 lines)
   - Parameter ID extraction from HTML
   - Parameter ID extraction from spec
   - Set comparison logic

7. **Member order deep-dive** (lines 393-429, ~36 lines)
   - Detailed 36-line explanation
   - Why declaration order matters
   - Destruction order examples

8. **Native JUCE workflow placeholder** (lines 142-151, ~9 lines)
   - Unimplemented fallback description

**Subtotal extracted:** ~334 lines

### Phase 2: Condense Remaining Content

**Workflow section (lines 99-680):** Reduce from ~580 to ~80 lines
- Keep 12-step workflow as 9-item list (workflow headers only)
- Remove detailed substeps (bullets under each step)
- Example:
  ```markdown
  <workflow>
  1. Validate contracts (check mockup exists, BLOCK if missing)
  2. Verify parameter consistency (mockup IDs ↔ spec IDs)
  3. Copy UI files (mockup → Source/ui/public/)
  4. Generate PluginEditor.h (relays + attachments in correct order)
  5. Generate PluginEditor.cpp (initialize + register)
  6. Implement resource provider (URLs → BinaryData)
  7. Update CMakeLists.txt (binary data + WebView config)
  8. Self-validate (member order, bindings, file structure)
  9. Return JSON report (status, outputs, issues, ready_for_next_stage)
  </workflow>
  ```

**Validation section (lines 631-680):** Reduce from ~50 to ~30 lines
- Keep checklist (9 items)
- Remove Python verification logic (lines 649-680)
- Reference: "See stage5-webview-guide.md for validation commands"

**JSON report format (lines 687-791):** Reduce from ~104 to ~40 lines
- Keep schema reference (prominent)
- Keep success example (compact)
- Keep failure example (compact)
- Remove extended failure example (lines 771-791)

**Common issues (lines 810-862):** Remove entirely (~52 lines)
- Move to reference document

**Relay pattern table (lines 793-809):** Remove entirely (~16 lines)
- Move to reference document

### Phase 3: Add Condensed Sections

**New sections (add ~50 lines):**
- `<quick_reference>`: 20 lines (inputs, outputs, critical patterns)
- `<handoff_protocol>`: 15 lines (what happens after report returned)
- Enhanced `<critical_patterns>`: 15 lines (consolidated member order warning)

### Condensation Math

**Removals:**
- Extract to reference: -334 lines
- Condense workflow: -500 lines (580 → 80)
- Condense validation: -20 lines (50 → 30)
- Condense JSON format: -64 lines (104 → 40)
- Remove common issues: -52 lines
- Remove relay table: -16 lines
**Subtotal removed:** -986 lines

**Additions:**
- New sections: +50 lines
- Expanded YAML description: +2 lines
**Subtotal added:** +52 lines

**Net change:** 903 - 986 + 52 = **-31 lines** (wait, this doesn't work...)

**Recalculation:**
- Current: 903 lines total
- Keep core structure: ~200 lines (role, context, responsibilities, required reading, critical patterns, workflow headers, validation checklist, JSON format, success criteria)
- Add new sections: +50 lines
- **Target: 250 lines**

### Reference Document Structure

`.claude/skills/plugin-workflow/references/stage5-webview-guide.md`:

```markdown
# Stage 5 WebView Implementation Guide

Detailed reference for gui-agent. Read when generating specific code or debugging issues.

## Code Generation Patterns

### Relay Declaration Generation
[Python pseudocode from lines 352-363]

### Attachment Declaration Generation
[Python pseudocode from lines 365-374]

### Relay Initialization
[Python pseudocode from lines 434-445]

[etc. - all extracted implementation details]

## CMake Configuration

### Binary Data Setup
[Lines 578-584]

### Module Linking
[Lines 586-591]

[etc.]

## Troubleshooting

### Issue 1: Parameter ID Mismatch
- **Symptom:** HTML references parameter IDs not in spec
- **Detection:** Step 1.5 validation catches this
- **Resolution:** Regenerate mockup with correct IDs, or update spec

[etc. - all 5 common issues]

## Relay Pattern Reference

[Table from lines 793-809]

## Resource Provider Implementation

[Code from lines 518-564]

## Parameter Consistency Validation

[Bash commands replacing Python pseudocode]
```

**Benefits:**
- Detailed implementation remains accessible (reference doc)
- Core prompt focuses on workflow and critical patterns
- Token budget preserved for contract files
- Subagent can reference guide when needed

---

## Verification Checklist

After all fixes applied:

### Configuration
- [ ] YAML frontmatter uses only standard Claude Code fields
- [ ] `tools` field (not `allowed-tools`) contains comma-separated list
- [ ] `description` includes invocation triggers (2-3 sentences)
- [ ] `model: sonnet` explicitly set
- [ ] All non-standard fields removed (`type`, `preconditions`)

### Prompt Length
- [ ] Total lines reduced from 903 to ~250
- [ ] Reference documentation created: `.claude/skills/plugin-workflow/references/stage5-webview-guide.md`
- [ ] Implementation details moved to reference
- [ ] Critical patterns preserved in prompt
- [ ] Workflow condensed to high-level steps (headers only)

### Functionality Preservation
- [ ] System prompt functionality unchanged (same outputs)
- [ ] XML tag structure preserved
- [ ] Member order emphasis retained
- [ ] Validation checklist intact
- [ ] JSON report format preserved
- [ ] Success/failure criteria clear
- [ ] Required Reading reference included

### Code Quality
- [ ] Python pseudocode replaced with Bash commands (or removed)
- [ ] Native JUCE placeholder removed
- [ ] Member order warnings consolidated
- [ ] Handoff instruction explicit
- [ ] Schema reference prominent
- [ ] Terminology consistent (skills, subagents, commands)
- [ ] Quick reference summary added

### Syntax
- [ ] File syntax valid (YAML frontmatter + Markdown body)
- [ ] No YAML parsing errors
- [ ] XML-style tags properly closed
- [ ] Code blocks properly fenced
- [ ] No broken internal references

---

## Risk Assessment

**What could go wrong:**

1. **Prompt condensation loses critical information**
   - **Mitigation:** Extract to reference before condensing, verify all implementation details preserved
   - **Test:** Run Stage 5 on existing plugin, compare outputs before/after

2. **Tool restrictions break with comma-separated format**
   - **Mitigation:** Follow Claude Code documentation exactly, verify syntax
   - **Test:** Invoke gui-agent, check tool access logs

3. **Reference document not consulted by subagent**
   - **Mitigation:** Add explicit instruction in prompt: "See stage5-webview-guide.md for detailed implementation"
   - **Test:** Monitor subagent output, verify reference file mentioned

4. **YAML frontmatter syntax invalid**
   - **Mitigation:** Validate YAML syntax with online parser before committing
   - **Rollback:** Git revert to previous version if subagent fails to load

5. **Workflow steps too condensed, subagent loses context**
   - **Mitigation:** Keep workflow clear with brief description per step
   - **Test:** Run Stage 5, verify all steps executed correctly

**Rollback plan:**
1. Git commit before starting changes: `git commit -m "feat: backup gui-agent before condensation refactor"`
2. Keep copy of original: `cp .claude/agents/gui-agent.md .claude/agents/backups/gui-agent-original-20251112.md`
3. If issues detected: `git revert HEAD` or restore from backup
4. Reference document can be deleted if unused (no dependencies)

---

## Post-Fix Testing

**How to validate the fixes:**

### Test 1: Configuration Validation
```bash
# Verify YAML frontmatter syntax
head -n 10 .claude/agents/gui-agent.md | python3 -c "import yaml, sys; yaml.safe_load(sys.stdin)"

# Verify tools field format
grep "^tools:" .claude/agents/gui-agent.md
# Expected: "tools: Read, Edit, Write, Bash, mcp__context7__resolve-library-id, mcp__context7__get-library-docs"

# Verify no non-standard fields
grep -E "^(type|preconditions|allowed-tools):" .claude/agents/gui-agent.md
# Expected: no output (fields removed)
```

### Test 2: Prompt Length Verification
```bash
# Count total lines
wc -l .claude/agents/gui-agent.md
# Expected: ~250 lines (vs 903 before)

# Verify reference document created
ls -lh .claude/skills/plugin-workflow/references/stage5-webview-guide.md
# Expected: file exists, ~450 lines
```

### Test 3: Functional Testing (Stage 5 execution)
1. Select existing plugin with finalized mockup (e.g., TapeAge)
2. Manually invoke gui-agent via plugin-workflow
3. Verify outputs:
   - [ ] PluginEditor.h/cpp generated
   - [ ] UI files copied to Source/ui/public/
   - [ ] CMakeLists.txt updated
   - [ ] JSON report returned
   - [ ] Member order correct
4. Compare outputs to previous Stage 5 run (should be identical)

### Test 4: Reference Document Usage
1. Monitor subagent output during Stage 5
2. Verify reference to stage5-webview-guide.md if detailed implementation needed
3. Check that subagent doesn't fail due to missing information

**Success criteria:**
- [ ] Agent loads without errors (YAML valid)
- [ ] Tools properly restricted (Read, Edit, Write, Bash, MCP tools only)
- [ ] Prompt length reduced by ~70% (903 → ~250 lines)
- [ ] Stage 5 implementation quality unchanged (outputs identical)
- [ ] Reference document accessible and complete
- [ ] No regression in generated code quality
- [ ] Member order warning preserved and effective
- [ ] JSON report format compliant with schema

---

## Notes

### Why 250 lines is the target

- **Token efficiency:** Subagents receive full prompt + contract files in context
- **Typical contract size:**
  - v[N]-ui.html: ~200 lines
  - parameter-spec.md: ~100 lines
  - creative-brief.md: ~50 lines
  - Total contracts: ~350 lines
- **Token budget (sonnet):** ~8K input tokens optimal for response quality
- **Math:** 250 (prompt) + 350 (contracts) + 100 (orchestrator handoff) = ~700 lines = ~5K tokens
- **Remaining:** ~3K tokens for response generation (PluginEditor.h/cpp, CMake updates)

### Why reference document works

- Subagents can reference external files when needed via Read tool
- Prompt provides high-level workflow (WHAT to do)
- Reference provides implementation details (HOW to do it)
- Keeps prompt focused on critical patterns and validation
- Details available on-demand without consuming base context

### Alternative approach (not recommended)

- Could reduce to 150 lines with more aggressive condensation
- Risk: Lose critical patterns and validation logic
- Trade-off: Not worth the extra 100 lines saved
- 250 lines balances completeness with efficiency

### Future optimizations

After this refactor proves stable:
1. Extract relay patterns to shared reference (used by multiple subagents)
2. Create subagent prompt template (standard sections for all agents)
3. Consider JSON schema validation in PostToolUse hook (catch report errors early)
