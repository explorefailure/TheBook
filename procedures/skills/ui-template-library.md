# ui-template-library

**Purpose:** Save and reuse visual aesthetics and design language across plugins, creating a cohesive plugin family without rigid layout constraints.

---

## Core Concept

Templates are **aesthetic systems**, not rigid layouts. When you save a design you like, the system extracts the **visual language** (colors, styling, spacing philosophy, vibe) - not the specific control positions or layout grid.

**What gets saved:**
- Color palette and schemes
- Control styling (knob design, slider appearance, button style)
- Typography choices (fonts, sizes, weights)
- Visual effects (shadows, glows, textures, gradients)
- Spacing philosophy (tight/airy/generous)
- Overall aesthetic vibe (vintage/modern/minimal/aggressive/industrial)

**What does NOT get saved:**
- Number of controls
- Control positions or layout grid
- Specific parameter mappings
- Window dimensions (suggested, not enforced)

## How It Works

**Saving from 3-knob compressor:**
```
Aesthetic captured:
- Warm vintage vibe
- Color palette: Orange (#d97742), cream (#f5e6d3), dark brown (#2b2015)
- Large rotary knobs (70px) with tick marks and center dots
- Subtle paper texture background
- Generous 32px spacing between elements
- Drop shadows for depth (0 4px 12px rgba(0,0,0,0.3))
- Rounded corners (8px border-radius)
```

**Applying to 8-parameter reverb:**
```
Claude generates:
- NEW layout appropriate for 8 parameters (maybe 2x4 grid + wet/dry)
- SAME color palette (orange/cream/brown)
- SAME knob styling (70px, tick marks, center dots)
- SAME texture background
- SAME 32px spacing philosophy
- SAME shadows and rounded corners

Result: Completely different layout, unified visual language
```

## What It Does

Manages a personal library of aesthetic systems that can be applied to new plugins with any number of parameters. The system interprets the saved aesthetic and generates an appropriate layout for the target plugin.

## When Invoked

**Automatically offered after mockup creation:**
```
✓ Mockup v2 created

What's next?
1. Iterate on this design
2. Finalize and proceed
3. Save aesthetic as template ← Extracts visual language
4. Start fresh
5. Other
```

**Automatically offered before mockup creation:**
```
Ready to design ReverbPlugin UI.

How would you like to start?
1. Brainstorm together
2. Apply an aesthetic (3 available) ← Saved design languages
3. Upload a sketch
4. Describe it directly
5. Other
```

**Manual invocation:**
- "Save this aesthetic as a template"
- "Apply [aesthetic name] to [Plugin]"
- "Show me my aesthetics"
- "Delete aesthetic [name]"

## Template Storage

**Location:** `.claude/aesthetics/`

**Structure:**
```
.claude/aesthetics/
├── manifest.json                   # Aesthetic registry
└── [aesthetic-id]/
    ├── aesthetic.yaml              # Visual system definition
    ├── preview.html                # Visual sample (reference only)
    └── metadata.json               # Aesthetic info
```

### manifest.json Format

```json
{
  "aesthetics": [
    {
      "id": "warm-vintage-tape",
      "name": "Warm Vintage Tape",
      "created": "2025-11-10T14:23:00Z",
      "source_plugin": "TapeDelay",
      "source_mockup": "v2",
      "vibe": "vintage",
      "primary_color": "#d97742",
      "tags": ["vintage", "warm", "analog", "textured"]
    }
  ]
}
```

### metadata.json Format

```json
{
  "aesthetic_id": "warm-vintage-tape",
  "name": "Warm Vintage Tape",
  "description": "Warm analog aesthetic with orange accents, paper texture, generous spacing. Feels like vintage tape hardware.",
  "created": "2025-11-10T14:23:00Z",
  "source_plugin": "TapeDelay",
  "source_mockup_version": "v2",
  "vibe": "vintage",
  "mood": ["warm", "nostalgic", "organic", "tactile"],
  "best_for": "Effects processors, time-based effects, character processors"
}
```

### aesthetic.yaml Format

Captures the visual system, not layout:

