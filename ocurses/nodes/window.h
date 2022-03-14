/*
 * windows.h
 *
 *  Created on: 14.08.2021
 *      Author: herbert
 */

#ifndef OCURSES_NODES_WINDOW_H_
#define OCURSES_NODES_WINDOW_H_

#include "ncursesw/curses.h"
#include <ncursesw/menu.h>
#include <utility>
#include "../konstanten.h"
#include "../util.h"
#include "events.h"
#include "panel.h"
#include "../nodes.h"
#include "../../ocurses.h"
#include "otextutil.h"
#include "omemory.h"


using TextUtil::Dimension;
using Memory::StackPointer;




namespace Ocurses {



class PanelWinNode;
class MenuWindow;
class MenuBar;


/*
                     AbstractWindowNode:
*/

class AbstractWindowNode : public AbstractNode<WINDOW> {
   //
   AbstractWindowNode* parent = nullptr;

protected:
   AbstractWindowNode(AbstractWindowNode* _parent = nullptr) : AbstractNode<WINDOW>("WINDOW"), parent(_parent) {}

public:
   /* Kopier- und Zuweisschutz: */
   AbstractWindowNode(const AbstractWindowNode&) = delete;
   AbstractWindowNode(const AbstractWindowNode&&) = delete;
   AbstractWindowNode& operator=(const AbstractWindowNode&) = delete;
   AbstractWindowNode& operator=(const AbstractWindowNode&&) = delete;

   virtual ~AbstractWindowNode();

   virtual void deleteSource() override; /* Löscht *WINDOW */

   virtual void reset() {
      AbstractNode<WINDOW>.reset();
      parent = nullptr;
   }

   /* Wenn kein parent, gibt nullptr zurück: */
   AbstractWindowNode* getParent() const
   {
      return parent;
   }

   /* Hier müssen die Subwindows, Formulare und Menüs initialisiert werden: */
   virtual void initChildren() { /* Unterklassen... */ }

   /* NICHT verwenden, wenn Panels im Einsatz!
    * Diesfalls WindowManager::updatePanels() verwenden! */
   virtual void update() const;

   virtual bool isInit()
   {
      return pointerIsValid();
   }

   Dimension getSize() const override;

   Dimension getPosition() const override;

   int doResize(int height, int width)
   {
      return wresize(getCPointer(), height, width);
   }

   virtual int doResize()
   {
      return doResize(LINES, COLS);   /* Fullscreen */
   }

   int doClear() const
   {
      return wclear(getCPointer());
   }

   int doErase() const
   {
      return werase(getCPointer());
   }

   virtual void gotFocus()  { /* Unterklassen */ }

   virtual void lostFocus() { /* Unterklassen */ }

   int getChar() const;

   int ungetChar(int ch) const;

   virtual void draw() { /* Unterklassen... */ }

   void drawBox() const;

   Dimension getCursor() const;

   void moveCursor(TextUtil::Dimension pos) const;

   void clearLine(int y) const;

   template <typename... Args>
   void mvprintf(Dimension pos, const std::string& fmt, Args... args) const;

   void println(int line, const std::string& text, int startx = 0) const
   {
      mvprintf(Dimension(line, startx), text);
   }

   /* Bei Scrollproblemen diese Funktion benützen und mit \n abschließen. Siehe dazu
    * https://stackoverflow.com/questions/29565403/how-to-scroll-a-window-other-than-stdscreen-in-ncurses */
   template <typename... Args>
   void printf(const std::string& fmt, Args... args) const;


   void printlnZentriert(int line, const std::string& text) const;

   void printlnBetont(int line, const std::string& text) const;

   /* Gibt -1 zurück, wenn keine Daten vorhanden: */
   int colorPairIndex() const;

   void color(ColorPair& cp);

   void uncolor(ColorPair& cp);

   virtual void setBackground(ColorPair& cp);

   void setLineBackground(int line, ColorPair& cp, int startx = 0) const;

   short getBgColorIndex() const;

   /* The keypad option enables the keypad of the user's terminal.
      If enabled (bf is TRUE), the user can press a function key
      (such as an arrow key) and wgetch returns a single value
      representing the function key, as in KEY_LEFT. If disabled
      (bf is FALSE), curses does not treat function keys specially
      and the program has to interpret the escape sequences itself.  */
   void setKeypad() const;

   void enableScrolling(bool toggle = true) const;

