#include "gitInteraction.hpp"

#include <filesystem>
#include <cstdlib>
#include <string>
#include <optional>
#include <vector>
#include <ranges>
#include <functional>
#include <cstdio>
#include <memory>

std::optional<std::string> getHomeDir() {
    const char* homeDir = std::getenv("HOME");
    if (!homeDir) 
	homeDir = std::getenv("USERPROFILE");
    return homeDir ? std::make_optional(std::string(homeDir)) : std::nullopt;
}

std::optional<std::vector<std::filesystem::directory_entry>> gitInteraction::getBaseDirs(const std::string& base) {
  return getHomeDir().transform([&base](auto home) {
      auto filePaths = std::vector<std::filesystem::directory_entry>{};
      auto homePath = std::filesystem::path{home};
      for (const auto &entry : std::filesystem::directory_iterator(homePath)) {
	  filePaths.push_back(entry);
      }
      return std::views::filter(filePaths, [&base](auto& path){
	  return path.path().filename().string().starts_with(base);
      }) | std::ranges::to<std::vector>();
  });
}

std::optional<std::string> executeInDirectory(const std::filesystem::path& directory, const std::function<std::optional<std::string>()> f) {
  // TODO: change this to a directory guard using RAII.
  // currently this risks f failing and the path not being changed back
  // if you put this in a destructor that will mean it is certainly changed back

    auto original = std::filesystem::current_path();
    std::filesystem::current_path(directory);
    auto output = f();
    std::filesystem::current_path(original);
    return output;
}

std::vector<std::string> gitInteraction::getCurrentBranches(
    const std::vector<std::filesystem::directory_entry> &directories) {
    auto getBranch = []() -> std::optional<std::string> {
	auto pipe = popen(gitCommand.c_str(), "r");
        if (!pipe) {
	    return std::nullopt;
        }
	auto pipe_wrapper = std::unique_ptr<FILE, decltype(&pclose)>(pipe, pclose);
        std::string result;
	char buffer[128];
        while (fgets(buffer, sizeof(buffer), pipe_wrapper.get()) != nullptr) {
	    result += buffer;
        }
	return result;
    };
    return directories
	| std::views::transform([&getBranch](const auto &dir) {
	    return executeInDirectory(dir, [&getBranch]{ return getBranch(); });
	})
	| std::views::filter([](const auto& optional){ return optional.has_value(); })
	| std::views::transform([](const auto& optional) { return *optional; })
	| std::ranges::to<std::vector<std::string>>();
}
