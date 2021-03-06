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
#include <cctype>
#include <unordered_set>
#include <iostream>
#include <iomanip>
#include <random>
#include <chrono>
#include <vector>
#include <thread>
#include <atomic>
#include <memory>
#include <string>
#include <sstream>
#include <algorithm>
#include "src/vpgo.hpp"
#include "src/hash.hpp"
#include "src/board.hpp"

#ifdef VERSION
#undef VERSION
#endif

const std::size_t BOARD_SIZE = 9;
const std::size_t NUM_SIM = 100000;
const std::size_t PASS = BOARD_SIZE * BOARD_SIZE;
const std::size_t RESIGN = PASS + 1;

struct Game {
	Board b{BOARD_SIZE};
	PlayerColour winner = PlayerColour::NONE;
	bool lastMoveWasPass = false;
	std::unordered_set<std::uint_least64_t> hashes = {
	    HashValues::getInstance()->getInitialValue()};

	bool isIllegal(std::size_t offset, PlayerColour col) const {
		if (offset == RESIGN) {
			return false;
		}
		if (offset == PASS) {
			return false;
		}
		if (b.getValue(offset) != PlayerColour::NONE) {
			return true;
		}
		if (b.isSuicide(offset, col)) {
			return true;
		}
		auto newHash = b.preComputeHash(offset, col);
		if (hashes.find(newHash) != hashes.end()) {
			return true;
		}
		return false;
	}
	void playMove(std::size_t offset, PlayerColour col) {
		if (offset == RESIGN) {
			winner = col.invert();
			return;
		}
		if (offset == PASS) {
			if (lastMoveWasPass) {
				winner = countPoints();
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
	PlayerColour countPoints() {
		auto [black, white] = b.countPoints();
		if (black > white) {
			return PlayerColour::BLACK;
		} else {
			return PlayerColour::WHITE;
		}
	}
};

std::string moveToString(std::size_t move) {
	if (move == PASS) {
		return "pass";
	}
	if (move == RESIGN) {
		return "resign";
	}
	int x = move % BOARD_SIZE;
	int y = move / BOARD_SIZE;
	if (x >= 8) {
		++x;
	}
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

void printBoard(Board *b) {
	for (std::size_t y = 0; y < BOARD_SIZE; ++y) {
		for (std::size_t x = 0; x < BOARD_SIZE; ++x) {
			PlayerColour v = b->getValue(x, y);
			switch (v) {
			case PlayerColour::BLACK:
				std::cerr << " B ";
				break;
			case PlayerColour::WHITE:
				std::cerr << " W ";
				break;
			default:
				std::cerr << " - ";
			}
		}
		std::cerr << std::endl;
	}
}

struct Node {
	std::vector<std::unique_ptr<Node>> nodes;
	std::atomic<std::size_t> visits = 0;
	std::atomic<std::size_t> wins = 0;
	std::atomic<bool> expanding = false;
	std::atomic<bool> expanded = false;
	std::atomic<bool> explored = false;
	std::size_t move;

	Node() = default;
	explicit Node(std::size_t m) {
		move = m;
	}
};

struct ThreadData {
	std::default_random_engine gen;
	std::vector<std::size_t> moves = std::vector<std::size_t>(RESIGN);
	std::uniform_int_distribution<unsigned int> distrDefault{0, PASS};
};

PlayerColour playout(Game *g, PlayerColour toMove, ThreadData *td) {
	PlayerColour col = toMove;
	int delay = PASS * 2 - g->b.getNumberOfStones();
	bool ignorePass = false;
	while (g->winner == PlayerColour::NONE) {
		std::size_t move = td->distrDefault(td->gen);
		if (g->isIllegal(move, col) ||
		    (move == PASS && (ignorePass || g->countPoints() != col))) {
			if (delay > 0) {
				--delay;
				if (move == PASS) {
					ignorePass = true;
				}
				continue;
			}
			std::size_t possibleMoves = 0;
			for (std::size_t i = 0; i < RESIGN; ++i) {
				if ((i < PASS && (g->b.getValue(i) != PlayerColour::NONE ||
				                     g->b.isSuicide(i, col))) ||
				    (move == PASS && i == PASS)) {
					continue;
				}
				td->moves[possibleMoves] = i;
				++possibleMoves;
			}
			if (possibleMoves == 0) {
				move = RESIGN;
			} else {
				for (;;) {
					std::size_t moveIndex;
					if (possibleMoves > 1) {
						std::uniform_int_distribution<unsigned int> distr(
						    0, possibleMoves - 1);
						moveIndex = distr(td->gen);
					} else {
						moveIndex = 0;
					}
					move = td->moves[moveIndex];
					if (g->isIllegal(move, col) ||
					    (move == PASS &&
					        (ignorePass || g->countPoints() != col))) {
						--possibleMoves;
						if (possibleMoves == 0) {
							move = RESIGN;
							break;
						}
						td->moves[moveIndex] = td->moves[possibleMoves];
						continue;
					}
					break;
				}
			}
		}
		g->playMove(move, col);
		col = col.invert();
		delay = PASS * 2 - g->b.getNumberOfStones();
		ignorePass = false;
	}
	return g->winner;
}

std::size_t bestMove(Node *n) {
	if (1.0 * n->wins / n->visits < 0.1) {
		return RESIGN;
	}
	std::size_t maxVis = 0;
	std::size_t move = RESIGN;
	for (auto &m : n->nodes) {
		if (m->visits > maxVis) {
			move = m->move;
			maxVis = m->visits;
		}
	}
	return move;
}

void printStats(Node *n) {
	std::vector<int> winP(RESIGN);
	std::vector<int> effortMap(RESIGN);
	for (auto &m : n->nodes) {
		winP[m->move] =
		    static_cast<int>(100 - 100.0 * m->wins / m->visits + 0.5);
		effortMap[m->move] =
		    static_cast<int>(100.0 * m->visits / n->visits + 0.5);
	}
	std::cerr << "Win %: " << 1.0 * n->wins / n->visits << std::endl;
	std::cerr << "Playouts: " << n->visits << std::endl;
	std::cerr << "Win % map:" << std::endl;
	for (std::size_t y = 0; y < BOARD_SIZE; ++y) {
		for (std::size_t x = 0; x < BOARD_SIZE; ++x) {
			if (x != 0) {
				std::cerr << ' ';
			}
			std::cerr << std::setw(2) << winP[x + y * BOARD_SIZE];
		}
		std::cerr << std::endl;
	}
	std::cerr << "PASS = " << std::setw(2) << winP[PASS] << std::endl;
	std::cerr << "Effort:" << std::endl;
	for (std::size_t y = 0; y < BOARD_SIZE; ++y) {
		for (std::size_t x = 0; x < BOARD_SIZE; ++x) {
			if (x != 0) {
				std::cerr << ' ';
			}
			std::cerr << std::setw(2) << effortMap[x + y * BOARD_SIZE];
		}
		std::cerr << std::endl;
	}
	std::cerr << "PASS = " << std::setw(2) << effortMap[PASS] << std::endl;
	std::cerr << "Best line: ";
	Node *curMove = n;
	for (std::size_t i = 0; i < 5; ++i) {
		if (!curMove->expanded) {
			break;
		}
		Node *next = nullptr;
		std::size_t maxVisits = 0;
		for (auto &m : curMove->nodes) {
			if (m->visits > maxVisits) {
				next = m.get();
				maxVisits = m->visits;
			}
		}
		if (next == nullptr || next->visits < 100) {
			break;
		}
		std::size_t move = next->move;
		curMove = next;
		std::cerr << moveToString(move) << ' ';
		std::cerr << '(' << std::setw(0) << curMove->visits << ") ";
	}
	std::cerr << std::endl;
}

Node *selectMove(Node *n, ThreadData *td) {
	Node *child = n->nodes[0].get();
	double bestVal = 0;
	for (auto &m : n->nodes) {
		double curVal;
		if (m->visits == 0) {
			curVal = 100 + td->gen();
		} else {
			curVal = 1.0 - 1.0 * m->wins / m->visits +
			         std::sqrt(std::log(static_cast<double>(n->visits)) /
			                   m->visits / 2);
		}
		if (curVal > bestVal) {
			child = m.get();
			bestVal = curVal;
		}
	}
	return child;
}

void expandTree(Node *n, Game *g, PlayerColour col) {
	for (std::size_t move = 0; move < RESIGN; ++move) {
		if (g->isIllegal(move, col)) {
			continue;
		}
		n->nodes.push_back(std::make_unique<Node>(move));
	}
	n->expanded = true;
}

void simulate(Game *g, Node *n, PlayerColour col, ThreadData *td) {
	if (g->winner == PlayerColour::NONE) {
		if (n->explored.exchange(true) && !n->expanded &&
		    !n->expanding.exchange(true)) {
			expandTree(n, g, col);
		}
		if (n->expanded) {
			auto child = selectMove(n, td);
			g->playMove(child->move, col);
			simulate(g, child, col.invert(), td);
		} else {
			playout(g, col, td);
		}
	}
	n->visits++;
	if (g->winner == col) {
		n->wins++;
	}
}

std::size_t findMove(
    Game *g, PlayerColour col, std::default_random_engine::result_type seed) {
	auto start = std::chrono::high_resolution_clock::now();
	Node root;
	std::atomic<std::size_t> playouts = 0;
	std::seed_seq seq{seed};
	std::vector<std::thread> threads;
	std::size_t cpuCount = std::thread::hardware_concurrency();
	if (cpuCount == 0) {
		cpuCount = 1;
	}
	std::vector<std::seed_seq::result_type> seeds(cpuCount);
	seq.generate(seeds.begin(), seeds.end());
	for (std::size_t i = 0; i < cpuCount; ++i) {
		threads.emplace_back(
		    [&](std::seed_seq::result_type threadSeed) {
			    ThreadData td;
			    td.gen.seed(threadSeed);
			    while (++playouts <= NUM_SIM) {
				    Game clone = *g;
				    simulate(&clone, &root, col, &td);
				    std::this_thread::yield();
			    }
		    },
		    seeds[i]);
	}
	for (auto &t : threads) {
		t.join();
	}
	printStats(&root);
	std::cerr << "Passed time: "
	          << std::chrono::duration_cast<std::chrono::milliseconds>(
	                 std::chrono::high_resolution_clock::now() - start)
	                     .count() /
	                 1000.0
	          << 's' << std::endl;
	return bestMove(&root);
}

struct GtpCommandDef {
	bool hasCommandId;
	int commandId;
	std::string commandName;
	std::vector<std::string> arguments;
};

enum class GtpCommand {
	UNKNOWN,
	PROTOCOL_VERSION,
	NAME,
	VERSION,
	KNOWN_COMMNAD,
	LIST_COMMANDS,
	QUIT,
	BOARDSIZE,
	CLEAR_BOARD,
	KOMI,
	PLAY,
	GENMOVE
};

GtpCommandDef parseCommand(const std::string &inputCommand) {
	std::string command;
	command.reserve(inputCommand.size());
	bool truncateComment = false;
	for (auto c : inputCommand) {
		if (c == '#') {
			break;
		}
		if (c == 9) {
			command += ' ';
		} else if (c >= 32 && c != 127) {
			command += c;
		}
	}
	auto leftPos = command.find_first_not_of(" ");
	if (leftPos == std::string::npos) {
		return GtpCommandDef();
	}
	auto rightPos = command.find_last_not_of(" ");
	command = command.substr(leftPos, rightPos + 1);
	std::stringstream ss(command);
	GtpCommandDef res;
	if (ss >> res.commandId) {
		res.hasCommandId = true;
	} else {
		res.hasCommandId = false;
		ss.clear();
	}
	ss >> res.commandName;
	std::string argument;
	while (ss >> argument) {
		res.arguments.push_back(argument);
	}
	return res;
}

bool ignoreCommand(const GtpCommandDef &cd) {
	return !cd.hasCommandId && cd.commandName.size() == 0;
}

GtpCommand parseCommandName(const std::string &s) {
	if (s == "protocol_version") {
		return GtpCommand::PROTOCOL_VERSION;
	} else if (s == "name") {
		return GtpCommand::NAME;
	} else if (s == "version") {
		return GtpCommand::VERSION;
	} else if (s == "known_command") {
		return GtpCommand::KNOWN_COMMNAD;
	} else if (s == "list_commands") {
		return GtpCommand::LIST_COMMANDS;
	} else if (s == "quit") {
		return GtpCommand::QUIT;
	} else if (s == "boardsize") {
		return GtpCommand::BOARDSIZE;
	} else if (s == "clear_board") {
		return GtpCommand::CLEAR_BOARD;
	} else if (s == "komi") {
		return GtpCommand::KOMI;
	} else if (s == "play") {
		return GtpCommand::PLAY;
	} else if (s == "genmove") {
		return GtpCommand::GENMOVE;
	} else {
		return GtpCommand::UNKNOWN;
	}
}

void printSuccessMessage(const GtpCommandDef &cd, const std::string &s) {
	std::cout << '=';
	if (cd.hasCommandId) {
		std::cout << cd.commandId;
	}
	if (s.size() > 0) {
		std::cout << ' ' << s;
	}
	std::cout << std::endl << std::endl;
}

void printFailureMessage(const GtpCommandDef &cd, const std::string &s) {
	std::cout << '?';
	if (cd.hasCommandId) {
		std::cout << cd.commandId;
	}
	if (s.size() > 0) {
		std::cout << ' ' << s;
	}
	std::cout << std::endl << std::endl;
}

void unknownCommand(const GtpCommandDef &cd) {
	printFailureMessage(cd, "unknown command");
}
void printProtocolVersion(const GtpCommandDef &cd) {
	printSuccessMessage(cd, "2");
}
void printName(const GtpCommandDef &cd) {
	printSuccessMessage(cd, "vpgo-random");
}
void printVersion(const GtpCommandDef &cd) {
	printSuccessMessage(cd, "poc");
}
void printKnownCommand(const GtpCommandDef &cd) {
	bool known = false;
	if (cd.arguments.size() > 0) {
		auto cmd = parseCommandName(cd.arguments[0]);
		known = cmd != GtpCommand::UNKNOWN;
	}
	if (known) {
		printSuccessMessage(cd, "true");
	} else {
		printSuccessMessage(cd, "false");
	}
}
void printCommands(const GtpCommandDef &cd) {
	const char knownCommands[] =
	    "protocol_version\n"
	    "name\n"
	    "version\n"
	    "known_command\n"
	    "list_commands\n"
	    "quit\n"
	    "boardsize\n"
	    "clear_board\n"
	    "komi\n"
	    "play\n"
	    "genmove";
	printSuccessMessage(cd, knownCommands);
}
void setBoardSize(const GtpCommandDef &cd) {
	int newSize;
	bool syntaxError = false;
	if (cd.arguments.size() != 1) {
		syntaxError = true;
	}
	if (!syntaxError) {
		std::stringstream ss(cd.arguments[0]);
		syntaxError = !(ss >> newSize);
	}
	if (syntaxError) {
		printFailureMessage(cd, "syntax error");
	} else if (newSize != BOARD_SIZE) {
		printFailureMessage(cd, "unacceptable size");
	} else {
		printSuccessMessage(cd, "");
	}
}
void clearBoard(const GtpCommandDef &cd, Game *g) {
	*g = Game();
	printSuccessMessage(cd, "");
}
void setKomi(const GtpCommandDef &cd) {
	float newKomi;
	bool syntaxError = false;
	if (cd.arguments.size() != 1) {
		syntaxError = true;
	}
	if (!syntaxError) {
		std::stringstream ss(cd.arguments[0]);
		syntaxError = !(ss >> newKomi);
	}
	if (syntaxError) {
		printFailureMessage(cd, "syntax error");
	} else {
		// TODO(me): We actually ignore any komi
		printSuccessMessage(cd, "");
	}
}
void playMove(const GtpCommandDef &cd, Game *g) {
	bool syntaxError = false;
	std::size_t move;
	PlayerColour col;
	if (cd.arguments.size() != 2) {
		syntaxError = true;
	}
	if (!syntaxError) {
		std::string colourStr = cd.arguments[0];
		std::string moveStr = cd.arguments[1];
		std::transform(colourStr.begin(), colourStr.end(), colourStr.begin(),
		    [](char c) { return std::tolower(c); });
		std::transform(moveStr.begin(), moveStr.end(), moveStr.begin(),
		    [](char c) { return std::tolower(c); });
		if (colourStr == "white" || colourStr == "w") {
			col = PlayerColour::WHITE;
		} else if (colourStr == "black" || colourStr == "b") {
			col = PlayerColour::BLACK;
		} else {
			syntaxError = true;
		}
		if (moveStr == "pass") {
			move = PASS;
		} else if (moveStr.size() != 2) {
			syntaxError = true;
		} else {
			int x = moveStr[0] - 'a';
			int y = moveStr[1] - '1';
			if (x >= 9) {
				--x;
			}
			y = BOARD_SIZE - y - 1;
			if (x < 0 || x > BOARD_SIZE || y < 0 || y > BOARD_SIZE) {
				syntaxError = true;
			} else {
				move = y * BOARD_SIZE + x;
			}
		}
	}
	if (syntaxError) {
		printFailureMessage(cd, "syntax error");
	} else if (g->isIllegal(move, col)) {
		printFailureMessage(cd, "illegal move");
	} else {
		g->playMove(move, col);
		printBoard(&g->b);
		printSuccessMessage(cd, "");
	}
}
void genMove(const GtpCommandDef &cd, Game *g,
    std::default_random_engine::result_type seed) {
	bool syntaxError = false;
	PlayerColour col;
	if (cd.arguments.size() != 1) {
		syntaxError = true;
	}
	if (!syntaxError) {
		std::string colourStr = cd.arguments[0];
		std::transform(colourStr.begin(), colourStr.end(), colourStr.begin(),
		    [](char c) { return std::tolower(c); });
		if (colourStr == "white" || colourStr == "w") {
			col = PlayerColour::WHITE;
		} else if (colourStr == "black" || colourStr == "b") {
			col = PlayerColour::BLACK;
		} else {
			syntaxError = true;
		}
	}
	if (syntaxError) {
		printFailureMessage(cd, "syntax error");
	} else if (g->winner != PlayerColour::NONE) {
		printFailureMessage(cd, "game is already decided");
	} else {
		auto move = findMove(g, col, seed);
		g->playMove(move, col);
		printBoard(&g->b);
		printSuccessMessage(cd, moveToString(move));
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
	std::default_random_engine gen;
	gen.seed(
	    std::chrono::high_resolution_clock::now().time_since_epoch().count() +
	    rd());
	HashValues::getInstance()->init(PASS);
	Game g;
	PlayerColour col = PlayerColour::BLACK;
	std::string commandStr;
	while (std::getline(std::cin, commandStr)) {
		auto cd = parseCommand(commandStr);
		if (ignoreCommand(cd)) {
			continue;
		}
		auto cmd = parseCommandName(cd.commandName);
		bool quit = false;
		switch (cmd) {
		case GtpCommand::UNKNOWN:
			unknownCommand(cd);
			break;
		case GtpCommand::QUIT:
			quit = true;
			break;
		case GtpCommand::PROTOCOL_VERSION:
			printProtocolVersion(cd);
			break;
		case GtpCommand::NAME:
			printName(cd);
			break;
		case GtpCommand::VERSION:
			printVersion(cd);
			break;
		case GtpCommand::KNOWN_COMMNAD:
			printKnownCommand(cd);
			break;
		case GtpCommand::LIST_COMMANDS:
			printCommands(cd);
			break;
		case GtpCommand::BOARDSIZE:
			setBoardSize(cd);
			break;
		case GtpCommand::CLEAR_BOARD:
			clearBoard(cd, &g);
			break;
		case GtpCommand::KOMI:
			setKomi(cd);
			break;
		case GtpCommand::PLAY:
			playMove(cd, &g);
			break;
		case GtpCommand::GENMOVE:
			genMove(cd, &g, gen());
			break;
		}
		if (quit) {
			break;
		}
	}
	return 0;
}
