/**
 * @file src/board.cc
 * Implementation for Board class
 */
/*
    Simple Go engine
    Copyright (C) 2019-2021 Ivanov Viktor

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
#include <queue>

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
	m_Hash ^= HashValues::getInstance()->getValue(offset, colour);
	m_State[offset] = colour;
	m_Groups[offset] = {0, 1};
	m_GroupRelation[offset] = offset;
	++m_Stones;
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
	m_Hash ^= HashValues::getInstance()->getValue(offset, colour);
	m_State[offset] = PlayerColour::NONE;
	--m_Stones;
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
bool Board::isSuicide(std::size_t x, std::size_t y, PlayerColour colour) const {
	assert(x < m_Size);
	assert(y < m_Size);
	std::unordered_map<std::size_t, std::size_t> sameNeighbours;
	std::unordered_map<std::size_t, std::size_t> oppositeNeighbours;
	for (auto [tx, ty, toffset] :
	    BoardTraverse(x, y, coordsToOffset(x, y), m_Size)) {
		if (m_State[toffset] == PlayerColour::NONE) {
			return false;
		}
		const std::size_t group = getGroupLocation(toffset);
		if (m_State[group] == colour) {
			auto neighbour =
			    sameNeighbours.insert({group, m_Groups[group].edges}).first;
			--neighbour->second;
		} else {
			auto neighbour =
			    oppositeNeighbours.insert({group, m_Groups[group].edges})
			        .first;
			--neighbour->second;
			if (neighbour->second == 0) {
				return false;
			}
		}
	}
	for (auto [group, edges] : sameNeighbours) {
		if (edges > 0) {
			return false;
		}
	}
	return true;
}

/**
 * Count points
 *
 * @return Pair {Black, White}
 */
std::pair<std::size_t, std::size_t> Board::countPoints() const {
	std::vector<bool> visited(m_State.size());
	std::vector<bool> traversed(m_State.size());
	std::size_t black = 0;
	std::size_t white = 0;
	for (std::size_t y = 0, offset = 0; y < m_Size; ++y) {
		for (std::size_t x = 0; x < m_Size; ++x, ++offset) {
			if (visited[offset]) {
				continue;
			}
			visited[offset] = true;
			if (m_State[offset] == PlayerColour::BLACK) {
				++black;
			} else if (m_State[offset] == PlayerColour::WHITE) {
				++white;
			} else {
				PlayerColour colour = PlayerColour::NONE;
				std::size_t count = 0;
				std::queue<std::tuple<std::size_t, std::size_t, std::size_t>>
				    queue;
				queue.push({x, y, offset});
				while (queue.size() > 0) {
					auto [qx, qy, qoffset] = queue.front();
					queue.pop();
					if (traversed[qoffset]) {
						continue;
					}
					if (m_State[qoffset] == PlayerColour::NONE) {
						++count;
						for (auto [tx, ty, toffset] :
						    BoardTraverse(qx, qy, qoffset, m_Size)) {
							queue.push({tx, ty, toffset});
						}
						visited[qoffset] = true;
						traversed[qoffset] = true;
						continue;
					}
					if (colour == PlayerColour::NONE) {
						colour = m_State[qoffset];
					} else if (colour != m_State[qoffset]) {
						colour = PlayerColour::NEUTRAL;
					}
				}
				if (colour == PlayerColour::BLACK) {
					black += count;
				} else if (colour == PlayerColour::WHITE) {
					white += count;
				}
			}
		}
	}
	return {black, white};
}

/**
 * Pre-computes hash
 *
 * @param x X coord
 * @param y Y coord
 * @param colour Stone colour
 * @return Hash value
 */
std::uint_least64_t Board::preComputeHash(
    std::size_t x, std::size_t y, PlayerColour colour) const {
	assert(x < m_Size);
	assert(y < m_Size);
	std::size_t offset = coordsToOffset(x, y);
	std::uint_least64_t currentHash =
	    m_Hash ^ HashValues::getInstance()->getValue(offset, colour);
	std::unordered_map<std::size_t, std::size_t> neighbours;
	std::queue<std::tuple<std::size_t, std::size_t, std::size_t>>
	    stonesToRemove;
	std::unordered_set<std::size_t> removedStones;
	for (auto [tx, ty, toffset] :
	    BoardTraverse(x, y, coordsToOffset(x, y), m_Size)) {
		if (m_State[toffset] != colour.invert()) {
			continue;
		}
		const std::size_t group = getGroupLocation(toffset);
		auto neighbour =
		    neighbours.insert({group, m_Groups[group].edges}).first;
		--neighbour->second;
		if (neighbour->second == 0) {
			stonesToRemove.push({tx, ty, toffset});
		}
	}
	while (stonesToRemove.size() > 0) {
		auto [qx, qy, qoffset] = stonesToRemove.front();
		stonesToRemove.pop();
		bool skip = !removedStones.insert(qoffset).second;
		if (skip) {
			continue;
		}
		currentHash ^=
		    HashValues::getInstance()->getValue(qoffset, colour.invert());
		for (auto [tx, ty, toffset] : BoardTraverse(qx, qy, qoffset, m_Size)) {
			if (m_State[toffset] != colour.invert()) {
				continue;
			}
			stonesToRemove.push({tx, ty, toffset});
		}
	}
	return currentHash;
}
