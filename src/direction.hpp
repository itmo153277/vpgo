/**
 * @file direction.hpp
 *
 * Direction definitions
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

#ifndef SRC_DIRECTION_HPP_
#define SRC_DIRECTION_HPP_

#include "vpgo.hpp"

/**
 * Direction
 */
class Direction {
public:
	/**
	 * Value enum
	 */
	enum Value {
		UP,    //!< Up
		LEFT,  //!< Left
		DOWN,  //!< Down
		RIGHT  //!< Right
	};

	/**
	 * Number of values
	 */
	static constexpr std::size_t COUNT = 4;

private:
	/**
	 * Value
	 */
	Value m_Val;

public:
	/**
	 * Default ctor
	 */
	Direction() = default;
	/**
	 * Conversion ctor
	 */
	constexpr Direction(Value value)  // NOLINT(runtime/explicit)
	    : m_Val(value) {
	}
	/**
	 * Default dtor
	 */
	~Direction() = default;
	/**
	 * Assigment operator
	 *
	 * @param value Enum value
	 * @return Self
	 */
	constexpr Direction &operator=(Value value) {
		m_Val = value;
		return *this;
	}
	/**
	 * Conversion to enum
	 *
	 * @return Enum value
	 */
	constexpr operator Value() const {
		return m_Val;
	}

	/**
	 * Invert direction
	 *
	 * @return Invered direction
	 */
	constexpr Direction invert() const {
		switch (m_Val) {
		case UP:
			return DOWN;
		case DOWN:
			return UP;
		case LEFT:
			return RIGHT;
		case RIGHT:
			return LEFT;
		default:
			return m_Val;
		}
	}
};

#endif  // SRC_DIRECTION_HPP_
