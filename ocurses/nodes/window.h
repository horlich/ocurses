/*
 * windows.h
 *
 *  Created on: 14.08.2021
 *      Author: herbert
 */

#ifndef OCURSES_NODES_WINDOW_H_
#define OCURSES_NODES_WINDOW_H_




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
#include "panel.h"


using TextUtil::Dimension;
using Memory::StackPointer;




namespace Ocurses {



class PanelWinNode;



class AbstractWindowNode : public AbstractNode<WINDOW> {
	//
	/* Kopier- und Zuweisschutz: */
	AbstractWindowNode(const AbstractWindowNode&);
	AbstractWindowNode& operator=(const AbstractWindowNode&);

protected:
	AbstractWindowNode() : AbstractNode<WINDOW>("WINDOW") {}

public:
	virtual ~AbstractWindowNode();

	/* NICHT verwenden, wenn Panels im Einsatz!
	 * Diesfalls WindowManager::updatePanels() verwenden! */
	virtual void update() const;

	Dimension getSize() const override;

	Dimension getPosition() const override;

	int doResize(int height, int width) { return wresize(getCPointer(), height, width); }

	virtual int doResize() { return doResize(LINES, COLS); } /* Fullscreen */

	int doClear() const { return wclear(getCPointer()); }

	int doErase() const { return werase(getCPointer()); }

	int getChar() const;

	int ungetChar(int ch) const;

	virtual void draw() {}

	void drawBox() const;

	Dimension getCursor() const;

	void moveCursor(TextUtil::Dimension pos) const;

	void clearLine(int y) const;

	template <typename... Args>
	void mvprintf(Dimension pos, const std::string& fmt, Args... args) const;

	void println(int line, const std::string& text, int startx = 0) const { mvprintf(Dimension(line, startx), text); }

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

	void setKeypad() const;

	void enableScrolling(bool toggle = true) const;

	void scrolln(int lines = 1) const;
};



template <typename... Args>
void AbstractWindowNode::mvprintf(Dimension pos, const std::string& fmt, Args... args) const {
	mvwprintw(getCPointer(), pos.height, pos.width, fmt.c_str(), args...);
}


/* Bei Scrollproblemen diese Funktion benützen und mit \n abschließen. Siehe dazu
 * https://stackoverflow.com/questions/29565403/how-to-scroll-a-window-other-than-stdscreen-in-ncurses */
template <typename... Args>
void AbstractWindowNode::printf(const std::string& fmt, Args... args) const {
	wprintw(getCPointer(), fmt.c_str(), args...);
}



class WinNode : public AbstractWindowNode {
//
public:
	WinNode(TextUtil::Dimension size, TextUtil::Dimension pos);

	WinNode(WINDOW* w);

	void popUp(PanelWinNode& popup);
};



class FullScreenWinNode : public WinNode {
// Fullscreen !
public:
	FullScreenWinNode() : WinNode(TextUtil::Dimension(LINES, COLS), TextUtil::Dimension(0,0)) {}

	virtual ~FullScreenWinNode() = default;
};



class PanelWinNode : public WinNode {
	//
	Memory::HeapPointer<PanelNode> panel;

public:
	PanelWinNode(Dimension size = Dimension(LINES, COLS), Dimension pos = Dimension(0,0));

	virtual PanelNode* getPanel() { return panel.getPointer(); }

	virtual ~PanelWinNode() { panel.deletePointer(); }

	virtual WindowResponse readKey(int ch) { /* Unterklassen fügen hinzu... */
		return ASK_PARENT;
	}
};



class AbstractSubWinNode : public AbstractWindowNode {
	AbstractWindowNode& parent;

public:
	AbstractSubWinNode(AbstractWindowNode& w) : parent(w) {}

	WINDOW* getParentSource() const { return wgetparent(getCPointer()); }

	const AbstractWindowNode& getParentNode() const { return parent; }

	void update() const override;

	virtual ~AbstractSubWinNode() = default;
};



class DerWinNode : public AbstractSubWinNode {
//
public:
	DerWinNode(AbstractWindowNode& w, TextUtil::Dimension size, TextUtil::Dimension pos);

	virtual ~DerWinNode() = default;
};



class SubWinNode : public AbstractSubWinNode {
//
public:
	SubWinNode(AbstractWindowNode& w, TextUtil::Dimension size, TextUtil::Dimension pos);

	virtual ~SubWinNode() = default;
};






class ScreenNode : public AbstractWindowNode {
	//
	bool imFarbmodus = false;

	/* Kopier- und Zuweisschutz: */
	ScreenNode(const ScreenNode&);
	ScreenNode& operator=(const ScreenNode&);

protected:
	void startColor();

public:
	ScreenNode();

	virtual ~ScreenNode();

	void initDefaults();
};






} // Ende Namespace Ocurses






#endif /* OCURSES_NODES_WINDOW_H_ */
