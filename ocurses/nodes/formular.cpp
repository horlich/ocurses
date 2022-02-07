/*
 * formular.cpp
 *
 *  Created on: 14.08.2021
 *      Author: herbert
 */


#include "formular.h"

using namespace Ocurses;
using namespace std;




/*-------------------------------/ FieldNode: /--------------------------------*/


//FieldNode::FieldNode(Geometry g, int offscreen) :
//   AbstractNode("FIELD", new_field(g[0], g[1], g[2], g[3], offscreen, 0))
//{
//   if (! pointerIsValid()) throw BadDesign(__PRETTY_FUNCTION__, "new_field() mißlungen.");
//   field_opts_off(getCPointer(), O_AUTOSKIP); /* nicht automatisch zum nächsten Feld weiter... */
//};


void FieldNode::init(Geometry g, int offscreen)   /* Number of offscreen-rows */
{
   setPointer(new_field(g[0], g[1], g[2], g[3], offscreen, 0));
   if (! pointerIsValid()) throw BadDesign(__PRETTY_FUNCTION__, "new_field() mißlungen.");
   field_opts_off(getCPointer(), O_AUTOSKIP); /* nicht automatisch zum nächsten Feld weiter... */
}

FieldNode::~FieldNode()
{
   if (pointerIsValid()) free_field(getCPointer());
}


int FieldNode::getIndex() const
{
   return field_index(getCPointer());
}



int FieldNode::setOption(Field_Options opt, bool enable) const
{
   return (enable) ?  field_opts_on(getCPointer(), opt) : field_opts_off(getCPointer(), opt);
}


void FieldNode::setRegexType(const std::string& regex) const
{
   if (set_field_type(getCPointer(), TYPE_REGEXP, regex.c_str()) != E_OK)
      throw BadDesign(__PRETTY_FUNCTION__, "Regexp nicht erkannt.");
}

void FieldNode::foreground(ColorPair& cp)
{
   set_field_fore(getCPointer(), cp.getAttr());
}

void FieldNode::background(ColorPair& cp)
{
   set_field_back(getCPointer(), cp.getAttr());
}


int FieldNode::setText(const std::string& str) const
{
   return set_field_buffer(getCPointer(), 0, str.c_str());
}

std::string FieldNode::getText() const
{
   /* field_buffer füllt den Buffer immer mit Leerzeichen      *
    * auf. Siehe 'man field_buffer'! Hier werden die hängenden *
    * Leerzeichen abgeschnitten.                               */
   std::string str = field_buffer(getCPointer(), 0);
   int strlength;
   for (strlength = str.size(); strlength > 0; strlength--) {
      char c = str.at(strlength-1);
      if (! isspace(c)) break;
   }
   return str.substr(0, strlength);
}


Dimension FieldNode::getSize() const
{
   FIELD* f = getCPointer();
   return Dimension(f->rows, f->cols);
}


Dimension FieldNode::getPosition() const
{
   FIELD* f = getCPointer();
   return Dimension(f->frow, f->fcol);
}



/*-----------------------------/ DatumFieldNode /-------------------------------*/


//DatumFieldNode::DatumFieldNode() : FieldNode(g, offscreen) {}


void DatumFieldNode::setToday()
{
   setText(Zeit::today().toString());
}




/*-------------------------------/ FormNode: /----------------------------------*/

void FormNode::deleteAll()
{
   if (! pointerIsValid()) return; /* init() wurde gar nicht aufgerufen. */
   FORM* pt = getCPointer();
   unpost_form(pt);
   free_form(pt);
   delete[] fieldArray;
   fieldArray = nullptr;
}

FormNode::~FormNode()
{
   deleteAll();
}



void FormNode::setFields(FieldVec&& vec)
{
   if (pointerIsValid()) { /* FORM* existiert schon -> löschen */
      deleteAll();
      posted = false;
   }
   felder = vec;
   int size = vec.size();
   fieldArray = new FIELD*[size + 1];
   int index = 0;
   for (FieldNode* fn : vec) {
      fieldArray[index++] = fn->getCPointer();
      felder.push_back(fn);
   }
   fieldArray[index] = nullptr; /* Null-Terminieren */
   setPointer(new_form(fieldArray));
   if (! pointerIsValid()) throw BadDesign(__PRETTY_FUNCTION__, "new_form(FIELDS**) mißlungen.");
}




