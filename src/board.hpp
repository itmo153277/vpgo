/**
 * @file board.hpp
 * Board
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

#ifndef SRC_BOARD_HPP_
#define SRC_BOARD_HPP_

#include <cassert>
#include <cstddef>
#include <utility>
#include <vector>
#include <unordered_set>
#include "vpgo.hpp"
#include "colour.hpp"

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
		 * Number of liberties
		 */
		std::size_t libs = 0;
		/**
		 * Number of stones
		 */
		std::size_t stones = 0;
	};

	/**
	 * Board size
	 */
	std::size_t m_Size;
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
	std::vector<std::size_t> m_GroupRelation;

	/**
	 * Convert coords to offset
	 *
	 * @param x X coord
	 * @param y Y coord
	 * @return Offset
	 */
	std::size_t coordsToOffset(std::size_t x, std::size_t y) const {
		return y * m_Size + x;
	}
	/**
	 * Convert offset to coords
	 *
	 * @param offset Offset
	 * @return A pair of x and y coords
	 */
	std::pair<std::size_t, std::size_t> offsetToCoords(
	    std::size_t offset) const {
		return std::make_pair(offset % m_Size, offset / m_Size);
	}

	/**
	 * Merge two groups into one
	 *
	 * @param from Smaller group
	 * @param to Larger group
	 */
	void mergeGroups(std::size_t from, std::size_t to);

	/**
	 * Finds group location for specified stone
	 *
	 * @param offset Stone offset
	 * @return std::size_t Position of the group
	 */
	std::size_t getGroupLocation(std::size_t offset) const {
		assert(offset < m_Size * m_Size);
		assert(m_State[offset] == PlayerColour::BLACK ||
		       m_State[offset] == PlayerColour::WHITE);
		std::size_t groupLoc = offset;
		while (m_GroupRelation[groupLoc] != groupLoc) {
			groupLoc = m_GroupRelation[groupLoc];
		}
		return groupLoc;
	}

	/**
	 * Reduce number of liberties of a group
	 *
	 * @param targetOffset Group to reduce libs
	 * @param currentOffset Group to increase libs
	 * @param groups Group set
	 * @param maxGroup Current max group
	 */
	void reduceLiberties(std::size_t targetOffset, std::size_t currentOffset,
	    std::unordered_set<std::size_t> *groups, std::size_t *maxGroup);

	/**
	 * Remove group of stones
	 *
	 * @param offset Offset
	 * @param x X coord
	 * @param y Y coord
	 */
	void removeGroup(std::size_t offset, std::size_t x, std::size_t y);

	/**
	 * Increase liberties or remove group
	 *
	 * @param offset Offset
	 * @param x X coord
	 * @param y Y coord
	 * @param colour Current colour
	 */
	void increaseLiberties(
	    std::size_t offset, std::size_t x, std::size_t y, PlayerColour colour);

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
	explicit Board(std::size_t size)
	    : m_Size(size)
	    , m_State(size * size, PlayerColour::NONE)
	    , m_Groups(size * size)
	    , m_GroupRelation(size * size) {
	}
	/**
	 * Copy ctor
	 *
	 * @param s Source
	 */
	Board(const Board &s)
	    : m_Size(s.m_Size)
	    , m_State(s.m_State)
	    , m_Groups(s.m_Groups)
	    , m_GroupRelation(s.m_GroupRelation) {
	}
	/**
	 * Move ctor
	 *
	 * @param s Source
	 */
	Board(Board &&s) noexcept
	    : m_Size(s.m_Size)
	    , m_State(std::move(s.m_State))
	    , m_Groups(std::move(s.m_Groups))
	    , m_GroupRelation(std::move(s.m_GroupRelation)) {
	}

	/**
	 * Default dtor
	 */
	~Board() = default;

	/**
	 * Copy assignment
	 *
	 * @param s Source
	 * @return Self
	 */
	Board &operator=(const Board &s) {
		m_Size = s.m_Size;
		m_State = s.m_State;
		m_Groups = s.m_Groups;
		m_GroupRelation = s.m_GroupRelation;
		return *this;
	}
	/**
	 * Move assignment
	 *
	 * @param s Source
	 * @return Self
	 */
	Board &operator=(Board &&s) noexcept {
		m_Size = s.m_Size;
		m_State = std::move(s.m_State);
		m_Groups = std::move(s.m_Groups);
		m_GroupRelation = std::move(s.m_GroupRelation);
		return *this;
	}

	/**
	 * Size
	 *
	 * @return Size
	 */
	std::size_t getSize() const {
		return m_Size;
	}
	/**
	 * Value
	 *
	 * @param offset Offset
	 * @return Value
	 */
	PlayerColour getValue(std::size_t offset) const {
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
	PlayerColour getValue(std::size_t x, std::size_t y) const {
		return getValue(coordsToOffset(x, y));
	}

	/**
	 * Get number of liberties
	 *
	 * @param offset Location offset
	 * @return Number of liberties
	 */
	std::size_t getLiberties(std::size_t offset) const {
		return m_Groups[getGroupLocation(offset)].libs;
	}

	/**
	 * Get number of liberties
	 *
	 * @param x X coord
	 * @param y Y coord
	 * @return Number of liberties
	 */
	std::size_t getLiberties(std::size_t x, std::size_t y) const {
		return getLiberties(coordsToOffset(x, y));
	}

	/**
	 * Play a move
	 *
	 * @param x X coord
	 * @param y Y coord
	 * @param colour Stone colour
	 */
	void playMove(std::size_t x, std::size_t y, PlayerColour colour);

	/**
	 * Check if move is a suicide
	 *
	 * @param x X coord
	 * @param y Y coord
	 * @param colour Stone colour
	 * @return True if suicide
	 */
	bool isSuicide(std::size_t x, std::size_t y, PlayerColour colour);
};

#endif  // SRC_BOARD_HPP_
