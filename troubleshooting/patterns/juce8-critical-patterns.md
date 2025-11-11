# JUCE 8.x Critical Patterns - REQUIRED READING

**Purpose:** Prevent repeat mistakes across all plugin implementations.

**When to read:** Before ANY code generation in Stages 2-5.

---

## 1. CMakeLists.txt - Header Generation (ALWAYS REQUIRED)

### ❌ WRONG (Will fail with "JuceHeader.h not found")
```cmake
target_link_libraries(MyPlugin
    PRIVATE
        juce::juce_audio_processors
)

target_compile_definitions(MyPlugin
    PUBLIC
        JUCE_VST3_CAN_REPLACE_VST2=0
)
```

### ✅ CORRECT
```cmake
target_link_libraries(MyPlugin
    PRIVATE
        juce::juce_audio_processors
)

# CRITICAL: Generate JuceHeader.h (JUCE 8 requirement)
juce_generate_juce_header(MyPlugin)

target_compile_definitions(MyPlugin
    PUBLIC
        JUCE_VST3_CAN_REPLACE_VST2=0
)
```

**Why:** JUCE 8.x does NOT ship a pre-built JuceHeader.h. The `juce_generate_juce_header()` function:
1. Scans linked JUCE modules
2. Auto-generates JuceHeader.h in build artifacts
3. Adds it to compiler include paths

**Placement:** MUST come after `target_link_libraries()`, BEFORE `target_compile_definitions()`

**Documented in:** `troubleshooting/build-failures/juceheader-not-found-cmake-JUCE-20251106.md`

---

## 2. Include Style - Prefer Module Headers

### ✅ PREFERRED (Modern JUCE 8)
```cpp
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
```

### ⚠️ ACCEPTABLE (But requires juce_generate_juce_header())
```cpp
#include <JuceHeader.h>
```

**Recommendation:** Use individual module headers. They're explicit, don't require CMake generation, and match JUCE 8 best practices.

---

## 3. WebView UI - Module Requirements

### Required CMake Configuration
```cmake
target_link_libraries(MyPlugin
    PRIVATE
        juce::juce_gui_extra  # REQUIRED for WebBrowserComponent
)

target_compile_definitions(MyPlugin
    PUBLIC
        JUCE_WEB_BROWSER=1     # Enable WebView
        JUCE_USE_CURL=0        # Disable CURL (not needed for local HTML)
)
```

### Required Includes
```cpp
#include <juce_gui_extra/juce_gui_extra.h>  // For WebBrowserComponent
```

**Documented in:** Stage 5 (GUI integration)

---

## 4. Bus Configuration - Effects vs Instruments

### Effects (Audio In → Audio Out)
```cpp
AudioProcessor(BusesProperties()
    .withInput("Input", juce::AudioChannelSet::stereo(), true)
    .withOutput("Output", juce::AudioChannelSet::stereo(), true))
```

### Instruments (MIDI In → Audio Out)
```cpp
AudioProcessor(BusesProperties()
    .withOutput("Output", juce::AudioChannelSet::stereo(), true))
```

**Common mistake:** Adding input bus to instruments causes "missing input" errors in DAWs.

**Documented in:** `troubleshooting/runtime-issues/vst3-bus-config-effect-missing-input-Shake-20251106.md`

---

## 5. Threading - UI ↔ Audio Thread

### ❌ WRONG (Thread violation - will crash)
```cpp
// In PluginEditor (UI thread)
button.onClick = [this] {
    audioProcessor.processBlock(...);  // ILLEGAL
};
```

### ✅ CORRECT
```cpp
// In PluginEditor (UI thread) - use parameters or atomic flags
button.onClick = [this] {
    audioProcessor.getAPVTS().getParameter("trigger")->setValueNotifyingHost(1.0f);
};
```

**Rule:** NEVER call audio processing code from UI thread. Use AudioProcessorValueTreeState (APVTS) for communication.

**Documented in:** `troubleshooting/build-failures/ui-trigger-button-processblock-invocation-thread-violation-JUCE-20251106.md`

---

