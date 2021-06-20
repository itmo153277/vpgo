/**
 * @file src/pattern.cpp
 * 3x3 patterns
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

#include "pattern.hpp"

#include <cassert>

/**
 * Premutation
 *
 * @tparam P Modifier
 */
template <template <class> class P>
struct Premutation {
	/**
	 * Invoke pattern search with premutation
	 *
	 * @tparam T Pattern search class
	 * @tparam Loc Locator
	 * @param board Board
	 * @param x X coordinate
	 * @param y Y coordinate
	 * @param col Player colour
	 * @return true Uf matches any pattern
	 */
	template <template <class> class T, class Loc>
	static bool invoke(const Board &board, board_coord_t x, board_coord_t y,
	    PlayerColour col) {
		if (T<Loc>::invoke(board, x, y, col)) {
			return true;
		}
		return T<P<Loc>>::invoke(board, x, y, col);
	}
};

/**
 * List of premutations
 *
 * @tparam P Premutations
 */
template <class... P>
struct PremutationList;

/**
 * Premutation iterator
 *
 * @tparam List Premutation list
 * @tparam T Pattern Search class
 */
template <class List, template <class> class T>
struct PremutationIter;

/**
 * Premutation iterator
 *
 * @tparam T Pattern Search class
 */
template <template <class> class T>
struct PremutationIter<PremutationList<>, T> {
	/**
	 * Oattern search class override
	 *
	 * @tparam Loc Locator
	 */
	template <class Loc>
	struct Invoke {
		/**
		 * Invoke search
		 *
		 * @param board Board
		 * @param x X coordingate
		 * @param y Y coordinate
		 * @param col PLayer colour
		 * @return true if matches any pattern
		 */
		static bool invoke(const Board &board, board_coord_t x, board_coord_t y,
		    PlayerColour col) {
			return T<Loc>::invoke(board, x, y, col);
		}
	};
};

/**
 * Premutation iterator
 *
 * @tparam T Pattern Search class
 * @tparam P Premutation
 * @tparam List Other premutations
 */
template <template <class> class T, class P, class... List>
struct PremutationIter<PremutationList<P, List...>, T> {
	/**
	 * Oattern search class override
	 *
	 * @tparam Loc Locator
	 */
	template <class Loc>
	struct Invoke {
		/**
		 * Invoke search
		 *
		 * @param board Board
		 * @param x X coordingate
		 * @param y Y coordinate
		 * @param col PLayer colour
		 * @return true if matches any pattern
		 */
		static bool invoke(const Board &board, board_coord_t x, board_coord_t y,
		    PlayerColour col) {
			return P::template invoke<
			    PremutationIter<PremutationList<List...>, T>::template Invoke,
			    Loc>(board, x, y, col);
		}
	};
};

/**
 * Normal locator
 *
 */
struct NormalLoc {
	/**
	 * Get value from the board
	 *
	 * @param board Board
	 * @param x X coordinate
	 * @param y Y coordinate
	 * @param x0 Original X coordinate
	 * @param y0 Original Y coordinate
	 * @return Value
	 */
	static PlayerColour getValue(
	    const Board &board, int x, int y, board_coord_t x0, board_coord_t y0) {
		if (x < 0 && x0 == 0) {
			return PlayerColour::NEUTRAL;
		}
		if (y < 0 && y0 == 0) {
			return PlayerColour::NEUTRAL;
		}
		if (x > 0 && x0 == board.getSize() - 1) {
			return PlayerColour::NEUTRAL;
		}
		if (y > 0 && y0 == board.getSize() - 1) {
			return PlayerColour::NEUTRAL;
		}
		return board.getValue(x + x0, y + y0);
	}
};

/**
 * X/Y swap modifier
 *
 * @tparam T Original locator
 */
