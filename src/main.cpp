
#include <iostream>
#include <ranges>
#include <numeric>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include "gitInteraction.hpp"

namespace ftx = ftxui;

// idea right now
// we have an optional config file that specifies
// directory base name, 3 commands we can run in that directory

auto selectBranch(unsigned int index, std::vector<bool>& selectedState) {
  if (index >= selectedState.size()) {
      return;
  }
  selectedState = std::vector(selectedState.size(), false);
  selectedState[index] = true;
}

int main() {
    auto baseDirs = gitInteraction::getBaseDirs(base);
    auto optionalBranches = baseDirs.transform([](auto &baseDirs) {
	auto branches = gitInteraction::getCurrentBranches(baseDirs);
	for (const auto &branch : branches) {
	    std::cout << branch << std::endl;
	}
	return branches;
    });
    if (!optionalBranches.has_value()) {
	std::cerr << "Couldn't find any git repositories that start with that prefix in your home repo" << std::endl;
	return 1;
    }
    auto branches = optionalBranches.value();
    auto branchSelectionState = std::vector(branches.size(), false);
    selectBranch(0, branchSelectionState);

    auto screen = ftx::ScreenInteractive::Fullscreen();
    auto component =
        ftx::CatchEvent(ftx::Renderer([&branches, &branchSelectionState] {
          auto branchElements =
	      std::views::iota(0) | std::views::take(branches.size()) |
              std::views::transform([&branchSelectionState, &branches](const auto index) {
		  auto element = ftx::text(branches[index]);
		  if (branchSelectionState[index]) {
		      return element | ftx::border;
                  } else {
		      return element;
                  }
	      })
	      | std::ranges::to<std::vector<ftx::Element>>();
	return ftx::vbox(branchElements) | ftx::center;
	}),
	[&](ftx::Event event) {
	    if (event == ftx::Event::Character('q')) {
		screen.ExitLoopClosure()();
		return true;
	    }
	    return false;
	}
	);

    screen.Loop(component);
    return 0;
}
