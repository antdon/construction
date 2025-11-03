#include <optional>
#include <vector>
#include <string>
#include <filesystem>

namespace gitInteraction {
    std::optional<std::vector<std::filesystem::directory_entry>> getBaseDirs(const std::string& base);
    std::vector<std::string> getCurrentBranches(
        const std::vector<std::filesystem::directory_entry> &directories);
}

const std::string base = "TikTok";
const std::string gitCommand = "git branch --show";
