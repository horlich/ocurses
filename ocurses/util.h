#ifndef UTIL_H_INCLBUDED
#define UTIL_H_INCLBUDED

#include <array>
#include <iostream>
#include <sstream>
#include <ncursesw/curses.h>
#include "exceptions.h"


namespace Ocurses {



/*
                          Geometry:
*/

using DimArray = std::array<int, 4>;

class Geometry : public DimArray {
/* Arraybelegung: lines | cols | begin_y | begin_x */
public:
   Geometry(int lines = LINES, int cols = COLS, int begin_y = 0, int begin_x = 0);
};

std::ostream& operator<< (std::ostream& os, const Geometry& sd);


Geometry fullScreen();




/*-----------------/ ColorPair: /--------------------------*/

class ColorPair {
	/*
	 *    start_color() nicht vergessen!!!
	 *
	 * */
private:
	/* index > 0 && < 256 ! */
	int index, foreground, background;
	bool hasInit = false;
	chtype attr = 0; /* Irgendwie initialisieren.             *
	 	 * chtype ist laut curses.h eine typedef für long int */
public:
	ColorPair(int fg = COLOR_BLACK, int bg = COLOR_WHITE);

   /* Kopier- und Zuweisschutz: */
   ColorPair(const ColorPair&) = delete;
   ColorPair(const ColorPair&&) = delete;
   ColorPair& operator=(const ColorPair&) = delete;
   ColorPair& operator=(const ColorPair&&) = delete;

	virtual ~ColorPair() = default;

	inline int getIndex() const { return index; }

	// Darf erst nach WindowManager::startColor() aufgerufen werden:
	chtype init();

	// Darf erst nach WindowManager::startColor() aufgerufen werden:
	chtype getAttr() const;
};









} // Ende Namespace Ocurses


#endif // UTIL_H_INCLBUDED
