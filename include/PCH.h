#pragma once

// ---------------------------------------------------------------------------
// Standard library
// ---------------------------------------------------------------------------
#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

// CommonLibSSE-NG — must come before Windows.h
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "SKSE/Impl/PCH.h"

// Windows.h — after CommonLib
#include <Windows.h>

// Logging
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

// Shared per-plugin logger — call PluginLogger::Init("Name", "Display Name")
// in your SKSEPluginLoad. Provides LOG_INFO / LOG_WARN / LOG_ERROR / LOG_DEBUG.
#include "PluginLogger.h"

using namespace std::literals;
