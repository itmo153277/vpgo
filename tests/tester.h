/**
 * @file   tester.h
 * @brief  Список всех тест-сборщиков
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

#ifndef TESTS_TESTER_H_
#define TESTS_TESTER_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/test/unit_test.hpp>
#include <json/json.h>

namespace vpgo {

namespace tests {

/**
 * Функция составления набора тестов
 *
 * @param Object Входные параметры теста
 * @return Набор тестов
 */
typedef boost::unit_test::test_suite *(*ParserHandler)(
		const Json::Value &Object);

/**
 * Тестер корректной работы unit-test фреймворка
 *
 * @param Object Входные параметры теста
 * @return Набор тестов
 */
boost::unit_test::test_suite *UT_Test(const Json::Value &Object);

}

}

#endif
