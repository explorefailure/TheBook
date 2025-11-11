#include "PluginEditor.h"
#include "BinaryData.h"

TapeAgeAudioProcessorEditor::TapeAgeAudioProcessorEditor(TapeAgeAudioProcessor& p)
    : AudioProcessorEditor(&p)
    , processorRef(p)
{
    // Initialize relays with parameter IDs (MUST match APVTS IDs exactly)
    driveRelay = std::make_unique<juce::WebSliderRelay>("drive");
    ageRelay = std::make_unique<juce::WebSliderRelay>("age");
    mixRelay = std::make_unique<juce::WebSliderRelay>("mix");

    // Initialize WebView with options
    webView = std::make_unique<juce::WebBrowserComponent>(
        juce::WebBrowserComponent::Options{}
            .withNativeIntegrationEnabled()  // CRITICAL: Enables JUCE JavaScript library
            .withResourceProvider([this](const auto& url) { return getResource(url); })
            .withKeepPageLoadedWhenBrowserIsHidden()  // FL Studio fix
            .withOptionsFrom(*driveRelay)     // Register each relay
            .withOptionsFrom(*ageRelay)
            .withOptionsFrom(*mixRelay)
    );

    // Initialize attachments (connect parameters to relays)
    driveAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("drive"), *driveRelay, nullptr);
    ageAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("age"), *ageRelay, nullptr);
    mixAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *processorRef.parameters.getParameter("mix"), *mixRelay, nullptr);

    // Add WebView to editor
    addAndMakeVisible(*webView);

    // Navigate to UI
    webView->goToURL(juce::WebBrowserComponent::getResourceProviderRoot());

    // Set editor size to match mockup dimensions (500x450 from v3-ui.html)
    setSize(500, 450);

    // Phase 5.2: Start timer for VU meter updates (30 FPS)
    startTimerHz(30);
}

TapeAgeAudioProcessorEditor::~TapeAgeAudioProcessorEditor()
{
    // Phase 5.2: Stop timer
    stopTimer();
}

void TapeAgeAudioProcessorEditor::paint(juce::Graphics& g)
{
    // WebView handles all painting
    juce::ignoreUnused(g);
}

void TapeAgeAudioProcessorEditor::resized()
{
    // WebView fills entire editor
    webView->setBounds(getLocalBounds());
}

void TapeAgeAudioProcessorEditor::timerCallback()
{
    // Phase 5.2: Send VU meter updates to JavaScript
    // Read peak level from audio processor (atomic, thread-safe)
    float dbLevel = processorRef.outputLevel.load(std::memory_order_relaxed);

    // Emit event to JavaScript (only if WebView is visible)
    webView->emitEventIfBrowserIsVisible("updateVUMeter", dbLevel);
}

std::optional<juce::WebBrowserComponent::Resource>
TapeAgeAudioProcessorEditor::getResource(const juce::String& url)
{
    // Helper lambda to convert raw binary data to vector<byte>
    auto makeVector = [](const char* data, int size) {
        return std::vector<std::byte>(
            reinterpret_cast<const std::byte*>(data),
            reinterpret_cast<const std::byte*>(data) + size
        );
    };

    // Handle root URL (redirect to index.html)
    if (url == "/" || url == "/index.html") {
        return juce::WebBrowserComponent::Resource {
            makeVector(BinaryData::index_html, BinaryData::index_htmlSize),
            juce::String("text/html")
        };
    }

    // JUCE frontend library
    if (url == "/js/juce/index.js") {
        return juce::WebBrowserComponent::Resource {
            makeVector(BinaryData::index_js, BinaryData::index_jsSize),
            juce::String("text/javascript")
        };
    }

    // JUCE native interop checker
    if (url == "/js/juce/check_native_interop.js") {
        return juce::WebBrowserComponent::Resource {
            makeVector(BinaryData::check_native_interop_js,
                      BinaryData::check_native_interop_jsSize),
            juce::String("text/javascript")
        };
    }

    // Resource not found
    juce::Logger::writeToLog("Resource not found: " + url);
    return std::nullopt;
}
