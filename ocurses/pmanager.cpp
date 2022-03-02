#include "pmanager.h"

namespace Ocurses {





/*
                         AbstractPanelManager:
*/

void AbstractPanelManager::setTop(PanelWinNode* pn)
{
   PanelWinNode* oldw = getTopWindow();
   if (! pn->pointerIsValid())
      throw BadDesign("AbstractPanelManager::setTop(): init() für Fenster wurde nicht aufgerufen!");
   if (oldw != nullptr) oldw->lostFocus();
   topWindow.setPointer(pn);
   pn->getPanel()->top();
   pn->draw();
   pn->gotFocus();
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











} // Ende Namespace Ocurses
