/**
 * @file   tester.cpp
 * @brief  Программа запуска тестов
 *
 * Программа парсит тесты в виде json-файлов, собирает набор и выполняет
 */

/*
 A simple bot playing Go
 Copyright (C) 2015 Ivanov Viktor

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

using namespace boost::unit_test;

/**
 * Функция парсинга тестового файла и регистрации тестов
 * @param argc Количество аргументов тестера
 * @param argv Аргументы тестера
 * @return Всегда NULL, так как все тесты регистрируются самостоятельно
 */
test_suite*
init_unit_test_suite( int argc, char* argv[] )
{
    return 0;
}
