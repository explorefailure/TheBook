---
name: aesthetic-dreaming
description: Create aesthetic templates without first creating a plugin - capture visual design concepts through adaptive questioning
allowed-tools:
  - Read
  - Write
  - Bash
  - AskUserQuestion
  - Skill
preconditions:
  - None (entry point)
---

# aesthetic-dreaming Skill

**Purpose:** Create aesthetic templates before implementing any plugins. Capture visual design systems through adaptive questioning, generate structured aesthetic.md, and optionally create test previews demonstrating the aesthetic across different plugin types.

**Solves:** The friction of needing to create a full plugin just to design and save an aesthetic template. Build a library of visual design systems first, apply them later.

---

<workflow_overview>
<critical_sequence enforcement="strict">
This workflow MUST execute phases in order. Do NOT skip phases or proceed to finalization before completing gap analysis and question batches.

**Phase Flow:**
1. **Phase 1: Free-form collection** - Gather initial aesthetic vision
2. **Phase 2: Gap analysis** - Identify missing visual elements
3. **Phase 3: Question batch** - Ask 4 contextual questions via AskUserQuestion
4. **Phase 3.5: Decision gate** - Finalize / Continue / Add context
5. **Phase 3.7: Name aesthetic** (if finalizing)
6. **Phase 4: Test preview selection**
7. **Phase 5: Generate files**
8. **Phase 6: Decision menu**

**Loop Structure:**
Phases 2-3-3.5 repeat until user chooses "finalize" at decision gate.

**Critical Rules:**
- Do NOT skip phases
- Do NOT execute phases out of order
- Do NOT proceed to finalization before decision gate approval
- NEVER auto-proceed past decision gates
</critical_sequence>
</workflow_overview>

---

<phase id="1" name="free_form_collection">
<instructions>
Present open invitation to user. Extract all mentioned visual concepts verbatim and accumulate in context.
</instructions>

<required_prompt>
What aesthetic are you envisioning?

Describe the visual design system you want to create—share as much or as little as you want. I'll ask follow-ups for anything missing.

Consider: overall vibe, color palette, control styling, spacing, textures, typography, and plugin types this would suit.

You can reference existing designs, describe feelings, or provide technical specifics.
</required_prompt>

<extraction_targets>
- Visual Identity: Overall vibe, mood, design philosophy
- Color System: Palette, warm/cool, contrast, specific colors
- Typography: Font choices, sizing philosophy
- Control Styling: Knobs, sliders, buttons, rotaries
- Spacing: Density, breathing room, layout approach
- Surface Treatment: Textures, shadows, depth, glossy vs. matte
- Details: Special features, animations, microinteractions
- Best For: Plugin types, use cases
- Inspirations: References to existing designs
</extraction_targets>

<state_requirement>
MUST accumulate all extracted concepts in persistent context. This context carries forward through ALL phases and question batches. NEVER lose information from previous phases.
</state_requirement>
</phase>

---

<phase id="2" name="gap_analysis">
<critical_sequence>
1. Parse user's accumulated context (from Phase 1 + any previous question batches)
2. Check coverage against tier priority system
3. Identify top 4 gaps by tier priority
4. NEVER ask about already-provided information
</critical_sequence>

<tier_system>
**Tier 1 (Critical):** Overall vibe/mood, color philosophy, control style
**Tier 2 (Visual Core):** Specific colors, typography, spacing, textures, control details
**Tier 3 (Context):** Best suited for, special features, inspirations
</tier_system>

<anti_pattern>
NEVER ask redundant questions. If user stated "dark modern blue", do NOT ask:
- "What's the overall vibe?" (modern already stated)
- "Light or dark?" (dark already stated)
- "What accent color?" (blue already stated)

Instead focus on uncovered gaps: typography, spacing, textures, plugin types.
</anti_pattern>

<delegation_rule>
Detailed question banks are in references/aesthetic-questions.md. Reference for gap analysis strategies and question options.
</delegation_rule>
</phase>

---

<phase id="3" name="question_batch">
<instructions>
Generate exactly 4 questions using AskUserQuestion tool based on gaps identified in Phase 2.
</instructions>

<requirements>
- MUST use AskUserQuestion tool (not inline menu)
- Provide 3-4 meaningful options per question (not just open text)
- Always include "Other" option for custom input
- Users can type "skip" in "Other" to skip questions
- Questions MUST target identified gaps, prioritized by tier
</requirements>

<tool_usage>
Tool: AskUserQuestion
Parameters:
  - questions: array[1-4] of question objects
  - Each question has: question, header, multiSelect, options[2-4]
  - Always include "Other" option automatically
