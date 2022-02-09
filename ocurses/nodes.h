/*
 * nodes.h
 *
 *  Created on: 17.06.2020
 *      Author: herbert
 */

#ifndef OCURSES_NODES_H_
#define OCURSES_NODES_H_

#include <ncursesw/curses.h>
//#include <ncursesw/form.h>
//#include <ncursesw/panel.h>
//#include <ncursesw/menu.h>

#include <omemory.h>
#include <otextutil.h>
#include <string>
#include <stack>

#include "konstanten.h"
#include "exceptions.h"


using TextUtil::Dimension;
using Memory::StackPointer;




namespace Ocurses {





class ColorPair {
	/*
	 *    start_color() nicht vergessen!!!
	 *
	 * */
private:
	/* index > 0 && < 256 ! */
	int index, foreground, background;
	bool hasInit = false;
	chtype attr = 0; /* Irgendwie initialisieren.             *
	 	 * chtype ist laut curses.h eine typedef für long int */
public:
	ColorPair(int fg = COLOR_BLACK, int bg = COLOR_WHITE);

   /* Kopier- und Zuweisschutz: */
   ColorPair(const ColorPair&) = delete;
   ColorPair(const ColorPair&&) = delete;
   ColorPair& operator=(const ColorPair&) = delete;
   ColorPair& operator=(const ColorPair&&) = delete;

	virtual ~ColorPair() = default;

	inline int getIndex() const { return index; }

	// Darf erst nach WindowManager::startColor() aufgerufen werden:
	chtype init();

	// Darf erst nach WindowManager::startColor() aufgerufen werden:
	chtype getAttr() const;
};






template<class C>
class AbstractNode {
	StackPointer<C> cpointer;

protected:
	AbstractNode(const std::string& name, C* pt = nullptr) : cpointer(name, pt) {}

	void setPointer(C* pt) { cpointer.setPointer(pt); }

public:
	/* Kopier- und Zuweisschutz: */
	AbstractNode(const AbstractNode&) = delete;
	AbstractNode(const AbstractNode&&) = delete;
	AbstractNode& operator=(const AbstractNode&) = delete;
	AbstractNode& operator=(const AbstractNode&&) = delete;

	virtual ~AbstractNode() = default;

	bool pointerIsValid() const { return cpointer.hasValidPointer(); }

	C* getCPointer() const { return cpointer.getValidPointer(); }

	virtual Dimension getSize() const = 0;

	virtual Dimension getPosition() const = 0;

};





} // Ende Namespace Ocurses


#endif /* OCURSES_NODES_H_ */
