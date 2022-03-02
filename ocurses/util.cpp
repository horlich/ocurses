#include "util.h"

namespace Ocurses {



/*
                          Geometry:
*/

Geometry::Geometry(int lines, int cols, int begin_y, int begin_x) :
      DimArray{lines, cols, begin_y, begin_x} {}

std::ostream& operator<< (std::ostream& os, const Geometry& sd) {
   os << '(' << sd[0] << ", " << sd[1] << ", " << sd[2] << ", " << sd[3] << ')';
   return os;
}

Geometry fullScreen() { return Geometry(); }


ColorPair WEISS_SCHWARZ(COLOR_WHITE, COLOR_BLACK);
ColorPair WEISS_BLAU(COLOR_WHITE, COLOR_BLUE);
ColorPair BLAU_WEISS(COLOR_BLUE, COLOR_WHITE);
ColorPair SCHWARZ_WEISS(COLOR_BLACK, COLOR_WHITE);
ColorPair ROT_WEISS(COLOR_RED, COLOR_WHITE);
ColorPair WEISS_ROT(COLOR_WHITE, COLOR_RED);



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















} // Ende Namespace Ocurses