## 6. Rotary Sliders - Bounds vs setBounds

### ❌ WRONG (Non-interactive sliders)
```cpp
slider.setBounds(x, y, w, h);  // Absolute positioning breaks mouse tracking
```

### ✅ CORRECT
```cpp
addAndMakeVisible(slider);
// In resized():
slider.setBounds(x, y, w, h);  // OK in resized()

// OR use FlexBox/Grid layout
```

**Rule:** Sliders need proper parent component hierarchy for mouse events. Absolute positioning works in `resized()` but not during construction.

**Documented in:** `troubleshooting/api-usage/rotary-slider-absolute-positioning-interaction-TapeMachine-20251107.md`

---

## 7. WebView ↔ Parameter Binding

### ❌ WRONG (Parameters not updating)
```cpp
// JavaScript sends:
{ type: "parameterChanged", id: "gain", value: 0.5 }

// C++ expects:
{ type: "parameter_change", ... }
```

### ✅ CORRECT
```cpp
// Standardized event format:
// JS → C++: { type: "parameter_change", id: "gain", value: 0.5 }
// C++ → JS: { type: "parameter_update", id: "gain", value: 0.5 }
```

**Rule:** WebView integration requires exact event type matching between JS and C++.

**Documented in:** `troubleshooting/api-usage/webview-parameter-not-updating-wrong-event-format-JUCE-20251108.md`

---

## 8. WebView Resource Provider - Explicit URL Mapping (ALWAYS REQUIRED)

### ❌ WRONG (Generic loop - breaks resource loading)
```cpp
std::optional<juce::WebBrowserComponent::Resource>
getResource(const juce::String& url)
{
    auto path = url.substring(1);  // Remove leading slash

    // Generic loop - hard to debug, easy to break
    for (int i = 0; i < BinaryData::namedResourceListSize; ++i) {
        if (path == BinaryData::namedResourceList[i]) {
            // ... conversion logic ...
        }
    }
    return std::nullopt;
}
```

**Problem:** BinaryData flattens paths (`js/juce/index.js` → `index_js`), but HTML requests `/js/juce/index.js`. Generic loop can't match.

### ✅ CORRECT (Explicit URL mapping)
```cpp
std::optional<juce::WebBrowserComponent::Resource>
getResource(const juce::String& url)
{
    auto makeVector = [](const char* data, int size) {
        return std::vector<std::byte>(
            reinterpret_cast<const std::byte*>(data),
            reinterpret_cast<const std::byte*>(data) + size
        );
    };

    // Explicit mapping - clear, debuggable, reliable
    if (url == "/" || url == "/index.html") {
        return juce::WebBrowserComponent::Resource {
            makeVector(BinaryData::index_html, BinaryData::index_htmlSize),
            juce::String("text/html")
        };
    }

    if (url == "/js/juce/index.js") {
        return juce::WebBrowserComponent::Resource {
            makeVector(BinaryData::index_js, BinaryData::index_jsSize),
            juce::String("text/javascript")
        };
    }

    return std::nullopt;  // 404
}
```

**Why:**
- BinaryData converts paths to valid C++ identifiers (`index.js` → `index_js`)
- HTML/JS use original paths (`./js/juce/index.js`)
- Explicit mapping bridges this gap clearly
- Easy to debug (can log URL mismatches)

**Placement:** PluginEditor.cpp private method

**Documented in:** `troubleshooting/gui-issues/webview-frame-load-interrupted-TapeAge-20251111.md`

---

## 9. CMakeLists.txt - NEEDS_WEB_BROWSER for VST3 (ALWAYS REQUIRED)

### ❌ WRONG (VST3 won't appear in DAW)
```cmake
juce_add_plugin(MyPlugin
    COMPANY_NAME "YourCompany"
    PLUGIN_CODE Plug
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "My Plugin"
    # Missing NEEDS_WEB_BROWSER - VST3 will be built but won't load
)
```

### ✅ CORRECT
```cmake
juce_add_plugin(MyPlugin
    COMPANY_NAME "YourCompany"
    PLUGIN_CODE Plug
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "My Plugin"
    NEEDS_WEB_BROWSER TRUE  # REQUIRED for VST3 WebView support
)
```