template <class T>
struct SwapAxesMod {
	/**
	 * Get value from the board
	 *
	 * @param board Board
	 * @param x X coordinate
	 * @param y Y coordinate
	 * @param x0 Original X coordinate
	 * @param y0 Original Y coordinate
	 * @return Value
	 */
	static PlayerColour getValue(
	    const Board &board, int x, int y, board_coord_t x0, board_coord_t y0) {
		return T::getValue(board, y, x, x0, y0);
	}
};

/**
 * Inverse X modifier
 *
 * @tparam T Original locator
 */
template <class T>
struct InverseXMod {
	/**
	 * Get value from the board
	 *
	 * @param board Board
	 * @param x X coordinate
	 * @param y Y coordinate
	 * @param x0 Original X coordinate
	 * @param y0 Original Y coordinate
	 * @return Value
	 */
	static PlayerColour getValue(
	    const Board &board, int x, int y, board_coord_t x0, board_coord_t y0) {
		return T::getValue(board, -x, y, x0, y0);
	}
};

/**
 * Inverse Y modifier
 *
 * @tparam T Original locator
 */
template <class T>
struct InverseYMod {
	/**
	 * Get value from the board
	 *
	 * @param board Board
	 * @param x X coordinate
	 * @param y Y coordinate
	 * @param x0 Original X coordinate
	 * @param y0 Original Y coordinate
	 * @return Value
	 */
	static PlayerColour getValue(
	    const Board &board, int x, int y, board_coord_t x0, board_coord_t y0) {
		return T::getValue(board, x, -y, x0, y0);
	}
};

/**
 * Premutation with X-Y swap
 */
using SwapAxesPremutation = Premutation<SwapAxesMod>;
/**
 * Premutation with inverse X
 */
using InverseXPremutation = Premutation<InverseXMod>;
/**
 * Premutation with inverse Y
 */
using InverseYPremutation = Premutation<InverseYMod>;

/**
 * 3x3 pattern search class
 *
 * @tparam Loc Locator class
 */
template <class Loc>
struct PatternSearch {
	/**
	 * Invoke search
	 *
	 * @param board Board
	 * @param x X coordingate
	 * @param y Y coordinate
	 * @param col PLayer colour
	 * @return true if matches any pattern
	 */
	static bool invoke(const Board &board, board_coord_t x, board_coord_t y,
	    PlayerColour col) {
		/*
		    Patterns:

		    BWB BW. BW? BWW
		    .B. .B. BB. .B.
		    ??? ?.? ?.? ?.?

		 */
		if (Loc::getValue(board, -1, -1, x, y) != col) {
			return false;
		}
		if (Loc::getValue(board, 0, -1, x, y) != col.invert()) {
			return false;
		}
		if (Loc::getValue(board, 1, 0, x, y) != PlayerColour::NONE) {
			return false;
		}
		if (Loc::getValue(board, -1, 0, x, y) == col) {
			if (Loc::getValue(board, 0, 1, x, y) != PlayerColour::NONE) {
				return false;
			}
			return true;
		}
		if (Loc::getValue(board, -1, 0, x, y) != PlayerColour::NONE) {
			return false;
		}
		if (Loc::getValue(board, 1, -1, x, y) == col) {
			return true;
		}
		if (Loc::getValue(board, 0, 1, x, y) != PlayerColour::NONE) {
			return false;
		}
		return true;
	}
};

/**
 * Pattern search that includes premutations (rotation + flip)
 */
using PatterSearchWithPremutations =
    PremutationIter<PremutationList<SwapAxesPremutation, InverseXPremutation,
                        InverseYPremutation>,
        PatternSearch>::Invoke<NormalLoc>;

/**
 * Match 3x3 patterns
 *
 * @param board Board
 * @param x X coord
 * @param y Y coord
 * @param col Player to move
 * @return true if match any pattern
 */
bool patternMatch(
    const Board &board, board_coord_t x, board_coord_t y, PlayerColour col) {
	assert(board.getValue(x, y) == PlayerColour::NONE);
	return PatterSearchWithPremutations::invoke(board, x, y, col);
}
