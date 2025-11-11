<objective>
Upgrade the troubleshooting-docs skill to use the superior schema from the old system at /Users/lexchristopherson/Developer/juce-plugins-refactor/.claude/skills/troubleshooting-docs/, which provides enum-validated fields and richer metadata. This enables better search, filtering, pattern analysis, and prevents invalid data entry.
</objective>

<context>
We have two implementations of troubleshooting-docs:
1. Current (simplified): plugin, date, symptom (string), severity, tags
2. Old (superior): problem_type, component, symptoms (array), root_cause, resolution_type, plus severity and tags

The old system is better because:

- Enum validation prevents garbage data
- Structured symptoms capture multiple behaviors
- Enables filtering by component, root_cause, problem_type
- Pattern analysis across issues
- Schema file validates all entries

We also have 10 existing docs in `docs/troubleshooting/to port/` already using the old format that need importing.
</context>

<requirements>
1. Create schema.yaml with enum definitions from the old system
2. Update troubleshooting-docs SKILL.md to use the enhanced schema
3. Add `plugin` and `date` fields to the old schema (best of both)
4. Update resolution-template.md with new format
5. Import the 10 existing docs with proper categorization
6. Ensure backward compatibility with deep-research skill integration
</requirements>

<implementation>
Step 1: Create schema file
- Extract enums from existing docs and old SKILL.md
- Save to `.claude/skills/troubleshooting-docs/schema.yaml`
- Include validation rules for each field

Step 2: Update SKILL.md

- Change from simplified to enhanced schema throughout
- Update Step 5 (YAML validation) to reference schema.yaml
- Update Step 6 (documentation creation) with new template
- Update all examples to use new format
- Keep the simplified directory structure (by category, not dual-index)

Step 3: Enhanced schema format

```yaml
plugin: [PluginName] # NEW from current system
date: [YYYY-MM-DD] # NEW from current system
problem_type: [enum] # OLD validated enum
component: [enum] # OLD validated enum
symptoms: # OLD array format
  - [symptom 1]
  - [symptom 2]
root_cause: [enum] # OLD validated enum
resolution_type: [enum] # OLD validated enum
severity: [enum] # BOTH systems
tags: [array] # BOTH systems
```

Step 4: Import existing docs

- Read each file in `docs/troubleshooting/to port/`
- Add `plugin` field (extract from content or use "JUCE" for generic)
- Add `date` field (use 2025-11-10)
- Map problem_type to category directory
- Generate new filename: [simplified-symptom]-[Plugin]-20251110.md
- Copy to appropriate troubleshooting/[category]/ directory

Category mapping:

- build_error → build-failures/
- runtime_error → runtime-issues/
- ui_layout → gui-issues/
- api_misuse → api-usage/
- dsp_issue → dsp-issues/
- state_management → parameter-issues/
- performance → runtime-issues/
- thread_violation → runtime-issues/
  </implementation>

<verification>
After implementation, verify:
1. schema.yaml exists with all enum definitions
2. SKILL.md references schema.yaml for validation
3. resolution-template.md uses enhanced format
4. All 10 docs imported to correct categories
5. Test creating a new doc with validation
6. Verify deep-research can still search docs
7. Check grep/find commands still work with new structure
</verification>

<success_criteria>

- All fields validated against enums (no arbitrary strings)
- Symptoms remain as array (multiple observable behaviors)
- Can filter docs by: plugin, component, problem_type, root_cause
- Existing 10 docs successfully imported
- New docs created with richer metadata
- Pattern analysis possible across issues
  </success_criteria>
