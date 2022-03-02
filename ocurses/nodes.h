/*
 * nodes.h
 *
 *  Created on: 17.06.2020
 *      Author: herbert
 */

#ifndef OCURSES_NODES_H_
#define OCURSES_NODES_H_

#include <omemory.h>
#include <otextutil.h>


using TextUtil::Dimension;
using Memory::StackPointer;




namespace Ocurses {








template<class C>
class AbstractNode {
   //
	StackPointer<C> cpointer;

protected:
	AbstractNode(const std::string& name, C* pt = nullptr) : cpointer(name, pt) {}

	inline void setPointer(C* pt) { cpointer.setPointer(pt); }

public:
	/* Kopier- und Zuweisschutz: */
	AbstractNode(const AbstractNode&) = delete;
	AbstractNode(const AbstractNode&&) = delete;
	AbstractNode& operator=(const AbstractNode&) = delete;
	AbstractNode& operator=(const AbstractNode&&) = delete;

	virtual ~AbstractNode() = default;

	inline bool pointerIsValid() const { return cpointer.hasValidPointer(); }

	inline C* getCPointer() const { return cpointer.getValidPointer(); }

	virtual Dimension getSize() const = 0;

	virtual Dimension getPosition() const = 0;

};





} // Ende Namespace Ocurses


#endif /* OCURSES_NODES_H_ */
