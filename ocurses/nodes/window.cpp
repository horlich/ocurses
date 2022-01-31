/*
 * windows.cpp
 *
 *  Created on: 14.08.2021
 *      Author: herbert
 */





#include "window.h"

using namespace Ocurses;
using namespace std;










AbstractWindowNode::~AbstractWindowNode() {
	if (pointerIsValid()) {
//		MESSAGE.stream() << "vor delwin" << endl;
		delwin(getCPointer());
		setPointer(nullptr);
	}
}

Dimension AbstractWindowNode::getSize() const {
	Dimension ret;
	getmaxyx(getCPointer(), ret.height, ret.width);
	return ret;
}

Dimension AbstractWindowNode::getPosition() const {
	Dimension ret;
	getbegyx(getCPointer(), ret.height, ret.width);
	return ret;
}


void AbstractWindowNode::update() const {
	wnoutrefresh(getCPointer());
}


int AbstractWindowNode::getChar() const { return wgetch(getCPointer()); }


int AbstractWindowNode::ungetChar(int ch) const {
	return ungetch(ch);
}


void AbstractWindowNode::drawBox() const {
	box(getCPointer(),0,0); /* entspricht: box(source, ACS_VLINE, ACS_HLINE); */
}


void AbstractWindowNode::moveCursor(TextUtil::Dimension pos) const {
	wmove(getCPointer(), pos.height, pos.width);
}


//void AbstractWindowNode::showCursor(bool show = true) const {
//	short i = (show) ? 1 : 0;
//	curs_set(i);
//}


void AbstractWindowNode::clearLine(int y) const {
	moveCursor(TextUtil::Dimension(0, y));
	wclrtoeol(getCPointer());
}


TextUtil::Dimension AbstractWindowNode::getCursor() const {
	TextUtil::Dimension ret;
	getyx(getCPointer(),ret.height, ret.width);
	return ret;
}


/* Gibt -1 zurück, wenn keine Daten vorhanden: */
int AbstractWindowNode::colorPairIndex() const {
	attr_t attribut;
	short index = -1;
	wattr_get(getCPointer(), &attribut, &index, nullptr);
	if (attribut & A_COLOR) return index;
	return index;
}


void AbstractWindowNode::color(ColorPair& cp) {
	wattron(getCPointer(), COLOR_PAIR(cp.getIndex()));
}

void AbstractWindowNode::uncolor(ColorPair& cp) {
	wattroff(getCPointer(), COLOR_PAIR(cp.getIndex()));
}


short AbstractWindowNode::getBgColorIndex() const {
	attr_t attributes;
	short cpair;
	int result = wattr_get(getCPointer(), &attributes, &cpair, nullptr);
	if (result == ERR) throw BadDesign(__PRETTY_FUNCTION__, "wattr_get() mißlungen.");
	if (attributes & A_COLOR) {
		MESSAGE.stream() << __PRETTY_FUNCTION__ << ": SWI=" << Ocurses::SCHWARZ_WEISS.getIndex() << "; BCI=" << cpair << std::endl;
		return cpair;
	}
	throw BadDesign(__PRETTY_FUNCTION__, "Keine Farbe gefunden. Seltsam.");
}


//void AbstractWindowNode::markLine(short colorIndex, int line, int startx) const {
//	mvwchgat(getCPointer(), line, startx, -1, A_COLOR, colorIndex, nullptr);
//}
//
//
//void AbstractWindowNode::markLine(ColorPair& cp, int line, int startx) const { markLine(cp.getIndex(), line, startx); }


void AbstractWindowNode::enableScrolling(bool toggle) const {
	scrollok(getCPointer(), toggle);
	idlok(getCPointer(), toggle); /* braucht es das? */
}


void AbstractWindowNode::setBackground(ColorPair& cp) {
	wbkgd(getCPointer(), cp.getAttr());
}


void AbstractWindowNode::setLineBackground(int line, ColorPair& cp, int startx) const {
	mvwchgat(getCPointer(), line, startx, -1, cp.getAttr(), cp.getIndex(), nullptr);
}

void AbstractWindowNode::setKeypad() const {
	if (keypad(getCPointer(), true) == ERR) {
		throw OcursesException(OException::envNameMess(__PRETTY_FUNCTION__, "Unbekannter Fehler"));
	}
}


void AbstractWindowNode::scrolln(int lines) const {
	int res = wscrl(getCPointer(), lines);
	if (res == ERR) throw BadDesign(__PRETTY_FUNCTION__, "Scrolling gescheitert.");
}



