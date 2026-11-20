/*******************************************************************************
 * Copyright (c) 2018-2026 openblack developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/openblack/openblack
 *
 * openblack is licensed under the GNU General Public License version 3.
 *******************************************************************************/

#include <Debug/DebugGuiInterface.h>
#include <Game.h>
#include <Locator.h>
#include <SDL3/SDL_events.h>
#include <gtest/gtest.h>

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables): external macro
TEST(GameInitialize, initializeOnly)
{
	static const auto mockGamePath = std::filesystem::path(TEST_BINARY_DIR) / "mock";
	auto args = openblack::Arguments {
	    .graphicsBackend = openblack::GraphicsBackend::Noop,
	    .gamePath = mockGamePath.string(),
	    .numFramesToSimulate = 0,
	    .logFile = "stdout",
	    .startLevel = "Land1.txt",
	};
	std::fill_n(args.logLevels.begin(), args.logLevels.size(), spdlog::level::debug);
	auto game = std::make_unique<openblack::Game>(std::move(args));
	ASSERT_TRUE(game->Initialize());
	game.reset();
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables): external macro
TEST(GameInitialize, run0Frames)
{
	static const auto mockGamePath = std::filesystem::path(TEST_BINARY_DIR) / "mock";
	auto args = openblack::Arguments {
	    .graphicsBackend = openblack::GraphicsBackend::Noop,
	    .gamePath = mockGamePath.string(),
	    .numFramesToSimulate = 0,
	    .logFile = "stdout",
	    .startLevel = "Land1.txt",
	};
	std::fill_n(args.logLevels.begin(), args.logLevels.size(), spdlog::level::debug);
	auto game = std::make_unique<openblack::Game>(std::move(args));
	ASSERT_TRUE(game->Initialize());
	ASSERT_TRUE(game->Run());
	game.reset();
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables): external macro
TEST(GameInitialize, toggleDebugGui)
{
	static const auto mockGamePath = std::filesystem::path(TEST_BINARY_DIR) / "mock";
	auto args = openblack::Arguments {
	    .graphicsBackend = openblack::GraphicsBackend::Noop,
	    .gamePath = mockGamePath.string(),
	    .numFramesToSimulate = 0,
	    .logFile = "stdout",
	    .startLevel = "Land1.txt",
	    .drawGui = false,
	};
	std::fill_n(args.logLevels.begin(), args.logLevels.size(), spdlog::level::debug);
	auto game = std::make_unique<openblack::Game>(std::move(args));
	ASSERT_TRUE(game->Initialize());

	auto& config = openblack::Locator::config::value();
	auto& debugGui = openblack::Locator::debugGui::value();
	EXPECT_FALSE(config.drawGui);
	EXPECT_FALSE(debugGui.Loop());
	debugGui.Draw(nullptr, nullptr);
	EXPECT_FALSE(debugGui.StealsFocus());

	SDL_Event f10Down {};
	f10Down.type = SDL_EVENT_KEY_DOWN;
	f10Down.key.key = SDLK_F10;
	f10Down.key.repeat = false;

	EXPECT_TRUE(debugGui.ProcessEvents(f10Down));
	EXPECT_TRUE(config.drawGui);

	EXPECT_TRUE(debugGui.ProcessEvents(f10Down));
	EXPECT_FALSE(config.drawGui);
	EXPECT_FALSE(debugGui.StealsFocus());

	SDL_Event f10Repeat = f10Down;
	f10Repeat.key.repeat = true;
	EXPECT_FALSE(debugGui.ProcessEvents(f10Repeat));
	EXPECT_FALSE(config.drawGui);

	SDL_Event ctrlF10 = f10Down;
	ctrlF10.key.mod = SDL_KMOD_CTRL;
	EXPECT_FALSE(debugGui.ProcessEvents(ctrlF10));
	EXPECT_FALSE(config.drawGui);

	game.reset();
}
