/*
 * menue.cpp
 *
 *  Created on: 14.08.2021
 *      Author: herbert
 */


#include "menue.h"

using namespace Ocurses;
using namespace std;


/* statische Klassenvariable müssen so initialisiert werden: */
int MenuNode::lastID = 0;
int MenuWindow::lastID = 0;


/*-------------------/ MenuNode: /-----------------------*/

void MenuNode::deleteSources()
{
   if (! pointerIsValid()) return; /* wurde bereits gelöscht */
   MENU* pt = getCPointer();
   unpost_menu(pt);
   free_menu(pt);
   ITEM** ptr = itemArray;
   while (ptr != nullptr) {
      delete ptr++;
   }
   delete[] itemArray;
   setPointer(nullptr);
   itemArray = nullptr;
//   itempairs.clear();
}


MenuNode::~MenuNode()
{
   deleteSources();
}


void MenuNode::addItem(const char* name, const char* description)
{
   /* name und description müssen const char* sein. Bei
      std::string.c_str() werden rätselhafterweise
      fehlerhafte Zeichen angezeigt.                 */
   ItemPair p(name, description);
   itempairs.push_back(p);
}


void MenuNode::setForeground(ColorPair& cp) const
{
   set_menu_fore(getCPointer(), cp.getAttr());
}

void MenuNode::setBackground(ColorPair& cp) const
{
   set_menu_back(getCPointer(), cp.getAttr());
}


void MenuNode::showDescription(bool b)
{
   b ? menu_opts_on(getCPointer(), O_SHOWDESC) : menu_opts_off(getCPointer(), O_SHOWDESC);
}


void MenuNode::init()
{
   if (pointerIsValid())
      throw BadDesign(__PRETTY_FUNCTION__, " darf nur einmal aufgerufen werden!");
   int size = itempairs.size();
   if (size == 0) return; /* Keine Items definiert */
   itemArray = new ITEM*[size + 1];
   int index = 0;
   for (ItemPair p : itempairs) itemArray[index++] = new_item(p.first, p.second);
   itemArray[index] = nullptr; /* Null-Terminieren */
   setPointer(new_menu(itemArray));
   if (! pointerIsValid()) throw BadDesign(__PRETTY_FUNCTION__, "new_menu(FIELDS**) mißlungen.");
}


int MenuNode::getItemCount() const
{
   return item_count(getCPointer());
}


int MenuNode::setMenuMark( const char* mark ) const
{
   return set_menu_mark(getCPointer(), mark);
}


int MenuNode::menuDriver(int command) const
{
   /* siehe dazu man menu_driver: */
   int ret = menu_driver(getCPointer(), command);
   if (ret == E_OK) return ret;
   std::string err;
   switch (ret) {
   case E_BAD_ARGUMENT:
      err = "Bad Argument";
      break;
   case E_BAD_STATE:
      err = "Bad State";
      break;
   case E_NOT_POSTED:
      err = "post_menu(MENU) noch nicht ausgeführt";
      break;
   case E_NO_MATCH:
      err = "No Match";
      break;
   case E_REQUEST_DENIED:
      err = "Request denied";
      break;
   case E_SYSTEM_ERROR:
      err = "SYSTEM ERROR";
      break;
   case E_UNKNOWN_COMMAND:
      err = "Unbekannter Befehl";
      break;
   default:
      err = "Das darf nicht sein";
   }
   if (ret != E_OK) throw BadDesign(__PRETTY_FUNCTION__, err);
   return ret;
}


int MenuNode::post()
{
   if (! pointerIsValid())
      throw BadDesign("MenuNode::post(): setItems() noch nicht aufgerufen!");
   return post_menu(getCPointer());
}



/* Bei Fehler wird -1 zurückgegeben. */
int MenuNode::getActiveIndex() const
{
   ITEM* f = current_item(getCPointer());
   return (f == nullptr) ? -1 : item_index(f);
}


int MenuNode::setActive(int index) const
{
   if ((index < 0) || (index >= getItemCount())) {
      std::stringstream buf;
      buf << __PRETTY_FUNCTION__ << ": Ungültiger Index " << index << ".";
      throw BadDesign(buf.str());
   }
   ITEM* it = itemArray[index];
   return set_current_item(getCPointer(), it);
}


Dimension MenuNode::scaleMenu()
{
   Dimension scale;
   scale_menu(getCPointer(), &(scale.height), &(scale.width));
   return scale;
}


