/*
 * ocurses.cpp
 *
 *  Created on: 29.08.2019
 *      Author: kanzlei
 */

#include "ocurses.h"
#include "ocurses/exceptions.h"

using namespace std;
using TextUtil::Dimension;
//using namespace Ocurses;


namespace Ocurses {


void AbstractPanelManager::setTop(PanelWinNode* pn) {
	topWindow.setPointer(pn);
	pn->getPanel()->top();
	pn->draw();
	update_panels();
	doupdate();
}



void AbstractPanelManager::keyResizePressed(AbstractWindowNode* win) const {
/* Terminalfenster wurde verändert... *
 * KEY_RESIZE wurde ausgelöst.        *
 * Siehe dazu: https://stackoverflow.com/questions/13707137/resizing-glitch-with-ncurses?rq=1 *
 * oder 'man resizeterm'                                                                      */
	endwin();
	win->doResize();
	update_panels();
	win->doClear();
	win->draw();
}




class Curses_impl {
//
private:
//	StackPointer<AbstractTheme> currentTheme;
	ScreenNode* snode;

public:
	/* oder etwa auch: 'de_AT.UTF-8'.           *
	 * Das Locale muß eingestellt werden, damit *
	 * die Umlaute korrekt dargestellt werden.  */
	Curses_impl(const std::string& locale = "");

	virtual ~Curses_impl();

	ScreenNode* getScreen() const;

	/* Alle im Programm benötigten Farbenpaare können hier gesammelt initiiert werden. */
	void initColours(std::vector<ColorPair*> vec) const;


	static Dimension getScreenSize() { return Dimension(LINES, COLS); }

	void startPanelManager(AbstractPanelManager* pm);

	virtual void updatePanels() const;

	virtual void beforeExit() { /* Unterklassen! Bevor das Programm endet */ }
};





Curses_impl::Curses_impl(const std::string& locale) {
	setlocale(LC_ALL, locale.c_str());
	snode = new ScreenNode();
	snode->initDefaults();
}


Curses_impl::~Curses_impl() {
	delete snode;
}


ScreenNode* Curses_impl::getScreen() const { return snode; }


void Curses_impl::initColours(std::vector<ColorPair*> vec) const { /* kann mit initColours({ SCHWARZ_WEISS, BLAU_WEISS, ... }) aufgerufen werden */
	for (auto it = vec.begin(); it != vec.end(); it++) { (*it)->init(); }
}


void Curses_impl::updatePanels() const {
	update_panels();
	doupdate();
}


void Curses_impl::startPanelManager(AbstractPanelManager* pm) {
	while(1) {
		WindowResponse wr = pm->readKey(getch());
		switch(wr) {
			case Ocurses::CONTINUE_LISTENING:
				/* nichts zu tun... */
				break;
			case Ocurses::QUIT_PROGRAM_KEY:
//				MESSAGE.println("Before Exit");
				beforeExit();
				return;
		}
		updatePanels();
	}
}


void showCursor(bool show) {
	short i = (show) ? 1 : 0;
	curs_set(i);
}













} // Ende namespace Ocurses

