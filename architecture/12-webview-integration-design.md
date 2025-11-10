## WebView Integration Design

### HTML → UI Architecture

**Concept:** HTML mockup IS the production UI (no translation)

**Flow:**

```
Design Phase:
User creates mockup → v1-ui-test.html (browser preview)
User iterates → v2-ui-test.html
User finalizes → "This is the one"

Finalization Phase:
System extracts controls from mockup
System generates parameter-spec.md (intelligent defaults)
User reviews/approves parameter specs
System generates v[N]-ui.html (JUCE-integrated production UI)

Implementation Phase (Stage 5):
gui-agent copies v[N]-ui.html to ui/public/index.html
gui-agent implements PluginEditor using v[N]-editor.h/cpp templates
CMake zips ui/ directory and embeds in binary
Plugin loads, WebView renders HTML from embedded zip
```

**WebView Implementation:** All WebView integration must follow the proven patterns documented in `procedures/webview/`. See `procedures/webview/README.md` for the complete documentation series. Do not use generic JUCE WebView examples as they may be outdated or incomplete.

#### UI Template System

**Architecture:** Global rules + optional templates for rapid design iteration

**Components:**

**1. ui-design-rules.md** - Non-negotiable constraints
- WebView compatibility requirements (CSS constraints, sizing rules)
- JUCE parameter binding patterns
- Platform-specific considerations
- Performance guidelines

**2. Aesthetic Library** - User-created visual systems
```
.claude/aesthetics/
├── manifest.json
└── [aesthetic-id]/
    ├── aesthetic.yaml      # Visual system (colors, typography, controls)
    ├── preview.html        # Visual reference
    └── metadata.json       # Aesthetic info
```

**Template structure:**
```yaml
name: vintage-hardware
description: Warm, retro, skeuomorphic design
colors:
  background: "#2a2a2a"
  accent: "#ff6b35"
  text: "#f0e7d8"
typography:
  heading: "Bebas Neue"
  body: "Roboto"
  mono: "Roboto Mono"
components:
  knob: rotary-vintage
  slider: vertical-wood
  button: toggle-mechanical
```

**Workflow integration:**

```
ui-mockup invocation:
├─> First mockup for plugin?
│   ├─> YES: Offer template selection
│   │   ├─> "1. Vintage Hardware (warm, retro)"
│   │   ├─> "2. Modern Minimal (clean, flat)"
│   │   ├─> "3. [Your saved templates...]"
│   │   └─> "4. Custom (start from rules only)"
│   └─> NO: Use current template or ask to switch
│
├─> Generate mockup from:
│   ├─> Template YAML (if selected) + ui-design-rules.md
│   └─> ui-design-rules.md only (if custom)
│
└─> Can switch templates between iterations:
    User: "Try modern-minimal instead"
    → Regenerate with different template
```

**Benefits:**
- Faster initial design (template pre-applies aesthetics)
- Consistent look across related plugins
- User can save finalized designs as new templates
- Still allows complete customization when needed

#### WebView Implementation Constraints

**Critical CSS constraints** (violations cause runtime failures):

```css
/* ❌ WRONG - Causes blank display on first load */
body { min-height: 100vh; }

/* ✅ CORRECT - Works on initial render */
body { min-height: 100%; }
html, body { height: 100%; }
```

**Rationale:** JUCE WebView doesn't initialize viewport units until first resize event

See `procedures/webview/best-practices.md` for comprehensive WebView constraints and patterns.

**Native application feel** (required for professional plugins):

```css
body {
  user-select: none;           /* Disable text selection */
  -webkit-user-select: none;   /* Safari/WebKit */
  cursor: default;             /* No text cursor */
  overflow: hidden;            /* No scrolling if fixed size */
}
```

```javascript
// Disable right-click context menu
document.addEventListener("contextmenu", (e) => e.preventDefault());
```

**Sizing strategy:**

| Approach | Use Case | C++ | CSS |
|----------|----------|-----|-----|
| **Fixed size** | Simple plugins, single purpose | `setResizable(false, false)` | Fixed widths, no responsive units |
| **Resizable** | Complex plugins, many parameters | `setResizable(true, true)` + aspect ratio lock | `clamp()`, responsive units, test min/max sizes |

