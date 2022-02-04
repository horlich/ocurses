/*
 * formular.h
 *
 *  Created on: 14.08.2021
 *      Author: herbert
 */

#ifndef OCURSES_NODES_FORMULAR_H_
#define OCURSES_NODES_FORMULAR_H_



#include <ncursesw/form.h>
#include <omemory.h>
#include <otextutil.h>
#include "../nodes.h"
#include "../../ocurses.h"


using TextUtil::Dimension;
using Memory::StackPointer;




namespace Ocurses {


/*--------------------------/ FieldNode: /------------------------------------*/

class FieldNode : public AbstractNode<FIELD> {
   //
public:
   FieldNode() : AbstractNode("FIELD") {}

   /* Kopier- und Zuweisschutz: */
   FieldNode(const FieldNode&) = delete;
   FieldNode(const FieldNode&&) = delete;
   FieldNode& operator=(const FieldNode&) = delete;
   FieldNode& operator=(const FieldNode&&) = delete;

   virtual ~FieldNode();

   void init(Geometry g, int offscreen = 0);   /* Number of offscreen-rows */

   bool isConnected()
   {
      return field_index(getCPointer()) != ERR;
   }

   int getIndex() const;

   int setOption(Field_Options opt, bool enable) const;

   void setRegexType(const std::string& regex) const;

   void foreground(ColorPair&);

   void background(ColorPair&);

   int setText(const std::string& str) const;

   std::string getText() const;

   Dimension getSize() const override;

   Dimension getPosition() const override;
}; // FieldNote



/*--------------------------/ DatumFieldNode: /------------------------------------*/

class DatumFieldNode : public FieldNode {
//
public:
   DatumFieldNode(Geometry, int offscreen = 0);

   /* Kopier- und Zuweisschutz: */
   DatumFieldNode(const DatumFieldNode&) = delete;
   DatumFieldNode(const DatumFieldNode&&) = delete;
   DatumFieldNode& operator=(const DatumFieldNode&) = delete;
   DatumFieldNode& operator=(const DatumFieldNode&&) = delete;

   virtual ~DatumFieldNode() = default;

   void setToday();

   /* wirft IllegalArgumentException, wenn Datum nicht erkannt wird: */
   Zeit::Day getDay()
   {
      return Zeit::Day(getText());
   }
}; // DatumFieldNote



/*--------------------------/ FormNode: /------------------------------------*/

using FieldVec = std::vector<FieldNode*>;


class FormNode : public AbstractNode<FORM> {
   //
   FIELD** fieldArray = nullptr;
   FieldVec felder;

   void deleteAll();

protected:
   bool posted = false;

   Dimension scaleForm();

public:
   FormNode() : AbstractNode<FORM>("FORM") {}

   /* Kopier- und Zuweisschutz: */
   FormNode(const FormNode&) = delete;
   FormNode(const FormNode&&) = delete;
   FormNode& operator=(const FormNode&) = delete;
   FormNode& operator=(const FormNode&&) = delete;

   virtual ~FormNode();

   /* Gebrauch: setFields({&feld_eins, &feld_zwei [,...]}) */
   void setFields(FieldVec&& vec);

   int getFieldCount() const;

   int setFormSub(const AbstractWindowNode& w, const TextUtil::Dimension& pos);

   int formDriver(int command) const;

   /* WICHTIG post() muß ausgeführt werden, *
    * um das Formular anzuzeigen!!!         */
   virtual int post();

   bool isPosted()
   {
      return posted;
   }

   /* current col in field window:	*/
   int getCurcol()
   {
      return getCPointer()->curcol;
   }

   /* current row in field window:	*/
   int getCurrow()
   {
      return getCPointer()->currow;
   }

   int posFormCursor()
   {
      return pos_form_cursor(getCPointer());
   }

   FIELD* getMember(int index) const;

   /* Sollte vor dem Auslesen der Felder ausgeführt werden, *
    * um Fehler zu vermeiden:                               */
   int validiereDaten()
   {
      return formDriver(REQ_VALIDATION);
   }

   FIELD* getActiveField() const;

   /* Bei Fehler wird -1 zurückgegeben. */
   int getActiveIndex() const;

   /* Bei Fehler wird nullptr zurückgegeben. */
   FIELD* setActive(int index = 0) const;

   /* Gibt den Index zurück; bei Fehler -1: */
   int setActive(FieldNode& f) const;

   void focusNext() const;

   void focusPrev() const;

   Ocurses::WindowResponse readKey(int ch); /* vgl. dazu PanelWinNode::readKey(int) */

   int printKey(int ch) const;

   Dimension getSize() const override;

   Dimension getPosition() const override;
}; // FormNode



/*--------------------------/ WindowFormNode: /------------------------------------*/

class WindowFormNode : public FormNode {
   /* *this ist das form_win */
   AbstractWindowNode& parentWin;
   TextUtil::Dimension position;
public:
   /* pos ist die Position des form_sub relativ zu *this, also zum form_win: */
   WindowFormNode(AbstractWindowNode& parent, TextUtil::Dimension pos);

   /* Kopier- und Zuweisschutz: */
   WindowFormNode(const WindowFormNode&) = delete;
   WindowFormNode(const WindowFormNode&&) = delete;
   WindowFormNode& operator=(const WindowFormNode&) = delete;
   WindowFormNode& operator=(const WindowFormNode&&) = delete;

   virtual ~WindowFormNode() = default;

   WINDOW* getSubWin() const
   {
      return getCPointer()->sub;
   }

   WINDOW* getWindow() const
   {
      return getCPointer()->win;
   }

   WINDOW* getCurrentFieldArea() const
   {
      return getCPointer()->w;
   }

   Dimension getPosition() const override;

   int post() override;
};






} // Ende Namespace Ocurses




#endif /* OCURSES_NODES_FORMULAR_H_ */
