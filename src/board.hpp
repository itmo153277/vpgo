/**
 * @file src/board.hpp
 * Board
 */
/*
    Simple Go engine
    Copyright (C) 2019-2021 Ivanov Viktor

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

#ifndef SRC_BOARD_HPP_
#define SRC_BOARD_HPP_

#include <cassert>
#include <cstdint>
#include <utility>
#include <vector>
#include <unordered_set>
#include <iterator>
#include <tuple>
#include "vpgo.hpp"
#include "colour.hpp"
#include "hash.hpp"

/**
 * Board offset type
 */
using board_offset_t = unsigned int;
/**
 * Coordinate offset type
 */
using board_coord_t = unsigned int;
/**
 * Board size type
 */
using board_size_t = unsigned int;

/**
 * Board traverse helper
 */
struct BoardTraverse {
	/**
	 * Iterator for traverse
	 */
	class iterator {
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type =
		    std::tuple<board_coord_t, board_coord_t, board_offset_t>;
		using reference = void;
		using pointer = void;
		using difference_type = int;

	private:
		/**
		 * Reference
		 */
		const BoardTraverse *m_Ref;
		/**
		 * Current stage
		 */
		int m_i;

		/**
		 * No default ctor
		 */
		iterator() = delete;

		/**
		 * Iterator ctor
		 *
		 * @param ref Reference
		 * @param i Current stage
		 */
		constexpr iterator(const BoardTraverse *ref, int i)
		    : m_Ref(ref), m_i(i) {
		}

	public:
		/**
		 * Checks for inequality
		 *
		 * @param other Other iterator
		 * @return this != other
		 */
		constexpr bool operator!=(const iterator &other) const {
			return other.m_i != m_i;
		}

		/**
		 * Increment
		 *
		 * @return Self
		 */
		constexpr iterator &operator++() {
			m_i = m_Ref->getNext(m_i);
			return *this;
		}

		/**
		 * Increment
		 *
		 * @return Old copy
		 */
		constexpr iterator operator++(int) {
			auto copy = *this;
			m_i = m_Ref->getNext(m_i);
			return copy;
		}

		/**
		 * Dereference
		 *
		 * @return Value
		 */
		constexpr value_type operator*() const {
			return m_Ref->getValue(m_i);
		}

		friend BoardTraverse;
	};

private:
	/**
	 * X coord
	 */
	board_coord_t m_x;
	/**
	 * Y coord
	 */
	board_coord_t m_y;
	/**
	 * Offset
	 */
	board_offset_t m_Offset;
	/**
	 * Size
	 */
	board_size_t m_Size;

	/**
	 * Get next value
	 *
	 * @param i Current stage
	 * @return Next value
	 */
	constexpr int getNext(int i = -1) const {
		switch (i) {
		case -1:
			if (m_x > 0) {
				return 0;
			}
			[[fallthrough]];
		case 0:
			if (m_y > 0) {
				return 1;
			}
			[[fallthrough]];
		case 1:
			if (m_x < m_Size - 1) {
				return 2;
			}
			[[fallthrough]];
		case 2:
			if (m_y < m_Size - 1) {
				return 3;
			}
		}
		return -1;
	}
	/**
	 * Get value
	 *
	 * @param i Current stage
	 * @return Value
	 */
	constexpr iterator::value_type getValue(int i) const {
		switch (i) {
		case 0:
			return {m_x - 1, m_y, m_Offset - 1};
		case 1:
			return {m_x, m_y - 1, m_Offset - m_Size};
		case 2:
			return {m_x + 1, m_y, m_Offset + 1};
		case 3:
			return {m_x, m_y + 1, m_Offset + m_Size};
		}
		return {};
	}

public:
	/**
	 * Deleted default ctor
	 */
	BoardTraverse() = delete;

	/**
	 * Ctor
	 *
	 * @param x X coordinate
	 * @param y Y coordinate
	 * @param offset Offset
	 * @param size Size
	 */
	constexpr BoardTraverse(board_coord_t x, board_coord_t y,
	    board_offset_t offset, board_size_t size)
	    : m_x(x), m_y(y), m_Offset(offset), m_Size(size) {
	}

	/**
	 * Get begin iterator
	 *
	 * @return iterator
	 */
	constexpr iterator begin() const {
		return iterator(this, getNext());
	}
	/**
	 * Gen end iterator
	 *
	 * @return iterator
	 */
	constexpr iterator end() const {
		return iterator(this, -1);
	}
};

/**
 * Board
 */
class Board {
private:
	/**
	 * Group information
	 */
	struct GroupInfo {
		/**
		 * Number of edges
		 */
		int edges = 0;
		/**
		 * Number of stones
		 */
		int stones = 0;
		/**
		 * Group hash
		 */
		hash_t hash;
	};

	/**
	 * Board size
	 */
	board_size_t m_Size;
	/**
	 * Board state
	 */
	std::vector<PlayerColour> m_State;
	/**
	 * Groups
	 */
	std::vector<GroupInfo> m_Groups;
	/**
	 * Releation between groups
	 */
	std::vector<board_offset_t> m_GroupRelation;
	/**
	 * Hash
	 */
	hash_t m_Hash;
	/**
	 * Number of stones
	 */
	int m_Stones;

	/**
	 * Merge two groups into one
	 *
	 * @param from Smaller group
	 * @param to Larger group
	 */
	void mergeGroups(board_offset_t from, board_offset_t to);

