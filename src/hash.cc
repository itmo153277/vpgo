/**
 * @file src/hash.cc
 * Implementation for HashValues
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

#include "hash.hpp"
#include <chrono>

/**
 * Private ctor
 */
HashValues::HashValues() {
	std::random_device rd;
	auto time =
	    std::chrono::high_resolution_clock::now().time_since_epoch().count();
	auto newSeed = rd() + time;
	seed(newSeed);
}

/**
 * Reseed
 *
 * @param newSeed New seed
 */
void HashValues::seed(int newSeed) {
	m_RandomEngine.seed(newSeed);
	m_InitialValue = generateRandomNumber();
	for (auto &v : m_Values) {
		v = generateRandomNumber();
	}
}

/**
 * Init values for the size
 *
 * @param size
 */
void HashValues::init(std::size_t size) {
	for (; m_Size < size; ++m_Size) {
		m_Values.push_back(generateRandomNumber());
		m_Values.push_back(generateRandomNumber());
	}
}