```yaml
aesthetic_name: "Warm Vintage Tape"
vibe: vintage

# Color System
colors:
  background: "#2b2015"      # Dark warm brown
  surface: "#3a2a1f"         # Slightly lighter brown
  primary: "#d97742"         # Warm orange
  secondary: "#f5e6d3"       # Cream/beige
  accent: "#c95d32"          # Darker orange
  text_primary: "#f5e6d3"    # Cream
  text_secondary: "#a08060"  # Muted tan

# Typography
typography:
  font_family: "'Courier New', monospace"
  font_sizes:
    large: "24px"
    medium: "16px"
    small: "12px"
  font_weights:
    bold: 700
    normal: 400

# Control Styling
controls:
  rotary_knob:
    diameter: 70px
    style: "tick-marks"
    tick_count: 21
    center_dot: true
    border: "2px solid #d97742"
    background: "#3a2a1f"
    shadow: "0 4px 12px rgba(0, 0, 0, 0.3)"

  vertical_slider:
    width: 40px
    height: 180px
    track_color: "#3a2a1f"
    thumb_color: "#d97742"
    border: "1px solid #4a3a2f"

  button:
    padding: "8px 16px"
    background: "#d97742"
    text_color: "#2b2015"
    border_radius: 4px
    hover_brightness: 1.1

# Layout Philosophy
spacing:
  philosophy: "generous"
  between_controls: 32px
  section_padding: 24px
  edge_margin: 20px
  control_label_gap: 8px

# Visual Effects
effects:
  background_texture: "subtle-paper"
  shadow_depth: "medium"
  border_radius: 8px
  glow_on_active: false

# Suggested Window (not enforced)
suggested_window:
  min_width: 350
  max_width: 800
  aspect_ratio_preference: "wider"  # or "taller" or "square"
```

## Saving Aesthetics

### Process

1. **User selects "Save aesthetic as template" after mockup creation**

2. **System extracts visual language:**
   - Analyzes color palette and relationships
   - Captures control styling patterns
   - Identifies typography system
   - Detects spacing patterns
   - Determines visual effects (shadows, textures, etc.)
   - Infers aesthetic vibe (vintage/modern/minimal/etc.)

3. **System asks for aesthetic name:**
   ```
   What should we call this aesthetic?

   Suggested: "Warm Vintage Tape" (based on colors and vibe)

   Enter name: _
   ```

4. **System asks for description:**
   ```
   Describe this aesthetic (helps you remember when to use it):

   Example: "Warm analog aesthetic with orange accents, paper texture, generous spacing. Feels like vintage tape hardware."

   Enter description: _
   ```

5. **System generates aesthetic.yaml:**
   - Extracts all color values
   - Captures control styling rules
   - Documents spacing philosophy
   - Identifies visual effects
   - Suggests window sizing (not enforced)

6. **System creates preview.html:**
   - Shows color palette swatches
   - Displays sample controls styled correctly
   - Demonstrates spacing and effects
   - Visual reference only (not used for generation)

7. **System saves to `.claude/aesthetics/[aesthetic-id]/`**

8. **System updates manifest.json**

9. **Confirmation:**
   ```
   ✓ Aesthetic saved: "Warm Vintage Tape"

   This aesthetic can be applied to any plugin regardless of parameter count.
   Preview: .claude/aesthetics/warm-vintage-tape/preview.html
   ```

### What Gets Extracted

**Colors:**
- All unique color values
- Color relationships (background → surface → accent)
- Text colors for different contexts

**Control Styling:**
- For each control type used (knob, slider, button, toggle)
- Size, shape, borders, shadows
- Hover/active states
- Label positioning

**Typography:**
- Font families
- Size scale
- Weight usage
- Text colors

**Spacing:**
- Gaps between controls
- Section padding
- Edge margins
- Philosophy (tight/normal/generous)

**Effects:**
- Background textures or patterns
- Shadow styles and depths
- Border radius values
- Glow/highlight effects
- Transition timings

**Vibe:**
- Inferred mood (vintage/modern/industrial/minimal/etc.)
- Best use cases (based on source plugin type)

## Applying Aesthetics

### Process

1. **User selects "Apply an aesthetic" when starting mockup**

2. **System shows inline numbered list:**
   ```
   Available aesthetics:

   1. Warm Vintage Tape
      Vibe: Vintage analog
      Colors: Orange/cream/brown with paper texture
      Best for: Effects processors, time-based effects
      From: TapeDelay mockup v2

   2. Modern Minimal Blue
      Vibe: Clean, modern
      Colors: Blue/gray/white with subtle shadows
      Best for: Utility plugins, EQs, analyzers
      From: EQ4Band mockup v1

   3. Aggressive Industrial
      Vibe: Dark, edgy
      Colors: Red/black/steel with metal texture
      Best for: Distortion, compression, aggressive effects
      From: HardClipper mockup v3

   4. None (start from scratch)

   Choose aesthetic: _
   ```