**gui-agent responsibility:**
- Enforce these constraints in generated HTML/CSS
- Validate CSS doesn't use viewport units
- Include native feel JavaScript
- Choose fixed vs resizable based on complexity

### Parameter Binding Design

**Architecture:**

```
C++ Parameter (AudioProcessorValueTreeState)
        ↕ (normalized 0-1)
ParameterAttachment
        ↕
WebSliderRelay / WebToggleButtonRelay / WebComboBoxRelay
        ↕ (JavaScript events)
WebBrowserComponent
        ↕ (JavaScript state object)
HTML Control (<input>, <select>, <button>)
```

**Implementation Details:** The relay pattern and parameter attachment system is fully specified in `procedures/webview/04-parameter-binding.md`.

**Code example:**

**C++ (PluginEditor.h):**
```cpp
class PluginEditor : public juce::AudioProcessorEditor {
public:
    PluginEditor(PluginProcessor& p)
        : AudioProcessorEditor(p), processor(p)
    {
        // 1. Create relays (before WebView!)
        gainRelay = std::make_unique<juce::WebSliderRelay>("GAIN");
        toneRelay = std::make_unique<juce::WebSliderRelay>("TONE");
        mixRelay = std::make_unique<juce::WebSliderRelay>("MIX");

        // 2. Create WebView with relay options
        webView = std::make_unique<juce::WebBrowserComponent>(
            juce::WebBrowserComponent::Options{}
                .withNativeIntegrationEnabled()
                .withOptionsFrom(*gainRelay)
                .withOptionsFrom(*toneRelay)
                .withOptionsFrom(*mixRelay)
                .withResourceProvider([this](const auto& url) {
                    return getResource(url);
                })
        );

        // 3. Create parameter attachments (after WebView!)
        gainAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
            *processor.parameters.getParameter("GAIN"),
            *gainRelay
        );
        toneAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
            *processor.parameters.getParameter("TONE"),
            *toneRelay
        );
        // ...

        addAndMakeVisible(*webView);
    }

private:
    PluginProcessor& processor;

    std::unique_ptr<juce::WebSliderRelay> gainRelay;
    std::unique_ptr<juce::WebSliderRelay> toneRelay;
    std::unique_ptr<juce::WebSliderRelay> mixRelay;

    std::unique_ptr<juce::WebSliderParameterAttachment> gainAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> toneAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> mixAttachment;

    std::unique_ptr<juce::WebBrowserComponent> webView;
};
```

**JavaScript (ui/public/index.html):**
```html
<script type="module">
import * as Juce from "./js/juce/index.js";

// Get state objects
const gainState = Juce.getSliderState("GAIN");
const toneState = Juce.getSliderState("TONE");
const mixState = Juce.getSliderState("MIX");

// Get HTML elements
const gainSlider = document.getElementById("gain-slider");
const toneKnob = document.getElementById("tone-knob");
const mixSlider = document.getElementById("mix-slider");

// Bind HTML → C++
gainSlider.addEventListener("input", (e) => {
    gainState.setNormalisedValue(parseFloat(e.target.value));
});

// Bind C++ → HTML
gainState.valueChangedEvent.addListener((newValue) => {
    gainSlider.value = newValue;
    updateGainDisplay(newValue);
});

// Initialize
gainSlider.value = gainState.getNormalisedValue();
</script>
```

### Resource Management

**Embedded resources (production build):**

```cmake
# CMakeLists.txt
juce_add_binary_data(UIResources
    SOURCES
        ui/public/index.html
        ui/public/js/juce/index.js
)

target_link_libraries(${PLUGIN_NAME} PRIVATE UIResources)
```

**Resource provider:**

```cpp
std::optional<juce::WebBrowserComponent::Resource> getResource(
    const juce::String& url
) {
    if (url == "/") {
        return juce::WebBrowserComponent::Resource {
            BinaryData::index_html,
            BinaryData::index_htmlSize,
            "text/html"
        };
    }

    if (url == "/js/juce/index.js") {
        return juce::WebBrowserComponent::Resource {
            BinaryData::juce_index_js,
            BinaryData::juce_index_jsSize,
            "text/javascript"
        };
    }

    return std::nullopt;
}
```

### Communication Protocol

**Complete bidirectional communication architecture:**

#### C++ → JavaScript (4 methods)

**1. Emit Events** (recommended for real-time updates)
```cpp
webView->emitEventIfBrowserIsVisible("parameterChanged", {
    { "id", "GAIN" },
    { "value", 0.75 }
});
```

