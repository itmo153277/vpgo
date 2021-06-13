/**
 * @file tests/game.cc
 * Tests for Game class
 */
/*
   Simple Go engine
   Copyright (C) 2021 Ivanov Viktor

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "../src/game.hpp"

#include <utility>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

/**
 * Move description
 */
using move_description = std::tuple<game_move_t, PlayerColour>;

/**
 * Convert coords to offset
 *
 * @param x X coordinate
 * @param y Y coordinate
 * @param size Board size
 * @return Move
 */
constexpr game_move_t coordToMove(
    board_coord_t x, board_coord_t y, board_size_t size) {
	return y * size + x;
}

namespace std {

/**
 * Print operator for move descriptor
 *
 * @param os Stream
 * @param v Value
 * @return Stream
 */
ostream &operator<<(ostream &os, move_description v) {
	auto [move, colour] = v;
	os << "move = " << move << ", ";
	os << "col = " << colour;
	return os;
}

/**
 * Print operator for move list
 *
 * @param os Stream
 * @param v Value
 * @return Stream
 */
ostream &operator<<(ostream &os, const std::vector<move_description> &v) {
	std::copy(
	    v.begin(), v.end(), std::ostream_iterator<move_description>(os, ", "));
	return os;
}

}  // namespace std

/**
 * Board iterator
 */
struct GameIterator {
	using iterator_category = std::forward_iterator_tag;
	using value_type = PlayerColour;
	using reference = void;
	using pointer = void;
	using difference_type = int;

	board_offset_t i;
	const Game *game;

	bool operator!=(const GameIterator &other) {
		return i != other.i;
	}
	GameIterator &operator++() {
		++i;
		return *this;
	}
	GameIterator operator++(int) {
		auto copy = *this;
		++i;
		return copy;
	}
	PlayerColour operator*() {
		return game->getBoard().getValue(i);
	}
};

/**
 * Convert string to Game
 *
 * @param size Board size
 * @param s String
 * @return Game
 */
Game strToGame(board_size_t size, const std::string &s) {
	std::stringstream ss(s);
	float komi;
	ss >> komi;
	Game game(size, static_cast<int>(komi));
	switch (ss.get()) {
	case 'W':
		game.setWinner(PlayerColour::WHITE);
		break;
	case 'B':
		game.setWinner(PlayerColour::BLACK);
	}
	for (board_coord_t y = 0; y < size; ++y) {
		for (board_coord_t x = 0; x < size; ++x) {
			switch (ss.get()) {
			case 'B':
				game.playMove(x, y, PlayerColour::BLACK);
				break;
			case 'W':
				game.playMove(x, y, PlayerColour::WHITE);
			}
		}
	}
	return game;
}

/**
 * Convert Game to string
 *
 * @param game Game
 * @return String
 */
std::string gameToStr(const Game &game) {
	std::vector<char> data;
	std::transform(GameIterator({0, &game}),
	    GameIterator(
	        {game.getBoard().getSize() * game.getBoard().getSize(), &game}),
	    std::back_inserter(data), [](PlayerColour c) {
		    switch (c) {
		    case PlayerColour::BLACK:
			    return 'B';
		    case PlayerColour::WHITE:
			    return 'W';
		    default:
			    return ' ';
		    }
	    });
	char winner;
	switch (game.getWinner()) {
	case PlayerColour::BLACK:
		winner = 'B';
		break;
	case PlayerColour::WHITE:
		winner = 'W';
		break;
	default:
		winner = ' ';
	}
	std::stringstream ss;
	ss << game.getKomi() << ".5" << winner
	   << std::string(data.data(), data.size());
	return ss.str();
}

/**
 * Fixture for game tests
 */
struct game_fixture {
	game_fixture() {
		HashValues::getInstance()->init(5 * 5);
	}
};

BOOST_FIXTURE_TEST_SUITE(game_tests, game_fixture)

/**
 * Test board position
 */
const char testBoard[] =
    "7.5"
    " "
    "   "
    " B "
    "WW ";

BOOST_AUTO_TEST_CASE(game_position) {
	Game g = strToGame(3, testBoard);
	auto g_string = gameToStr(g);
	BOOST_TEST(g_string == testBoard);
}

