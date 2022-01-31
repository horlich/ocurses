/*
 * menue.cpp
 *
 *  Created on: 14.08.2021
 *      Author: herbert
 */


#include "menue.h"

using namespace Ocurses;
using namespace std;






int MenuItemNode::getIndex() const { return item_index(getCPointer()); }


Dimension MenuItemNode::getSize() const {
	MENU* m = getMenu();
	return Dimension(1, m->width);
}


Dimension MenuItemNode::getPosition() const {
	ITEM* i = getCPointer();
	return Dimension(i->y, i->x);
}






MenuNode::~MenuNode() {
	MENU* pt = getCPointer();
	unpost_menu(pt);
	free_menu(pt);
	delete[] itemArray;
}



void MenuNode::foreground(chtype attr) { set_menu_fore(getCPointer(), attr); }

void MenuNode::background(chtype attr) { set_menu_back(getCPointer(), attr); }


//MenuItemNode* MenuNode::addItem(const std::string& name, const std::string& description) {
//	if (itemArray != nullptr) throw BadDesign(__PRETTY_FUNCTION__, "createMenu() wurde schon aufgerufen.");
//	MenuItemNode* ret = new MenuItemNode(name, description);
//	itemNodes.push_back(ret);
//	return ret;
//}

void MenuNode::createMenu() {
	int size = items.size();
	itemArray = new ITEM*[size + 1];
	int index = 0;
	for (auto it = items.begin(); it != items.end(); it++) {
		itemArray[index++] = (*it)->getCPointer();
	}
	itemArray[index] = nullptr; /* Null-Terminieren */
	setPointer(new_menu(itemArray));
	if (! pointerIsValid()) throw BadDesign(__PRETTY_FUNCTION__, "new_menu(FIELDS**) mißlungen.");
}


int MenuNode::getItemCount() const { return item_count(getCPointer()); }



int MenuNode::menuDriver(int command) const {
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



int MenuNode::post() const { return post_menu(getCPointer()); }


ITEM* MenuNode::getMember(int index) const {
	int anz = getItemCount();
	if ((index < 0) || (index >= anz)) {
		std::stringstream buf;
		buf << __PRETTY_FUNCTION__ << ": Ungültiger Index " << index << " bei Arraysize " << anz << ".";
		throw BadDesign(buf.str());
	}
	return itemArray[index];
}


ITEM* MenuNode::getActiveItem() const { return current_item(getCPointer()); }

/* Bei Fehler wird -1 zurückgegeben. */
int MenuNode::getActiveIndex() const {
	ITEM* f = getActiveItem();
	return (f == nullptr) ? -1 : item_index(f);
}


/* Bei Fehler wird nullptr zurückgegeben. */
ITEM* MenuNode::setActive(int index) const {
	ITEM* ret = getMember(index);
	return (set_current_item(getCPointer(), ret) == E_OK) ? ret : nullptr;
}

/* Gibt den Index zurück; bei Fehler -1: */
int MenuNode::setActive(MenuItemNode& f) const {
	int ret = f.getIndex();
	ITEM* n = setActive(ret);
	if (n == nullptr) return -1;
	if (f.getCPointer() != n) throw BadDesign(__PRETTY_FUNCTION__, "Fremdes Feld als Argument.");
	return ret;
}


Dimension MenuNode::scaleMenu() {
	Dimension scale;
	scale_menu(getCPointer(), &(scale.height), &(scale.width));
	return scale;
}


int MenuNode::setMenuSub(const AbstractWindowNode& w, const TextUtil::Dimension& pos) {
	TextUtil::Dimension sc = scaleMenu();
	return set_menu_sub(getCPointer(), derwin(w.getCPointer(), sc.height, sc.width, pos.height, pos.width));
//	return set_menu_sub(getCPointer(), derwin(w.getCPointer(), 4, 20, 8, 5));
}


int MenuNode::printKey(int ch) const {
	return menu_driver(getCPointer(), ch);
}


Dimension MenuNode::getSize() const {
	MENU* m = getCPointer();
	return Dimension(m->height, m->width);
}


Dimension MenuNode::getPosition() const {
	return Dimension(0,0);
}










WindowMenuNode::WindowMenuNode(ItemVec items, AbstractWindowNode* w, TextUtil::Dimension position) :
		MenuNode(items) {
	set_menu_win(getCPointer(), w->getCPointer());
	setMenuSub(*w, position);
}



//void WindowMenuNode::createMenu() {
//	MenuNode::createMenu();
//	AbstractWindowNode* w = spWindow.getValidPointer();
//	set_menu_win(getCPointer(), w->getCPointer());
//	setMenuSub(*w, pos);
//}

Dimension WindowMenuNode::getPosition() const {
	Dimension ret;
	getbegyx(getSubWin(), ret.height, ret.width);
	return ret;
}



