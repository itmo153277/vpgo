/**
 * @file main.cpp
 *
 * Entry point
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

#include <cstddef>
#include <cstdint>
#include <cmath>
#include <unordered_set>
#include <iostream>
#include <iomanip>
#include <random>
#include <chrono>
#include <vector>
#include <memory>
#include "src/vpgo.hpp"
#include "src/hash.hpp"
#include "src/board.hpp"

const std::size_t BOARD_SIZE = 5;
const std::size_t NUM_SIM = 100000;
const std::size_t PASS = BOARD_SIZE * BOARD_SIZE;
const std::size_t RESIGN = PASS + 1;

struct Game {
	Board b{BOARD_SIZE};
	PlayerColour winner = PlayerColour::NONE;
	bool lastMoveWasPass = false;
	std::unordered_set<std::uint_least64_t> hashes = {
	    HashValues::getInstance()->getInitialValue()};

	void playMove(std::size_t offset, PlayerColour col) {
		if (offset == RESIGN) {
			winner = col.invert();
			return;
		}
		if (offset == PASS) {
			if (lastMoveWasPass) {
				auto [black, white] = b.countPoints();
				if (black > white) {
					winner = PlayerColour::BLACK;
				} else {
					winner = PlayerColour::WHITE;
				}
			} else {
				lastMoveWasPass = true;
			}
			return;
		}
		lastMoveWasPass = false;
		if (b.getValue(offset) != PlayerColour::NONE) {
			winner = col.invert();
			return;
		}
		if (b.isSuicide(offset, col)) {
			winner = col.invert();
			return;
		}
		b.playMove(offset, col);
		bool newPosition = hashes.insert(b.getHash()).second;
		if (!newPosition) {
			winner = col.invert();
		}
	}
};

std::uniform_int_distribution<unsigned int> distr(0, PASS);
std::default_random_engine gen;

std::size_t randomMove() {
	return distr(gen);
}

std::string moveToString(std::size_t move) {
	if (move == PASS) {
		return "pass";
	}
	if (move == RESIGN) {
		return "resign";
	}
	int x = move % BOARD_SIZE;
	int y = move / BOARD_SIZE;
	std::string ret = "A1";
	ret[0] += x;
	ret[1] += BOARD_SIZE - y - 1;
	return ret;
}

std::string playerToString(PlayerColour col) {
	switch (col) {
	case PlayerColour::BLACK:
		return "b";
	case PlayerColour::WHITE:
		return "w";
	}
	return "?";
}

struct Node {
	std::vector<std::unique_ptr<Node>> moves{RESIGN};
	std::size_t visits = 0;
	std::size_t wins = 0;
};

PlayerColour playout(Game *g, PlayerColour toMove) {
	PlayerColour col = toMove;
	while (g->winner == PlayerColour::NONE) {
		auto move = randomMove();
		g->playMove(move, col);
		col = col.invert();
	}
	return g->winner;
}

std::size_t bestMove(Node *n) {
	std::cout << "Win %: " << 1.0 * n->wins / n->visits << std::endl;
	for (std::size_t y = 0; y < BOARD_SIZE; ++y) {
		for (std::size_t x = 0; x < BOARD_SIZE; ++x) {
			Node *m = n->moves[x + y * BOARD_SIZE].get();
			std::cout << std::setw(2)
			          << static_cast<int>(
			                 100 - 100.0 * m->wins / m->visits + 0.5)
			          << ' ';
		}
		std::cout << std::endl;
	}
	Node *m = n->moves[PASS].get();
	std::cout << "PASS = " << std::setw(2)
	          << static_cast<int>(100 - 100.0 * m->wins / m->visits + 0.5)
	          << std::endl;
	if (1.0 * n->wins / n->visits < 0.1) {
		return RESIGN;
	}
	std::size_t maxVis = 0;
	std::size_t move = 0;
	std::size_t curMove = 0;
	for (auto &m : n->moves) {
		if (m->visits > maxVis) {
			move = curMove;
			maxVis = m->visits;
		}
		++curMove;
	}
	return move;
}

std::pair<Node *, std::size_t> selectMove(Node *n) {
	std::size_t move = 0;
	Node *child = nullptr;
	if (n->visits <= RESIGN) {
		std::uniform_int_distribution<unsigned int> d(0, RESIGN - n->visits);
		auto num = d(gen);
		move = num;
		for (auto &m : n->moves) {
			if (m) {
				++move;
				continue;
			}
			if (num == 0) {
				m = std::make_unique<Node>();
				child = m.get();
				break;
			}
			num--;
		}
	} else {
		double bestVal = 0;
		std::size_t curMove = 0;
		for (auto &m : n->moves) {
			double val = 1.0 - 1.0 * m->wins / m->visits +
			             3.0 * std::sqrt(std::log(n->visits) / m->visits / 2);
			if (val > bestVal) {
				bestVal = val;
				move = curMove;
				child = m.get();
			}
			++curMove;
		}
	}
	return {child, move};
}

void simulate(Game *g, Node *n, PlayerColour col) {
	if (n->visits == 0) {
		playout(g, col);
	} else if (g->winner == PlayerColour::NONE) {
		auto [child, move] = selectMove(n);
		g->playMove(move, col);
		simulate(g, child, col.invert());
	}
	n->visits++;
	if (g->winner == col) {
		n->wins++;
	}
}

std::size_t findMove(Game *g, PlayerColour col) {
	Node root;
	for (std::size_t i = 0; i < NUM_SIM; ++i) {
		Game clone = *g;
		simulate(&clone, &root, col);
	}
	return bestMove(&root);
}

void printBoard(Board *b) {
	for (std::size_t y = 0; y < BOARD_SIZE; ++y) {
		for (std::size_t x = 0; x < BOARD_SIZE; ++x) {
			PlayerColour v = b->getValue(x, y);
			switch (v) {
			case PlayerColour::BLACK:
				std::cout << " B ";
				break;
			case PlayerColour::WHITE:
				std::cout << " W ";
				break;
			default:
				std::cout << "   ";
			}
		}
		std::cout << std::endl;
	}
}

/**
 * Entry point
 *
 * @param argc Number of command line arguments
 * @param argv Array of command line arguments
 * @return Exit code
 */
int main(int argc, char **argv) {
	std::random_device rd;
	gen.seed(
	    std::chrono::high_resolution_clock::now().time_since_epoch().count() +
	    rd());
	HashValues::getInstance()->init(PASS);
	Game g;
	PlayerColour col = PlayerColour::BLACK;
	while (g.winner == PlayerColour::NONE) {
		auto move = findMove(&g, col);
		std::cout << playerToString(col) << ' ' << moveToString(move)
		          << std::endl;
		g.playMove(move, col);
		printBoard(&g.b);
		col = col.invert();
	}
	std::cout << "Winner: " << playerToString(g.winner) << std::endl;
	return 0;
}
