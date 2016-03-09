/**
 * @file point.cpp
 * @brief Описывает пункт доски
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

#include "point.h"

using namespace ::vpgo;
using namespace ::vpgo::core;

/* Point */

/**
 * Стандартный конструктор
 */
vpgo::core::Point::Point() :
		m_Coordinates(0), m_State(PointFree) {
}

/**
 * Конструктор по начальным значениям
 *
 * @param Coordinates Координаты
 * @param State       Состояние
 */
vpgo::core::Point::Point(int Coordinates, PointState State) :
		m_Coordinates(Coordinates), m_State(State) {
}

/**
 * Конструктор по копии
 *
 * @param Value Копия
 */
vpgo::core::Point::Point(const Point &Value) :
		m_Coordinates(Value.m_Coordinates), m_State(Value.m_State) {
}
