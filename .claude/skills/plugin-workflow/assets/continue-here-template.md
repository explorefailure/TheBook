---
plugin: PluginName
stage: N
phase: M
status: in_progress | complete
last_updated: YYYY-MM-DD HH:MM:SS
complexity_score: X.Y
phased_implementation: true | false
---

# Resume Point

## Current State: Stage N(.M) - Description

[Current stage/phase description and what's being worked on]

## Completed So Far

**Stage 0:** ✓ Complete
- Research completed → research.md created
- JUCE modules identified
- Professional examples researched
- Technical feasibility confirmed

**Stage 1:** ✓ Complete
- Plan created → plan.md created
- Complexity score: X.Y
- Implementation strategy: [Single-pass | Phased]
- Contracts verified: parameter-spec.md, architecture.md

[Additional completed stages...]

**Current Stage:** 🚧 In Progress
- [What's been done so far in this stage]
- [Next immediate action]

## Next Steps

1. [First action to take]
2. [Second action to take]
3. [Third action to take]

## Context to Preserve

**Key Decisions:**
- [Important decision 1]
- [Important decision 2]
- [Important decision 3]

**Files Modified:**
- [File path] (description of changes)
- [File path] (description of changes)

**Current Build Status:**
- Last build: [Successful | Failed]
- Last test: [Passed | Failed | Not run]
- Issues: [Any known issues]

**Research References:**
- research.md lines [X-Y] ([what they cover])
- plan.md [specific sections]
- [Any other relevant contract references]

## How to Resume

**In new chat session:**
1. User: `/continue [PluginName]`
2. context-resume skill loads this file
3. System presents current state and next steps
4. User confirms continuation
5. plugin-workflow resumes at documented stage/phase

**After manual fixes:**
1. User makes code changes manually
2. User: "resume automation" or "continue workflow"
3. System reads this file
4. System picks up from "Next Steps"
5. Workflow continues