</tool_usage>

<example>
See references/question-examples.md for detailed AskUserQuestion formatting examples and question banks by category.
</example>

<state_transition>
After receiving question batch answers:
1. Merge answers with accumulated context (cumulative, never overwrite)
2. Proceed DIRECTLY to decision gate (Phase 3.5)
3. Decision gate will route to Phase 2 (re-analyze) or Phase 3.7 (finalize)
</state_transition>
</phase>

---

<decision_gate id="finalize_or_continue" phase="3.5" enforcement="blocking">
<critical_sequence>
MUST present this decision gate after EVERY question batch (Phase 3).
MUST use AskUserQuestion tool (not inline numbered list).
MUST wait for user response before proceeding.
</critical_sequence>

<required_options>
1. "Yes, finalize it" → Proceed to Phase 3.7 (name aesthetic)
2. "Ask me 4 more questions" → Return to Phase 2 (re-analyze gaps)
3. "Let me add more context first" → Collect free-form text, return to Phase 2
</required_options>

<tool_format>
AskUserQuestion({
  questions: [{
    question: "Ready to finalize the aesthetic concept?",
    header: "Next step",
    multiSelect: false,
    options: [
      { label: "Yes, finalize it", description: "Create aesthetic.md and optional previews" },
      { label: "Ask me 4 more questions", description: "Continue refining the design" },
      { label: "Let me add more context first", description: "Provide additional details before questions" }
    ]
  }]
})
</tool_format>

<routing_logic>
IF option 1 THEN proceed to Phase 3.7
ELSE IF option 2 THEN return to Phase 2 (with accumulated context)
ELSE IF option 3 THEN collect user input, merge with context, return to Phase 2
</routing_logic>

<state_requirement>
Context accumulation is cumulative across all iterations. Never lose previously collected information.
</state_requirement>
</decision_gate>

---

<phase id="3.7" name="name_aesthetic">
<preconditions>
Only execute if user chose "finalize" at decision gate (Phase 3.5).
</preconditions>

<conditional_execution>
IF aesthetic name already provided in initial description THEN skip this phase and proceed to Phase 4.
ELSE ask user for name via AskUserQuestion with 3 suggested names + "Other" option.
</conditional_execution>

<name_generation_rules>
- Suggest 3 names based on accumulated context (vibe + colors + style)
- Examples: "Dark modern blue" → "Modern Professional", "Blue Tech", "Dark Elegance"
- Allow user to provide custom name via "Other" option
</name_generation_rules>

<slug_generation>
- Slugify name: "Modern Professional" → "modern-professional"
- Check for existing IDs in manifest.json and append counter: "modern-professional-001"
- Increment until unique ID found
</slug_generation>
</phase>

---

<phase id="4" name="test_preview_selection">
<instructions>
Present test preview selection menu using AskUserQuestion with multiSelect: true.
</instructions>

<requirements>
- MUST use AskUserQuestion tool with multiSelect: true
- Offer 4 test plugin types: simple-compressor, complex-reverb, drum-machine, simple-synth
- User can select 0-4 options (0 = skip previews entirely)
</requirements>

<tool_format>
AskUserQuestion({
  questions: [{
    question: "Generate test previews to see this aesthetic applied? (select 0-4 plugin types)",
    header: "Test previews",
    multiSelect: true,
    options: [
      { label: "Simple compressor", description: "4 parameters: threshold, ratio, attack, release" },
      { label: "Complex reverb", description: "8 parameters: size, damping, predelay, mix, tone, width, decay, diffusion" },
      { label: "Drum machine", description: "16-pad grid + 4 controls: tempo, swing, volume, pattern" },
      { label: "Simple synth", description: "6 oscillator controls: waveform, frequency, attack, decay, sustain, release" }
    ]
  }]
})
</tool_format>

<routing_logic>
IF user selects 0 options THEN skip test preview generation, proceed directly to Phase 5 (aesthetic.md only)
ELSE IF user selects 1-4 options THEN proceed to Phase 5 with selected preview types
</routing_logic>

<state_requirement>
Store selected preview types in context for Phase 5 generation.
</state_requirement>
</phase>

---

<phase id="5" name="generate_files">
<critical_sequence enforcement="strict" allow_reordering="false">
MUST execute steps in this exact order (no parallelization):
1. Read template
2. Generate prose (requires template)
3. Write aesthetic.md (requires prose)
4. Generate metadata.json (requires aesthetic ID)
5. Generate test previews (requires aesthetic.md completion)
6. Update manifest (requires metadata.json)
7. Commit changes to git (requires all files created)
8. Present confirmation (requires git commit complete)
</critical_sequence>

