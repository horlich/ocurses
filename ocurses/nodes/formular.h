/*
 * formular.h
 *
 *  Created on: 14.08.2021
 *      Author: herbert
 */

#ifndef OCURSES_NODES_FORMULAR_H_
#define OCURSES_NODES_FORMULAR_H_



#include <ncursesw/form.h>
#include <ncursesw/panel.h>
#include <ncursesw/menu.h>

#include <omemory.h>
#include <otextutil.h>
#include <string>
#include <stack>

#include "../nodes.h"
#include "window.h"
#include "../konstanten.h"
#include "../exceptions.h"


using TextUtil::Dimension;
using Memory::StackPointer;




namespace Ocurses {



class FieldNode : public AbstractNode<FIELD> {
	//
	/* Kopier- und Zuweisschutz: */
	FieldNode(const FieldNode&);
	FieldNode& operator=(const FieldNode&);

public:
	FieldNode(Dimension size, Dimension pos, int offscreen = 0);

	virtual ~FieldNode();

	bool isConnected() { return field_index(getCPointer()) != ERR; }

	int getIndex() const;

	int setOption(Field_Options opt, bool enable) const;

	void setRegexType(const std::string& regex) const;

	void foreground(chtype attr);

	void background(chtype attr);

	int setText(const std::string& str) const;

	std::string getText() const;

	Dimension getSize() const override;

	Dimension getPosition() const override;
};




class DatumFieldNode : public FieldNode {
//
public:
	DatumFieldNode(Dimension size, Dimension pos, int offscreen = 0);

	virtual ~DatumFieldNode() = default;

	void setToday();

	/* wirft IllegalArgumentException, wenn Datum nicht erkannt wird: */
	Zeit::Day getDay() { return Zeit::Day(getText()); }
};



using FieldVec = std::vector<FieldNode*>;


class FormNode : public AbstractNode<FORM> {
	//
	FIELD** fieldArray = nullptr;
	FieldVec felder;
//	Dimension scale;

	/* Kopier- und Zuweisschutz: */
	FormNode(const FormNode&);
	FormNode& operator=(const FormNode&);

protected:
	bool posted = false;

	Dimension scaleForm();

	void createForm();

public:
	FormNode (FieldVec vec) : AbstractNode<FORM>("FORM"), felder(vec) {}

	virtual ~FormNode();

//	FieldNode* addField(Dimension size, Dimension pos, int offscreen);

	int getFieldCount() const;

	int setFormSub(const AbstractWindowNode& w, const TextUtil::Dimension& pos);

	int formDriver(int command) const;

	/* WICHTIG post() muß ausgeführt werden, *
	 * um das Formular anzuzeigen!!!         */
	virtual int post();

	bool isPosted() { return posted; }

	/* current col in field window:	*/
	int getCurcol() { return getCPointer()->curcol; }

	/* current row in field window:	*/
	int getCurrow() { return getCPointer()->currow; }

	int posFormCursor() { return pos_form_cursor(getCPointer()); }

	FIELD* getMember(int index) const;

	/* Sollte vor dem Auslesen der Felder ausgeführt werden, *
	 * um Fehler zu vermeiden:                               */
	int validiereDaten() { return formDriver(REQ_VALIDATION); }


	FIELD* getActiveField() const;

	/* Bei Fehler wird -1 zurückgegeben. */
	int getActiveIndex() const;

	/* Bei Fehler wird nullptr zurückgegeben. */
	FIELD* setActive(int index = 0) const;

	/* Gibt den Index zurück; bei Fehler -1: */
	int setActive(FieldNode& f) const;

	void focusNext() const {
		formDriver(REQ_NEXT_FIELD);
		formDriver(REQ_END_LINE);
	}



	void focusPrev() const {
		formDriver(REQ_PREV_FIELD);
		formDriver(REQ_END_LINE);
	}


	Ocurses::WindowResponse readKey(int ch) { /* vgl. dazu PanelWinNode::readKey(int) */
		const int KEY_TAB = 9;
		int ai = getActiveIndex();
		FieldNode* active = felder.at(ai);
		if (active == nullptr) return Ocurses::CONTINUE_LISTENING;
		switch (ch) {
		case KEY_DOWN:
			focusNext();
			return CONTINUE_LISTENING;
		case KEY_TAB:
			focusNext();
			return CONTINUE_LISTENING;
		case KEY_UP:
			focusPrev();
			return CONTINUE_LISTENING;
		case KEY_LEFT: {
			if (getCurcol() > 0) formDriver(REQ_LEFT_CHAR);
			return CONTINUE_LISTENING;
		}
		case KEY_RIGHT: {
			if (getSize().width > (getCurcol() + 1)) formDriver(REQ_RIGHT_CHAR );
			return CONTINUE_LISTENING;
		}
		case KEY_BACKSPACE: {
			if (getCurcol() > 0) formDriver(REQ_DEL_PREV);
			return CONTINUE_LISTENING;
		}
		default: /* Unbekannte Taste... */
			if (printKey(ch) == E_UNKNOWN_COMMAND) return ASK_PARENT;
			return CONTINUE_LISTENING;
		}
	}



	int printKey(int ch) const;

	Dimension getSize() const override;

	Dimension getPosition() const override;
};






class WindowFormNode : public FormNode {
	/* *this ist das form_win */
	AbstractWindowNode* parentWin;
	TextUtil::Dimension position;
public:
	/* pos ist die Position des form_sub relativ zu *this, also zum form_win: */
	WindowFormNode(FieldVec vec, AbstractWindowNode* parent, TextUtil::Dimension pos);

	virtual ~WindowFormNode() = default;

	WINDOW* getSubWin() const { return getCPointer()->sub; }

	WINDOW* getWindow() const { return getCPointer()->win; }

	WINDOW* getCurrentFieldArea() const { return getCPointer()->w; }

	Dimension getPosition() const override;

	int post() override;
};






} // Ende Namespace Ocurses




#endif /* OCURSES_NODES_FORMULAR_H_ */
