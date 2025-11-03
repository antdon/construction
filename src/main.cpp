
#include <iostream>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include "gitInteraction.hpp"

namespace ftx = ftxui;

// idea right now
// we have an optional config file that specifies
// directory base name, 3 commands we can run in that directory


int main() {
    auto baseDirs = gitInteraction::getBaseDirs(base);
    auto branches = baseDirs.transform([](auto &baseDirs) {
	auto branches = gitInteraction::getCurrentBranches(baseDirs);
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