```javascript
window.__JUCE__.backend.addEventListener("parameterChanged", (data) => {
    console.log(`${data.id} changed to ${data.value}`);
});
```

**2. Evaluate JavaScript** (for direct function calls)
```cpp
webView->evaluateJavascript("updateMeter(0.5);");
```

**3. Native Functions** (expose C++ functions to JavaScript)
```cpp
webView = std::make_unique<juce::WebBrowserComponent>(
    Options{}
        .withNativeFunction("savePreset", [this](auto& var, auto& callback) {
            auto presetName = var[0].toString();
            bool success = processor.savePreset(presetName);
            callback(success);
        })
);
```

```javascript
const savePreset = Juce.getNativeFunction("savePreset");
savePreset("MyPreset", (success) => {
    console.log(success ? "Saved!" : "Failed");
});
```

**4. Initialization Data** (pass initial state on load)
```cpp
webView = std::make_unique<juce::WebBrowserComponent>(
    Options{}
        .withInitialisationData({
            { "pluginName", "ReverbPlugin" },
            { "version", "1.0.0" },
            { "sampleRate", processor.getSampleRate() }
        })
);
```

```javascript
const initData = window.__JUCE__.initialisationData;
console.log(`Plugin: ${initData.pluginName} v${initData.version}`);
```

#### JavaScript → C++ (2 methods)

**1. Call Native Functions** (preferred for actions)
```javascript
const savePreset = Juce.getNativeFunction("savePreset");
savePreset({ name: "MyPreset", data: {...} });
```

**2. Emit Events** (for notifications)
```javascript
window.__JUCE__.backend.emitEvent("presetSaved", { name: "MyPreset" });
```

```cpp
// C++ listener (less common, usually use native functions instead)
webView->onEventEmitted = [](const auto& event, const auto& data) {
    if (event == "presetSaved") {
        // Handle event
    }
};
```

#### Communication Pattern Recommendations

**Implementation Details:** Complete communication patterns, including event handling, native functions, and state synchronization, are specified in `procedures/webview/03-communication-patterns.md`.

**Use emitEvent (C++→JS) for:**
- Real-time parameter updates
- Meter/visualization data (timer-based)
- State notifications

**Use evaluateJavascript for:**
- One-off function calls
- Initialization sequences
- Quick testing/debugging

**Use nativeFunction (JS→C++) for:**
- User actions (save preset, load file, etc.)
- DAW integration (tempo, transport)
- Two-way communication with callbacks

**Use initializationData for:**
- Plugin metadata
- Initial configuration
- Static values needed on load

**Example: Real-time meter updates**

```cpp
// PluginEditor.cpp - Timer callback (60ms = ~16 FPS)
void PluginEditor::timerCallback() {
    float gainReduction = processor.getGainReduction();

    webView->emitEventIfBrowserIsVisible("meterUpdate", {
        { "gainReduction", gainReduction },
        { "timestamp", juce::Time::currentTimeMillis() }
    });
}
```

```javascript
// index.html - JavaScript listener
window.__JUCE__.backend.addEventListener("meterUpdate", (data) => {
    updateMeterDisplay(data.gainReduction);
});
```

---

## Related Procedures

This architecture is implemented through the complete WebView documentation series:

- `procedures/webview/README.md` - Documentation overview and quick start guide
- `procedures/webview/01-overview.md` - WebView integration fundamentals and architecture
- `procedures/webview/02-setup.md` - Project setup and configuration
- `procedures/webview/03-communication-patterns.md` - C++ ↔ JavaScript communication (events, native functions, initialization data)
- `procedures/webview/04-parameter-binding.md` - Relay pattern for parameter binding (WebSliderRelay, WebToggleButtonRelay, WebComboBoxRelay)
- `procedures/webview/05-resource-management.md` - Embedded resource provider implementation
- `procedures/webview/06-testing.md` - WebView testing strategies
- `procedures/webview/07-distribution.md` - Distribution and deployment considerations
- `procedures/webview/best-practices.md` - Critical patterns including member declaration order, CSS constraints
- `procedures/webview/common-problems.md` - Troubleshooting guide for WebView issues
- `procedures/webview/webview-vs-native.md` - Decision guide for choosing WebView vs native JUCE components