3. **User selects aesthetic**

4. **System analyzes target plugin:**
   ```
   ReverbPlugin has 8 parameters:
   - PRE_DELAY (rotary)
   - ROOM_SIZE (rotary)
   - DAMPING (rotary)
   - DIFFUSION (rotary)
   - LOW_CUT (rotary)
   - HIGH_CUT (rotary)
   - MIX (vertical slider, prominent)
   - OUTPUT (vertical slider)

   Applying "Warm Vintage Tape" aesthetic...
   ```

5. **System generates appropriate layout:**
   - Determines best layout for 8 controls (e.g., 3x2 grid + 2 sliders)
   - Applies color palette to all elements
   - Styles all controls using aesthetic.yaml rules
   - Uses spacing philosophy for gaps
   - Applies visual effects (texture, shadows, etc.)
   - Chooses window size based on control count + suggested dimensions

6. **System generates all mockup files:**
   ```
   ✓ Mockup v1 created with "Warm Vintage Tape" aesthetic

   Files created:
   - plugins/ReverbPlugin/.ideas/mockups/v1-ui.yaml
   - plugins/ReverbPlugin/.ideas/mockups/v1-ui.html
   - [5 more files...]

   Layout generated: 3x2 grid (6 knobs) + 2 vertical sliders
   Color palette: Orange/cream/brown (from aesthetic)
   Styling: Tick-mark knobs, paper texture, generous spacing

   Next: Open v1-ui-test.html in browser to preview
   ```

### Layout Generation Logic

**Based on parameter count and types:**

**1-3 parameters:**
- Horizontal row of large controls
- Window: ~400x300px

**4-6 parameters:**
- 2x2 or 3x2 grid
- Window: ~500x400px

**7-10 parameters:**
- 2x4 or 3x3 grid, or sections
- Window: ~600x500px

**11+ parameters:**
- Multi-section layout with grouped controls
- Window: ~800x600px

**Special handling:**
- Prominent controls (mix, output) get vertical sliders
- Groups of related parameters get visual sections
- Less important controls can be smaller

**All styled with the chosen aesthetic system.**

## Aesthetic Adaptation Examples

### 3-Knob Compressor → 8-Parameter Reverb

**Saved from Compressor:**
```yaml
colors:
  primary: "#d97742"  # Orange
  background: "#2b2015"
controls:
  rotary_knob:
    diameter: 70px
    tick_marks: true
spacing:
  between_controls: 32px
```

**Applied to Reverb:**
```yaml
# Generated layout: 3x2 grid + 2 sliders
plugin_name: ReverbPlugin
window: {width: 650, height: 450}

colors:
  primary: "#d97742"  # Same orange
  background: "#2b2015"  # Same background

sections:
  main_parameters:
    layout: grid
    columns: 3
    rows: 2
    gap: 32px  # Same spacing philosophy
    components:
      - type: rotary_knob
        id: PRE_DELAY
        diameter: 70px  # Same size
        tick_marks: true  # Same styling
      # ... 5 more knobs, all styled identically

  output_section:
    layout: horizontal
    components:
      - type: vertical_slider
        id: MIX
        height: 180px
        # Styled with same colors and effects
      - type: vertical_slider
        id: OUTPUT
```

**Result:** Completely different layout, unified visual language.

### Aesthetic Across Plugin Family

**User builds 5 plugins over time:**
1. TapeDelay (3 knobs) - Creates "Warm Vintage Tape" aesthetic
2. ReverbPlugin (8 params) - Applies aesthetic → Different layout, same vibe
3. ChorusEffect (5 knobs) - Applies aesthetic → Different layout, same vibe
4. CompressorPlus (6 params + 2 buttons) - Applies aesthetic → Different layout, same vibe
5. FilterBank (12 params) - Applies aesthetic → Multi-section, same vibe

**Result:** 5 plugins with wildly different layouts, but visually cohesive as a family.

## Template Management

### List All Aesthetics

**Command:** "Show me my aesthetics"

