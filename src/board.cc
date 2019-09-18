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
	m_Groups[to].libs += m_Groups[from].libs;
	m_Groups[to].stones += m_Groups[from].stones;
}

/**
 * Reduce number of liberties of a group
 *
 * @param targetOffset Group to reduce libs
 * @param currentOffset Group to increase libs
 * @param groups Group set
 * @param maxGroup Current max group
 */
void Board::reduceLiberties(std::size_t targetOffset, std::size_t currentOffset,
    std::unordered_set<std::size_t> *groups, std::size_t *maxGroup) {
	if (m_State[targetOffset] == PlayerColour::NONE) {
		m_Groups[currentOffset].libs++;
	} else {
		std::size_t targetGroup = getGroupLocation(targetOffset);
		auto i = groups->insert(targetGroup);
		if (!i.second) {
			return;
		}
		m_Groups[targetGroup].libs--;
		if (m_State[targetGroup] == m_State[currentOffset] &&
		    m_Groups[*maxGroup].stones < m_Groups[targetGroup].stones) {
			*maxGroup = targetGroup;
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
	assert(x < m_Size && y < m_Size);
	assert(offset == coordsToOffset(x, y));
	assert(m_State[offset] == PlayerColour::WHITE ||
	       m_State[offset] == PlayerColour::BLACK);
	PlayerColour colour = m_State[offset];
	m_State[offset] = PlayerColour::NONE;
	if (x > 0) {
		increaseLiberties(offset - 1, x - 1, y, colour);
	}
	if (y > 0) {
		increaseLiberties(offset - m_Size, x, y - 1, colour);
	}
	if (x < m_Size - 1) {
		increaseLiberties(offset + 1, x + 1, y, colour);
	}
	if (y < m_Size - 1) {
		increaseLiberties(offset + m_Size, x, y + 1, colour);
	}
}

/**
 * Increase liberties or remove group
 *
 * @param offset Offset
 * @param x X coord
 * @param y Y coord
 * @param colour Current colour
 */
void Board::increaseLiberties(
    std::size_t offset, std::size_t x, std::size_t y, PlayerColour colour) {
    if (m_State[offset] == colour) {
        removeGroup(offset, x, y);
    } else if (m_State[offset] == colour.inverse()) {
        std::size_t group = getGroupLocation(offset);
        m_Groups[group].libs++;
    }
}

/**
 * Play a move
 *
 * @param x X coord
 * @param y Y coord
 * @param colour Stone colour
 */
void Board::playMove(std::size_t x, std::size_t y, PlayerColour colour) {
	assert(x < m_Size && y < m_Size);
	assert(colour == PlayerColour::BLACK || colour == PlayerColour::WHITE);
	const std::size_t offset = coordsToOffset(x, y);
	assert(m_State[offset] == PlayerColour::NONE);

	std::unordered_set<std::size_t> groups;
	std::size_t maxGroup = offset;
	m_Groups[offset].libs = 0;
	m_Groups[offset].stones = 1;
	m_GroupRelation[offset] = offset;
	m_State[offset] = colour;
	if (x > 0) {
		reduceLiberties(offset - 1, offset, &groups, &maxGroup);
	}
	if (y > 0) {
		reduceLiberties(offset - m_Size, offset, &groups, &maxGroup);
	}
	if (x < m_Size - 1) {
		reduceLiberties(offset + 1, offset, &groups, &maxGroup);
	}
	if (y < m_Size - 1) {
		reduceLiberties(offset + m_Size, offset, &groups, &maxGroup);
	}
	for (auto &group : groups) {
		if (group != maxGroup && m_State[group] == colour) {
			mergeGroups(group, maxGroup);
		}
	}
	if (maxGroup != offset) {
		mergeGroups(offset, maxGroup);
	}
	for (auto &group : groups) {
		if (m_State[group] != colour && m_Groups[group].libs == 0) {
			auto pos = offsetToCoords(group);
			removeGroup(group, pos.first, pos.second);
		}
	}
}
