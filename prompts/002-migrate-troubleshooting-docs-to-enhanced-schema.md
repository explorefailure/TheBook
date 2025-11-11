<objective>
Migrate all existing troubleshooting documentation from docs/troubleshooting/ to the new category-based system with enhanced schema validation at troubleshooting/. This ensures all documentation uses enum-validated YAML frontmatter and is correctly categorized for searchability.
</objective>

<context>
We recently upgraded the troubleshooting-docs system from a simplified schema to an enhanced schema with enum validation. The new system uses:

- Category-based filing (not dual-index with symlinks)
- Enhanced YAML frontmatter with validated enums (problem_type, component, root_cause, resolution_type, severity)
- Direct filing in troubleshooting/[category]/ based on problem_type enum

Files to migrate:
- docs/troubleshooting/rotating-lighting-on-knobs.md (needs schema upgrade)
- docs/troubleshooting/to port/*.md (10 files - already have enhanced schema but need fixing)
- troubleshooting/validation-problems/*.md (10 files - already imported but have YAML formatting issues)

Schema reference: @.claude/skills/troubleshooting-docs/schema.yaml
Template: @.claude/skills/troubleshooting-docs/assets/resolution-template.md
</context>

<requirements>
1. Read each file in docs/troubleshooting/ (including to port/ subdirectory)
2. For each file, validate and upgrade YAML frontmatter to match enhanced schema
3. Determine correct category based on problem_type enum
4. Fix any YAML formatting issues (plugin/date fields on correct lines)
5. Move file to troubleshooting/[category]/ with proper naming
6. After all migrations complete, remove docs/troubleshooting/ directory entirely
7. Ensure all files validate against schema.yaml
</requirements>

<implementation>
For each file:

**Step 1: Read and analyze**
- Read the file content
- Extract existing YAML frontmatter (if any)
- Identify the problem type, component, symptoms, root cause

**Step 2: Upgrade YAML frontmatter**

Required fields (must match schema enums):
- plugin: [PluginName] or "JUCE" for generic issues
- date: [YYYY-MM-DD] - use 2025-11-10 if not present
- problem_type: [build_error|runtime_error|api_misuse|validation_failure|ui_layout|dsp_issue|state_management|performance|thread_violation]
- component: [cmake|juce_processor|juce_editor|juce_dsp|juce_gui_basics|juce_audio_utils|apvts|pluginval|xcode|system|webview]
- symptoms: [array of 1-5 specific observable symptoms]
- root_cause: [missing_framework|missing_module|wrong_api|thread_violation|missing_constraint|state_sync|memory_issue|config_error|version_incompatibility|logic_error|event_timing|url_protocol]
- resolution_type: [code_fix|config_change|environment_setup|api_migration]
- severity: [critical|moderate|minor]

Optional fields:
- juce_version: [X.Y.Z]
- tags: [array of lowercase, hyphen-separated keywords]

**Step 3: Map to category**

Based on validated problem_type:
- build_error → troubleshooting/build-failures/
- runtime_error → troubleshooting/runtime-issues/
- ui_layout → troubleshooting/gui-issues/
- api_misuse → troubleshooting/api-usage/
- dsp_issue → troubleshooting/dsp-issues/
- state_management → troubleshooting/parameter-issues/
- performance → troubleshooting/runtime-issues/
- thread_violation → troubleshooting/runtime-issues/
- validation_failure → troubleshooting/validation-problems/

**Step 4: Generate filename**

Format: [sanitized-symptom]-[plugin]-[YYYYMMDD].md

Example: missing-juce-dsp-module-GainKnob-20251110.md

**Step 5: Write to new location**

Write the file with corrected YAML frontmatter to troubleshooting/[category]/[filename].md

**Step 6: Handle duplicates**

If a file already exists in troubleshooting/validation-problems/ with same content (from previous import), remove the duplicate and use the new correctly-formatted version.
</implementation>

<validation>
Before declaring complete, verify:

1. **Schema compliance**: All YAML frontmatter validates against schema.yaml
   - All enum fields match allowed values
   - All required fields present
   - symptoms is array format
   - date in YYYY-MM-DD format

2. **Categorization**: Each file is in correct category based on problem_type

3. **No duplicates**: No duplicate files in troubleshooting/ directories

4. **Original removed**: docs/troubleshooting/ directory completely removed

5. **File count**: Account for all files (1 rotating-lighting-on-knobs.md + 10 in to port/)
</validation>

<output>
For each migrated file:
- Read from: docs/troubleshooting/[path]
- Write to: troubleshooting/[category]/[filename].md

After all migrations:
- Remove: docs/troubleshooting/ (entire directory)
- Clean up: troubleshooting/validation-problems/ (remove any duplicates)

Present summary:
```
✓ Migrated [N] troubleshooting docs to enhanced schema

Category breakdown:
- build-failures/: [count] files
- runtime-issues/: [count] files
- gui-issues/: [count] files
- api-usage/: [count] files
- dsp-issues/: [count] files
- parameter-issues/: [count] files
- validation-problems/: [count] files

All files validated against schema.yaml
docs/troubleshooting/ removed
```
</output>

<success_criteria>
- All 11 files from docs/troubleshooting/ migrated to category-based structure
- All YAML frontmatter validates against enhanced schema
- All files in correct categories based on problem_type enum
- No formatting issues (plugin/date on correct lines)
- docs/troubleshooting/ directory removed
- No duplicate files in troubleshooting/
</success_criteria>
