/**
 * @file src/colour.hpp
 *
 * Player colours
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

#ifndef SRC_COLOUR_HPP_
#define SRC_COLOUR_HPP_

#include "vpgo.hpp"

/**
 * Player colour
 */
class PlayerColour {
public:
	/**
	 * Value enum
	 */
	enum Value {
		NONE,    //!< Empty
		BLACK,   //!< Black
		WHITE,   //!< White
		NEUTRAL  //!< Neutral
	};

	/**
	 * Number of values
	 */
	static constexpr int COUNT = 4;

private:
	/**
	 * Internal value
	 */
	Value m_val;

public:
	/**
	 * Default ctor
	 */
	PlayerColour() : m_val(Value::NONE) {
	}
	/**
	 * Conversion ctor
	 */
	constexpr PlayerColour(Value value)  // NOLINT(runtime/explicit)
	    : m_val(value) {
	}
	/**
	 * Default dtor
	 */
	~PlayerColour() = default;

	/**
	 * Assignment by enum value
	 *
	 * @param value Enum value
	 * @return Self
	 */
	constexpr PlayerColour &operator=(Value value) {
		m_val = value;
		return *this;
	}
	/**
	 * Conversion to enum value
	 *
	 * @return Value Enum value
	 */
	constexpr operator Value() const {
		return m_val;
	}

	/**
	 * Inverse colour
	 *
	 * @return Inverted colour
	 */
	constexpr PlayerColour invert() const {
		switch (m_val) {
		case BLACK:
			return WHITE;
		case WHITE:
			return BLACK;
		default:
			return m_val;
		}
	}
};

#endif  // SRC_COLOUR_HPP_
