// Copyright Azahar Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the misc/licenses/gplv2.txt file included.

#include <algorithm>
#include "common/common_paths.h"
#include "common/file_util.h"
#include "common/string_util.h"
#include "video_core/post_processing.h"

namespace VideoCore {

namespace {

std::string GetShaderDir(bool anaglyph) {
    std::string shader_dir = FileUtil::GetUserPath(FileUtil::UserPath::ShaderDir);
    if (!FileUtil::IsDirectory(shader_dir)) {
        FileUtil::CreateDir(shader_dir);
    }
    if (anaglyph) {
        shader_dir += "anaglyph";
        if (!FileUtil::IsDirectory(shader_dir)) {
            FileUtil::CreateDir(shader_dir);
        }
    }
    return shader_dir;
}

/// Returns the base name if virtual_name is a file with a .glsl extension (any case).
std::string GlslBaseName(const std::string& directory, const std::string& virtual_name) {
    if (FileUtil::IsDirectory(directory + DIR_SEP + virtual_name)) {
        return {};
    }
    const std::size_t dot_pos = virtual_name.rfind('.');
    if (dot_pos == std::string::npos ||
        Common::ToLower(virtual_name.substr(dot_pos + 1)) != "glsl") {
        return {};
    }
    return virtual_name.substr(0, dot_pos);
}

} // Anonymous namespace

std::vector<std::string> GetPostProcessingShaderList(bool anaglyph) {
    std::vector<std::string> shader_names;
    const auto callback = [&shader_names](u64*, const std::string& directory,
                                          const std::string& virtual_name) -> bool {
        std::string name = GlslBaseName(directory, virtual_name);
        if (!name.empty()) {
            shader_names.push_back(std::move(name));
        }
        return true;
    };
    FileUtil::ForeachDirectoryEntry(nullptr, GetShaderDir(anaglyph), callback);
    std::sort(shader_names.begin(), shader_names.end());
    return shader_names;
}

std::string LoadPostProcessingShaderSource(bool anaglyph, std::string_view shader_name) {
    std::string shader_path;
    const auto callback = [&shader_name, &shader_path](u64*, const std::string& directory,
                                                       const std::string& virtual_name) -> bool {
        if (GlslBaseName(directory, virtual_name) == shader_name) {
            shader_path = directory + DIR_SEP + virtual_name;
            return false;
        }
        return true;
    };
    FileUtil::ForeachDirectoryEntry(nullptr, GetShaderDir(anaglyph), callback);
    if (shader_path.empty()) {
        return {};
    }

    std::string source;
    if (FileUtil::ReadFileToString(true, shader_path, source) == 0) {
        return {};
    }
    return source;
}

} // namespace VideoCore
