#pragma once

// Common live-refresh utility shared by every query screen (see
// docs/design/phase6-live-refresh.md, docs/feature/live-refresh.md).
//
// Extracted out of ConsoleApp so it can be reused by every handler that
// needs to stamp its output with the moment the underlying data source was
// (re)loaded.

#include <string>

// Returns the current local time formatted as "YYYY-MM-DD HH:MM:SS"
// (e.g. "2026-04-16 09:32:15").
std::string CurrentTimestampString();