**Output:**
```
UI Aesthetic Library (3 aesthetics):

1. Warm Vintage Tape
   Vibe: Vintage analog
   Colors: Orange (#d97742), cream, dark brown
   Style: Tick-mark knobs, paper texture, generous spacing
   Created: 2025-11-05 from TapeDelay
   Used in: TapeDelay, ReverbPlugin, ChorusEffect

2. Modern Minimal Blue
   Vibe: Clean, modern
   Colors: Blue (#4a9eff), gray, white
   Style: Flat design, subtle shadows, tight spacing
   Created: 2025-11-07 from EQ4Band
   Used in: EQ4Band

3. Aggressive Industrial
   Vibe: Dark, edgy
   Colors: Red (#c92020), black, steel gray
   Style: Angular shapes, metal texture, high contrast
   Created: 2025-11-09 from HardClipper
   Used in: HardClipper, DistortionFX

Preview: Open .claude/aesthetics/[aesthetic-id]/preview.html
```

### Delete Aesthetic

**Command:** "Delete aesthetic [name]"

**Process:**
1. Find aesthetic in manifest.json
2. Show plugins using it (warning if any)
3. Confirm deletion
4. Remove from manifest and filesystem

### Update Aesthetic

**Command:** "Update aesthetic [name] from [Plugin] mockup"

**Process:**
1. Extracts aesthetic from specified mockup
2. Overwrites existing aesthetic.yaml
3. Updates preview.html
4. Confirms update

**Use case:** You refined the aesthetic in a new plugin, want to save improvements.

## Integration

### Reads from:
- `.claude/aesthetics/manifest.json` - Aesthetic registry
- `.claude/aesthetics/[aesthetic-id]/aesthetic.yaml` - Visual system

### Used by:
- `ui-mockup` skill - Offers aesthetics at start, saves after creation

### Offers to call:
- `ui-mockup` skill - After applying aesthetic, offers to iterate

### Creates:
- `.claude/aesthetics/manifest.json` - First time aesthetic is saved
- `.claude/aesthetics/[aesthetic-id]/` directory per aesthetic

## Best Practices

**Name by vibe, not plugin:** "Warm Vintage" not "Tape Delay Style"

**Describe mood and use cases:** "Aggressive industrial aesthetic, best for distortion/compression"

**Refine and update:** If you improve an aesthetic, update the template

**Build a family:** Use same aesthetic across related plugins for brand consistency

**Mix and match:** Try different aesthetics on same plugin to explore options (Esc+Esc to rewind)

**Preview before applying:** Open `preview.html` to see colors and styling

## Common Workflows

### Save First Aesthetic

```
User: /mockup TapeDelay
→ Creates v2 mockup (warm orange, vintage vibe)
→ Mockup complete
User: 3 (Save aesthetic as template)
→ Names it "Warm Vintage Tape"
→ Aesthetic extracted and saved
```

### Apply to Different Plugin

```
User: /mockup ReverbPlugin (8 parameters)
System: "How would you like to start?"
User: 2 (Apply an aesthetic)
→ System shows "Warm Vintage Tape" option
User: 1
→ System generates 8-parameter layout with warm vintage aesthetic
→ Result: Different layout, same visual language
```

### Build Plugin Family

```
Session 1: TapeDelay (3 params) → Save aesthetic
Session 2: ReverbPlugin (8 params) → Apply aesthetic
Session 3: ChorusEffect (5 params) → Apply aesthetic
Session 4: FilterBank (12 params) → Apply aesthetic

Result: 4 plugins with unified visual brand, different layouts
```

### Explore Different Vibes

```
User: /mockup Compressor
System: "How would you like to start?"
User: 2 (Apply aesthetic)
User: 1 (Warm Vintage)
→ Mockup generated

User: Esc + Esc (rewind)
User: Restore code only
System: "How would you like to start?"
User: 2 (Apply aesthetic)
User: 3 (Aggressive Industrial)
→ Different aesthetic on same plugin
→ User compares, chooses preferred vibe
```

## Discovery Through Use

**First time user:**
- Creates mockup they like
- Sees "Save aesthetic as template"
- Thinks: "What's that?"
- Tries it → Learns aesthetic system exists

**Second mockup:**
- Starts new plugin (different parameter count)
- Sees "Apply an aesthetic (1 available)"
- Thinks: "Oh, that design I liked!"
- Applies it → Fast, cohesive design

**Becomes habit:**
- Builds personal aesthetic library
- Quickly applies to new plugins
- Creates visually unified plugin family
- No manual reading required

Feature discovered through contextual options, not documentation.
