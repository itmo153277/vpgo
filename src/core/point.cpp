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
#include <cassert>

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

/* PointIteratorBase */

/**
 * Основной конструктор
 */
vpgo::core::PointIteratorBase::PointIteratorBase() :
		m_Control(NULL), m_Current(0), m_EndFlag(true) {
}

/**
 * Конструктор с начальными значениями
 *
 * @param Control Управляющий объект
 * @param Index Начальный индекс
 * @param EndFlag Флаг окончания
 */
vpgo::core::PointIteratorBase::PointIteratorBase(PointIteratorControl *Control,
		std::size_t Index, bool EndFlag) :
		m_Control(Control), m_Current(Index), m_EndFlag(
				EndFlag || !Control->Exists(Index)) {
}

/**
 * Получает экзмепляр объекта
 *
 * @return Экзмепляр объекта
 */
Point &vpgo::core::PointIteratorBase::Deref() const {
	assert(m_Control != NULL && m_Control->Exists(m_Current));

	return m_Control->Deref(m_Current);
}

/**
 * Инкремент итератора
 */
void vpgo::core::PointIteratorBase::Increment() {
	assert(m_Control != NULL);

	++m_Current;
	if (!m_Control->Exists(m_Current)) {
		m_EndFlag = true;
	}
}

/**
 * Декремент итератора
 */
void vpgo::core::PointIteratorBase::Decrement() {
	if (m_EndFlag) {
		m_Current = m_Control->GetSize();
	}
	if (m_Current > 0) {
		--m_Current;
	}
}

/**
 * Вычисляет расстояние между итераторами
 *
 * @param Value Итератор для вычисления
 * @return Расстояние между итераторами
 */
int vpgo::core::PointIteratorBase::ComputeDistance(
		const PointIteratorBase &Value) const {
	assert(m_Control == Value.m_Control && m_Control != NULL);

	if (m_EndFlag && Value.m_EndFlag) {
		return 0;
	}
	size_t l = !Value.m_EndFlag ? Value.m_Current : m_Control->GetSize(), r =
			!m_EndFlag ? m_Current : m_Control->GetSize();
	return int(l - r);
}

/**
 * Сравнивает итераторы
 *
 * @param Value Итератор для сравнения
 * @return Истина, если итераторы идентичны
 */
bool vpgo::core::PointIteratorBase::IsEqual(
		const PointIteratorBase &Value) const {
	assert(m_Control == Value.m_Control && m_Control != NULL);

	return (Value.m_EndFlag && m_EndFlag) || Value.m_Current == m_Current;
}

/**
 * Копирует итератор
 *
 * @param Value Копия
 */
void vpgo::core::PointIteratorBase::Copy(const PointIteratorBase &Value) {
	m_Control = Value.m_Control;
	m_Current = Value.m_Current;
	m_EndFlag = Value.m_EndFlag;
}

/* PointIterator */

/**
 * Основной конструктор
 */
vpgo::core::PointIterator::PointIterator() :
		PointIteratorBase() {
}

/**
 * Конструктор копирования
 *
 * @param Value Копия
 */
vpgo::core::PointIterator::PointIterator(const PointIteratorBase &Value) :
		PointIteratorBase(Value) {
}

/**
 * Конструктор для внутреннего использования
 *
 * @param Control Управляющий объект
 * @param Index Индекс
 * @param EndFlag Флаг окончания
 */
vpgo::core::PointIterator::PointIterator(PointIteratorControl *Control,
		std::size_t Index, bool EndFlag) :
		PointIteratorBase(Control, Index, EndFlag) {
}

/**
 * Оператор присвоения
 *
 * @param Value Итератор для присвоения
 * @return Полученный экзмпляр
 */
PointIterator &vpgo::core::PointIterator::operator=(
		const PointIteratorBase &Value) {
	Copy(Value);
	return *this;
}

/* PointIteratorConst */

/**
 * Основной конструктор
 */
vpgo::core::PointIteratorConst::PointIteratorConst() :
		PointIteratorBase() {
}

/**
 * Конструктор копирования
 *
 * @param Value Копия
 */
vpgo::core::PointIteratorConst::PointIteratorConst(
		const PointIteratorBase &Value) :
		PointIteratorBase(Value) {
}

/**
 * Конструктор для внутреннего использования
 *
 * @param Control Управляющий объект
 * @param Index Индекс
 * @param EndFlag Флаг окончания
 */
vpgo::core::PointIteratorConst::PointIteratorConst(
		PointIteratorControl *Control, std::size_t Index, bool EndFlag) :
		PointIteratorBase(Control, Index, EndFlag) {
}

/**
 * Оператор присвоения
 *
 * @param Value Итератор для присвоения
 * @return Полученный экзмпляр
 */
PointIteratorConst &vpgo::core::PointIteratorConst::operator=(
		const PointIteratorBase &Value) {
	Copy(Value);
	return *this;
}

/* PointSet */

/**
 * Основной конструктор
 */
vpgo::core::PointSet::PointSet() :
		m_Control(NULL) {
}

/**
 * Конструктор копирования
 *
 * @param Value Копия
 */
vpgo::core::PointSet::PointSet(const PointSet &Value) :
		m_Control(Value.m_Control) {
}

/**
 * Конструктор по управляющему объекту
 *
 * @param Control Управляющий объект
 */
vpgo::core::PointSet::PointSet(PointIteratorControl *Control) :
		m_Control(Control) {
}

/**
 * Начало набора (константа)
 *
 * @return Итератор для начала (константный)
 */
vpgo::core::PointSet::const_iterator vpgo::core::PointSet::begin() const {
	return const_iterator(m_Control, 0, false);
}

/**
 * Конец набора (константа)
 *
 * @return Итератор для конца (константный)
 */
vpgo::core::PointSet::const_iterator vpgo::core::PointSet::end() const {
	return const_iterator(m_Control, 0, true);
}

/**
 * Начало набора
 *
 * @return Итератор для начала
 */
vpgo::core::PointSet::iterator vpgo::core::PointSet::begin() {
	return iterator(m_Control, 0, false);
}

/**
 * Конец набора
 *
 * @return Итератор для конца
 */
vpgo::core::PointSet::iterator vpgo::core::PointSet::end() {
	return iterator(m_Control, 0, true);
}

/**
 * Размер набора
 *
 * @return Размер
 */
std::size_t vpgo::core::PointSet::size() {
	assert(m_Control != NULL);

	return m_Control->GetSize();
}

/**
 * Флаг пустоты
 *
 * @return Истина, если набор пустой
 */
bool vpgo::core::PointSet::empty() {
	assert(m_Control != NULL);

	return !m_Control->Exists(0);
}

/**
 * Оператор разыменования по индексу
 *
 * @param Index Индекс
 * @return Ссылка на объект
 */
Point &vpgo::core::PointSet::operator[](std::size_t Index) {
	assert(m_Control != NULL && m_Control->Exists(Index));

	return m_Control->Deref(Index);
}

/**
 *  Оператор разыменования по индексу (константа)
 *
 * @param Index Индекс
 * @return Ссылка на константный объект
 */
const Point &vpgo::core::PointSet::operator[](std::size_t Index) const {
	assert(m_Control != NULL && m_Control->Exists(Index));

	return m_Control->Deref(Index);
}
