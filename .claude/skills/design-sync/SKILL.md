---
name: design-sync
description: Validates mockup ↔ creative brief consistency and catches design drift before implementation begins
allowed-tools:
  - Read # Read contracts (brief, parameter-spec, mockup YAML)
  - Edit # Update brief's Evolution section
preconditions:
  - creative-brief.md exists
  - Mockup has been finalized (parameter-spec.md generated)
extended-thinking: true  # Enables mcp__sequential-thinking tool for semantic analysis (Step 3)
thinking-budget: 8000    # Token budget for extended thinking in semantic validation
---

# design-sync Skill

**Purpose:** Validate mockup ↔ creative brief consistency and catch design drift before implementation begins.

## Overview

This skill compares the finalized mockup against the original creative brief to detect drift—where the implemented design diverges from the original vision. Catches misalignments before Stage 5 (GUI implementation) starts, preventing wasted work.

**Why this matters:** Creative briefs capture intent; mockups capture reality. During iteration these can diverge (parameter mismatches, visual style drift, missing features). Detecting drift early enables course correction before implementation.

**Key innovation:** Dual validation (quantitative + semantic) with categorized drift levels and appropriate decision menus.

**Prerequisites:**
- system-setup must have run successfully (requires Python 3.8+, jq for JSON/YAML processing)
- creative-brief.md exists (created via plugin-ideation or manually)
- mockup finalized (parameter-spec.md generated via ui-mockup)

If dependencies missing, design-sync will error with actionable recovery menu.

---

## Entry Points

**Invoked by:**

1. **Auto-invoked by ui-mockup** after Phase 4.5 finalization (before C++ generation)
2. **Manual:** `/sync-design [PluginName]`
3. **MANDATORY: plugin-workflow Stage 1→2 transition** (before foundation-agent dispatch)

**Entry parameters:**

- **Plugin name**: Which plugin to validate
- **Mockup version** (optional): Specific version to check (defaults to latest)

---

## Workflow

<critical_sequence>
<sequence_name>design-sync-validation</sequence_name>
<enforcement>MUST execute all 7 steps in order. MUST NOT skip any step. MUST NOT auto-proceed past decision gates. MUST NOT continue until user responds to gates.</enforcement>

## Validation Progress Checklist

Track completion as you execute:

```
- [ ] Step 1: Load contracts (brief, parameter-spec, mockup)
- [ ] Step 2: Quantitative checks (parameter counts, features)
- [ ] Step 3: Semantic validation (extended thinking analysis)
- [ ] Step 4: Categorize drift (none/acceptable/attention/critical)
- [ ] Step 5: Present findings (appropriate decision menu)
- [ ] Step 6: Execute user choice
- [ ] Step 7: Route back to ui-mockup
```

---

### Step 0: Check Cache
<step_requirement>MUST check cache before proceeding to Step 1. Skip validation if cache hit.</step_requirement>

Before analyzing contracts, check if validation already performed on current content:

```bash
# Source cache utilities
source .claude/utils/validation-cache.sh

# Check if cached
if is_cached "design-sync" "$PLUGIN_NAME" \
    "plugins/$PLUGIN_NAME/.ideas/creative-brief.md" \
    "plugins/$PLUGIN_NAME/.ideas/parameter-spec.md" \
    plugins/$PLUGIN_NAME/.ideas/mockups/*.yaml; then

    echo "✓ design-sync cache hit (content unchanged since last validation)"
    echo ""
    get_cached_result "design-sync" "$PLUGIN_NAME"
    exit 0
fi
```

If cache hit, display cached result and exit. Do not proceed to Step 1.

If cache miss (content changed or expired), continue to Step 1.

---

### Step 1: Load Contracts
<step_requirement>MUST complete before Step 2. If files missing, BLOCK with error menu.</step_requirement>

Read three files:

- `plugins/[PluginName]/.ideas/creative-brief.md` - Original vision
- `plugins/[PluginName]/.ideas/parameter-spec.md` - Finalized parameters from mockup
- `plugins/[PluginName]/.ideas/mockups/vN-ui.yaml` - Finalized mockup

**Error handling:** If any missing, BLOCK with clear message:

