/**
 * @file colour.cc
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
	BOOST_CHECK(c == PlayerColour::BLACK);
}

BOOST_AUTO_TEST_CASE(colour_assign_ctor) {
	PlayerColour c;
	c = PlayerColour::BLACK;
	BOOST_CHECK(c == PlayerColour::BLACK);
}

BOOST_AUTO_TEST_CASE(colour_inverse) {
	BOOST_CHECK(
	    PlayerColour(PlayerColour::BLACK).inverse() == PlayerColour::WHITE);
	BOOST_CHECK(
	    PlayerColour(PlayerColour::WHITE).inverse() == PlayerColour::BLACK);
	BOOST_CHECK(
	    PlayerColour(PlayerColour::NONE).inverse() == PlayerColour::NONE);
	BOOST_CHECK(
	    PlayerColour(PlayerColour::NEUTRAL).inverse() == PlayerColour::NEUTRAL);
}

BOOST_AUTO_TEST_SUITE_END()
