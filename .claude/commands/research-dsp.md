---
description: Research a DSP topic and optionally save to patterns library
arguments:
  - name: topic
    description: DSP topic to research (e.g., "compression", "delay lines", "FFT")
    required: true
---

# DSP Research: $ARGUMENTS.topic

## Instructions

1. **Check existing patterns first**:
   ```
   ls .claude/skills/dsp-patterns/patterns/
   ```
   If a relevant pattern exists, read and present it.

2. **If no pattern exists**, research the topic:
   - Search for JUCE implementation approaches
   - Look for professional plugin examples
   - Identify common pitfalls and gotchas
   - Find recommended parameter ranges

3. **Present findings** to the user with:
   - Overview of the technique
   - JUCE modules and code patterns
   - Common mistakes to avoid
   - CPU considerations

4. **Offer to save**:
   ```
   Would you like to save this research to the DSP patterns library?
   1. Yes - save for future plugins
   2. No - just needed it for now
   ```

5. **If saving**, create a new pattern file following the template in:
   `.claude/skills/dsp-patterns/SKILL.md`

## Pattern File Location

Save new patterns to:
```
.claude/skills/dsp-patterns/patterns/[topic-name].md
```

Use kebab-case for filenames (e.g., `delay-lines.md`, `envelope-follower.md`).
