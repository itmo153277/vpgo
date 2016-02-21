/**
 * @file tester.cpp
 * @brief Программа запуска тестов
 *
 * Программа парсит тесты в виде json-файлов, собирает набор и выполняет
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

#include <boost/test/included/unit_test.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <cerrno>
#include "tester.h"

using namespace ::boost::unit_test;
using namespace ::std;
using namespace ::vpgo;
using namespace ::vpgo::tests;

/**
 * Функция парсинга тестового файла и регистрации тестов
 *
 * @param argc Количество аргументов тестера
 * @param argv Аргументы тестера
 * @return Всегда NULL, так как все тесты регистрируются самостоятельно
 */
test_suite*
init_unit_test_suite(int argc, char* argv[]) {
	if (argc < 2) {
		return NULL;
	}

	ifstream inp(argv[1]);
	stringstream json;

	if (inp.fail()) {
		return NULL;
	}
	json << inp.rdbuf();

	Json::Reader Reader;
	Json::Value Root;

	if (!Reader.parse(json.str(), Root)) {
		return NULL;
	}

	static const int ParsersCount = 1;
	static const char *ParserAliases[ParsersCount] = { "ut-test" };
	ParserHandler ParserHandlers[ParsersCount] = { &UT_Test };

	for (auto TestSuite : Root["suites"]) {
		string SuiteName = TestSuite["name"].asString();
		for (int i = 0; i < ParsersCount; i++) {
			if (SuiteName == ParserAliases[i]) {
				framework::master_test_suite().add(
						(*ParserHandlers[i])(TestSuite));
				break;
			}
		}
	}
	return NULL;
}
