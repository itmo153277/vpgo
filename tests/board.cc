/**
 * @file board.cc
 * Tests for Board class
 */
/*
    Simple Go engine
    Copyright (C) 2019 Ivanov VIktor

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

#include <cstddef>
#include <utility>
#include <array>
#include <boost/test/unit_test.hpp>
#include "../src/board.hpp"

BOOST_AUTO_TEST_SUITE(board_tests)

template <typename T, std::size_t... S>
constexpr std::array<T, sizeof...(S)> makeArrayHelper(
    std::integer_sequence<std::size_t, S...>, T &&value) {
	return {(S, value)...};
}

template <std::size_t N, typename T>
constexpr auto makeArray(T &&value) {
	return makeArrayHelper(
	    std::make_integer_sequence<std::size_t, N>(), std::forward<T>(value));
}

BOOST_AUTO_TEST_CASE(board_default_ctor) {
	const Board board(9);
	BOOST_TEST_REQUIRE(board.getSize() == 9);
	const std::array<PlayerColour, 81> bv1 =
	    makeArray<81, PlayerColour>(PlayerColour::NONE);
	std::array<PlayerColour, 81> bv2;
	for (std::size_t i = 0; i < 81; ++i) {
		bv2[i] = board.getValue(i);
	}
	BOOST_TEST(bv1 == bv2, boost::test_tools::per_element());
}

BOOST_AUTO_TEST_CASE(board_liberties) {
	Board board(9);

	board.playMove(0, 0, PlayerColour::BLACK);
	board.playMove(0, 1, PlayerColour::BLACK);
	board.playMove(1, 1, PlayerColour::WHITE);
	BOOST_TEST(board.getLiberties(0, 0) == 2);
	BOOST_TEST(board.getLiberties(0, 1) == 2);
	BOOST_TEST(board.getLiberties(1, 1) == 3);
}

BOOST_AUTO_TEST_CASE(board_capture) {
	Board board(9);

	board.playMove(0, 0, PlayerColour::BLACK);
	board.playMove(0, 1, PlayerColour::BLACK);
	board.playMove(1, 0, PlayerColour::WHITE);
	board.playMove(1, 1, PlayerColour::WHITE);
	board.playMove(0, 2, PlayerColour::WHITE);
	BOOST_TEST(board.getValue(0, 0) == PlayerColour::NONE);
	BOOST_TEST(board.getValue(0, 1) == PlayerColour::NONE);
	BOOST_TEST(board.getLiberties(0, 2) == 3);
	BOOST_TEST(board.getLiberties(1, 0) == 5);
	BOOST_TEST(board.getLiberties(1, 1) == 5);
}

BOOST_AUTO_TEST_SUITE_END()