   void scrolln(int lines = 1) const;
}; // class AbstractWindowNode


/* Gleich, wenn beide auf dieselbe Adresse zeigen, also ident sind
   Wird gebraucht für std::find():                               */
bool operator==(const AbstractWindowNode&, const AbstractWindowNode&);


template <typename... Args>
void AbstractWindowNode::mvprintf(Dimension pos, const std::string& fmt, Args... args) const
{
   mvwprintw(getCPointer(), pos.height, pos.width, fmt.c_str(), args...);
}


/* Bei Scrollproblemen diese Funktion benützen und mit \n abschließen. Siehe dazu
 * https://stackoverflow.com/questions/29565403/how-to-scroll-a-window-other-than-stdscreen-in-ncurses */
template <typename... Args>
void AbstractWindowNode::printf(const std::string& fmt, Args... args) const
{
   wprintw(getCPointer(), fmt.c_str(), args...);
}


/*
                             WinNode:
*/

class WinNode : public AbstractWindowNode {
//
public:
   WinNode(AbstractWindowNode* parent = nullptr) : AbstractWindowNode(parent) {}

   /* Kopier- und Zuweisschutz: */
   WinNode(const WinNode&) = delete;
   WinNode(const WinNode&&) = delete;
   WinNode& operator=(const WinNode&) = delete;
   WinNode& operator=(const WinNode&&) = delete;

   virtual ~WinNode() = default;

   virtual void init(Geometry);

   void popUp(PanelWinNode& popup);

   void popUpMenu(MenuWindow& popup, Dimension pos);

   void menuBarSelect(MenuBar&);

   void showSimpleMessage(const std::wstring& message);
}; // class WinNode



/*
                         PanelWinNode:
*/

class PanelWinNode : public WinNode {
   //
   Memory::HeapPointer<PanelNode> panel;

public:
   PanelWinNode(AbstractWindowNode* parent = nullptr);

   /* Kopier- und Zuweisschutz: */
   PanelWinNode(const PanelWinNode&) = delete;
   PanelWinNode(const PanelWinNode&&) = delete;
   PanelWinNode& operator=(const PanelWinNode&) = delete;
   PanelWinNode& operator=(const PanelWinNode&&) = delete;

   virtual void deleteSource() override;

   virtual void init(Geometry) override;

   virtual PanelNode* getPanel()
   {
      return panel.getPointer();
   }

   virtual ~PanelWinNode();

   virtual WindowResponse readKey(int ch)   /* Unterklassen fügen hinzu... */
   {
      return ASK_PARENT;
   }
}; // PanelWinNode


/*
                        AbstractSubWinNode:
*/

class AbstractSubWinNode : public AbstractWindowNode {
//   AbstractWindowNode& parent;

public:
   AbstractSubWinNode(AbstractWindowNode* w) : AbstractWindowNode(w)
   {
      if (w == nullptr) throw BadDesign("AbstractSubWinNode(): parent == nullptr!");
   }

   /* Kopier- und Zuweisschutz: */
   AbstractSubWinNode(const AbstractSubWinNode&) = delete;
   AbstractSubWinNode(const AbstractSubWinNode&&) = delete;
   AbstractSubWinNode& operator=(const AbstractSubWinNode&) = delete;
   AbstractSubWinNode& operator=(const AbstractSubWinNode&&) = delete;

   virtual ~AbstractSubWinNode() = default;

   /* Diese Methode setzt voraus, dass mein *WINDOW schon
      definiert ist, also daß init() schon aufgerufen wurde.
      *WINDOW kann auch mit getParentNode().getCPointer()
      abgerufen werden. */
   WINDOW* getParentSource() const;

   virtual void update() const override;

   virtual void quit();

}; // AbstractSubWinNode



/*
                         DerWinNode:
*/

class DerWinNode : public AbstractSubWinNode {
   /* derwin ist im Prinzip das selbe wie subwin. Während die Position (begin_y, begin_x)
      des Fensters bei der Funktion subwin aber *relativ zum Screen* festgelegt wird, ist
      die Position bei derwin relativ zum *orig-Fenster*.                              */
public:
   DerWinNode(AbstractWindowNode* parent);

   /* Kopier- und Zuweisschutz: */
   DerWinNode(const DerWinNode&) = delete;
   DerWinNode(const DerWinNode&&) = delete;
   DerWinNode& operator=(const DerWinNode&) = delete;
   DerWinNode& operator=(const DerWinNode&&) = delete;

   virtual void init(Geometry d);