```
❌ Cannot validate design sync

Missing required files:
- creative-brief.md: [EXISTS/MISSING]
- parameter-spec.md: [EXISTS/MISSING]
- mockups/v[N]-ui.yaml: [EXISTS/MISSING]

design-sync requires finalized mockup (parameter-spec.md generated).

Actions:
1. Generate parameter-spec.md - Finalize mockup first
2. Create creative brief - Document vision before mockup
3. Skip validation - Proceed without sync (not recommended)
4. Other
```

### Step 2: Quantitative Checks
<step_requirement>MUST complete before Step 3. Extract counts first, then compare.</step_requirement>

**Extract parameter counts and feature lists from contracts using these patterns:**

**From creative-brief.md:**
- Count parameter mentions (parameter names in quotes or CAPS, e.g., "GAIN", "FREQUENCY")
- Detect feature keywords: "preset", "bypass", "meter", "visualization", "tabs", "sections"

**From parameter-spec.md:**
- Count total parameters defined
- Extract parameter names list

**From mockup YAML (render-spec.yaml):**
- Count controls in layout (knob, slider, button elements)
- Detect UI features: preset_browser, meter, spectrum_analyzer, tabs, multi_page_layout

**Comparison logic:**
```typescript
briefParamCount = extractParameterCount(creativeBrief);
specParamCount = countParameters(parameterSpec);
mockupParamCount = countControlsInLayout(mockup.layout);

parameterMatch = (briefParamCount == specParamCount == mockupParamCount);

briefFeatures = extractFeatureKeywords(brief);
mockupFeatures = detectUIFeatures(mockup);
featureMatch = compareFeatureLists(briefFeatures, mockupFeatures);
```

**Parameter count thresholds:**
- Match (0-1 difference): No drift
- Small increase (2-4): Acceptable evolution
- Large increase (5+): Drift requiring attention
- Massive (2x or more): Critical drift
- Any decrease: Drift requiring attention (scope reduction)

**Verification checkpoint:**

Confirm parameter counts extracted successfully:
- Creative brief: [X] parameters
- Parameter-spec: [Y] parameters
- Mockup: [Z] controls

If counts couldn't be extracted (files malformed, unclear), BLOCK and present error menu:

```
❌ Cannot extract parameter counts

Unable to parse parameter counts from contracts. Manual verification needed.

Actions:
1. Provide counts manually - Enter parameter counts to continue
2. Skip quantitative check - Proceed with semantic validation only
3. Debug extraction - Show contract excerpts for inspection
4. Other
```

Only proceed to Step 3 when counts are confirmed.

### Step 3: Semantic Validation (Extended Thinking)
<step_requirement>MUST use extended thinking. MUST NOT skip semantic analysis.</step_requirement>

**Invoke the `mcp__sequential-thinking__sequentialthinking` tool with the following prompt:**

1. **Visual style alignment:**

   - Brief aesthetic: [extract quotes from "UI Vision" section]
   - Mockup aesthetic: [analyze YAML design system: colors, typography, layout style]
   - Match? Yes/No + reasoning

2. **Feature completeness:**

   - Brief promises: [list all mentioned features]
   - Mockup delivers: [list all implemented features]
   - Missing: [list gaps]
   - Assessment: Complete / Partial / Missing core features

3. **Scope assessment:**
   - Additions justified? (evolution vs creep)
   - Reductions justified? (simplification vs missing)
   - Core concept preserved?

**MUST use this exact extended thinking prompt for semantic validation:**

```
Compare creative brief with mockup to assess alignment:

Creative Brief:
- Concept: [extract]
- Use Cases: [extract]
- UI Vision: [extract]
- Parameters: [list]

Mockup:
- Layout: [from YAML]
- Components: [from YAML]
- Visual style: [colors, typography from YAML]
- Parameters: [from parameter-spec.md]

Answer:
1. Does mockup visual style match brief aesthetic intent? (vintage warmth vs modern, minimal vs complex, etc.)
2. Are all brief-mentioned features present?
3. Are mockup additions reasonable evolution or scope creep?
4. Does mockup support the use cases in brief?

Assess confidence: HIGH / MEDIUM / LOW
```

