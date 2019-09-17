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
#include "vpgo.hpp"
#include "colour.hpp"

/**
 * Board
 */
class Board {
private:
	/**
	 * Board size
	 */
	std::size_t m_Size;
	/**
	 * Board state
	 */
	std::vector<PlayerColour> m_State;

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
	    : m_Size(size), m_State(size * size, PlayerColour::NONE) {
	}
	/**
	 * Copy ctor
	 *
	 * @param s Source
	 */
	Board(const Board &s) : m_Size(s.m_Size), m_State(s.m_State) {
	}
	/**
	 * Move ctor
	 *
	 * @param s Source
	 */
	Board(Board &&s) noexcept
	    : m_Size(s.m_Size), m_State(std::move(s.m_State)) {
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
		return *this;
	}

    /**
     * Size
     * 
     * @return Size
     */
	const std::size_t getSize() const {
		return m_Size;
	}
    /**
     * Value
     * 
     * @param offset Offset
     * @return Value
     */
	const PlayerColour getValue(std::size_t offset) const {
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
	const PlayerColour getValue(std::size_t x, std::size_t y) const {
        return getValue(coordsToOffset(x, y));
	}
};

#endif  // SRC_BOARD_HPP_