   virtual ~DerWinNode() = default;
};



/*
                         SubWinNode:
*/

class SubWinNode : public AbstractSubWinNode {
//
public:
   SubWinNode(AbstractWindowNode* w);

   /* Kopier- und Zuweisschutz: */
   SubWinNode(const SubWinNode&) = delete;
   SubWinNode(const SubWinNode&&) = delete;
   SubWinNode& operator=(const SubWinNode&) = delete;
   SubWinNode& operator=(const SubWinNode&&) = delete;

   virtual void init(Geometry d);

   virtual ~SubWinNode() = default;
};




/*-------------------/ MenuWindow: /-----------------------*/

using ItemPair = std::pair<const char*, const char*>;
using ItemVec = std::vector<ItemPair>;

class MenuWindow : public DerWinNode {
   /*
      Alle menu-Methoden unter'man menu(3x)'
   */
public:
   enum class Align {vertikal, horizontal};

   MenuWindow(AbstractWindowNode* parent, Align a = Align::vertikal) : DerWinNode(parent), menunode("MENU"), myID(++lastID), menu_align(a) {}

   /* Kopier- und Zuweisschutz: */
   MenuWindow(const MenuWindow&) = delete;
   MenuWindow(const MenuWindow&&) = delete;
   MenuWindow& operator=(const MenuWindow&) = delete;
   MenuWindow& operator=(const MenuWindow&&) = delete;

   virtual ~MenuWindow();

   void setEventListener(EventListener* el)
   {
      elist = el;
   }

   EventListener* getEventListener() { return elist; }

   void addItem(const char* name, const char* description = "");

   /* Items ohne Description auf einmal setzen: */
   void setItems(std::initializer_list<const char*> li);

   virtual void init(Dimension position);

   inline int getID() const
   {
      return myID;
   }

   void setBackground(ColorPair& cp) override;

   void setForeground(ColorPair& cp);

   void setItemSpacing(int i) { item_spacing = i; }

   /* Muß vor init() aufgerufen werden: */
   void showDescription();

   /* Muß vor init() aufgerufen werden: */
   void setMenuMark( const std::string& );

   Dimension getMenuSize() const;

   int getItemCount() const;

   int getLastItemIndex() const
   {
      return getItemCount() - 1;
   }

   /* Bei Fehler wird -1 zurückgegeben. */
   int getActiveIndex() const;

   /* Gibt die Rückgabewerte laut 'man set_current_item' zurück: */
   int setActive(int index = 0) const;

   virtual WindowResponse readKey(int ch);

   void deleteSource() override;

protected:
   int menuDriver(int command) const;

   int getItemSpacing() const { return item_spacing; }

   const ItemVec& getItems() const { return itempairs; }

private:
   Memory::StackPointer<MENU> menunode;
   ITEM** itemArray = nullptr;
   ItemVec itempairs;
   EventListener* elist = nullptr;
   static int lastID; /* muß in window.cpp initialisiert werden! */
   int myID = 0;
   int item_spacing = 1;
   bool show_description = false;
   std::string menu_mark;
   Align menu_align;

   virtual void init(Geometry d) {}; /* Funktion sperren! */

   void setMenuFormat();
   void setMenuSpacing();
}; // class MenuWindow






/*-------------------/ Menu: /-----------------------*/

class Menu : public MenuWindow {
public:
   Menu(AbstractWindowNode* parent) : MenuWindow(parent, Align::vertikal) {}

   virtual ~Menu() = default;

}; // class Menu





/*-------------------/ MenuBar: /-----------------------*/

using MenuVec = std::vector<Menu*>;

class MenuBar : public MenuWindow {
   //
public:
   MenuBar(AbstractWindowNode* parent) : MenuWindow(parent, Align::horizontal) {}

   virtual ~MenuBar() = default;

   void addMenu(Menu& m);

   virtual WindowResponse readKey(int ch) override;

private:
   MenuVec mvec;

   /* Beginn des ersten Zeichens von Item mit Index item: */
   int getStartX(int item) const;
}; // class MenuBar



/*
                      ScreenNode:
*/

class ScreenNode : public AbstractWindowNode {
   //
   bool imFarbmodus = false;

protected:
   void startColor();

public:
   /* Kopier- und Zuweisschutz: */
   ScreenNode(const ScreenNode&) = delete;
   ScreenNode& operator=(const ScreenNode&) = delete;

   ScreenNode();

   virtual ~ScreenNode();

   void initDefaults();
};






} // Ende Namespace Ocurses






#endif /* OCURSES_NODES_WINDOW_H_ */