void AbstractWindowNode::printlnZentriert(int line, const std::string& text) const {
	int maxLength = getSize().width;
	int x;
	if ((int)text.length() >= maxLength) {
		x = 0;
	} else {
		x = (maxLength - (int)text.length()) / 2;
	}
	println(line, text, x);
}


namespace Ocurses {
	//
	void _putn(std::ostream& os, int n, char c) {
		for (; n > 0; n--) os.put(c);
	}
}

void AbstractWindowNode::printlnBetont(int line, const std::string& text) const {
	int textLength = text.length();
	int maxLength = getSize().width;
	const int abstand = 2;
	if (textLength >= maxLength) {
		println(line, text);
	} else if ((textLength + (abstand * 4)) >= maxLength) {
		printlnZentriert(line, text);
	} else {
		char bet = '=';
		int xx = ((maxLength - (int)text.length()) / 2) - abstand;
		std::stringstream buf;
		_putn(buf, xx, bet);
		_putn(buf, abstand, ' ');
		buf << text;
		_putn(buf, abstand, ' ');
		xx = maxLength - buf.tellp();
		_putn(buf, xx, bet);
		println(line,  buf.str());
	}
}










WinNode::WinNode(TextUtil::Dimension size, TextUtil::Dimension pos) {
	setPointer(newwin(size.height, size.width, pos.height, pos.width));
	if (! pointerIsValid()) {
		std::stringstream buf;
		buf << __PRETTY_FUNCTION__ << ": newwin() fehlgeschlagen mit folgenden Parametern:\n" <<
				"h=" << size.height << "; w=" << size.width << "; y=" << pos.height <<"; x=" << pos.width << endl;
		throw BadDesign(buf.str());
	}
}


WinNode::WinNode(WINDOW* w) { this->setPointer(w);	}


void WinNode::popUp(PanelWinNode& popup) {
	popup.draw();
	popup.getPanel()->top();
	update_panels();
	doupdate();
	WindowResponse resp = CONTINUE_LISTENING;
	while (resp != Ocurses::QUIT_WINDOW) {
//		resp = popup.readKey(popup.getChar());
		resp = popup.readKey(wgetch(popup.getCPointer()));
	}
	popup.getPanel()->hide();
}





PanelWinNode::PanelWinNode(Dimension size, Dimension pos) :
	WinNode(size, pos), panel("Panel") {
	PanelNode* pn = new PanelNode(this->getCPointer());
	panel.setPointer(pn);
	update_panels();
}





void AbstractSubWinNode::update() const {
	touchwin(getParentSource()); /* siehe dazu man subwin! */
	AbstractWindowNode::update();
}


DerWinNode::DerWinNode(AbstractWindowNode& w, TextUtil::Dimension size, TextUtil::Dimension pos) : AbstractSubWinNode(w) {
	setPointer(derwin(w.getCPointer(), size.height, size.width, pos.height, pos.width));
	if (! pointerIsValid()) {
		std::stringstream buf;
		buf << __PRETTY_FUNCTION__ << ": derwin() fehlgeschlagen mit folgenden Parametern:\n" <<
				"Parent-Pointer=" << w.getCPointer() << "; h=" << size.height << "; w=" << size.width << "; y=" << pos.height <<"; x=" << pos.width << endl;
		throw BadDesign(buf.str());
	}
}

SubWinNode::SubWinNode(AbstractWindowNode& w, TextUtil::Dimension size, TextUtil::Dimension pos): AbstractSubWinNode(w) {
	setPointer(subwin(w.getCPointer(), size.height, size.width, pos.height, pos.width));
	if (! pointerIsValid()) {
		std::stringstream buf;
		buf << __PRETTY_FUNCTION__ << ": subwin() fehlgeschlagen mit folgenden Parametern:\n" <<
				"Parent-Pointer=" << w.getCPointer() << "; h=" << size.height << "; w=" << size.width << "; y=" << pos.height <<"; x=" << pos.width << endl;
		throw BadDesign(buf.str());
	}
}








void ScreenNode::startColor() {
	if (! has_colors()) throw ColorException("Dieses Terminal hat keine Farben.");
	int err = start_color();
	if (err != OK) throw ColorException("start_color() mißlungen.");
	imFarbmodus = true;
}

ScreenNode::ScreenNode() {
	setPointer(initscr());
}

ScreenNode::~ScreenNode() {
//	MESSAGE.stream() << "vor endwin()" << endl;
	endwin();
	setPointer(nullptr);
}

void ScreenNode::initDefaults() {
	startColor();
	cbreak(); /* Keine Pufferung bei Texteingabe */
//	nonl(); /* nl() [=default] und nonl() betreffen die Enter-Taste */
	noecho();
	curs_set(0);
	keypad(getCPointer(), TRUE); /* getCPointer() == stdscr */
}










