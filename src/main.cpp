
#include <filesystem>
#include <iostream>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <cstdlib>
#include <string>
#include <optional>
#include <vector>
#include <ranges>
#include <functional>
#include <cstdio>
#include <memory>

namespace ftx = ftxui;

// idea right now
// we have an optional config file that specifies
// directory base name, 3 commands we can run in that directory

const std::string base = "TikTok";
const std::string gitCommand = "git branch --show";

std::optional<std::string> getHomeDir() {
    const char* homeDir = std::getenv("HOME");
    if (!homeDir) 
	homeDir = std::getenv("USERPROFILE");
    return homeDir ? std::make_optional(std::string(homeDir)) : std::nullopt;
}

std::optional<std::vector<std::filesystem::directory_entry>> getBaseDirs(const std::string& base) {
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

std::vector<std::string> getCurrentBranches(
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

int main() {
    auto baseDirs = getBaseDirs(base);
    auto branches = baseDirs.transform([](auto &baseDirs) {
      auto branches = getCurrentBranches(baseDirs);
      for (const auto &branch : branches) {
	  std::cout << branch << std::endl;
      }
      return branches;
    });


    
    return 0;
}

// int main() {
// auto screen = ftx::ScreenInteractive::Fullscreen();
// auto component = ftx::CatchEvent(
// ftx::Renderer([] {
// return ftx::text("Press any key") | ftx::center;
// }),
// [&](ftx::Event event) {
// if (event == ftx::Event::Character('q')) {
// screen.ExitLoopClosure()();
// return true;
// }
// return false;
// }
// );
// 
// screen.Loop(component);
// return 0;
// }