**Confidence check:**

After extended thinking completes, assess the confidence level returned.

**If confidence is LOW:**
1. Re-examine contracts for ambiguity or missing information
2. Present clarification menu to user:

```
⚠️ Semantic validation uncertain (LOW confidence)

Extended thinking analysis could not confidently determine alignment.

Ambiguity detected:
[Specific reason from extended thinking - e.g., "Brief aesthetic 'vintage warmth' is subjective"]

Actions:
1. Provide clarification - Answer: Is this mockup aligned with your vision?
2. Review side-by-side - Show brief vs mockup details
3. Proceed with quantitative only - Skip semantic validation (not recommended)
4. Other
```

**If confidence is MEDIUM:**
- Note uncertainty in findings presentation
- Proceed to Step 4 with caveat

**If confidence is HIGH:**
- Proceed to Step 4 normally

Do not proceed to Step 4 until confidence is at least MEDIUM or user provides clarification.

**Retry pattern:**

If extended thinking tool fails, times out, or returns error:

1. **Retry once** with simplified prompt:
   ```
   Compare creative brief with mockup to assess alignment.
   Brief concept: [extract]
   Mockup delivers: [extract]
   Answer: Does mockup match brief? (YES/NO/UNCERTAIN)
   Reasoning: [explain]
   ```

2. **If retry fails**, fall back to quantitative-only validation:
   - Warn user: "Semantic validation unavailable (tool failure). Proceeding with parameter counts and feature detection only."
   - Present findings based on Step 2 quantitative checks alone
   - Mark assessment with caveat: "Quantitative-only validation (semantic analysis unavailable)"

3. **If quantitative validation also failed**, BLOCK with error menu:
   ```
   ❌ Validation failed

   Both semantic and quantitative validation failed. Cannot assess drift reliably.

   Actions:
   1. Manual verification - Review contracts manually
   2. Retry validation - Attempt design-sync again
   3. Skip validation - Proceed without sync (not recommended)
   4. Other
   ```

### Step 4: Categorize Drift
<step_requirement>MUST categorize based on Step 2+3 findings before presenting.</step_requirement>

**Use these thresholds and indicators to categorize drift objectively:**

**No drift detected:**
- Parameter counts match (±1 difference)
- All brief-mentioned features present in mockup
- Semantic validation confidence: HIGH
- Visual style aligned with brief aesthetic
- Mockup delivers on brief promise
- Use cases supported

**Acceptable evolution:**
- Parameter count increased 2-4 parameters
- Visual polish added (animations, gradients, refinements)
- Layout refined for UX (better organization, spacing)
- Additions improve design and are justified (semantic validation explains why)
- Core concept intact (same plugin type, same goals)
- Brief use cases still supported

**Drift requiring attention:**
- Missing features mentioned in brief
- Visual style mismatch (different aesthetic direction)
- Significant scope change (±5 parameters OR different feature set)
- Parameter count decreased (scope reduction)
- Brief and mockup tell different stories
- Semantic validation confidence: MEDIUM or contradictory findings

**Critical drift:**
- Mockup contradicts brief's core concept (different plugin type)
- Missing essential/primary features
- Massive scope change (2x parameters or more, OR completely different feature set)
- Completely opposite visual style (e.g., vintage warmth → stark modern minimal)
- Brief use cases NOT supported by mockup
- Semantic validation confidence: LOW or identifies fundamental misalignment

**Categorization decision tree:**
```typescript
if (parameterMatch && featureMatch && styleAligned) {
  return "NO_DRIFT";
} else if (minorParamIncrease && coreConceptIntact && semanticConfidence >= MEDIUM) {
  return "ACCEPTABLE_EVOLUTION";
} else if (majorExpansion || missingFeatures || styleMismatch) {
  return "DRIFT_REQUIRING_ATTENTION";
} else if (contradictsCoreGoals || massiveExpansion || fundamentalMisalignment) {
  return "CRITICAL_DRIFT";
}
```

**For detailed examples of each category, see `references/drift-detection.md`.**

### Step 5: Present Findings
<step_requirement>MUST present appropriate decision menu based on drift category.</step_requirement>

