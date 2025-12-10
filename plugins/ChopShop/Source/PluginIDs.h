#pragma once

namespace PluginIDs
{
    // Parameter IDs (used for APVTS and UI bindings)
    namespace Params
    {
        static constexpr const char* STUTTER_RATE = "stutterRate";
        static constexpr const char* PITCH_SHIFT = "pitchShift";
        static constexpr const char* SPEED = "speed";
        static constexpr const char* LINK_PITCH_SPEED = "linkPitchSpeed";
        static constexpr const char* REVERSE = "reverse";
        static constexpr const char* TRIGGER_MODE = "triggerMode";
        static constexpr const char* THRESHOLD = "threshold";
        static constexpr const char* MIX = "mix";

        // Chain order is stored in ValueTree, not APVTS
        static constexpr const char* CHAIN_ORDER = "chainOrder";
    }
}
