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


/*
                          ScreenDimensions:
*/

ScreenDimensions::ScreenDimensions(int lines, int cols, int begin_y, int begin_x) :
      DimArray{lines, cols, begin_y, begin_x} {}

std::ostream& operator<< (std::ostream& os, const ScreenDimensions& sd) {
   os << '(' << sd[0] << ", " << sd[1] << ", " << sd[2] << ", " << sd[3] << ')';
   return os;
}

ScreenDimensions fullScreen() { return ScreenDimensions(); }





/*
                         AbstractPanelManager:
*/

void AbstractPanelManager::setTop(PanelWinNode* pn)
{
   topWindow.setPointer(pn);
   pn->getPanel()->top();
   pn->draw();
   update_panels();
}


void AbstractPanelManager::keyResizePressed() const
{
   PanelWinNode* wn = getTopWindow();
   if (wn == nullptr) return;
   Ocurses::keyResizePressed(wn);
}

WindowResponse AbstractPanelManager::myReadKey(int ch)
{
   if (ch == KEY_RESIZE) {
      keyResizePressed();
      return CONTINUE_LISTENING;
   }
   return readKey(ch);
}


/*
                         statische Methoden:
*/

void keyResizePressed(AbstractWindowNode* win)
{
   /* Terminalfenster wurde verändert... *
    * KEY_RESIZE wurde ausgelöst.        *
    * Siehe dazu: https://stackoverflow.com/questions/13707137/resizing-glitch-with-ncurses?rq=1 *
    * oder 'man resizeterm'                                                                      */
   endwin();
   win->doResize();
   win->doClear();
   win->draw();
}


Dimension getScreenSize()
{
   return Dimension(LINES, COLS);
}


void showCursor(bool show)
{
   short i = (show) ? 1 : 0;
   curs_set(i);
}



/*
                      Curses:
*/

Curses::Curses()
{
   /* oder etwa auch: 'de_AT.UTF-8'.           *
    * Das Locale muß eingestellt werden, damit *
    * die Umlaute korrekt dargestellt werden.  */
   setlocale(LC_ALL, "");
   snode = new ScreenNode();
   snode->initDefaults();
}


Curses::~Curses()
{
   delete snode;
}


ScreenNode* Curses::getScreen() const
{
   return snode;
}


void Curses::initColours(std::vector<ColorPair*> vec) const   /* kann mit initColours({ &SCHWARZ_WEISS, &BLAU_WEISS, ... }) aufgerufen werden */
{
   for (auto it = vec.begin(); it != vec.end(); it++) {
      (*it)->init();
   }
}


void Curses::updatePanels() const
{
   update_panels();
   doupdate();
}


void Curses::start(AbstractPanelManager& pm)
{
   PanelWinNode* wn = pm.getStartWindow();
   if (wn != nullptr) { /* Es ist ein Startwindow definiert... */
      wn->init(fullScreen());
      pm.setTop(wn);
      updatePanels();
   } /* andernfalls wird auf getScreen() gearbeitet. */
   while(1) {
      WindowResponse wr = pm.myReadKey(getch());
      switch(wr) {
      case Ocurses::CONTINUE_LISTENING:
         /* nichts zu tun... */
         break;
      case Ocurses::QUIT_PROGRAM_KEY:
         return;
      }
      updatePanels();
   }
}








} // Ende namespace Ocurses

