/**
 * @file point.h
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

#ifndef CORE_POINT_H_
#define CORE_POINT_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cstddef>
#include "board_def.h"

namespace vpgo {

namespace core {

/**
 * Описывает пункт доски
 */
struct Point {
public:
	/**
	 * Возможные состояния пункта доски
	 */
	enum PointState {
		PointFree,  //!< Пункт свободен
		PointBlack, //!< Пункт занят Черными
		PointWhite  //!< Пункт занят Белыми
	};
private:
	/**
	 * Координаты
	 */
	int m_Coordinates;
	/**
	 * Состояние
	 */
	PointState m_State;
public:
	/**
	 * Стандартный конструктор
	 */
	Point();
	/**
	 * Конструктор по начальным значениям
	 *
	 * @param Coordinates Координаты
	 * @param State       Состояние
	 */
	Point(int Coordinates, PointState State);
	/**
	 * Конструктор по копии
	 *
	 * @param Value Копия
	 */
	explicit Point(const Point &Value);

	/**
	 * Получить координаты
	 *
	 * @return Координаты пункта
	 */
	inline int GetCoordinates() const {
		return m_Coordinates;
	}
	/**
	 * Установить координаты
	 *
	 * @param Coordinates Координаты пункта
	 */
	inline void SetCoordinates(int Coordinates) {
		m_Coordinates = Coordinates;
	}
	/**
	 * Получить состояние пункта
	 *
	 * @return Состояние пункта
	 */
	inline PointState GetState() const {
		return m_State;
	}
	/**
	 * Изменить состояние пункта
	 *
	 * @param State Состояние пункта
	 */
	inline void SetState(PointState State) {
		m_State = State;
	}
};

/**
 * Интерфейс управления итераторами для пунктов
 */
class PointIteratorControl {
private:
	/**
	 * Приватный конструктор копирования
	 *
	 * @param Value Копия
	 */
	explicit PointIteratorControl(const PointIteratorControl &Value) = default;
public:
	/**
	 * Основной конструктор
	 */
	PointIteratorControl() {
	}
	/**
	 *  Виртуальный деструктор
	 */
	virtual ~PointIteratorControl() {
	}

	/**
	 * Получает количество элментов
	 *
	 * @return Количество элементов
	 */
	virtual std::size_t GetSize() = 0;
	/**
	 * Получает ссылку на элемент по индексу
	 *
	 * @param Index Индекс элемента
	 * @return Экзмепляр элемента
	 */
	virtual Point &Deref(std::size_t Index) = 0;
	/**
	 * Проверяет на существование элемента по индексу
	 *
	 * @param Index Индекс элмента
	 * @return Истина, если элмент существует
	 */
	virtual bool Exists(std::size_t Index) = 0;
};

/**
 * Описывает базу итератора для пунктов
 */
