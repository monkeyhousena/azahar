// Copyright Azahar Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the misc/licenses/gplv2.txt file included.

#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace VideoCore {

/// Returns the names (without extension) of the .glsl shaders available in the user's
/// "shaders" directory, or "shaders/anaglyph" when anaglyph is true.
std::vector<std::string> GetPostProcessingShaderList(bool anaglyph);

/// Returns the raw source of the named shader with no header prepended,
/// or an empty string if it cannot be found or read.
std::string LoadPostProcessingShaderSource(bool anaglyph, std::string_view shader_name);

} // namespace VideoCore
