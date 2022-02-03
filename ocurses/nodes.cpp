/*
 * nodes.cpp
 *
 *  Created on: 17.06.2020
 *      Author: herbert
 */


#include "nodes.h"

using namespace Ocurses;
using namespace std;







ColorPair Ocurses::WEISS_SCHWARZ(COLOR_WHITE, COLOR_BLACK);
ColorPair Ocurses::WEISS_BLAU(COLOR_WHITE, COLOR_BLUE);
ColorPair Ocurses::BLAU_WEISS(COLOR_BLUE, COLOR_WHITE);
ColorPair Ocurses::SCHWARZ_WEISS(COLOR_BLACK, COLOR_WHITE);



/*
 *
 *                    C O L O R P A I R :
 *
 * */

int _colorpair_index_counter = 0;
const int MAX_STD_COLORS     = 256; /* Siehe dazu man start_color(3) */

ColorPair::ColorPair(int fg, int bg) :
		index(++_colorpair_index_counter), foreground(fg), background(bg) {
	if (index >= MAX_STD_COLORS) {
		std::stringstream buf;
		buf << __PRETTY_FUNCTION__ << ": Farbindex überschreitet das Maximum von " << MAX_STD_COLORS;
		buf.put('.');
		throw ColorException(buf.str());
	}
}



chtype ColorPair::init() {
	if (hasInit) return attr;
	int err = init_pair(index, foreground, background);
	if (err != OK) {
		std::stringstream buf;
		buf << __PRETTY_FUNCTION__ << ": init_pair mißlungen.\n";
		buf << "Wurde start_color() schon ausgeführt?";
		throw ColorException(buf.str());
	}
	attr = COLOR_PAIR(index);
	hasInit = true;
	return init();
}



chtype ColorPair::getAttr() const {
	if (! hasInit) {
		std::stringstream buf;
		buf << __PRETTY_FUNCTION__ << ": ColorPair::init() für dieses Farbenpaar noch nicht aufgerufen.";
		throw ColorException(buf.str());
	}
	return attr;
}




