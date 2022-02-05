/*
 * windows.h
 *
 *  Created on: 14.08.2021
 *      Author: herbert
 */

#ifndef OCURSES_NODES_WINDOW_H_
#define OCURSES_NODES_WINDOW_H_




#include "../nodes.h"
#include "../../ocurses.h"
#include "panel.h"
#include "otextutil.h"
#include "omemory.h"


using TextUtil::Dimension;
using Memory::StackPointer;




namespace Ocurses {



class PanelWinNode;
class Geometry;


/*
                     AbstractWindowNode:
*/

class AbstractWindowNode : public AbstractNode<WINDOW> {
   //
protected:
   AbstractWindowNode() : AbstractNode<WINDOW>("WINDOW") {}

public:
   /* Kopier- und Zuweisschutz: */
   AbstractWindowNode(const AbstractWindowNode&) = delete;
   AbstractWindowNode(const AbstractWindowNode&&) = delete;
   AbstractWindowNode& operator=(const AbstractWindowNode&) = delete;
   AbstractWindowNode& operator=(const AbstractWindowNode&&) = delete;

   virtual ~AbstractWindowNode();

   virtual void init(Geometry d);

   /* Hier müssen die Subwindows und Formulare initialisiert werden: */
   virtual void initChildren() { /* Unterklassen... */ }

   /* NICHT verwenden, wenn Panels im Einsatz!
    * Diesfalls WindowManager::updatePanels() verwenden! */
   virtual void update() const;

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

   void setBackground(ColorPair& cp);

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
}; // AbstractWindowNode



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
   WinNode() = default;

//   WinNode(WINDOW* w);

   /* Kopier- und Zuweisschutz: */
   WinNode(const WinNode&) = delete;
   WinNode(const WinNode&&) = delete;
   WinNode& operator=(const WinNode&) = delete;
   WinNode& operator=(const WinNode&&) = delete;

   virtual ~WinNode() = default;

   void init(Geometry d);

   void popUp(PanelWinNode& popup);
}; // WinNode



/*
                         PanelWinNode:
*/

class PanelWinNode : public WinNode {
   //
   Memory::HeapPointer<PanelNode> panel;

public:
   PanelWinNode();

   /* Kopier- und Zuweisschutz: */
   PanelWinNode(const PanelWinNode&) = delete;
   PanelWinNode(const PanelWinNode&&) = delete;
   PanelWinNode& operator=(const PanelWinNode&) = delete;
   PanelWinNode& operator=(const PanelWinNode&&) = delete;


   void init(Geometry d);


   virtual PanelNode* getPanel()
   {
      return panel.getPointer();
   }

   virtual ~PanelWinNode()
   {
      panel.deletePointer();
   }

   virtual WindowResponse readKey(int ch)   /* Unterklassen fügen hinzu... */
   {
      return ASK_PARENT;
   }
}; // PanelWinNode


/*
                        AbstractSubWinNode:
*/

class AbstractSubWinNode : public AbstractWindowNode {
   AbstractWindowNode& parent;

public:
   AbstractSubWinNode(AbstractWindowNode& w) : parent(w) {}

   /* Kopier- und Zuweisschutz: */
   AbstractSubWinNode(const AbstractSubWinNode&) = delete;
   AbstractSubWinNode(const AbstractSubWinNode&&) = delete;
   AbstractSubWinNode& operator=(const AbstractSubWinNode&) = delete;
   AbstractSubWinNode& operator=(const AbstractSubWinNode&&) = delete;

   /* Diese Methode setzt voraus, dass mein *WINDOW schon
      definiert ist, also daß init() schon aufgerufen wurde.
      *WINDOW kann auch mit getParentNode().getCPointer()
      abgerufen werden. */
   WINDOW* getParentSource() const;

   const AbstractWindowNode& getParentNode() const
   {
      return parent;
   }

   void update() const override;

   virtual ~AbstractSubWinNode() = default;
}; // AbstractSubWinNode



/*
                         DerWinNode:
*/

class DerWinNode : public AbstractSubWinNode {
   /* derwin ist im Prinzip das selbe wie subwin. Während die Position (begin_y, begin_x)
      des Fensters bei der Funktion subwin aber *relativ zum Screen* festgelegt wird, ist
      die Position bei derwin relativ zum *orig-Fenster*.                              */
public:
   DerWinNode(AbstractWindowNode& w);

   /* Kopier- und Zuweisschutz: */
   DerWinNode(const DerWinNode&) = delete;
   DerWinNode(const DerWinNode&&) = delete;
   DerWinNode& operator=(const DerWinNode&) = delete;
   DerWinNode& operator=(const DerWinNode&&) = delete;

   void init(Geometry d);

   virtual ~DerWinNode() = default;
};



/*
                         SubWinNode:
*/

class SubWinNode : public AbstractSubWinNode {
//
public:
   SubWinNode(AbstractWindowNode& w);

   /* Kopier- und Zuweisschutz: */
   SubWinNode(const SubWinNode&) = delete;
   SubWinNode(const SubWinNode&&) = delete;
   SubWinNode& operator=(const SubWinNode&) = delete;
   SubWinNode& operator=(const SubWinNode&&) = delete;

   void init(Geometry d);

   virtual ~SubWinNode() = default;
};



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
