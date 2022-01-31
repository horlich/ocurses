/*
 * menue.h
 *
 *  Created on: 14.08.2021
 *      Author: herbert
 */

#ifndef OCURSES_NODES_MENUE_H_
#define OCURSES_NODES_MENUE_H_


#include <ncursesw/form.h>
#include <ncursesw/panel.h>
#include <ncursesw/menu.h>

#include <omemory.h>
#include <otextutil.h>
#include <string>
#include <stack>

#include "../nodes.h"
#include "../konstanten.h"
#include "../exceptions.h"
#include "window.h"

using TextUtil::Dimension;
using Memory::StackPointer;




namespace Ocurses {






class MenuItemNode : public AbstractNode<ITEM> {
	std::string name, description;

	/* Kopier- und Zuweisschutz: */
	MenuItemNode(const MenuItemNode&);
	MenuItemNode& operator=(const MenuItemNode&);

public:
	MenuItemNode(const std::string& n, const std::string& d = "") :
			AbstractNode("ITEM", new_item(n.c_str(), d.c_str())),
			name(n),
			description(d) {}

	virtual ~MenuItemNode() { free_item(getCPointer()); }

	int getIndex() const;

	MENU* getMenu() const { return getCPointer()->imenu; }

	Dimension getSize() const override;

	Dimension getPosition() const override;
};



using ItemVec = std::vector<MenuItemNode*>;



class MenuNode : public AbstractNode<MENU> {
	//
	ITEM** itemArray = nullptr;
//	std::vector<MenuItemNode*> itemNodes;
	ItemVec items;

	/* Kopier- und Zuweisschutz: */
	MenuNode(const MenuNode&);
	MenuNode& operator=(const MenuNode&);

protected:
	Dimension scaleMenu();

public:
	MenuNode (ItemVec vec) : AbstractNode<MENU>("MENU"), items(vec) {
		createMenu();
	}

	virtual ~MenuNode();

	void foreground(chtype attr);

	void background(chtype attr);

	int setMenuMark(const std::string& str) { return set_menu_mark(getCPointer(), str.c_str()); }

//	MenuItemNode* addItem(const std::string& name, const std::string& description = "");

	virtual void createMenu();

	int getItemCount() const;

	int setMenuSub(const AbstractWindowNode& w, const TextUtil::Dimension& pos);

	int menuDriver(int command) const;

	/* WICHTIG post() muß ausgeführt werden, *
	 * um das Menü anzuzeigen!!!             */
	int post() const;

	ITEM* getMember(int index) const;

	ITEM* getActiveItem() const;

	/* Bei Fehler wird -1 zurückgegeben. */
	int getActiveIndex() const;

	/* Bei Fehler wird nullptr zurückgegeben. */
	ITEM* setActive(int index = 0) const;

	/* Gibt den Index zurück; bei Fehler -1: */
	int setActive(MenuItemNode& f) const;

	int printKey(int ch) const;

	Dimension getSize() const override;

	Dimension getPosition() const override;
};




class WindowMenuNode : public MenuNode {
	//
//	StackPointer<AbstractWindowNode> spWindow;
//	Dimension pos;

public:
	WindowMenuNode(ItemVec items, AbstractWindowNode* parent, TextUtil::Dimension position);

	virtual ~WindowMenuNode() = default;

//	void createMenu() override;

	WINDOW* getSubWin() const { return getCPointer()->sub; }

	WINDOW* getWindow() const { return getCPointer()->win; }

	Dimension getPosition() const override;
};





} // Ende Namespace Ocurses





#endif /* OCURSES_NODES_MENUE_H_ */
