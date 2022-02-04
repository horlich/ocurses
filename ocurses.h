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
#include <memory>
#include <assert.h>

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
class ScreenNode;
class ColorPair;
class PanelWinNode;
class AbstractWindowNode;

using Memory::StackPointer;



/*
                          ScreenDimensions:
*/

using DimArray = std::array<int, 4>;

class ScreenDimensions : public DimArray {
/* lines | cols | begin_y | begin_x */
public:
   ScreenDimensions(int lines = LINES, int cols = COLS, int begin_y = 0, int begin_x = 0);
};

std::ostream& operator<< (std::ostream& os, const ScreenDimensions& sd);

ScreenDimensions fullScreen();



/*
                         AbstractPanelManager:
*/

class AbstractPanelManager {
   /* Unterklassen müssen die Methode
      readKey(int) implementieren     */
private:
   Memory::StackPointer<Ocurses::PanelWinNode> topWindow;

public:
   AbstractPanelManager() : topWindow("Top Window") {}

   /* Kopier- und Zuweisschutz: */
   AbstractPanelManager(const AbstractPanelManager&) = delete;
   AbstractPanelManager(const AbstractPanelManager&&) = delete;
   AbstractPanelManager& operator=(const AbstractPanelManager&) = delete;
   AbstractPanelManager& operator=(const AbstractPanelManager&&) = delete;

   virtual ~AbstractPanelManager() = default;

   /* Führt auch draw() aus und updatet: */
   virtual void setTop(PanelWinNode* pn);

   /* Gibt nullptr zurück, wenn topWindow nicht definiert ist: */
   virtual PanelWinNode* getTopWindow() const
   {
      return topWindow.getPointer();
   }

   /* Unterklassen können bestimmen, welches
      Fenster zu Programmstart angezeigt werden soll: */
   virtual PanelWinNode* getStartWindow()
   {
      return nullptr;
   }

   /* Code: KEY_RESIZE */
   virtual void keyResizePressed() const;

   virtual Ocurses::WindowResponse readKey(int ch) = 0;

   Ocurses::WindowResponse myReadKey(int ch);
};


void keyResizePressed(AbstractWindowNode* win);

Dimension getScreenSize();


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

   virtual void start(AbstractPanelManager& pm);

   virtual void updatePanels() const;
};



/*  S T A T I S C H E   M E T H O D E N :  */


void showCursor(bool show = true);






} // Ende namespace Ocurses


#endif /* OCURSES_H_ */