class PointIteratorBase {
private:
	/**
	 * Управляющий объект
	 */
	PointIteratorControl *m_Control;
	/**
	 * Текущий индекс
	 */
	std::size_t m_Current;
	/**
	 * Флаг конца
	 */
	bool m_EndFlag;
protected:
	/**
	 * Получает экзмепляр объекта
	 *
	 * @return Экзмепляр объекта
	 */
	Point &Deref() const;
	/**
	 * Инкремент итератора
	 */
	void Increment();
	/**
	 * Декремент итератора
	 */
	void Decrement();
	/**
	 * Вычисляет расстояние между итераторами
	 *
	 * @param Value Итератор для вычисления
	 * @return Расстояние между итераторами
	 */
	int ComputeDistance(const PointIteratorBase &Value) const;
	/**
	 * Сравнивает итераторы
	 *
	 * @param Value Итератор для сравнения
	 * @return Истина, если итераторы идентичны
	 */
	bool IsEqual(const PointIteratorBase &Value) const;
	/**
	 * Копирует итератор
	 *
	 * @param Value Копия
	 */
	void Copy(const PointIteratorBase &Value);
public:
	/**
	 * Основной конструктор
	 */
	PointIteratorBase();
	/**
	 * Конструктор с начальными значениями
	 *
	 * @param Control Управляющий объект
	 * @param Index Начальный индекс
	 * @param EndFlag Флаг окончания
	 */
	explicit PointIteratorBase(PointIteratorControl *Control, std::size_t Index,
			bool EndFlag);
	/**
	 * Оператор равенства
	 *
	 * @param Value Итератор для сравнения
	 * @return Истина, если итераторы идентичны
	 */
	inline bool operator==(const PointIteratorBase &Value) const {
		return IsEqual(Value);
	}
	/**
	 * Оператор неравенства
	 *
	 * @param Value Итератор для сравнения
	 * @return Истина, если итераторы различны
	 */
	inline bool operator!=(const PointIteratorBase &Value) const {
		return !IsEqual(Value);
	}
	/**
	 * Оператор разности
	 *
	 * @param Value Итератор для сравнения
	 * @return Расстояние между итераторами
	 */
	inline int operator-(const PointIteratorBase &Value) const {
		return Value.ComputeDistance(*this);
	}
};

/**
 * Константный итератор для пунктов
 */
class PointIteratorConst: public PointIteratorBase {
	friend class PointSet;
public:
	/**
	 * Основной конструктор
	 */
	PointIteratorConst();
	/**
	 * Конструктор копирования
	 *
	 * @param Value Копия
	 */
	PointIteratorConst(const PointIteratorBase &Value);
private:
	/**
	 * Конструктор для внутреннего использования
	 *
	 * @param Control Управляющий объект
	 * @param Index Индекс
	 * @param EndFlag Флаг окончания
	 */
	explicit PointIteratorConst(PointIteratorControl *Control,
			std::size_t Index, bool EndFlag);
public:
	/**
	 * Оператор присвоения
	 *
	 * @param Value Итератор для присвоения
	 * @return Полученный экзмпляр
	 */
	PointIteratorConst &operator=(const PointIteratorBase &Value);
	/**
	 * Оператор инкремента справа
	 *
	 * @return Старый экзмпляр
	 */
	inline PointIteratorConst operator++(int) {
		PointIteratorConst Dummy(*this);
		++*this;
		return Dummy;
	}
	/**
	 * Оператор декремента справа
	 *
	 * @return Старый экземпляр
	 */
	inline PointIteratorConst operator--(int) {
		PointIteratorConst Dummy(*this);
		--*this;
		return Dummy;
	}
	/**
	 * Оператор декремента слева
	 *
	 * @return Полученный экзмепляр
	 */
	inline PointIteratorConst &operator--() {
		Decrement();
		return *this;
	}
	/**
	 * Оператор инкремента слева
	 *
	 * @return Полученный экзмепляр
	 */
	inline PointIteratorConst &operator++() {
		Increment();
		return *this;
	}
	/**
	 * Оператор разыменования
	 *
	 * @return Экзмепляр объекта
	 */
	const Point &operator*() const {
		return Deref();
	}
	/**
	 * Оператор разыменования по указателю
	 *
	 * @return Указатель на объект
	 */
	const Point *operator->() const {
		return &Deref();
	}
};

/**
 * Итератор для пунктов
 */