<step id="1">
Read template: /Users/lexchristopherson/Developer/plugin-freedom-system/.claude/skills/ui-template-library/assets/aesthetic-template.md
</step>

<step id="2">
<prose_generation_requirements>
Transform accumulated context into interpretable prose, NOT rigid specifications.

MUST follow these principles:
- Be descriptive: "Generous spacing with 32px gaps" NOT "spacing: 32px"
- Capture intent: "Warm brass controls evoking vintage hardware" NOT "color: #d4a574"
- Explain WHY: Describe reasoning behind design choices
- Include examples: Balance prose with concrete values (colors, sizes)
- Follow template structure: All 12 sections, same order, every time

See ui-template-library/assets/aesthetic-template.md for exact section structure.
</prose_generation_requirements>
</step>

<step id="3">
Write aesthetic.md to: .claude/aesthetics/[aesthetic-id]/aesthetic.md
</step>

<step id="4">
Generate metadata.json with: id, name, description, created timestamp, tags (inferred from context), isTemplateOnly: true, testPreviews array
</step>

<step id="5">
<conditional_execution>
IF test previews selected in Phase 4 THEN:
  FOR EACH selected plugin type:
    - Read spec from assets/test-plugin-specs.json
    - Invoke ui-template-library skill "apply" operation
    - Save HTML to .claude/aesthetics/[id]/test-previews/[plugin-type].html
  END FOR
END IF
</conditional_execution>
See references/test-preview-protocol.md for detailed invocation protocol.
</step>

<step id="6">
Read .claude/aesthetics/manifest.json, append new aesthetic entry, write updated manifest.
</step>

<step id="7">
<instructions>
Commit aesthetic to git using conventional format.
</instructions>

<commit_format>
**Stage all aesthetic files:**

```bash
git add .claude/aesthetics/[aesthetic-id]/
```

**Commit with conventional format:**

```bash
git commit -m "feat(aesthetics): [aesthetic-name] - new aesthetic template

Created aesthetic with [N] test previews ([preview-types])

🤖 Generated with [Claude Code](https://claude.com/claude-code)

Co-Authored-By: Claude <noreply@anthropic.com>"
```

**Example:**

```bash
git add .claude/aesthetics/modern-professional-001/
git commit -m "feat(aesthetics): Modern Professional - new aesthetic template

Created aesthetic with 2 test previews (simple-compressor, complex-reverb)

🤖 Generated with [Claude Code](https://claude.com/claude-code)

Co-Authored-By: Claude <noreply@anthropic.com>"
```

**If no test previews selected:**

```bash
git commit -m "feat(aesthetics): [aesthetic-name] - new aesthetic template

🤖 Generated with [Claude Code](https://claude.com/claude-code)

Co-Authored-By: Claude <noreply@anthropic.com>"
```
</commit_format>
</step>

<step id="8">
Present confirmation with file paths and preview open commands.
</step>
</phase>

---

<phase id="6" name="decision_menu">
<checkpoint_protocol>
This is a system checkpoint. MUST follow Plugin Freedom System checkpoint protocol.
MUST use inline numbered list format (NOT AskUserQuestion tool) per CLAUDE.md checkpoint protocol.
</checkpoint_protocol>

<decision_menu format="inline_numbered_list">
✅ Aesthetic "[Name]" ready!

What's next?
1. Open test previews - View mockups in browser
2. Refine aesthetic - Adjust and regenerate aesthetic.md
3. Create another aesthetic - Start new aesthetic
4. Apply to existing plugin - Choose plugin from PLUGINS.md
5. Done

Choose (1-5): _
</decision_menu>

<routing_logic>
Option 1: Run `open` commands for test preview HTML files, return to menu
Option 2: Ask what to change, re-run question batches (Phases 2-3-3.5), regenerate aesthetic.md (preserve ID)
Option 3: Return to Phase 1 with fresh context
Option 4: Read PLUGINS.md, user selects plugin, invoke ui-mockup skill with aesthetic pre-selected
Option 5: Exit skill
</routing_logic>

<note>
Phase 3.5 and 3.7 use AskUserQuestion because they are INTERNAL decision gates (workflow branching).
Phase 6 uses inline numbered list because it is a SYSTEM CHECKPOINT (user discovers next actions).
</note>

<state_requirement>
MUST wait for user response. NEVER auto-proceed to any option.
</state_requirement>
</phase>

---

