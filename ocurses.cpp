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



Dimension getScreenSize()
{
   return Dimension(LINES, COLS);
}


bool showCursor(bool show)
{
   short i = (show) ? 1 : 0;
   return (curs_set(i) == 0) ? false : true;
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


void updatePanels()
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

