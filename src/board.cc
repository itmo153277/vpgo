/**
 * @file board.cc
 * Implementation for Board class
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

#include "board.hpp"
#include <cassert>
#include <cstddef>
#include <unordered_set>

/**
 * Merge two groups into one
 *
 * @param from Smaller group
 * @param to Larger group
 */
void Board::mergeGroups(std::size_t from, std::size_t to) {
	m_GroupRelation[from] = to;
	for (std::size_t i = 0; i < Direction::COUNT; ++i) {
		m_Groups[to].edges[i] += m_Groups[from].edges[i];
	}
	m_Groups[to].stones += m_Groups[from].stones;
}

/**
 * Play a move
 *
 * @param x X coord
 * @param y Y coord
 * @param colour Stone colour
 */
void Board::playMove(std::size_t x, std::size_t y, PlayerColour colour) {
	assert(x < m_Size);
	assert(y < m_Size);
	assert(colour == PlayerColour::BLACK || colour == PlayerColour::WHITE);
	const std::size_t offset = coordsToOffset(x, y);
	assert(m_State[offset] == PlayerColour::NONE);
	m_State[offset] = colour;
	m_Groups[offset] = {{}, 1};
	m_GroupRelation[offset] = offset;
	std::size_t maxGroup = offset;
	std::unordered_set<std::size_t> neighbours;
	neighbours.insert(offset);
	for (auto [tx, ty, toffset, direction] :
	    BoardTraverse(x, y, offset, m_Size)) {
		if (m_State[toffset] == PlayerColour::NONE) {
			m_Groups[offset].edges[direction]++;
		} else {
			const std::size_t group = getGroupLocation(toffset);
			m_Groups[group].edges[direction.invert()]--;
			if (m_State[group] == colour) {
				neighbours.insert(group);
				if (m_Groups[group].stones > m_Groups[maxGroup].stones) {
					maxGroup = group;
				}
			} else {
				bool empty = true;
				for (std::size_t i = 0; i < Direction::COUNT && empty; ++i) {
					empty = m_Groups[group].edges[i] == 0;
				}
				if (empty) {
					removeGroup(toffset, tx, ty);
				}
			}
		}
	}
	for (auto group : neighbours) {
		if (group != maxGroup) {
			mergeGroups(group, maxGroup);
		}
	}
}

/**
 * Remove group of stones
 *
 * @param offset Offset
 * @param x X coord
 * @param y Y coord
 */
void Board::removeGroup(std::size_t offset, std::size_t x, std::size_t y) {
	assert(x < m_Size);
	assert(y < m_Size);
	assert(offset == coordsToOffset(x, y));
	const PlayerColour colour = m_State[offset];
	assert(colour == PlayerColour::WHITE || colour == PlayerColour::BLACK);
	m_State[offset] = PlayerColour::NONE;
	for (auto [tx, ty, toffset, direction] :
	    BoardTraverse(x, y, offset, m_Size)) {
		if (m_State[toffset] == PlayerColour::NONE) {
			continue;
		}
		if (m_State[toffset] == colour) {
			removeGroup(toffset, tx, ty);
		} else {
			const std::size_t group = getGroupLocation(toffset);
			m_Groups[group].edges[direction.invert()]++;
		}
	}
}
