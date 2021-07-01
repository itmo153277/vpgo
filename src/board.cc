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
#include "src/colour.hpp"

/**
 * Merge two groups into one
 *
 * @param from Smaller group
 * @param to Larger group
 */
void Board::mergeGroups(board_offset_t from, board_offset_t to) {
	m_GroupRelation[from] = to;
	m_Groups[to].edges += m_Groups[from].edges;
	m_Groups[to].stones += m_Groups[from].stones;
	m_Groups[to].hash ^= m_Groups[from].hash;
}

/**
 * Play a move
 *
 * @param x X coord
 * @param y Y coord
 * @param colour Stone colour
 */
void Board::playMove(board_coord_t x, board_coord_t y, PlayerColour colour) {
	assert(x < m_Size);
	assert(y < m_Size);
	assert(colour == PlayerColour::BLACK || colour == PlayerColour::WHITE);
	const board_offset_t offset = coordsToOffset(x, y);
	assert(m_State[offset] == PlayerColour::NONE);
	m_State[offset] = colour;
	m_Groups[offset] = {
	    0, 1, HashValues::getInstance()->getValue(offset, colour)};
	m_Hash ^= m_Groups[offset].hash;
	m_GroupRelation[offset] = offset;
	++m_Stones;
	board_offset_t maxGroup = offset;
	board_offset_t neighbours[5] = {offset};
	int totalNeighbours = 1;
	for (auto [tx, ty, toffset] : BoardTraverse(x, y, offset, m_Size)) {
		if (m_State[toffset] == PlayerColour::NONE) {
			m_Groups[offset].edges++;
		} else {
			const board_offset_t group = getGroupLocation(toffset);
			m_Groups[group].edges--;
			if (m_State[group] == colour) {
				for (int i = 1; i < totalNeighbours; ++i) {
					if (neighbours[i] == group) {
						goto foundNeighbour;
					}
				}
				neighbours[totalNeighbours++] = group;
			foundNeighbour:
				if (m_Groups[group].stones > m_Groups[maxGroup].stones) {
					maxGroup = group;
				}
			} else if (m_Groups[group].edges == 0) {
				removeGroup(toffset, tx, ty);
			}
		}
	}
	for (int i = 0; i < totalNeighbours; ++i) {
		if (neighbours[i] != maxGroup) {
			mergeGroups(neighbours[i], maxGroup);
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
void Board::removeGroup(
    board_offset_t offset, board_coord_t x, board_coord_t y) {
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
			const board_offset_t group = getGroupLocation(toffset);
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
bool Board::isSuicide(
    board_coord_t x, board_coord_t y, PlayerColour colour) const {
	assert(x < m_Size);
	assert(y < m_Size);
	std::pair<board_offset_t, int> sameNeighbours[4];
	int totalSameNeighbours = 0;
	std::pair<board_offset_t, int> oppositeNeighbours[4];
	int totalOppositeNeighbours = 0;
	for (auto [tx, ty, toffset] :
	    BoardTraverse(x, y, coordsToOffset(x, y), m_Size)) {
		if (m_State[toffset] == PlayerColour::NONE) {
			return false;
		}
		const board_offset_t group = getGroupLocation(toffset);
		if (m_State[group] == colour) {
			int neighbour = -1;
			for (int i = 0; i < totalSameNeighbours; ++i) {
				if (sameNeighbours[i].first == group) {
					neighbour = i;
					break;
				}
			}
			if (neighbour == -1) {
				neighbour = totalSameNeighbours++;
				sameNeighbours[neighbour] = {group, m_Groups[group].edges};
			}
			--sameNeighbours[neighbour].second;
		} else {
			int neighbour = -1;
			for (int i = 0; i < totalOppositeNeighbours; ++i) {
				if (oppositeNeighbours[i].first == group) {
					neighbour = i;
					break;
				}
			}
			if (neighbour == -1) {
				neighbour = totalOppositeNeighbours++;
				oppositeNeighbours[neighbour] = {group, m_Groups[group].edges};
			}
			--oppositeNeighbours[neighbour].second;
			if (oppositeNeighbours[neighbour].second == 0) {
				return false;
			}
		}
	}
	for (int i = 0; i < totalSameNeighbours; ++i) {
		if (sameNeighbours[i].second > 0) {
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
std::pair<int, int> Board::countPoints() const {
	std::vector<bool> visited(m_State.size());
	std::vector<std::tuple<board_coord_t, board_coord_t, board_offset_t>> queue(
	    m_State.size());
	int black = 0;
	int white = 0;
	board_offset_t offset = 0;
	for (board_coord_t y = 0; y < m_Size; ++y) {
		for (board_coord_t x = 0; x < m_Size; ++x, ++offset) {
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
				int count = 0;
				int queueSize = 1;
				queue[0] = {x, y, offset};
				while (queueSize > 0) {
					auto [qx, qy, qoffset] = queue[0];
					queue[0] = queue[--queueSize];
					++count;
					for (auto [tx, ty, toffset] :
					    BoardTraverse(qx, qy, qoffset, m_Size)) {
						if (m_State[toffset] == PlayerColour::NONE) {
							if (!visited[toffset]) {
								visited[toffset] = true;
								queue[queueSize++] = {tx, ty, toffset};
							}
						} else if (colour == PlayerColour::NONE) {
							colour = m_State[toffset];
						} else if (colour != m_State[toffset]) {
							colour = PlayerColour::NEUTRAL;
						}
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
hash_t Board::preComputeHash(
    board_coord_t x, board_coord_t y, PlayerColour colour) const {
	assert(x < m_Size);
	assert(y < m_Size);
	board_offset_t offset = coordsToOffset(x, y);
	hash_t currentHash =
	    m_Hash ^ HashValues::getInstance()->getValue(offset, colour);
	std::pair<board_offset_t, int> neighbours[4];
	int totalNeighbours = 0;
	for (auto [tx, ty, toffset] :
	    BoardTraverse(x, y, coordsToOffset(x, y), m_Size)) {
		if (m_State[toffset] != colour.invert()) {
			continue;
		}
		const board_offset_t group = getGroupLocation(toffset);
		int neighbour = -1;
		for (int i = 0; i < totalNeighbours; ++i) {
			if (neighbours[i].first == group) {
				neighbour = i;
				break;
			}
		}
		if (neighbour == -1) {
			neighbour = totalNeighbours++;
			neighbours[neighbour] = {group, m_Groups[group].edges};
		}
		--neighbours[neighbour].second;
		if (neighbours[neighbour].second == 0) {
			currentHash ^= m_Groups[group].hash;
		}
	}
	return currentHash;
}

bool Board::isEyeLike(
    board_coord_t x, board_coord_t y, PlayerColour colour) const {
	assert(x < m_Size);
	assert(y < m_Size);
	assert(colour == PlayerColour::BLACK || colour == PlayerColour::WHITE);
	board_offset_t offset = coordsToOffset(x, y);
	assert(m_State[offset] == PlayerColour::NONE);
	std::pair<board_offset_t, int> neighbours[4];
	int totalNeighbours = 0;
	for (auto [tx, ty, toffset] : BoardTraverse(x, y, offset, m_Size)) {
		if (m_State[toffset] != colour) {
			return false;
		}
		auto group = getGroupLocation(toffset);
		int neighbour = -1;
		for (int i = 0; i < totalNeighbours; ++i) {
			if (group == neighbours[i].first) {
				neighbour = i;
				break;
			}
		}
		if (neighbour == -1) {
			neighbour = totalNeighbours++;
			neighbours[neighbour] = {group, m_Groups[group].edges};
		}
		--neighbours[neighbour].second;
		if (neighbours[neighbour].second == 0) {
			return false;
		}
	}
	return true;
}

bool Board::isCapture(
    board_coord_t x, board_coord_t y, PlayerColour colour) const {
	assert(x < m_Size);
	assert(y < m_Size);
	assert(colour == PlayerColour::BLACK || colour == PlayerColour::WHITE);
	board_offset_t offset = coordsToOffset(x, y);
	assert(m_State[offset] == PlayerColour::NONE);
	std::pair<board_offset_t, int> neighbours[4];
	int totalNeighbours = 0;
	for (auto [tx, ty, toffset] : BoardTraverse(x, y, offset, m_Size)) {
		if (m_State[toffset] != colour.invert()) {
			continue;
		}
		auto group = getGroupLocation(toffset);
		int neighbour = -1;
		for (int i = 0; i < totalNeighbours; ++i) {
			if (group == neighbours[i].first) {
				neighbour = i;
				break;
			}
		}
		if (neighbour == -1) {
			neighbour = totalNeighbours++;
			neighbours[neighbour] = {group, m_Groups[group].edges};
		}
		--neighbours[neighbour].second;
		if (neighbours[neighbour].second == 0) {
			return true;
		}
	}
	return false;
}

bool Board::isSelfAtari(
    board_coord_t x, board_coord_t y, PlayerColour colour) const {
	assert(x < m_Size);
	assert(y < m_Size);
	assert(colour == PlayerColour::BLACK || colour == PlayerColour::WHITE);
	board_offset_t offset = coordsToOffset(x, y);
	assert(m_State[offset] == PlayerColour::NONE);
	if (isCapture(x, y, colour)) {
		return false;
	}
	board_offset_t neighbours[4];
	int totalNeighbours = 0;
	int totalStones = 1;
	int totalEdges = 0;
	for (auto [tx, ty, toffset] : BoardTraverse(x, y, offset, m_Size)) {
		if (m_State[toffset] == PlayerColour::NONE) {
			totalEdges++;
		}
		if (m_State[toffset] != colour) {
			continue;
		}
		auto group = getGroupLocation(toffset);
		for (int i = 0; i < totalNeighbours; ++i) {
			if (group == neighbours[i]) {
				goto foundNeighbour;
			}
		}
		neighbours[totalNeighbours++] = group;
		totalStones += m_Groups[group].stones;
		totalEdges += m_Groups[group].edges;
	foundNeighbour:
		--totalEdges;
	}
	// Only consider group larger than 3 stones (sacrificing 4 and more to kill
	// is not realistic during playouts)
	// 3 and 4 edges are not interesting cases and can be allowed
	// TODO(me): 2 edges require additional check for actual dame points!
	//           Also, include 3 edges as well
	return totalStones > 3 && totalEdges < 3;
}