**Select menu template based on Step 4 categorization:**

- **No drift**: Present approval menu (see `references/decision-gates.md#no-drift-confirmation`)
- **Acceptable evolution**: Present evolution confirmation menu (see `references/decision-gates.md#acceptable-evolution-choice`)
- **Drift requiring attention**: Present drift resolution menu (see `references/decision-gates.md#drift-requiring-attention`)
- **Critical drift**: Present blocking menu, no override option (see `references/decision-gates.md#critical-drift-blocked`)

**For all menus:**
- Populate template with actual findings from Steps 2-3
- Present menu and WAIT for user selection
- NEVER auto-proceed to option 1, even if marked "(recommended)"
- ALWAYS use `<decision_gate>` tags with `<blocking>true</blocking>` and `<wait_for_user>REQUIRED</wait_for_user>`

See `references/decision-gates.md` for complete menu templates and examples.

### Step 6: Execute User Choice
<step_requirement>MUST execute choice from Step 5 gate before proceeding to Step 7.</step_requirement>

**Option 1: Update brief and continue**

Use the template from `assets/evolution-template.md` to document the evolution:

[Insert populated evolution log entry into creative-brief.md]

Update brief's "UI Vision" section to reflect current mockup (preserve original in "Evolution" section).

**Option 2: Update mockup**

- Exit design-sync (do not auto-invoke ui-mockup)
- Instruct user: "To update mockup, invoke ui-mockup skill with: `Skill('ui-mockup')`"
- Present drift findings for user to address in next ui-mockup iteration

**Option 3: Continue anyway (override)**

- Log override to `.validator-overrides.yaml`:
  ```yaml
  - date: 2025-11-10
    validator: design-sync
    finding: parameter-count-mismatch
    severity: attention
    override-reason: User confirmed evolution is intentional
    mockup-version: v3
  ```
- Return success (allow implementation)
- Warn: "Implementation may not match original vision"

### Step 7: Cache Result and Route Back to ui-mockup
<step_requirement>MUST cache validation result before presenting decision menu.</step_requirement>

**After Step 6 actions complete, cache the validation result:**

```bash
# Build result JSON
RESULT_JSON=$(jq -n \
    --arg status "success" \
    --argjson drift "$DRIFT_DETECTED" \
    --argjson findings "$FINDINGS_JSON" \
    '{status: $status, drift_detected: $drift, findings: $findings}')

# Cache for 24 hours
cache_result "design-sync" "$PLUGIN_NAME" 24 "$RESULT_JSON" \
    "plugins/$PLUGIN_NAME/.ideas/creative-brief.md" \
    "plugins/$PLUGIN_NAME/.ideas/parameter-spec.md" \
    plugins/$PLUGIN_NAME/.ideas/mockups/*.yaml
```

**Then return to ui-mockup Phase 5.5 decision menu:**

<handoff_protocol>
<target_skill>ui-mockup</target_skill>
<target_phase>5.5</target_phase>
<handoff_type>return_to_workflow</handoff_type>
<required_menu>Phase 5.5 decision menu (without "Check alignment" option)</required_menu>

**After caching and completing Step 6 actions (brief updated, mockup changed, or override logged), return to ui-mockup Phase 5.5 decision menu.**

**Present this menu:**

```
✓ Design-brief alignment complete

What's next?
1. Finalize and create implementation files (if satisfied and aligned)
2. Provide more refinements (iterate on design) ← Creates v[N+1]
3. Test in browser (open v[N]-ui-test.html)
4. Save as aesthetic template (add to library for reuse)
5. Finalize AND save aesthetic (do both operations)
6. Other

Choose (1-6): _
```

**This is the same decision point as ui-mockup Phase 5.5, minus the "Check alignment" option (already done).**

**Option handling:**
- **Option 1**: Proceed to ui-mockup Phase 6-9 (generate remaining 5 files)
- **Option 2**: Return to ui-mockup Phase 2 with new version number (iterate design)
- **Option 3**: Open test HTML in browser for review
- **Option 4**: Invoke ui-template-library "save" operation
- **Option 5**: Save aesthetic, then proceed to Phase 6-9
- **Option 6**: Other

