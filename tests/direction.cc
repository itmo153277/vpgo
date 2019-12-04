/**
 * @file direction.cc
 * Tests for Direction class
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
#include "../src/direction.hpp"

BOOST_AUTO_TEST_SUITE(direction_tests)

BOOST_AUTO_TEST_CASE(direction_copy_ctor) {
	Direction d = Direction::RIGHT;
	BOOST_TEST(d == Direction::RIGHT);
}

BOOST_AUTO_TEST_CASE(direction_assign_ctor) {
	Direction d;
	d = Direction::RIGHT;
	BOOST_TEST(d == Direction::RIGHT);
}

BOOST_AUTO_TEST_CASE(direction_inverse) {
	BOOST_TEST(Direction(Direction::UP).invert() == Direction::DOWN);
	BOOST_TEST(Direction(Direction::DOWN).invert() == Direction::UP);
	BOOST_TEST(Direction(Direction::LEFT).invert() == Direction::RIGHT);
	BOOST_TEST(Direction(Direction::RIGHT).invert() == Direction::LEFT);
}

static_assert(Direction(Direction::UP).invert() == Direction::DOWN);

BOOST_AUTO_TEST_SUITE_END()