**Why:** VST3 format requires explicit WebView flag even if AU works without it. Missing this flag causes:
- VST3 builds successfully
- Binary exists in build artifacts
- But plugin doesn't appear in DAW VST3 scanner
- Only AU format visible

**When:** ANY plugin using WebBrowserComponent for UI

**Documented in:** `troubleshooting/gui-issues/webview-frame-load-interrupted-TapeAge-20251111.md`

---

## 10. Testing GUI Changes - Always Install to System (CRITICAL WORKFLOW)

### ❌ WRONG (Tests stale cached builds)
```bash
# Build but DON'T install
./scripts/build-and-install.sh MyPlugin --no-install

# Test in DAW → loads OLD version from system folders
```

**Result:** DAW loads stale plugins from `~/Library/Audio/Plug-Ins/`, not fresh builds.

### ✅ CORRECT
```bash
# Build AND install to system folders
./scripts/build-and-install.sh MyPlugin

# Script automatically:
# 1. Builds fresh binaries
# 2. Removes old versions from system
# 3. Installs new versions
# 4. Clears DAW caches (Ableton DB, AU cache)
# 5. Verifies installation
```

**Then restart DAW** - Required for plugin rescan after cache clear.

**Why:**
- DAWs load plugins from system folders (`~/Library/Audio/Plug-Ins/`), NOT build directories
- Using `--no-install` means testing old builds while developing new code
- All "fixes" appear broken when you're testing stale binaries

**When:** ANY time you modify:
- PluginEditor code (UI changes)
- CMakeLists.txt (configuration)
- Resource files (HTML/CSS/JS)
- Parameter bindings

**Documented in:** `troubleshooting/gui-issues/webview-frame-load-interrupted-TapeAge-20251111.md`

---

## 11. WebView Member Initialization - Use std::unique_ptr (REQUIRED)

### ❌ WRONG (Raw members - initialization order issues)
```cpp
class MyPluginEditor : public juce::AudioProcessorEditor {
private:
    juce::WebSliderRelay gainRelay;
    juce::WebBrowserComponent webView;
    juce::WebSliderParameterAttachment gainAttachment;
};
```

### ✅ CORRECT
```cpp
class MyPluginEditor : public juce::AudioProcessorEditor {
private:
    // Order: Relays → WebView → Attachments
    std::unique_ptr<juce::WebSliderRelay> gainRelay;
    std::unique_ptr<juce::WebBrowserComponent> webView;
    std::unique_ptr<juce::WebSliderParameterAttachment> gainAttachment;
};
```

**Constructor:**
```cpp
MyPluginEditor::MyPluginEditor(MyProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    // 1. Create relays FIRST
    gainRelay = std::make_unique<juce::WebSliderRelay>("GAIN");

    // 2. Create WebView with relay options
    webView = std::make_unique<juce::WebBrowserComponent>(
        juce::WebBrowserComponent::Options{}
            .withNativeIntegrationEnabled()
            .withResourceProvider([this](auto& url) { return getResource(url); })
            .withOptionsFrom(*gainRelay)
    );

    // 3. Create attachments LAST
    gainAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("GAIN"), *gainRelay
    );

    addAndMakeVisible(*webView);
    webView->goToURL(juce::WebBrowserComponent::getResourceProviderRoot());
}
```

**Why:**
- Ensures correct construction order
- Prevents release build crashes (90% of member order issues)
- Destruction order is automatic (reverse of declaration)
- Matches JUCE examples and GainKnob reference implementation

**When:** ALL WebView-based plugin editors

**Documented in:** `troubleshooting/gui-issues/webview-frame-load-interrupted-TapeAge-20251111.md`

---

## 12. WebSliderParameterAttachment - Three Parameters Required (JUCE 8)

### ❌ WRONG (Knobs frozen - no parameter updates)
```cpp
// JUCE 7 style (2 parameters) - FAILS silently in JUCE 8
driveAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
    *processorRef.parameters.getParameter("drive"), *driveRelay);
```