int FormNode::getFieldCount() const
{
   return field_count(getCPointer());
}



int FormNode::formDriver(int command) const
{
   /* siehe dazu man form_driver: */
//	return form_driver(getCPointer(), command);
   int ret = form_driver(getCPointer(), command);
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
      err = "post_form(FORM) noch nicht ausgeführt";
      break;
   case E_INVALID_FIELD:
      err = "Invalid Field";
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



int FormNode::post()
{
   if (! pointerIsValid())
      throw BadDesign("FormNode::post(): setFields() noch nicht aufgerufen!");
   posted = true;
   return post_form(getCPointer());
}



FIELD* FormNode::getMember(int index) const
{
   int anz = getFieldCount();
   if ((index < 0) || (index >= anz)) {
      std::stringstream buf;
      buf << __PRETTY_FUNCTION__ << ": Ungültiger Index " << index << " bei Arraysize " << anz << ".";
      throw BadDesign(buf.str());
   }
   return fieldArray[index];
}


FIELD* FormNode::getActiveField() const
{
   return current_field(getCPointer());
}

/* Bei Fehler wird -1 zurückgegeben. */
int FormNode::getActiveIndex() const
{
   FIELD* f = getActiveField();
   return (f == nullptr) ? -1 : field_index(f);
}


/* Bei Fehler wird nullptr zurückgegeben. */
FIELD* FormNode::setActive(int index) const
{
   FIELD* ret = getMember(index);
   return (set_current_field(getCPointer(), ret) == E_OK) ? ret : nullptr;
}

/* Gibt den Index zurück; bei Fehler -1: */
int FormNode::setActive(FieldNode& f) const
{
   int ret = f.getIndex();
   FIELD* n = setActive(ret);
   if (n == nullptr) return -1;
   if (f.getCPointer() != n) throw BadDesign(__PRETTY_FUNCTION__, "Fremdes Feld als Argument.");
   return ret;
}



void FormNode::focusNext() const
{
   formDriver(REQ_NEXT_FIELD);
   formDriver(REQ_END_LINE);
}



void FormNode::focusPrev() const
{
   formDriver(REQ_PREV_FIELD);
   formDriver(REQ_END_LINE);
}


Ocurses::WindowResponse FormNode::readKey(int ch)   /* vgl. dazu PanelWinNode::readKey(int) */
{
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


Dimension FormNode::scaleForm()
{
   Dimension scale;
   scale_form(getCPointer(), &(scale.height), &(scale.width));
   return scale;
}


int FormNode::setFormSub(const AbstractWindowNode& w, const TextUtil::Dimension& pos)
{
   TextUtil::Dimension sc = scaleForm();
   return set_form_sub(getCPointer(), derwin(w.getCPointer(), sc.height, sc.width, pos.height, pos.width));
}


int FormNode::printKey(int ch) const
{
   return form_driver(getCPointer(), ch);
}


Dimension FormNode::getSize() const
{
   FORM* f = getCPointer();
   return Dimension(f->rows, f->cols);
}


Dimension FormNode::getPosition() const
{
   return Dimension(0,0);
}





/*-----------------------------/ WindowFormNode /-------------------------------*/


WindowFormNode::WindowFormNode(AbstractWindowNode& w, TextUtil::Dimension pos) :
   parentWin(w),
   position(pos) {}


Dimension WindowFormNode::getPosition() const
{
   Dimension ret;
   getbegyx(getSubWin(), ret.height, ret.width);
   return ret;
}



int WindowFormNode::post()
{
   if (! pointerIsValid())
      throw BadDesign("WindowFormNode::post(): setFields() noch nicht aufgerufen!");
   set_form_win(getCPointer(), parentWin.getCPointer());
   setFormSub(parentWin, position);
   posted = true;
   return post_form(getCPointer());
}




