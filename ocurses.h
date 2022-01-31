/*
 * ocurses.h
 *
 *  Created on: 29.08.2019
 *      Author: kanzlei
 */

#ifndef OCURSES_H_
#define OCURSES_H_

//#include <curses.h>
#include <ncursesw/menu.h>
#include <stddef.h>
#include <array>
#include <initializer_list>
#include <iterator>
#include <set>
#include <string>
#include <vector>

#include <omemory.h>
#include "otextutil.h"

#include "ocurses/konstanten.h"
#include "ocurses/nodes.h"
#include "ocurses/nodes/window.h"

/*
 *  Wichtig zum Kompilieren:
 *  statt der Bibliothek -lpanel die Bibliothek -lpanelw linken!
 *  statt -lncurses -lncursesw
 *  -lncursesw muß zuletzt kommen!
 *  also z.B. gcc -Wall -o "programm" "programm.c"  -lpanelw -lformw -lmenuw -lncursesw
 *
 *
 *  WICHTIG: das Programm muß die Locale setzen, damit Umlaute erkannt werden:
 *  #include <locale.h>
 *	setlocale(LC_ALL, "");
 * 
 * 	Siehe die Wrapper-Funktionen im Header cursesapp.h !!!
 *
 * */


namespace Ocurses {


/* in ocurses/windows.h: */
class ScreenNode;
class ColorPair;
//class PanelWinNode;
class AbstractWindowNode;

using Memory::StackPointer;
using TextUtil::Dimension;



//using PanelVec = std::vector<Ocurses::PanelNode*>;


class AbstractPanelManager {
//
private:
	Memory::StackPointer<Ocurses::PanelWinNode> topWindow;

public:
	AbstractPanelManager() : topWindow("Top Window") {}

	virtual ~AbstractPanelManager() = default;

	/* Führt auch draw() aus und updatet: */
	virtual void setTop(PanelWinNode* pn);

	virtual PanelWinNode* getTopWindow() { return topWindow.getValidPointer(); }

	virtual void keyResizePressed(AbstractWindowNode* win) const;

	virtual Ocurses::WindowResponse readKey(int ch) = 0;
};








class Curses {
//
private:
//	StackPointer<AbstractTheme> currentTheme;
	ScreenNode* snode;

public:
	/* oder etwa auch: 'de_AT.UTF-8'.           *
	 * Das Locale muß eingestellt werden, damit *
	 * die Umlaute korrekt dargestellt werden.  */
	Curses(const std::string& locale = "");

	virtual ~Curses();

	ScreenNode* getScreen() const;

	/* Alle im Programm benötigten Farbenpaare können hier gesammelt initiiert werden. */
	void initColours(std::vector<ColorPair*> vec) const;


	static Dimension getScreenSize() { return Dimension(LINES, COLS); }

	void startPanelManager(AbstractPanelManager* pm);

	virtual void updatePanels() const;

	virtual void beforeExit() { /* Unterklassen! Bevor das Programm endet */ }
};



/*  S T A T I S C H E   M E T H O D E N :  */


void showCursor(bool show = true);







} // Ende namespace Ocurses


#endif /* OCURSES_H_ */
