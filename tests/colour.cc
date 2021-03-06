/**
 * @file tests/colour.cc
 * Tests for PlayerColour class
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

#include <boost/test/unit_test.hpp>
#include "../src/colour.hpp"

BOOST_AUTO_TEST_SUITE(colour_tests)

BOOST_AUTO_TEST_CASE(colour_copy_ctor) {
	PlayerColour c = PlayerColour::BLACK;
	BOOST_TEST(c == PlayerColour::BLACK);
}

BOOST_AUTO_TEST_CASE(colour_assign_ctor) {
	PlayerColour c;
	c = PlayerColour::BLACK;
	BOOST_TEST(c == PlayerColour::BLACK);
}

BOOST_AUTO_TEST_CASE(colour_inverse) {
	BOOST_TEST(
	    PlayerColour(PlayerColour::BLACK).invert() == PlayerColour::WHITE);
	BOOST_TEST(
	    PlayerColour(PlayerColour::WHITE).invert() == PlayerColour::BLACK);
	BOOST_TEST(PlayerColour(PlayerColour::NONE).invert() == PlayerColour::NONE);
	BOOST_TEST(
	    PlayerColour(PlayerColour::NEUTRAL).invert() == PlayerColour::NEUTRAL);
}

static_assert(
    PlayerColour(PlayerColour::BLACK).invert() == PlayerColour::WHITE);

BOOST_AUTO_TEST_SUITE_END()
