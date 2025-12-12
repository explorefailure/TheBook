# DSP Patterns Library

Reusable DSP research and implementation patterns for JUCE audio plugins.

## Purpose

Store and retrieve DSP knowledge that applies across multiple plugins. When researching a new effect type, save the findings here so future plugins can build on that work.

## Trigger Detection

Activate when user:
- Runs `/research-dsp [topic]`
- Says "save this research", "add to DSP patterns", "remember this for later"
- Asks about DSP topics during `/dream` or `/plan`
- Says "remove pattern", "delete pattern" (for cleanup)

## Auto-Extraction Rules

**After pluginval passes**, automatically extract new DSP patterns:

1. Read the plugin's `architecture.md` for DSP concepts used
2. Read `PluginProcessor.cpp` for working implementation
3. Check existing patterns - only save NEW concepts
4. Generate pattern file with generalized, reusable code
5. Notify user: "Extracted new pattern: [name]"

**What counts as "new":**
- DSP technique not already in `.claude/skills/dsp-patterns/patterns/`
- Check by concept, not just filename (e.g., "saturation" and "soft-clipping" might overlap)

**Skip extraction if:**
- All techniques already have patterns
- Plugin is too simple (e.g., just gain staging)

## Manual Removal

User can remove any pattern by asking:
- "remove the [name] pattern"
- "delete [name] from patterns"
- "I don't want the [name] pattern"

Just delete the file from `patterns/` directory.

## Usage

### Saving Research

After completing DSP research (during `/plan` or standalone research), offer to save reusable patterns:

```
Would you like to save this research to the DSP patterns library?
1. Yes - save for future plugins
2. No - keep only in this plugin's docs
```

### Pattern File Format

Each pattern file in `patterns/` follows this structure:

```markdown
# [Pattern Name]

## Overview
Brief description of what this pattern does and when to use it.

## Use Cases
- Bullet list of plugin types that benefit from this pattern

## JUCE Implementation

### Required Modules
- juce::dsp::ModuleName

### Code Pattern
```cpp
// Key implementation code
```

### Gotchas
- Common mistakes and how to avoid them

## Parameters
Typical parameter ranges and recommendations.

## CPU Considerations
Performance notes and optimization tips.

## References
- Professional plugins that use this technique
- Academic papers or tutorials
```

### Retrieving Patterns

During `/plan` or `/implement`, the research-planning-agent and dsp-agent should check this library for relevant patterns:

```bash
ls .claude/skills/dsp-patterns/patterns/
```

Reference patterns in architecture.md when applicable.

## Pattern Categories

Organize patterns by DSP domain:

| Category | Examples |
|----------|----------|
| `distortion/` | Saturation, waveshaping, clipping, bitcrushing |
| `dynamics/` | Compression, limiting, gating, expansion |
| `filters/` | EQ, HPF/LPF, resonant filters, crossovers |
| `modulation/` | Chorus, flanger, phaser, vibrato, tremolo |
| `delay/` | Delay lines, reverb, echo, feedback |
| `analysis/` | FFT, envelope followers, meters |
| `utilities/` | Oversampling, DC blocking, gain staging, mixing |

## Commands

- `/research-dsp [topic]` - Research a DSP topic and optionally save to library
- `/list-patterns` - Show all saved patterns
- `/pattern [name]` - Read a specific pattern

## Integration with Plugin Workflow

1. **During /plan**: research-planning-agent checks patterns library before external research
2. **During /implement**: dsp-agent references saved patterns for implementation details
3. **After completion**: Offer to extract new patterns from successful implementations
