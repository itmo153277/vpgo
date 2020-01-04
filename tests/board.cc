/**
 * @file tests/board.cc
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
#include <iostream>
#include <string>
#include <algorithm>
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "../src/board.hpp"

namespace std {

/**
 * Print operator for board traverser
 *
 * @param os Stream
 * @param v Value
 * @return Stream
 */
ostream &operator<<(ostream &os, BoardTraverse::iterator::value_type v) {
	auto [x, y, offset] = v;
	os << "x = " << x << ", ";
	os << "y = " << y << ", ";
	os << "ofs = " << offset;
	return os;
}

/**
 * Print operator for board traverser answers
 *
 * @param os Stream
 * @param v Value
 * @return Stream
 */
ostream &operator<<(
    ostream &os, const vector<BoardTraverse::iterator::value_type> &v) {
	std::copy(v.begin(), v.end(),
	    std::ostream_iterator<BoardTraverse::iterator::value_type>(os, ", "));
	return os;
}

}  // namespace std

/**
 * Board iterator
 */
struct BoardIterator {
	using iterator_category = std::forward_iterator_tag;
	using value_type = PlayerColour;
	using reference = void;
	using pointer = void;
	using difference_type = int;

	std::size_t i;
	const Board *board;

	bool operator!=(const BoardIterator &other) {
		return i != other.i;
	}
	BoardIterator &operator++() {
		++i;
		return *this;
	}
	BoardIterator operator++(int) {
		auto copy = *this;
		++i;
		return copy;
	}
	value_type operator*() {
		return board->getValue(i);
	}
};

/**
 * Convert board to vector
 *
 * @param board Board
 * @return Vector of values
 */
std::vector<PlayerColour> convertToVector(const Board &board) {
	std::vector<PlayerColour> data(BoardIterator({0, &board}),
	    BoardIterator({board.getSize() * board.getSize(), &board}));

	return data;
}

/**
 * Convert board to string
 *
 * @param board Board
 * @return String
 */
std::string convertToString(const Board &board) {
	std::vector<char> data;
	std::transform(BoardIterator({0, &board}),
	    BoardIterator({board.getSize() * board.getSize(), &board}),
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
	return std::string(data.data(), data.size());
}

/**
 * Convert string to board
 *
 * @param size Board size
 * @param s String
 * @return Board
 */
Board convertFromString(std::size_t size, const std::string &s) {
	Board b(size);
	for (std::size_t i = 0, y = 0; y < size; ++y) {
		for (std::size_t x = 0; x < size; ++i, ++x) {
			switch (s[i]) {
			case 'B':
				b.playMove(x, y, PlayerColour::BLACK);
				break;
			case 'W':
				b.playMove(x, y, PlayerColour::WHITE);
			}
		}
	}
	return b;
}

BOOST_AUTO_TEST_SUITE(board_tests)

// clang-format off
/**
 * Test data for board traverse class
 */
const std::tuple<
    std::size_t,  // X
    std::size_t,  // Y
    std::size_t,  // Offset
    std::size_t,  // Size
    std::vector<BoardTraverse::iterator::value_type>  // Answer
> board_traverse_data[] = {
    {1, 1, 6, 5, {
        {0, 1, 5},
        {1, 0, 1},
        {2, 1, 7},
        {1, 2, 11}
    }},
    {0, 1, 5, 5, {
        {0, 0, 0},
        {1, 1, 6},
        {0, 2, 10}
    }},
    {1, 0, 1, 5, {
        {0, 0, 0},
        {2, 0, 2},
        {1, 1, 6}
    }},
    {0, 0, 0, 5, {
        {1, 0, 1},
        {0, 1, 5}
    }},
    {0, 0, 0, 1, {}},
    {4, 4, 24, 5, {
        {3, 4, 23},
        {4, 3, 19}
    }},
    {4, 3, 19, 5, {
        {3, 3, 18},
        {4, 2, 14},
        {4, 4, 24}
    }},
    {3, 4, 23, 5, {
        {2, 4, 22},
        {3, 3, 18},
        {4, 4, 24}
    }}
};
// clang-format on

BOOST_DATA_TEST_CASE(
    board_traverse, board_traverse_data, x, y, offset, size, answer) {
	BoardTraverse bt(x, y, offset, size);
	std::vector<BoardTraverse::iterator::value_type> result(
	    bt.begin(), bt.end());
	BOOST_TEST(result == answer, boost::test_tools::per_element());
}

// Compile-time check
static_assert(std::get<0>(*(BoardTraverse(1, 1, 6, 5).begin())) == 0);

/**
 * Test board position
 */
const char testBoard[] =
    "   "
    " B "
    "WW ";

BOOST_AUTO_TEST_CASE(board_position) {
	Board b = convertFromString(3, testBoard);
	auto b_string = convertToString(b);
	BOOST_TEST(b_string == testBoard);
}

BOOST_AUTO_TEST_CASE(board_copy_ctor) {
	Board b = convertFromString(3, testBoard);
	Board copy = b;
	auto b_data = convertToVector(b);
	auto copy_data = convertToVector(copy);
	BOOST_TEST(b_data == copy_data, boost::test_tools::per_element());
}

BOOST_AUTO_TEST_CASE(board_assign_ctor) {
	Board b = convertFromString(3, testBoard);
	Board copy(3);
	copy = b;
	auto b_data = convertToVector(b);
	auto copy_data = convertToVector(copy);
	BOOST_TEST(b_data == copy_data, boost::test_tools::per_element());
}

BOOST_AUTO_TEST_CASE(board_move_ctor) {
	Board b = convertFromString(3, testBoard);
	Board temp = b;
	Board copy = std::move(temp);
	auto b_data = convertToVector(b);
	auto copy_data = convertToVector(copy);
	BOOST_TEST(b_data == copy_data, boost::test_tools::per_element());
}

BOOST_AUTO_TEST_CASE(board_move_assign_ctor) {
	Board b = convertFromString(3, testBoard);
	Board temp = b;
	Board copy(3);
	copy = std::move(temp);
	auto b_data = convertToVector(b);
	auto copy_data = convertToVector(copy);
	BOOST_TEST(b_data == copy_data, boost::test_tools::per_element());
}

BOOST_AUTO_TEST_SUITE_END()