int MenuNode::setMenuSub(const AbstractWindowNode& w, const TextUtil::Dimension& pos)
{
   TextUtil::Dimension sc = scaleMenu();
   return set_menu_sub(getCPointer(), derwin(w.getCPointer(), sc.height, sc.width, pos.height, pos.width));
}

Ocurses::WindowResponse MenuNode::readKey(int ch)
{
   switch (ch) {
   case KEY_DOWN:
      if (getActiveIndex() < getLastItemIndex()) menuDriver(REQ_DOWN_ITEM);
      return CONTINUE_LISTENING;
   case KEY_UP:
      if (getActiveIndex() > 0) menuDriver(REQ_UP_ITEM);
      return CONTINUE_LISTENING;
   case '\n':
      if (elist != nullptr) { /* Es ist ein EventListener definiert */
         MenuEvent ev(getID());
         ev.setSelectedIndex(getActiveIndex());
         elist->eventTriggered(ev);
      }
      return CONTINUE_LISTENING;
   case '\E':
      return CONTINUE_LISTENING;
   default: /* Unbekannte Taste... */
      return CONTINUE_LISTENING;
   }
}

int MenuNode::printKey(int ch) const
{
   return menu_driver(getCPointer(), ch);
}


Dimension MenuNode::getSize() const
{
   MENU* m = getCPointer();
   return Dimension(m->height, m->width);
}


Dimension MenuNode::getPosition() const
{
   return Dimension(0,0);
}