<adaptive_questioning_strategy>
<critical_sequence>
This is the core algorithm for gap-driven questioning:

1. Extract concepts from user input (Phase 1 or additional context)
2. Check coverage against tier system (Tier 1 → Tier 2 → Tier 3)
3. Identify gaps (missing concepts by tier priority)
4. Generate 4 questions targeting highest-priority gaps (Phase 3)
5. Present decision gate (Phase 3.5)
6. IF continue THEN repeat steps 2-5 with accumulated context
7. IF finalize THEN proceed to Phase 3.7

NEVER ask about concepts already provided.
NEVER repeat questions in different words.
ALWAYS extract maximum information from each user response.
</critical_sequence>

<example>
User says: "I want a sleek, dark interface with neon blue accents and flat modern controls."

**Extraction:**
- Vibe: sleek, dark, modern
- Color: neon blue accents
- Controls: flat

**Gaps identified (by tier):**
- Typography choices? (Tier 2)
- Spacing philosophy? (Tier 2)
- Surface textures? (Tier 2)
- Best plugin types? (Tier 3)

**Generate 4 questions targeting these gaps.**
</example>

See references/aesthetic-questions.md for detailed gap analysis strategies.
</adaptive_questioning_strategy>

---

## Question Generation Examples

### Example 1: Detailed Input

```
User: "I want a vintage analog aesthetic with brass knobs, warm earth tones, and a subtle paper texture. Should feel like 1960s hardware."

Extracted:
- Vibe: Vintage analog, 1960s hardware ✓
- Color philosophy: Warm earth tones ✓
- Control style: Brass knobs ✓
- Surface: Paper texture ✓
- Typography: Not mentioned ✗
- Specific colors: Earth tones (generic) ✗
- Spacing: Not mentioned ✗
- Best for: Not mentioned ✗

Gaps identified (4 needed):
- Specific earth tone colors? (Tier 2)
- Typography approach? (Tier 2)
- Spacing density? (Tier 2)
- Best suited for what plugins? (Tier 3)

Question Batch 1:
1. "What specific earth tone palette?" → [Orange/brown/cream, Brown/tan/beige, Green/olive/khaki, Other]
2. "Typography approach?" → [Technical sans-serif, Classic serif, Vintage typewriter, Other]
3. "Spacing density?" → [Tight vintage, Comfortable balanced, Generous spacious, Other]
4. "Best suited for what plugins?" → [Tape/analog effects, Vintage compressors, All warm effects, Other]
```

### Example 2: Vague Input

```
User: "A clean modern aesthetic"

Extracted:
- Vibe: Clean, modern ✓
- Everything else: Not mentioned ✗

Gaps identified (4 needed):
- Color philosophy? (Tier 1)
- Control style? (Tier 1)
- Specific colors? (Tier 2)
- Typography? (Tier 2)

Question Batch 1:
1. "Color philosophy?" → [Dark with bright accents, Light with dark accents, Monochromatic, Other]
2. "Control styling?" → [Flat 2D, Subtle 3D, Skeuomorphic, Other]
3. "Primary color?" → [Blue professional, Gray neutral, Green organic, Other]
4. "Typography approach?" → [Technical/readable, Decorative/bold, Minimal/light, Other]

[Then decision gate]

If user chooses "Ask me 4 more questions":
- User answered: "Dark with bright accents", "Flat 2D", "Blue professional", "Technical/readable"

Updated context:
- Vibe: Clean, modern ✓
- Color: Dark with blue accents ✓
- Controls: Flat 2D ✓
- Typography: Technical/readable ✓

New gaps for Batch 2:
- Specific blue shade? (Tier 2)
- Spacing preference? (Tier 2)
- Surface treatment? (Tier 2)
- Best suited for? (Tier 3)
```

---

<handoff_protocol>
<invoked_by>
- /dream command (option: "Create aesthetic template")
- Natural language: "Create aesthetic template", "Design visual system"
- Direct skill invocation: Skill("aesthetic-dreaming")
</invoked_by>

