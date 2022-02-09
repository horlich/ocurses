/*
 * menue.cpp
 *
 *  Created on: 14.08.2021
 *      Author: herbert
 */


#include "menue.h"

using namespace Ocurses;
using namespace std;





/*-------------------/ MenuNode: /-----------------------*/

void MenuNode::deleteAll()
{
   MENU* pt = getCPointer();
   unpost_menu(pt);
   free_menu(pt);
   ITEM** ptr = itemArray;
   while (ptr != nullptr) {
      delete ptr++;
   }
   delete[] itemArray;
}


MenuNode::~MenuNode()
{
   deleteAll();
}


void MenuNode::addItem(const char* name, const char* description)
{
   /* name und description müssen const char* sein. Bei
      std::string.c_str() werden rätselhafterweise
      fehlerhafte Zeichen angezeigt.                 */
   ItemPair p(name, description);
   itempairs.push_back(p);
}


void MenuNode::foreground(ColorPair& cp) const
{
   set_menu_fore(getCPointer(), cp.getAttr());
}

void MenuNode::background(ColorPair& cp) const
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
      if (mlist != nullptr) {
      mlist->itemSelected(getActiveIndex());
      }
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





/*-------------------/ WindowMenuNode: /-----------------------*/

WindowMenuNode::WindowMenuNode(AbstractWindowNode& w, TextUtil::Dimension pos) : parentWin(w), position(pos) {}


Dimension WindowMenuNode::getPosition() const
{
   Dimension ret;
   getbegyx(getSubWin(), ret.height, ret.width);
   return ret;
}

int WindowMenuNode::post()
{
   if (! pointerIsValid())
      throw BadDesign("WindowFormNode::post(): setFields() noch nicht aufgerufen!");
   set_menu_win(getCPointer(), parentWin.getCPointer());
   setMenuSub(parentWin, position);
   return post_menu(getCPointer());
}