	/**
	 * Finds group location for specified stone
	 *
	 * @param offset Stone offset
	 * @return Position of the group
	 */
	board_offset_t getGroupLocation(board_offset_t offset) const {
		assert(offset < m_Size * m_Size);
		assert(m_State[offset] == PlayerColour::BLACK ||
		       m_State[offset] == PlayerColour::WHITE);
		board_offset_t groupLoc = offset;
		while (m_GroupRelation[groupLoc] != groupLoc) {
			groupLoc = m_GroupRelation[groupLoc];
		}
		return groupLoc;
	}

	/**
	 * Remove group of stones
	 *
	 * @param offset Offset
	 * @param x X coord
	 * @param y Y coord
	 */
	void removeGroup(board_offset_t offset, board_coord_t x, board_coord_t y);

public:
	/**
	 * Deleted default ctor
	 */
	Board() = delete;
	/**
	 * Construct a new Board object
	 *
	 * @param size Board size
	 */
	explicit Board(board_size_t size)
	    : m_Size{size}
	    , m_State(size * size, PlayerColour::NONE)
	    , m_Groups(size * size)
	    , m_GroupRelation(size * size)
	    , m_Hash{HashValues::getInstance()->getInitialValue()}
	    , m_Stones{0} {
	}

	/**
	 * Equality comparison
	 *
	 * @param s Other board
	 * @return True if same position
	 */
	bool operator==(const Board &s) {
		return m_Size == s.m_Size && m_Hash == s.m_Hash;
	}
	/**
	 * Inequality comparison
	 *
	 * @param s Other board
	 * @return True if same position
	 */
	bool operator!=(const Board &s) {
		return !(*this == s);
	}

	/**
	 * Size
	 *
	 * @return Size
	 */
	board_size_t getSize() const {
		return m_Size;
	}
	/**
	 * Value
	 *
	 * @param offset Offset
	 * @return Value
	 */
	PlayerColour getValue(board_offset_t offset) const {
		assert(offset < m_Size * m_Size);
		return m_State[offset];
	}
	/**
	 * Value
	 *
	 * @param x X coord
	 * @param y Y coord
	 * @return Value
	 */
	PlayerColour getValue(board_coord_t x, board_coord_t y) const {
		return getValue(coordsToOffset(x, y));
	}

	/**
	 * Get number of edges
	 *
	 * @param offset Location offset
	 * @return Number of edges
	 */
	int getEdges(board_offset_t offset) const {
		return m_Groups[getGroupLocation(offset)].edges;
	}

	/**
	 * Get number of edges
	 *
	 * @param x X coord
	 * @param y Y coord
	 * @return Number of edges
	 */
	int getEdges(board_coord_t x, board_coord_t y) const {
		return getEdges(coordsToOffset(x, y));
	}

	/**
	 * Play a move
	 *
	 * @param x X coord
	 * @param y Y coord
	 * @param colour Stone colour
	 */
	void playMove(board_coord_t x, board_coord_t y, PlayerColour colour);
	/**
	 * Play a move
	 *
	 * @param offset Offset
	 * @param colour Stone colour
	 */
	void playMove(board_offset_t offset, PlayerColour colour) {
		auto [x, y] = offsetToCoords(offset);
		playMove(x, y, colour);
	}

	/**
	 * Check if move is a suicide
	 *
	 * @param x X coord
	 * @param y Y coord
	 * @param colour Stone colour
	 * @return True if suicide
	 */
	bool isSuicide(board_coord_t x, board_coord_t y, PlayerColour colour) const;
	/**
	 * Check if move is a suicide
	 *
	 * @param offset Offset
	 * @param colour Stone colour
	 * @return True if suicide
	 */
	bool isSuicide(board_offset_t offset, PlayerColour colour) const {
		auto [x, y] = offsetToCoords(offset);
		return isSuicide(x, y, colour);
	}

	bool isEyeLike(board_coord_t x, board_coord_t y, PlayerColour colour) const;

	bool isEyeLike(board_offset_t offset, PlayerColour colour) const {
		auto [x, y] = offsetToCoords(offset);
		return isEyeLike(x, y, colour);
	}

	/**
	 * Pre-computes hash
	 *
	 * @param x X coord
	 * @param y Y coord
	 * @param colour Stone colour
	 * @return Hash value
	 */
	hash_t preComputeHash(
	    board_coord_t x, board_coord_t y, PlayerColour colour) const;
	/**
	 * Pre-computes hash
	 *
	 * @param offset Offset
	 * @param colour Stone colour
	 * @return Hash value
	 */
	hash_t preComputeHash(board_offset_t offset, PlayerColour colour) const {
		auto [x, y] = offsetToCoords(offset);
		return preComputeHash(x, y, colour);
	}

	/**
	 * Count points
	 *
	 * @return Pair {Black, White}
	 */
	std::pair<int, int> countPoints() const;

	/**
	 * Get hash
	 *
	 * @return Hash
	 */
	hash_t getHash() const {
		return m_Hash;
	}

	/**
	 * Get the number of stones
	 *
	 * @return Number of stones
	 */
	int getNumberOfStones() const {
		return m_Stones;
	}

	/**
	 * Convert coords to offset
	 *
	 * @param x X coord
	 * @param y Y coord
	 * @return Offset
	 */
	board_offset_t coordsToOffset(board_coord_t x, board_coord_t y) const {
		return y * m_Size + x;
	}
	/**
	 * Convert offset to coords
	 *
	 * @param offset Offset
	 * @return A pair of x and y coords
	 */
	std::pair<board_coord_t, board_coord_t> offsetToCoords(
	    board_offset_t offset) const {
		return std::make_pair(offset % m_Size, offset / m_Size);
	}
};

#endif  // SRC_BOARD_HPP_
