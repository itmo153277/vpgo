/**
 * @file ut-test.cpp
 * @brief Тест проверки работоспособности unit-test фреймворка
 *
 * Тест проверки работоспособности unit-test фреймворка
 */

/*
 A simple bot playing Go
 Copyright (C) 2016 Ivanov Viktor

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

#include "tester.h"

using namespace ::boost::unit_test;
using namespace ::vpgo;
using namespace ::vpgo::tests;

test_suite *::vpgo::tests::UT_Test(const Json::Value &Object) {
	test_suite *ts = BOOST_TEST_SUITE("ut-test");
	ts->add(BOOST_TEST_CASE([&]{ BOOST_CHECK(true); }));
	return ts;
}
