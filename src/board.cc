/**
 * @file src/board.cc
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
#include <unordered_map>

/**
 * Merge two groups into one
 *
 * @param from Smaller group
 * @param to Larger group
 */
void Board::mergeGroups(std::size_t from, std::size_t to) {
	m_GroupRelation[from] = to;
	m_Groups[to].edges += m_Groups[from].edges;
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
	m_Groups[offset] = {0, 1};
	m_GroupRelation[offset] = offset;
	std::size_t maxGroup = offset;
	std::unordered_set<std::size_t> neighbours;
	neighbours.insert(offset);
	for (auto [tx, ty, toffset] : BoardTraverse(x, y, offset, m_Size)) {
		if (m_State[toffset] == PlayerColour::NONE) {
			m_Groups[offset].edges++;
		} else {
			const std::size_t group = getGroupLocation(toffset);
			m_Groups[group].edges--;
			if (m_State[group] == colour) {
				neighbours.insert(group);
				if (m_Groups[group].stones > m_Groups[maxGroup].stones) {
					maxGroup = group;
				}
			} else if (m_Groups[group].edges == 0) {
				removeGroup(toffset, tx, ty);
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
	for (auto [tx, ty, toffset] : BoardTraverse(x, y, offset, m_Size)) {
		if (m_State[toffset] == PlayerColour::NONE) {
			continue;
		}
		if (m_State[toffset] == colour) {
			removeGroup(toffset, tx, ty);
		} else {
			const std::size_t group = getGroupLocation(toffset);
			m_Groups[group].edges++;
		}
	}
}

/**
 * Check if move is a suicide
 *
 * @param x X coord
 * @param y Y coord
 * @param colour Stone colour
 * @return True if suicide
 */
bool Board::isSuicide(std::size_t x, std::size_t y, PlayerColour colour) {
	std::unordered_map<std::size_t, std::size_t> sameNeightbours;
	std::unordered_map<std::size_t, std::size_t> oppositeNeightbours;
	for (auto [tx, ty, toffset] :
	    BoardTraverse(x, y, coordsToOffset(x, y), m_Size)) {
		if (m_State[toffset] == PlayerColour::NONE) {
			return false;
		}
		const std::size_t group = getGroupLocation(toffset);
		if (m_State[group] == colour) {
			auto neightbour =
			    sameNeightbours.insert({group, m_Groups[group].edges}).first;
			--neightbour->second;
		} else {
			auto neightbour =
			    oppositeNeightbours.insert({group, m_Groups[group].edges})
			        .first;
			--neightbour->second;
			if (neightbour->second == 0) {
				return false;
			}
		}
	}
	for (auto [group, edges] : sameNeightbours) {
		if (edges > 0) {
			return false;
		}
	}
	return true;
}
