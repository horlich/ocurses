/*
 * panel.h
 *
 *  Created on: 14.08.2021
 *      Author: herbert
 */

#ifndef OCURSES_NODES_PANEL_H_
#define OCURSES_NODES_PANEL_H_



#include <ncursesw/panel.h>
#include <omemory.h>
#include <otextutil.h>
#include "../nodes.h"


using TextUtil::Dimension;
using Memory::StackPointer;




namespace Ocurses {




class PanelNode : public AbstractNode<PANEL> {
	//
	/* Kopier- und Zuweisschutz: */
	PanelNode(const PanelNode&);
	PanelNode& operator=(const PanelNode&);

public:
	static const void* getUserPtr(PANEL* p) { return panel_userptr(p); }

	PanelNode(WINDOW* w);

	virtual ~PanelNode();

	void setUserPtr(const void* pt) const { set_panel_userptr(getCPointer(), pt); }

	const void* getUserPtr() const { return getUserPtr(getCPointer()); }

	PANEL* below() { return panel_below(getCPointer()); }

	void hide() const;

	void show() const;

	void top() const;

	void bottom() const;

	WINDOW* getWindow() const;

	Dimension getSize() const override;

	Dimension getPosition() const override;
};







} // Ende Namespace Ocurses




#endif /* OCURSES_NODES_PANEL_H_ */
