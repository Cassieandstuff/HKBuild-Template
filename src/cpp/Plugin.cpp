#include "PCH.h"

// ---------------------------------------------------------------------------
// TODO: Replace "MyPlugin" with your plugin's name throughout this file.
//       Replace "YourName" in CMakeLists.txt with your name.
// ---------------------------------------------------------------------------

namespace MyPlugin {

    // -----------------------------------------------------------------------
    // SKSE message handlers
    // -----------------------------------------------------------------------

    static void OnPostLoad()
    {
        // Called after all SKSE plugins have loaded.
        // Good place to query other plugins' APIs or register behavior graphs.

        LOG_INFO("MyPlugin: PostLoad complete.");
    }

    static void OnDataLoaded()
    {
        // Called after Skyrim's data files (ESPs/BSAs) are fully loaded.
        // Good place to look up forms by EditorID.

        LOG_INFO("MyPlugin: DataLoaded.");
    }

}  // namespace MyPlugin

// ---------------------------------------------------------------------------
// SKSE plugin entry point
// ---------------------------------------------------------------------------
SKSEPluginLoad(const SKSE::LoadInterface* skse)
{
    SKSE::Init(skse);
    PluginLogger::Init("MyPlugin", "My Plugin");

    LOG_INFO("MyPlugin loading.");

    auto* messaging = SKSE::GetMessagingInterface();
    if (!messaging) {
        LOG_ERROR("MyPlugin: failed to get messaging interface.");
        return false;
    }

    messaging->RegisterListener([](SKSE::MessagingInterface::Message* msg) {
        if (!msg) return;
        switch (msg->type) {
        case SKSE::MessagingInterface::kPostLoad:
            MyPlugin::OnPostLoad();
            break;
        case SKSE::MessagingInterface::kDataLoaded:
            MyPlugin::OnDataLoaded();
            break;
        default:
            break;
        }
    });

    LOG_INFO("MyPlugin: listeners registered.");
    return true;
}