class PointIterator: public PointIteratorBase {
	friend class PointSet;
public:
	/**
	 * Основной конструктор
	 */
	PointIterator();
	/**
	 * Конструктор копирования
	 *
	 * @param Value Копия
	 */
	PointIterator(const PointIteratorBase &Value);
private:
	/**
	 * Конструктор для внутреннего использования
	 *
	 * @param Control Управляющий объект
	 * @param Index Индекс
	 * @param EndFlag Флаг окончания
	 */
	explicit PointIterator(PointIteratorControl *Control, std::size_t Index,
			bool EndFlag);
public:
	/**
	 * Оператор присвоения
	 *
	 * @param Value Итератор для присвоения
	 * @return Полученный экзмпляр
	 */
	PointIterator &operator=(const PointIteratorBase &Value);
	/**
	 * Оператор инкремента справа
	 *
	 * @return Старый экзмпляр
	 */
	inline PointIterator operator++(int) {
		PointIterator Dummy(*this);
		++*this;
		return Dummy;
	}
	/**
	 * Оператор декремента справа
	 *
	 * @return Старый экземпляр
	 */
	inline PointIterator operator--(int) {
		PointIterator Dummy(*this);
		--*this;
		return Dummy;
	}
	/**
	 * Оператор декремента слева
	 *
	 * @return Полученный экзмепляр
	 */
	inline PointIterator &operator--() {
		Decrement();
		return *this;
	}
	/**
	 * Оператор инкремента слева
	 *
	 * @return Полученный экзмепляр
	 */
	inline PointIterator &operator++() {
		Increment();
		return *this;
	}
	/**
	 * Оператор разыменования
	 *
	 * @return Экзмепляр объекта
	 */
	Point &operator*() const {
		return Deref();
	}
	/**
	 * Оператор разыменования по указателю
	 *
	 * @return Указатель на объект
	 */
	Point *operator->() const {
		return &Deref();
	}
};

/**
 * Набор пунктов
 */
class PointSet {
public:
	/**
	 * Итератор
	 */
	typedef PointIterator iterator;
	/**
	 * Константный итератор
	 */
	typedef PointIterator const_iterator;
private:
	/**
	 * Управляющий объект
	 */
	PointIteratorControl *m_Control;
public:
	/**
	 * Основной конструктор
	 */
	PointSet();
	/**
	 * Конструктор копирования
	 *
	 * @param Value Копия
	 */
	PointSet(const PointSet &Value);
	/**
	 * Конструктор по управляющему объекту
	 *
	 * @param Control Управляющий объект
	 */
	PointSet(PointIteratorControl *Control);
	/**
	 * Начало набора (константа)
	 *
	 * @return Итератор для начала (константный)
	 */
	const_iterator begin() const;
	/**
	 * Конец набора (константа)
	 *
	 * @return Итератор для конца (константный)
	 */
	const_iterator end() const;
	/**
	 * Начало набора (константа)
	 *
	 * @return Итератор для начала (константный)
	 */
	inline const_iterator cbegin() const {
		return begin();
	}
	/**
	 * Конец набора (константа)
	 *
	 * @return Итератор для конца (константный)
	 */
	inline const_iterator cend() const {
		return end();
	}
	/**
	 * Начало набора
	 *
	 * @return Итератор для начала
	 */
	iterator begin();
	/**
	 * Конец набора
	 *
	 * @return Итератор для конца
	 */
	iterator end();
	/**
	 * Размер набора
	 *
	 * @return Размер
	 */
	std::size_t size();
	/**
	 * Флаг пустоты
	 *
	 * @return Истина, если набор пустой
	 */
	bool empty();
	/**
	 * Оператор разыменования по индексу
	 *
	 * @param Index Индекс
	 * @return Ссылка на объект
	 */
	Point &operator[](std::size_t Index);
	/**
	 * Разыменовывает по индексу
	 *
	 * @param Index Индекс
	 * @return Ссылка на объект
	 */
	inline Point &at(std::size_t Index) {
		return (*this)[Index];
	}
	/**
	 *  Оператор разыменования по индексу (константа)
	 *
	 * @param Index Индекс
	 * @return Ссылка на константный объект
	 */
	const Point &operator[](std::size_t Index) const;
	/**
	 * Разыменовывает по индексу (константа)
	 *
	 * @param Index Индекс
	 * @return Ссылка на константный объект
	 */
	inline const Point &at(std::size_t Index) const {
		return (*this)[Index];
	}

};

}

}

#endif