BOOST_AUTO_TEST_CASE(game_copy_ctor) {
	Game g = strToGame(3, testBoard);
	Game copy = g;
	auto copy_str = gameToStr(copy);
	BOOST_TEST(copy_str == testBoard);
	g.playMove(0, 0, PlayerColour::BLACK);
	copy_str = gameToStr(copy);
	BOOST_TEST(copy_str == testBoard);
}

BOOST_AUTO_TEST_CASE(game_assign_ctor) {
	Game g = strToGame(3, testBoard);
	Game copy(3, g.getKomi());
	copy = g;
	auto copy_str = gameToStr(copy);
	BOOST_TEST(copy_str == testBoard);
	g.playMove(0, 0, PlayerColour::BLACK);
	copy_str = gameToStr(copy);
	BOOST_TEST(copy_str == testBoard);
}

BOOST_AUTO_TEST_CASE(game_move_ctor) {
	Game g = strToGame(3, testBoard);
	Game temp = g;
	Game copy = std::move(temp);
	auto copy_str = gameToStr(copy);
	BOOST_TEST(copy_str == testBoard);
	g.playMove(0, 0, PlayerColour::BLACK);
	copy_str = gameToStr(copy);
	BOOST_TEST(copy_str == testBoard);
}

BOOST_AUTO_TEST_CASE(game_move_assign_ctor) {
	Game g = strToGame(3, testBoard);
	Game temp = g;
	Game copy(3, g.getKomi());
	copy = std::move(temp);
	auto copy_str = gameToStr(copy);
	BOOST_TEST(copy_str == testBoard);
	g.playMove(0, 0, PlayerColour::BLACK);
	copy_str = gameToStr(copy);
	BOOST_TEST(copy_str == testBoard);
}

// clang-format off
/**
 * Data for board play tests
 */
const std::tuple<
    std::string,                    // Board
    std::vector<move_description>,  // Moves
    PlayerColour                    // Winner
> game_play_test_data[] = {
    // Win by komi
    {
        "7.5"
        " "
        "     "
        "     "
        "     "
        "     "
        "     ",
        {
            {Game::PASS, PlayerColour::BLACK},
            {Game::PASS, PlayerColour::WHITE}
        },
        PlayerColour::WHITE
    },
    // Win by points
    {
        "7.5"
        " "
        "W   B"
        "   BB"
        "   BB"
        "   BB"
        "   BB",
        {
            {Game::PASS, PlayerColour::BLACK},
            {Game::PASS, PlayerColour::WHITE}
        },
        PlayerColour::BLACK
    },
    // Win by points
    {
        "7.5"
        " "
        "W   B"
        "    B"
        "   BB"
        "   BB"
        "   BB",
        {
            {Game::PASS, PlayerColour::BLACK},
            {Game::PASS, PlayerColour::WHITE}
        },
        PlayerColour::WHITE
    },
    // Resign
    {
        "7.5"
        " "
        "W   B"
        "    B"
        "   BB"
        "   BB"
        "   BB",
        {
            {Game::PASS, PlayerColour::BLACK},
            {Game::RESIGN, PlayerColour::WHITE}
        },
        PlayerColour::BLACK
    },
    // Illegal move
    {
        "7.5"
        " "
        "W   B"
        "    B"
        "   BB"
        "   BB"
        "   BB",
        {
            {Game::PASS, PlayerColour::BLACK},
            {coordToMove(0, 0, 5), PlayerColour::WHITE}
        },
        PlayerColour::BLACK
    },
    // Ko
    {
        "7.5"
        " "
        "     "
        "     "
        " BW  "
        "BW W "
        " BW  ",
        {
            {coordToMove(2, 3, 5), PlayerColour::BLACK},
            {coordToMove(1, 3, 5), PlayerColour::WHITE}
        },
        PlayerColour::BLACK
    }
};
// clang-format on

BOOST_DATA_TEST_CASE(
    game_play_test, game_play_test_data, position, moves, answer) {
	Game g = strToGame(5, position);
	for (auto &[move, col] : moves) {
		g.playMove(move, col);
	}
	PlayerColour result = g.getWinner();
	BOOST_CHECK(result == answer);
}

BOOST_AUTO_TEST_SUITE_END()
