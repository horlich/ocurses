/*
 * ocurses.h
 *
 *  Created on: 29.08.2019
 *      Author: kanzlei
 */

#ifndef OCURSES_H_
#define OCURSES_H_

#include "ocurses/util.h"
#include "ocurses/nodes/window.h"
#include "ocurses/pmanager.h"
#include "omemory.h"

/*
 *  Wichtig zum Kompilieren:
 *  statt der Bibliothek -lpanel die Bibliothek -lpanelw linken!
 *  statt -lncurses -lncursesw
 *  -lncursesw muß zuletzt kommen!
 *  also z.B. gcc -Wall -o "programm" "programm.cpp"  -lpanelw -lformw -lmenuw -lncursesw
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
//class ScreenNode;
//class ColorPair;
//class PanelWinNode;
//class AbstractWindowNode;
//class TextUtil::Dimension;

using Memory::StackPointer;
using TextUtil::Dimension;

class ScreenNode;
class AbstractPanelManager;


/*
                             Curses:
*/

class Curses {
//
private:
   ScreenNode* snode;

public:
   Curses();
   Curses(const Curses&) = delete;
   Curses& operator=(const Curses&) = delete;

   virtual ~Curses();

   virtual ScreenNode* getScreen() const;

   /* Alle im Programm benötigten Farbenpaare können hier gesammelt initiiert werden. */
   virtual void initColours(std::vector<ColorPair*> vec) const;

   /* führt auch init() und setTop() des StartWindows aus: */
   virtual void start(AbstractPanelManager& pm);
};




/*  S T A T I S C H E   M E T H O D E N :  */

/* Siehe 'man curs_set'! Der Status 'very visible' wird hier nicht
   unterstützt. Rückgabe: If the terminal supports the visibility requested,
   the previous cursor state is returned; otherwise, ERR is returned:     */
bool showCursor(bool show = true);


Dimension getScreenSize();


void updatePanels();


} // Ende namespace Ocurses


#endif /* OCURSES_H_ */
