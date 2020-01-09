/**
 * @file tests/hash.cc
 * Tests for HashValues class
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

#include <boost/test/unit_test.hpp>
#include "../src/hash.hpp"

BOOST_AUTO_TEST_CASE(hash_test) {
	HashValues::getInstance()->seed(0xEAEAEAEA);
	HashValues::getInstance()->init(1);
	BOOST_CHECK(HashValues::getInstance()->getInitialValue() !=
	            HashValues::getInstance()->getValue(0, PlayerColour::BLACK));
	BOOST_CHECK(HashValues::getInstance()->getInitialValue() !=
	            HashValues::getInstance()->getValue(0, PlayerColour::WHITE));
	BOOST_CHECK(HashValues::getInstance()->getValue(0, PlayerColour::BLACK) !=
	            HashValues::getInstance()->getValue(0, PlayerColour::WHITE));
}