### ✅ CORRECT
```cpp
// JUCE 8 requires 3 parameters (added undoManager)
driveAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
    *processorRef.parameters.getParameter("drive"), *driveRelay, nullptr);
```

**Why:**
- JUCE 8 changed WebSliderParameterAttachment constructor signature
- Old signature: `(parameter, relay)`
- New signature: `(parameter, relay, undoManager)`
- Missing third parameter causes **silent failure** - compiles but doesn't bind
- UI displays correctly but knobs don't respond to mouse input
- No compiler error or warning

**Symptoms:**
- WebView UI loads and displays properly
- Knobs visible but completely frozen (don't respond to drag)
- Parameters don't update when knobs are moved
- Audio processing works (parameters have default values)

**When:** ALL WebView-based plugins using WebSliderParameterAttachment

**Documented in:** Pattern discovered during TapeAge frozen knobs issue (2025-11-11)

---

## 13. check_native_interop.js - Required for WebView (CRITICAL)

### ❌ WRONG (Missing file - UI may freeze)
```cmake
# CMakeLists.txt
juce_add_binary_data(PluginName_UIResources
    SOURCES
        Source/ui/public/index.html
        Source/ui/public/js/juce/index.js
        # Missing: check_native_interop.js
)
```

### ✅ CORRECT
```cmake
# CMakeLists.txt
juce_add_binary_data(PluginName_UIResources
    SOURCES
        Source/ui/public/index.html
        Source/ui/public/js/juce/index.js
        Source/ui/public/js/juce/check_native_interop.js  # Required
)
```

**PluginEditor.cpp resource handler:**
```cpp
// Must serve check_native_interop.js
if (url == "/js/juce/check_native_interop.js") {
    return juce::WebBrowserComponent::Resource {
        makeVector(BinaryData::check_native_interop_js,
                  BinaryData::check_native_interop_jsSize),
        juce::String("text/javascript")
    };
}
```

**Why:**
- `check_native_interop.js` is JUCE's WebView initialization verification script
- Tests that native C++ ↔ JavaScript bridge is working
- Without it, WebView may appear to load but native integration fails silently
- Results in frozen UI, non-responsive controls, broken parameter binding

**Symptoms:**
- UI loads and displays correctly
- Knobs visible but don't respond to interaction
- Similar to missing nullptr, but root cause is missing initialization

**Source:** Copy from working plugin (e.g., GainKnob) or JUCE examples

**When:** ALL WebView-based plugins

**Documented in:** Pattern discovered during TapeAge frozen knobs issue (2025-11-11)

---

## 14. Changing PRODUCT_NAME - Manual Cleanup Required (CRITICAL)

### ❌ WRONG (Leaves orphaned plugins)
```bash
# Change PRODUCT_NAME in CMakeLists.txt
PRODUCT_NAME "TAPE AGE"  →  PRODUCT_NAME "TapeAge"

# Build and install
./scripts/build-and-install.sh PluginName

# Result: Both "TAPE AGE" and "TapeAge" exist in system folders
# DAW shows duplicate plugins, one broken
```

### ✅ CORRECT
```bash
# BEFORE changing PRODUCT_NAME, manually remove old versions:
rm -rf ~/Library/Audio/Plug-Ins/VST3/"TAPE AGE.vst3"
rm -rf ~/Library/Audio/Plug-Ins/Components/"TAPE AGE.component"

# THEN change PRODUCT_NAME in CMakeLists.txt
PRODUCT_NAME "TapeAge"

# THEN build and install
./scripts/build-and-install.sh PluginName

# Clear caches
killall -9 AudioComponentRegistrar
rm ~/Library/Preferences/Ableton/*/PluginDatabase.cfg
```

**Why:**
- Build script's "Phase 4: Remove Old Versions" searches for plugins matching the NEW product name
- When you change PRODUCT_NAME, old plugins have the OLD name, so they aren't found and removed
- DAW sees both versions, creating duplicate entries in plugin list
- Old version is broken (wrong Component ID, missing fixes), new version works

**Prevention:**
- Always manually remove old plugins BEFORE changing PRODUCT_NAME
- Or use `/uninstall [PluginName]` before changing name (searches by directory, not product name)
- After name change, verify only one version exists: `ls ~/Library/Audio/Plug-Ins/{VST3,Components}/ | grep -i PluginName`

**Common scenarios:**
- Removing spaces: "TAPE AGE" → "TapeAge"
- Fixing capitalization: "tapeage" → "TapeAge"
- Renaming plugin: "OldName" → "NewName"
- Fixing typos in product name

**When:** ANY time PRODUCT_NAME is modified in CMakeLists.txt

**Documented in:** Pattern discovered during TapeAge VST3/AU duplicate issue (2025-11-11)

---

## 15. WebView valueChangedEvent Callback - No Parameters Passed (CRITICAL)

### ❌ WRONG (Parameters undefined, knobs don't update)
```javascript
// JavaScript assumes callback receives value parameter
driveState.valueChangedEvent.addListener((newValue) => {
    // newValue is UNDEFINED! JUCE doesn't pass callback parameters
    updateKnobVisual(driveRotatable, newValue);  // angle = NaN, no rotation
});
```

**Result:** Knobs appear at 12 o'clock (0 degrees) or show `NaN` rotation. Parameters don't persist between sessions because visual updates fail.

### ✅ CORRECT
```javascript
// Call getNormalisedValue() INSIDE the callback to read from state
driveState.valueChangedEvent.addListener(() => {
    const value = driveState.getNormalisedValue();  // Read current value from state
    updateKnobVisual(driveRotatable, value);
});
```

**Why:**
- JUCE's WebView `valueChangedEvent` is a **notification event**, not a value-passing event
- The callback receives **no parameters** (or undefined if you declare them)
- Different from typical JavaScript event patterns (e.g., DOM events)
- The correct pattern: callback with no params → call `getNormalisedValue()` inside
- By the time the event fires, the state object has been updated by C++

**Event sequence:**
1. C++ calls `sendInitialUpdate()` on WebSliderParameterAttachment
2. JUCE WebView bridge updates JavaScript SliderState object
3. `valueChangedEvent` fires (no callback parameters)
4. JavaScript callback reads value via `getNormalisedValue()`
5. Visual update receives valid number (0-1 normalized)

**Common mistake:**
Assuming callback works like DOM events: `addEventListener('change', (event) => event.value)`

**Reference implementation:**
GainKnob example shows this pattern:
```javascript
this.paramState.valueChangedEvent.addListener(() => {
    this.render();  // render() internally calls getNormalisedValue()
});
```

**When:** ALL WebView-based plugins using parameter bindings

**Documented in:** `troubleshooting/gui-issues/webview-parameter-undefined-event-callback-TapeAge-20251111.md`

---

## 16. WebView Knob Interaction - Relative Drag (ALWAYS REQUIRED)

### ❌ WRONG (Absolute positioning - knob jumps to cursor)
```javascript
// Absolute positioning: knob tracks mouse Y-coordinate
knob.addEventListener('mousedown', (e) => {
    isDragging = true;
    startY = e.clientY;
    startRotation = getCurrentRotation();
});

document.addEventListener('mousemove', (e) => {
    if (!isDragging) return;

    const deltaY = startY - e.clientY;  // Total distance from START
    const newRotation = startRotation + deltaY;  // Proportional to cursor position
    setRotation(newRotation);
});
```

**Result:** Knob jumps to match cursor Y-position. Unnatural, imprecise interaction. User must move mouse to exact target position instead of dragging incrementally.

### ✅ CORRECT (Relative drag - industry standard)
```javascript
// Relative drag: knob increments based on frame-to-frame movement
let rotation = 0;
let lastY = 0;

knob.addEventListener('mousedown', (e) => {
    isDragging = true;
    lastY = e.clientY;  // Store CURRENT position, not start
});

document.addEventListener('mousemove', (e) => {
    if (!isDragging) return;

    const deltaY = lastY - e.clientY;  // Distance since LAST FRAME
    rotation += deltaY * 0.5;  // Increment rotation (sensitivity factor)
    rotation = Math.max(-135, Math.min(135, rotation));

    setRotation(rotation);
    lastY = e.clientY;  // Update for next frame
});
```

**Why:**
- **Frame-delta pattern:** Calculates movement since last frame, not since initial click
- **Incremental updates:** Adds to current rotation rather than replacing it
- **Natural feel:** Drag up = increase, drag down = decrease (proportional to drag distance)
- **Cursor position irrelevant:** Can drag infinitely, not bound to cursor location
- **Industry standard:** Pro Tools, Logic, Ableton, all professional VST/AU plugins use this

**Key differences:**
- `lastY` (previous frame) NOT `startY` (initial click)
- `rotation += deltaY` (increment) NOT `rotation = startRotation + deltaY` (replace)
- `lastY = e.clientY` (update tracking each frame)

**Common mistake:**
Using total distance from start point: `startY - e.clientY` creates absolute positioning where knob rotation directly maps to mouse Y-coordinate.

**When:** ALL WebView knob/rotary controls

**Documented in:** `troubleshooting/gui-issues/absolute-knob-drag-webview-20251111.md`

---

## 17. juce::dsp::Reverb API - Modern DSP Pipeline (ALWAYS REQUIRED)

### ❌ WRONG (API mismatch - will not compile)
```cpp
// Using old juce::Reverb API with juce::dsp::Reverb class
juce::dsp::Reverb reverb;

void prepareToPlay(double sampleRate, int samplesPerBlock) override {
    reverb.setSampleRate(sampleRate);  // No such method in juce::dsp::Reverb
    reverb.reset();
}

void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override {
    if (buffer.getNumChannels() == 1) {
        reverb.processMono(buffer.getWritePointer(0), buffer.getNumSamples());  // Wrong API
    } else if (buffer.getNumChannels() == 2) {
        reverb.processStereo(buffer.getWritePointer(0), buffer.getWritePointer(1), buffer.getNumSamples());  // Wrong API
    }
}
```

### ✅ CORRECT
```cpp
// Modern juce::dsp::Reverb with ProcessSpec and AudioBlock
juce::dsp::Reverb reverb;

void prepareToPlay(double sampleRate, int samplesPerBlock) override {
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());

    reverb.prepare(spec);
    reverb.reset();
}

void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override {
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    reverb.process(context);  // Handles all channel configurations automatically
}
```

**Why:** JUCE has two different reverb classes with incompatible APIs:
- `juce::Reverb` (old, non-DSP module) - Uses `setSampleRate()`, `processMono()`, `processStereo()`
- `juce::dsp::Reverb` (modern DSP module) - Uses `prepare(spec)` and `process(context)`

**Key differences:**
- Modern DSP uses ProcessSpec (not individual setters) in `prepare()`
- Modern DSP uses AudioBlock/ProcessContext (not raw pointers) in `process()`
- Modern DSP handles mono/stereo/multi-channel automatically via AudioBlock
- Modern DSP integrates seamlessly with other juce::dsp components (DryWetMixer, etc.)

**When:** ANY plugin using `juce::dsp::Reverb` or other `juce::dsp::` components

**Pattern applies to all juce::dsp components:**
- `juce::dsp::Compressor`
- `juce::dsp::Limiter`
- `juce::dsp::Chorus`
- `juce::dsp::Phaser`
- `juce::dsp::Reverb`
- `juce::dsp::DelayLine`
- `juce::dsp::IIR::Filter`

**Documented in:** `troubleshooting/api-usage/juce-dsp-reverb-wrong-api-methods-FlutterVerb-20251111.md`

---

## Usage Instructions

### For Subagents (foundation-agent, shell-agent, dsp-agent, gui-agent)

Add to your prompt:

```markdown
## CRITICAL: Before Implementation

Read and internalize JUCE 8 critical patterns:
@troubleshooting/patterns/juce8-critical-patterns.md

These are non-negotiable requirements. Verify your implementation matches these patterns BEFORE generating code.
```

### For Human Developers

Review this file before:
- Creating new plugins
- Modifying CMakeLists.txt
- Adding WebView UI
- Debugging build/runtime issues

---

## References

## 18. macOS Plugin Code Signing & Cache Management (ALWAYS REQUIRED)

### ❌ WRONG (Will cause plugins to not load or show old code)
```bash
# Building and installing without proper signing/cache clearing
cmake --build build --target MyPlugin_VST3 MyPlugin_AU
cp -R build/.../MyPlugin.vst3 ~/Library/Audio/Plug-Ins/VST3/
cp -R build/.../MyPlugin.component ~/Library/Audio/Plug-Ins/Components/

# OR: Using --deep flag when re-signing (corrupts binary!)
codesign --force --deep --sign - ~/Library/Audio/Plug-Ins/VST3/MyPlugin.vst3

# Expecting DAW to see changes without clearing caches
# (Launch Ableton and wonder why plugin doesn't update)
```

### ✅ CORRECT
```bash
# Step 1: Build plugins
cmake --build build --target MyPlugin_VST3 MyPlugin_AU

# Step 2: Remove old versions
rm -rf ~/Library/Audio/Plug-Ins/VST3/MyPlugin.vst3
rm -rf ~/Library/Audio/Plug-Ins/Components/MyPlugin.component

# Step 3: Install fresh from build artifacts
cp -R build/.../MyPlugin.vst3 ~/Library/Audio/Plug-Ins/VST3/
cp -R build/.../MyPlugin.component ~/Library/Audio/Plug-Ins/Components/

# Step 4: Sign WITHOUT --deep flag (prevents corruption)
codesign --force --sign - ~/Library/Audio/Plug-Ins/VST3/MyPlugin.vst3
codesign --force --sign - ~/Library/Audio/Plug-Ins/Components/MyPlugin.component

# Step 5: Verify signatures are valid
codesign --verify --deep --strict ~/Library/Audio/Plug-Ins/VST3/MyPlugin.vst3
codesign --verify --deep --strict ~/Library/Audio/Plug-Ins/Components/MyPlugin.component

# Step 6: Nuclear cache clear
killall "Ableton Live 12 Suite" "Logic Pro" 2>/dev/null
rm ~/Library/Preferences/Ableton/Live*/PluginScanner.txt 2>/dev/null
rm -rf ~/Library/Caches/AudioUnitCache 2>/dev/null
killall -9 AudioComponentRegistrar 2>/dev/null

# Step 7: System restart (required for AudioComponentRegistrar)
sudo reboot
```

**Why:** macOS has multiple layers of plugin caching and validation:

1. **Code signing validation**: Invalid signatures cause "sealed resource is missing or invalid" error
2. **DAW plugin scanner cache**: Caches plugin info, doesn't reload on simple file change
3. **macOS Audio Unit cache**: System-level cache of AU components
4. **AudioComponentRegistrar**: Process that holds plugin registry, persists across kills

**Critical mistakes:**

- **Using `codesign --deep`**: Modifies nested bundle contents, changing binary hash and corrupting plugin
- **Not clearing caches**: DAW loads cached old version even with new binary
- **Not restarting**: AudioComponentRegistrar cache survives process kills

**Check DAW scanner logs for errors:**
```bash
# Ableton
tail -50 ~/Library/Preferences/Ableton/Live*/PluginScanner.txt | grep -i error

# Logic Pro
tail -50 ~/Library/Logs/AudioUnitHosting/Logic*.log | grep -i error
```

**Documented in:** `troubleshooting/validation-problems/vst3-invalid-signature-plugin-cache-FlutterVerb-20251111.md`

---

All patterns documented with full context in:
- `troubleshooting/build-failures/`
- `troubleshooting/runtime-issues/`
- `troubleshooting/api-usage/`
- `troubleshooting/gui-issues/`
- `troubleshooting/validation-problems/`

See individual issue files for:
- Full problem descriptions
- What didn't work
- Why the solution works
- Prevention strategies
