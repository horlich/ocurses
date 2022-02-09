/*
 * menue.h
 *
 *  Created on: 14.08.2021
 *      Author: herbert
 */

#ifndef OCURSES_NODES_MENUE_H_
#define OCURSES_NODES_MENUE_H_


//#include <ncursesw/menu.h>
//
#include <omemory.h>
#include <otextutil.h>
#include <string>
#include "../nodes.h"
#include "window.h"

using TextUtil::Dimension;
using Memory::StackPointer;




namespace Ocurses {



struct MenuListener {
   /* Methode erhält den Index desjenigen Items übergeben,
      das mittels der Returntaste ausgewählt wurde:     */
   virtual void itemSelected(int index) = 0;
};


/*-------------------/ MenuNode: /-----------------------*/


using ItemPair = std::pair<const char*, const char*>;
using ItemVec = std::vector<ItemPair>;


class MenuNode : public AbstractNode<MENU> {
   //
   ITEM** itemArray = nullptr;
   ItemVec itempairs;
   MenuListener* mlist = nullptr;

   void deleteAll();

protected:
   Dimension scaleMenu();

public:
   MenuNode () : AbstractNode<MENU>("MENU") {}

   /* Kopier- und Zuweisschutz: */
   MenuNode(const MenuNode&) = delete;
   MenuNode(const MenuNode&&) = delete;
   MenuNode& operator=(const MenuNode&) = delete;
   MenuNode& operator=(const MenuNode&&) = delete;

   virtual ~MenuNode();

   void addItem(const char* name, const char* description);

   void setMenuListener(MenuListener* ml) { mlist = ml; }

   /* WICHTIG post() muß ausgeführt werden, *
    * um das Menü anzuzeigen!!!             */
   virtual int post();

   void foreground(ColorPair&) const;

   void background(ColorPair&) const;

   void showDescription(bool b);

   virtual void init();

   int getItemCount() const;

   int getLastItemIndex() const
   {
      return getItemCount() - 1;
   }

   /* Muß vor post() aufgerufen werden: */
   int setMenuMark( const char* mark ) const;

   int setMenuSub(const AbstractWindowNode& w, const TextUtil::Dimension& pos);

   int menuDriver(int command) const;

   /* Bei Fehler wird -1 zurückgegeben. */
   int getActiveIndex() const;

   /* Gibt die Rückgabewerte laut 'man set_current_item' zurück: */
   int setActive(int index = 0) const;

   Ocurses::WindowResponse readKey(int ch); /* vgl. dazu PanelWinNode::readKey(int) */

   int printKey(int ch) const;

   Dimension getSize() const override;

   Dimension getPosition() const override;
}; // MenuNode



/*-------------------/ WindowMenuNode: /-----------------------*/

class WindowMenuNode : public MenuNode {
   //
   AbstractWindowNode& parentWin;
   TextUtil::Dimension position;

public:
   WindowMenuNode(AbstractWindowNode& parent, TextUtil::Dimension position);

   /* Kopier- und Zuweisschutz: */
   WindowMenuNode(const WindowMenuNode&) = delete;
   WindowMenuNode(const WindowMenuNode&&) = delete;
   WindowMenuNode& operator=(const WindowMenuNode&) = delete;
   WindowMenuNode& operator=(const WindowMenuNode&&) = delete;

   virtual ~WindowMenuNode() = default;

   WINDOW* getSubWin() const
   {
      return getCPointer()->sub;
   }

   WINDOW* getWindow() const
   {
      return getCPointer()->win;
   }

   Dimension getPosition() const override;

   int post() override;
};





} // Ende Namespace Ocurses





#endif /* OCURSES_NODES_MENUE_H_ */
