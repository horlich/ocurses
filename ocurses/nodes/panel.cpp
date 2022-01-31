/*
 * panel.cpp
 *
 *  Created on: 14.08.2021
 *      Author: herbert
 */



#include "panel.h"

using namespace Ocurses;
using namespace std;








PanelNode::PanelNode(WINDOW* w) : AbstractNode("PANEL", new_panel(w)) {}



PanelNode::~PanelNode() {
	/* ACHTUNG: del_panel muß UNBEDINGT vor delwin ausgeführt werden! */
//	MESSAGE.stream() << "vor del_panel" << endl;
	del_panel(getCPointer());
}


void PanelNode::hide() const { hide_panel(getCPointer()); }


void PanelNode::show() const { show_panel(getCPointer()); }


void PanelNode::top() const { top_panel(getCPointer()); }


void PanelNode::bottom() const { bottom_panel(getCPointer()); }


WINDOW* PanelNode::getWindow() const { return panel_window(getCPointer()); }


Dimension PanelNode::getSize() const {
	Dimension ret;
	getmaxyx(getWindow(), ret.height, ret.width);
	return ret;
}


Dimension PanelNode::getPosition() const {
	Dimension ret;
	getbegyx(getWindow(), ret.height, ret.width);
	return ret;
}