//
///*-------------------/ MenuWindow: /-----------------------*/
//
//
//MenuWindow::~MenuWindow()
//{
//   deleteSource();
//}
//
//
//
//void MenuWindow::addItem(const char* name, const char* description)
//{
//   /* name und description müssen const char* sein. Bei
//      std::string.c_str() werden rätselhafterweise
//      fehlerhafte Zeichen angezeigt.                 */
//   if (pointerIsValid())
//      throw BadDesign(__PRETTY_FUNCTION__, " Menü ist bereits gepostet.");
//   ItemPair p(name, description);
//   itempairs.push_back(p);
//}
//
//
//void MenuWindow::init(Dimension position)
//{
//   /*----------------- MENU einrichten: ---------------------*/
//   if (pointerIsValid())
//      throw BadDesign(__PRETTY_FUNCTION__, " WINDOW schon vorhanden.");
//   int size = itempairs.size();
//   if (size == 0)
//      throw BadDesign(__PRETTY_FUNCTION__, " Keine Items definiert.");
//   itemArray = new ITEM*[size + 1];
//   int index = 0;
//   for (ItemPair p : itempairs) itemArray[index++] = new_item(p.first, p.second);
//   itemArray[index] = nullptr; /* Null-Terminieren! */
//   /* TODO: set_menu_format() allenfalls implementieren! */
//   menunode.setPointer(new_menu(itemArray));
//   if (! menunode.hasValidPointer()) throw BadDesign(__PRETTY_FUNCTION__, "new_menu(FIELDS**) mißlungen.");
//   if (! show_description) menu_opts_off(menunode.getPointer(), O_SHOWDESC); /* Description ausschalten */
//   set_menu_mark(menunode.getPointer(), menu_mark.c_str());
//
//
//   /*----------------- WINDOW einrichten: ---------------------*/
//   Dimension msize;
//   scale_menu(menunode.getPointer(), &(msize.height), &(msize.width));
//   DerWinNode::init(Geometry{msize.height + 2, msize.width + 2, position.height, position.width});
//   int result = set_menu_win(menunode.getPointer(), getCPointer());
//   if (result != E_OK)
//      throw BadDesign(__PRETTY_FUNCTION__, " set_menu_win() mißlungen.");
//   result = set_menu_sub(menunode.getPointer(), derwin(getCPointer(), msize.height, msize.width,1,1));
//   if (result != E_OK)
//      throw BadDesign(__PRETTY_FUNCTION__, " set_menu_sub() mißlungen.");
//   drawBox();
//   result = post_menu(menunode.getPointer());
//   if (result != E_OK) {
//      std::string errstr = (result == E_NO_ROOM) ?
//         "Menü paßt nicht ins Fenster. You should consider to use set_menu_format() to solve the problem." :
//         "post_menu mißlungen";
//      throw BadDesign(__PRETTY_FUNCTION__, errstr);
//   }
//}
//
//
//void MenuWindow::setBackground(ColorPair& cp)
//{
//   DerWinNode::setBackground(cp);
//   set_menu_back(menunode.getPointer(), cp.getAttr());
//}
//
//
//void MenuWindow::setForeground(ColorPair& cp)
//{
//   set_menu_fore(menunode.getPointer(), cp.getAttr());
//}
//
//
//void MenuWindow::showDescription()
//{
//   if (menunode.hasValidPointer())
//      throw BadDesign(__PRETTY_FUNCTION__, " MENU ist bereits initiiert.");
//   show_description = true;
//}
//
//
//void MenuWindow::setMenuMark( const std::string& mark )
//{
//   if (menunode.hasValidPointer())
//      throw BadDesign(__PRETTY_FUNCTION__, " MENU ist bereits initiiert.");
//   menu_mark = mark;
//}
//
//
//Dimension MenuWindow::getMenuSize() const
//{
//   MENU* m = menunode.getPointer();
//   return Dimension(m->height, m->width);
//}
//
//
//int MenuWindow::getItemCount() const
//{
//   return item_count(menunode.getPointer());
//}
//
//
//int MenuWindow::menuDriver(int command) const
//{
//   /* siehe dazu man menu_driver: */
//   int ret = menu_driver(menunode.getPointer(), command);
//   if (ret == E_OK) return ret;
//   std::stringstream buf;
//   buf << "menu_driver() konnte nicht ausgeführt werden: ";
//   switch (ret) {
//   case E_BAD_ARGUMENT:
//      buf << "Bad Argument";
//      break;
//   case E_BAD_STATE:
//      buf << "Bad State";
//      break;
//   case E_NOT_POSTED:
//      buf << "post_menu(MENU) noch nicht ausgeführt";
//      break;
//   case E_NO_MATCH:
//      buf << "No Match";
//      break;
//   case E_REQUEST_DENIED:
//      buf << "Request denied";
//      break;
//   case E_SYSTEM_ERROR:
//      buf << "SYSTEM ERROR";
//      break;
//   case E_UNKNOWN_COMMAND:
//      buf << "Unbekannter Befehl";
//      break;
//   default:
//      buf << "Das darf nicht sein";
//   }
//   throw BadDesign(__PRETTY_FUNCTION__, buf.str());
//   return ret;
//}
//
//
///* Bei Fehler wird -1 zurückgegeben. */
//int MenuWindow::getActiveIndex() const
//{
//   ITEM* f = current_item(menunode.getPointer());
//   return (f == nullptr) ? -1 : item_index(f);
//}
//
//
//int MenuWindow::setActive(int index) const
//{
//   if ((index < 0) || (index >= getItemCount())) {
//      std::stringstream buf;
//      buf << __PRETTY_FUNCTION__ << ": Ungültiger Index " << index << ".";
//      throw BadDesign(buf.str());
//   }
//   ITEM* it = itemArray[index];
//   return set_current_item(menunode.getPointer(), it);
//}
//
//Ocurses::WindowResponse MenuWindow::readKey(int ch)
//{
//   switch (ch) {
//   case KEY_DOWN:
//      if (getActiveIndex() < getLastItemIndex()) menuDriver(REQ_DOWN_ITEM);
//      return CONTINUE_LISTENING;
//   case KEY_UP:
//      if (getActiveIndex() > 0) menuDriver(REQ_UP_ITEM);
//      return CONTINUE_LISTENING;
//   case '\n':
//      if (elist != nullptr) { /* Es ist ein EventListener definiert */
//         MenuEvent ev(getID());
//         ev.setSelectedIndex(getActiveIndex());
//         elist->eventTriggered(ev);
//      }
//      quit();
//      return CONTINUE_LISTENING;
//   case '\E':
//      quit();
//      return CONTINUE_LISTENING;
//   default: /* Unbekannte Taste... */
//      return CONTINUE_LISTENING;
//   }
//}
//
//
//void MenuWindow::deleteSource()
//{
//   MENU* pt = menunode.getPointer();
//   unpost_menu(pt);
//   free_menu(pt);
//   ITEM** ptr = itemArray;
//   while (ptr != nullptr) {
//      delete ptr++;
//   }
//   delete[] itemArray;
//   menunode.setPointer(nullptr);
//   itemArray = nullptr;
//   DerWinNode::deleteSource();
//}
//