<invokes>
<skill name="ui-template-library">
  <operation>apply</operation>
  <context>Phase 5, Step 5: Test preview generation</context>

  <preconditions>
    MUST verify before invocation:
    - aesthetic.md exists at .claude/aesthetics/[id]/aesthetic.md
    - Test plugin spec exists in assets/test-plugin-specs.json
    - Aesthetic ID is valid and unique
    - Output directory .claude/aesthetics/[id]/test-previews/ exists
  </preconditions>

  <invocation_parameters>
    - aesthetic_id: string (e.g., "modern-professional-001")
    - target_plugin_name: string (e.g., "TestCompressor")
    - parameter_spec: object (from test-plugin-specs.json)
    - output_dir: string (.claude/aesthetics/[id]/test-previews/)
    - filename: string (e.g., "simple-compressor.html")
  </invocation_parameters>

  <postconditions>
    Verify after invocation:
    - HTML file created at specified path
    - File is valid standalone HTML (can be opened in browser)
    - Success status returned from skill
  </postconditions>

  <error_handling>
    IF skill invocation fails:
    - Log error message
    - Continue with remaining previews (don't abort entire Phase 5)
    - Report failure in Phase 5 confirmation message
  </error_handling>
</skill>

<skill name="ui-mockup">
  <operation>create_mockup_with_aesthetic</operation>
  <context>Phase 6, Option 4: Apply to existing plugin</context>

  <preconditions>
    MUST verify before invocation:
    - Plugin exists in PLUGINS.md
    - Plugin has parameter-spec.md
    - Aesthetic exists in manifest.json
  </preconditions>

  <handoff_data>
    Pass to ui-mockup:
    - aesthetic_id: string (pre-select in Phase 0)
    - plugin_name: string (user selected from PLUGINS.md)
  </handoff_data>

  <postconditions>
    ui-mockup skill takes over:
    - Loads aesthetic.md for interpretation
    - Generates mockup with aesthetic applied
    - aesthetic-dreaming skill exits after handoff
  </postconditions>
</skill>
</invokes>

<integration_notes>
When aesthetic-dreaming creates an aesthetic, it becomes immediately available in ui-mockup Phase 0 aesthetic selection menu.
</integration_notes>
</handoff_protocol>

---

<error_handling>
See references/error-handling.md for complete edge case handling and recovery strategies.
</error_handling>

---

## Success Criteria

Skill is successful when:
- Aesthetic concept captured through adaptive questions
- No redundant questions asked (respects already-provided info)
- aesthetic.md generated with all 12 sections filled
- Test previews generated for selected plugin types (if any)
- Files stored in correct locations (.claude/aesthetics/[id]/)
- Aesthetic immediately available in ui-mockup skill
- User feels creative flow, not interrogated
- Clear next steps presented

---

## Implementation Notes

### Why This Approach Works

**1. Adaptive questioning matches plugin-ideation pattern:**
- Free-form collection first (capture everything)
- Gap analysis identifies missing pieces
- Questions generated contextually, not from rigid script
- Decision gate allows iteration or finalization

**2. Visual design tiers make sense:**
- Tier 1 (Critical): Vibe, color philosophy, control style
- Tier 2 (Visual Core): Specific colors, typography, spacing, textures
- Tier 3 (Context): Best suited for, inspirations, special features

**3. Test previews solve the visualization problem:**
- User can see aesthetic applied to real layouts
- Multiple parameter counts demonstrate flexibility
- Optional (user can skip if they just want prose description)

**4. Integration with ui-template-library:**
- aesthetic-dreaming creates the aesthetic.md
- ui-template-library interprets and applies it
- Separation of concerns: creation vs application

### Format Consistency

**aesthetic.md structure:**
- Exactly 12 sections in fixed order
- Prose descriptions with concrete examples
- Interpretable by both humans and Claude
- Human-editable, machine-parseable

**Test previews:**
- Stored in test-previews/ subdirectory
- Named by plugin type: simple-compressor.html
- Standalone HTML files (open in browser)
- Demonstrate aesthetic across different layouts

### Quality Control

**Before saving aesthetic:**
- ✅ Verify all template sections are present
- ✅ Check for placeholder text not replaced
- ✅ Validate color codes are valid hex/rgb
- ✅ Ensure prose is descriptive (not just values)
- ✅ Confirm aesthetic ID is unique

**Before generating test previews:**
- ✅ Verify test plugin specs exist
- ✅ Check ui-template-library skill is available
- ✅ Ensure aesthetic.md is complete
- ✅ Validate output directory exists

---

## Reference Documentation

**See also:**
- `ui-template-library` skill - Apply and save operations
- `ui-mockup` skill - Integration with Phase 0 aesthetic selection
- `plugin-ideation` skill - Adaptive questioning pattern reference
- `assets/test-plugin-specs.json` - Test plugin parameter definitions
- `references/aesthetic-questions.md` - Question bank and strategies
- `references/question-examples.md` - AskUserQuestion formatting examples
- `references/test-preview-protocol.md` - Test preview generation protocol
- `references/error-handling.md` - Error handling and edge cases