**Why route back:**
- Validates the design is aligned before generating implementation files
- Prevents generating C++ boilerplate for misaligned mockups
- Maintains checkpoint protocol (user decides next action after validation)

</handoff_protocol>

</critical_sequence>

---

## Integration with ui-mockup

**ui-mockup Phase 4.5** (after finalization, before C++ generation):

When user chooses "Check alignment" from the ui-mockup Phase 4.5 decision menu → invokes design-sync via: `Skill('design-sync')` (see Step 7 for details on handoff protocol and menu structure).

---

## Integration with plugin-workflow Stage 1→2 Transition

**MANDATORY gate before Stage 2 begins:**

plugin-workflow MUST run design-sync before dispatching foundation-agent (Stage 2), IF mockup exists:

```markdown
━━━ Stage 1 Complete - Planning Finished ━━━

Contracts validated:
- creative-brief.md: EXISTS
- parameter-spec.md: EXISTS (from mockup v3)
- architecture.md: EXISTS
- plan.md: EXISTS

Mockup detected: v3 (finalized)
design-sync validation: REQUIRED before Stage 2

Running mandatory design-sync validation...
```

**Validation flow:**

1. Check for parameter-spec.md existence
2. If exists → invoke design-sync automatically via: `Skill('design-sync')` (not optional)
3. Present findings with decision menu
4. BLOCK Stage 2 dispatch until resolved:
   - No drift → Continue to Stage 2
   - Acceptable evolution → User confirms → Continue
   - Drift detected → User fixes → Continue
   - Critical drift → MUST resolve before proceeding

**If no mockup:**
Skip design-sync, proceed directly to Stage 2

**Why mandatory:**

Stage 2 generates CMakeLists.txt, boilerplate, and build system. If contracts are misaligned, all downstream stages implement the wrong specification. Catching drift at Stage 1→2 boundary prevents 10+ minutes of wasted implementation work.

---

## Success Criteria

Validation is successful when ALL of these are met:

- ✅ Both contracts loaded (creative brief + parameter-spec + mockup YAML)
- ✅ Quantitative checks completed (parameter count, feature detection)
- ✅ Semantic validation performed (extended thinking analysis)
- ✅ Drift category assigned (none / acceptable / attention / critical)
- ✅ Appropriate decision menu presented
- ✅ User action executed (update brief / update mockup / override)

MUST NOT mark validation complete until all requirements met.

---

## Error Handling

**Common error scenarios:**

1. **Missing contracts** (creative-brief.md, parameter-spec.md, or mockup not found)
   - BLOCK validation
   - Present menu: "Generate parameter-spec.md" / "Create creative brief" / "Skip validation"

2. **No finalized mockup** (YAML exists but parameter-spec.md not generated)
   - BLOCK validation
   - Route user to ui-mockup skill to finalize

3. **Ambiguous findings** (semantic validation returns MEDIUM/LOW confidence)
   - Present clarification menu
   - Ask user: "Is this mockup aligned with your vision?"
   - Use response to finalize categorization

4. **Override tracking**
   - When user overrides drift warnings, log to `.validator-overrides.yaml`
   - Include: timestamp, finding, severity, override reason, approved-by
   - Provides audit trail

See `references/error-handling.md` for complete error scenarios and menu examples.

---

## Common Pitfalls (Anti-Patterns)

**AVOID THESE:**

- Forgetting to use extended thinking for semantic validation (critical for accuracy)
- Auto-categorizing as "acceptable" without checking (be objective)
- Not presenting "Continue anyway" for non-critical drift (user should have option)
- Providing "override" for critical drift (should be blocked)
- Updating mockup instead of brief (mockup is source of truth, brief gets updated)
- Not logging overrides (audit trail required)

---

## Example Scenarios

See `references/examples.md` for four detailed validation scenarios:
1. **No Drift (Quick Validation)** - Parameter match, visual alignment (~90 seconds)
2. **Acceptable Evolution** - Minor additions, core concept intact (~3 minutes)
3. **Drift Requiring Attention** - Missing features, style mismatch (~5 minutes)
4. **Critical Drift (Blocked)** - Contradicts core concept, massive scope change (~10 minutes)

