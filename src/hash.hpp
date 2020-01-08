/**
 * @file src/hash.hpp
 * Hash functions
 */
/*
    Simple Go engine
    Copyright (C) 2020 Ivanov VIktor

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

#ifndef SRC_HASH_HPP_
#define SRC_HASH_HPP_

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <random>
#include "vpgo.hpp"
#include "colour.hpp"

/**
 * Hash values class
 */
struct HashValues {
	/**
	 * Get singleton instance
	 *
	 * @return Instance
	 */
	static HashValues *getInstance() {
		static HashValues val;
		return &val;
	}

	/**
	 * Init values for the size
	 *
	 * @param size
	 */
	void init(std::size_t size);
	/**
	 * Reseed
	 * 
	 * @param newSeed New seed
	 */
	void seed(int newSeed);

	/**
	 * Get value for the move
	 *
	 * @param offset Move offset
	 * @param colour Player colour
	 * @return Hash value
	 */
	std::uint16_t getValue(std::size_t offset, PlayerColour colour) const {
		assert(offset < m_Size);
		switch (colour) {
		case PlayerColour::BLACK:
			return m_Values[offset * 2];
		case PlayerColour::WHITE:
			return m_Values[offset * 2 + 1];
		}
		return m_InitialValue;
	}

	/**
	 * Get initial hash value
	 *
	 * @return Initial hash value
	 */
	std::uint16_t getInitialValue() const {
		return m_InitialValue;
	}

private:
	/**
	 * Move hash values
	 */
	std::vector<std::uint64_t> m_Values;
	/**
	 * Initial hash value
	 */
	std::uint64_t m_InitialValue;
	/**
	 * Current size
	 */
	std::size_t m_Size;
	/**
	 * Random engine
	 */
	std::default_random_engine m_RandomEngine;
	/**
	 * Private ctor
	 */
	HashValues();
};

#endif  // SRC_HASH_HPP_
