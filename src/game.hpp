/**
 * @file src/game.hpp
 * Game
 */
/*
    Simple Go engine
    Copyright (C) 2021 Ivanov Viktor

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

#ifndef SRC_GAME_HPP_
#define SRC_GAME_HPP_

#include <unordered_set>
#include <cstdint>
#include "vpgo.hpp"
#include "colour.hpp"
#include "hash.hpp"
#include "board.hpp"

/**
 * Move type
 */
using game_move_t = int;

/**
 * Game
 */
class Game {
public:
	/**
	 * Pass move
	 */
	static constexpr game_move_t PASS = -1;
	/**
	 * Resign move
	 */
	static constexpr game_move_t RESIGN = -2;

private:
	/**
	 * Game board
	 */
	Board m_Board;
	/**
	 * Komi
	 */
	int m_Komi;
	/**
	 * History of moves
	 */
	std::unordered_set<hash_t> m_History;
	/**
	 * Winner
	 */
	PlayerColour m_Winner;
	/**
	 * Flag for pass
	 */
	bool m_LastMoveWasPass;

public:
	/**
	 * Deleted default ctor
	 */
	Game() = delete;
	/**
	 * Construct a new Game object
	 *
	 * @param size Board size
	 */
	Game(board_size_t size, int komi)
	    : m_Board(size)
	    , m_Komi(komi)
	    , m_History{HashValues::getInstance()->getInitialValue()}
	    , m_Winner{PlayerColour::NONE}
	    , m_LastMoveWasPass{false} {
	}

	/**
	 * Check whether move is illegal
	 *
	 * @param move Move
	 * @param col Player colour
	 * @return true if illegal
	 */
	bool isIllegal(game_move_t move, PlayerColour col) const {
		if (move == RESIGN) {
			return false;
		}
		if (move == PASS) {
			return false;
		}
		if (m_Board.getValue(move) != PlayerColour::NONE) {
			return true;
		}
		if (m_Board.isSuicide(move, col)) {
			return true;
		}
		auto newHash = m_Board.preComputeHash(move, col);
		if (m_History.find(newHash) != m_History.end()) {
			return true;
		}
		return false;
	}

	/**
	 * Play move
	 *
	 * @param move Move
	 * @param col Player colour
	 */
	void playMove(game_move_t move, PlayerColour col) {
		assert(m_Winner == PlayerColour::NONE);
		if (move == RESIGN) {
			m_Winner = col.invert();
			return;
		}
		if (move == PASS) {
			if (m_LastMoveWasPass) {
				m_Winner = countPoints();
			} else {
				m_LastMoveWasPass = true;
			}
			return;
		}
		m_LastMoveWasPass = false;
		if (m_Board.getValue(move) != PlayerColour::NONE) {
			m_Winner = col.invert();
			return;
		}
		if (m_Board.isSuicide(move, col)) {
			m_Winner = col.invert();
			return;
		}
		m_Board.playMove(move, col);
		bool newPosition = m_History.insert(m_Board.getHash()).second;
		if (!newPosition) {
			m_Winner = col.invert();
		}
	}
	/**
	 * Play move
	 *
	 * @param x X coordinate
	 * @param y Y coordinate
	 * @param col Colour
	 */
	void playMove(board_coord_t x, board_coord_t y, PlayerColour col) {
		assert(m_Winner == PlayerColour::NONE);
		m_LastMoveWasPass = false;
		if (m_Board.getValue(x, y) != PlayerColour::NONE) {
			m_Winner = col.invert();
			return;
		}
		if (m_Board.isSuicide(x, y, col)) {
			m_Winner = col.invert();
			return;
		}
		m_Board.playMove(x, y, col);
		bool newPosition = m_History.insert(m_Board.getHash()).second;
		if (!newPosition) {
			m_Winner = col.invert();
		}
	}

	/**
	 * Count points on the board
	 *
	 * @return Winner
	 */
	PlayerColour countPoints() const {
		auto [black, white] = m_Board.countPoints();
		if (black > white + m_Komi) {
			return PlayerColour::BLACK;
		} else {
			return PlayerColour::WHITE;
		}
	}

	/**
	 * Get Board
	 *
	 * @return Board
	 */
	const Board &getBoard() const {
		return m_Board;
	}

	/**
	 * Get komi

	 * @return Komi
	 */
	int getKomi() const {
		return m_Komi;
	}

	/**
	 * Get winner
	 *
	 * @return Winner
	 */
	PlayerColour getWinner() const {
		return m_Winner;
	}
	/**
	 * Set winner
	 *
	 * @param winner Winner
	 */
	void setWinner(PlayerColour winner) {
		m_Winner = winner;
	}
};

#endif  // SRC_GAME_HPP_
