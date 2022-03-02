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
#include <ncursesw/menu.h>
#include <string>
#include "../nodes.h"
//#include "window.h"
#include "events.h"

using TextUtil::Dimension;
using Memory::StackPointer;




namespace Ocurses {

class MenuNode;
class AbstractWindowNode;




/*-------------------/ MenuNode: /-----------------------*/


using ItemPair = std::pair<const char*, const char*>;
using ItemVec = std::vector<ItemPair>;


class MenuNode : public AbstractNode<MENU> {
   //
   ITEM** itemArray = nullptr;
   ItemVec itempairs;
   EventListener* elist = nullptr;
   static int lastID; /* muß in menue.cpp initialisiert werden! */
   int myID = 0;

protected:

public:
   MenuNode () : AbstractNode<MENU>("MENU"), myID(++lastID) {}

   /* Kopier- und Zuweisschutz: */
   MenuNode(const MenuNode&) = delete;
   MenuNode(const MenuNode&&) = delete;
   MenuNode& operator=(const MenuNode&) = delete;
   MenuNode& operator=(const MenuNode&&) = delete;

   virtual ~MenuNode();

   void deleteSources();

   void addItem(const char* name, const char* description = "");

   void setEventListener(EventListener* ml)
   {
      elist = ml;
   }

   Dimension scaleMenu();

   /* WICHTIG post() muß ausgeführt werden, *
    * um das Menü anzuzeigen!!!             */
   virtual int post();

   inline int getID() const
   {
      return myID;
   }

   void setForeground(ColorPair&) const;

   void setBackground(ColorPair&) const;

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




//
///*-------------------/ MenuWindow: /-----------------------*/
//
//
//class MenuWindow : public DerWinNode {
//   /*
//      Alle menu-Methoden unter'man menu(3x)'
//   */
//   Memory::StackPointer<MENU> menunode;
//   ITEM** itemArray = nullptr;
//   ItemVec itempairs;
//   EventListener* elist = nullptr;
//   static int lastID; /* muß in menue.cpp initialisiert werden! */
//   int myID = 0;
//   bool show_description = false;
//   std::string menu_mark;
//
//   virtual void init(Geometry d) {}; /* Funktion sperren! */
//
//   int menuDriver(int command) const;
//
//public:
//   MenuWindow(AbstractWindowNode* parent) : DerWinNode(parent), menunode("MENU"), myID(++lastID) {}
//
//   /* Kopier- und Zuweisschutz: */
//   MenuWindow(const MenuWindow&) = delete;
//   MenuWindow(const MenuWindow&&) = delete;
//   MenuWindow& operator=(const MenuWindow&) = delete;
//   MenuWindow& operator=(const MenuWindow&&) = delete;
//
//   virtual ~MenuWindow();
//
//   void setEventListener(EventListener* ml)
//   {
//      elist = ml;
//   }
//
//   void addItem(const char* name, const char* description = "");
//
//   virtual void init(Dimension position);
//
//   inline int getID() const
//   {
//      return myID;
//   }
//
//   void setBackground(ColorPair& cp) override;
//
//   void setForeground(ColorPair& cp);
//
//   /* Muß vor init() aufgerufen werden: */
//   void showDescription();
//
//   /* Muß vor init() aufgerufen werden: */
//   void setMenuMark( const std::string& );
//
//   Dimension getMenuSize() const;
//
//   int getItemCount() const;
//
//   int getLastItemIndex() const
//   {
//      return getItemCount() - 1;
//   }
//
//   /* Bei Fehler wird -1 zurückgegeben. */
//   int getActiveIndex() const;
//
//   /* Gibt die Rückgabewerte laut 'man set_current_item' zurück: */
//   int setActive(int index = 0) const;
//
//   WindowResponse readKey(int ch);
//
//   void deleteSource() override;
//}; // MenuWindow
//





} // Ende Namespace Ocurses





#endif /* OCURSES_NODES_MENUE_H_ */
