/*
 * windows.cpp
 *
 *  Created on: 14.08.2021
 *      Author: herbert
 */





#include "window.h"
#include "window_impl.h"

using namespace std;



namespace Ocurses {
//

/*------------------------/ AbstractWindowNode /------------------------*/


void AbstractWindowNode::deleteSource()
{
   if (pointerIsValid()) {
      delwin(getCPointer());
      AbstractNode<WINDOW>::deleteSource();
   }
}


AbstractWindowNode::~AbstractWindowNode()
{
   deleteSource();
}


Dimension AbstractWindowNode::getSize() const
{
   Dimension ret;
   getmaxyx(getCPointer(), ret.height, ret.width);
   return ret;
}

Dimension AbstractWindowNode::getPosition() const
{
   Dimension ret;
   getbegyx(getCPointer(), ret.height, ret.width);
   return ret;
}

void AbstractWindowNode::update() const
{
   if (wnoutrefresh(getCPointer()) == ERR) throw OcursesException("wnoutrefresh() mißlungen");
}


int AbstractWindowNode::getChar() const
{
   return wgetch(getCPointer());
}


int AbstractWindowNode::ungetChar(int ch) const
{
   return ungetch(ch);
}


void AbstractWindowNode::drawBox() const
{
   box(getCPointer(),0,0); /* entspricht: box(source, ACS_VLINE, ACS_HLINE); */
}


void AbstractWindowNode::moveCursor(TextUtil::Dimension pos) const
{
   wmove(getCPointer(), pos.height, pos.width);
}


void AbstractWindowNode::clearLine(int y) const
{
   moveCursor(TextUtil::Dimension(0, y));
   wclrtoeol(getCPointer());
}


TextUtil::Dimension AbstractWindowNode::getCursor() const
{
   TextUtil::Dimension ret;
   getyx(getCPointer(),ret.height, ret.width);
   return ret;
}


/* Gibt -1 zurück, wenn keine Daten vorhanden: */
int AbstractWindowNode::colorPairIndex() const
{
   attr_t attribut;
   short index = -1;
   wattr_get(getCPointer(), &attribut, &index, nullptr);
   if (attribut & A_COLOR) return index;
   return index;
}


void AbstractWindowNode::color(ColorPair& cp)
{
   wattron(getCPointer(), COLOR_PAIR(cp.getIndex()));
}

void AbstractWindowNode::uncolor(ColorPair& cp)
{
   wattroff(getCPointer(), COLOR_PAIR(cp.getIndex()));
}


short AbstractWindowNode::getBgColorIndex() const
{
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



void AbstractWindowNode::enableScrolling(bool toggle) const
{
   scrollok(getCPointer(), toggle);
   idlok(getCPointer(), toggle); /* braucht es das? */
}


void AbstractWindowNode::setBackground(ColorPair& cp)
{
   wbkgd(getCPointer(), cp.getAttr());
}


void AbstractWindowNode::setLineBackground(int line, ColorPair& cp, int startx) const
{
   mvwchgat(getCPointer(), line, startx, -1, cp.getAttr(), cp.getIndex(), nullptr);
}

void AbstractWindowNode::setKeypad() const
{
   if (keypad(getCPointer(), true) == ERR) {
      throw OcursesException(OException::envNameMess(__PRETTY_FUNCTION__, "Unbekannter Fehler"));
   }
}


void AbstractWindowNode::scrolln(int lines) const
{
   int res = wscrl(getCPointer(), lines);
   if (res == ERR) throw BadDesign(__PRETTY_FUNCTION__, "Scrolling gescheitert.");
}



void AbstractWindowNode::printlnZentriert(int line, const std::string& text) const
{
   int maxLength = getSize().width;
   int x;
   if ((int)text.length() >= maxLength) {
      x = 0;
   }
   else {
      x = (maxLength - (int)text.length()) / 2;
   }
   println(line, text, x);
}


void _putn(std::ostream& os, int n, char c)
{
   for (; n > 0; n--) os.put(c);
}


void AbstractWindowNode::printlnBetont(int line, const std::string& text) const
{
   int textLength = text.length();
   int maxLength = getSize().width;
   const int abstand = 2;
   if (textLength >= maxLength) {
      println(line, text);
   }
   else if ((textLength + (abstand * 4)) >= maxLength) {
      printlnZentriert(line, text);
   }
   else {
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



/*------------------------/ WinNode: /------------------------*/

void WinNode::init(Geometry d)
{
   if (pointerIsValid()) delwin(getCPointer());
   setPointer(newwin(d[0], d[1], d[2], d[3]));
   if (! pointerIsValid()) {
      std::stringstream buf;
      buf << __PRETTY_FUNCTION__ << ": newwin() fehlgeschlagen mit Parametern " << d << endl;
      throw BadDesign(buf.str());
   }
   initChildren();
}


void WinNode::popUp(PanelWinNode& popup)
{
   if (! popup.pointerIsValid())
      throw BadDesign("WinNode::popUp(): init() für Popup-Fenster wurde nicht aufgerufen!");
   popup.draw();
   popup.getPanel()->top();
   popup.gotFocus();
   WindowResponse resp = CONTINUE_LISTENING;
   while (resp != Ocurses::QUIT_WINDOW) {
      resp = popup.readKey(wgetch(popup.getCPointer()));
   }
   popup.lostFocus();
   popup.deleteSource();
}


void WinNode::popUpMenu(MenuWindow& mw, Dimension pos)
{
   mw.init(pos);
   mw.draw();
   WindowResponse resp = CONTINUE_LISTENING;
   while (resp != Ocurses::QUIT_WINDOW) {
      resp = mw.readKey(wgetch(mw.getCPointer()));
   }
   mw.quit();
}


void WinNode::menuBarSelect(MenuBar& mb)
{
   if (! mb.pointerIsValid())
      throw BadDesign("__PRETTY_FUNCTION__", "init() für MenuBar wurde nicht aufgerufen!");
   WindowResponse resp = CONTINUE_LISTENING;
   while (resp != Ocurses::QUIT_WINDOW) {
      resp = mb.readKey(wgetch(mb.getCPointer()));
   }
}


void WinNode::showSimpleMessage(const std::wstring& message)
{
   SimpleMessage mess(message, this);
   mess.init();
   popUp(mess);
}



/*------------------------/ PanelWinNode: /------------------------*/

PanelWinNode::PanelWinNode(AbstractWindowNode* parent) : WinNode(parent), panel("Panel") {}


void PanelWinNode::init(Geometry d)
{
   WinNode::init(d);
   PanelNode* pn = new PanelNode(this->getCPointer());
   panel.setPointer(pn);
}


void PanelWinNode::deleteSource()
{
   if (pointerIsValid()) panel.deletePointer();
   WinNode::deleteSource();
}


PanelWinNode::~PanelWinNode()
{
   if (pointerIsValid()) panel.deletePointer();
}



/*------------------------/ AbstractSubWinNode: /------------------------*/

WINDOW* AbstractSubWinNode::getParentSource() const
{
   if (! pointerIsValid()) throw BadDesign("AbstractSubWinNode::getParentSource(): Mein *WINDOW existiert noch nicht!");
   return wgetparent(getCPointer());
}


void AbstractSubWinNode::update() const
{
   touchwin(getParentSource()); /* siehe dazu man subwin! */
   AbstractWindowNode::update();
}


void AbstractSubWinNode::quit()
{
   if (! isInit())
      throw BadDesign(__PRETTY_FUNCTION__, "auf nicht initiiertes Fenster angewandt.");
   doClear();
   deleteSource();
   getParent()->draw();
}


/*------------------------/ DerWinNode: /------------------------*/

DerWinNode::DerWinNode(AbstractWindowNode* w) : AbstractSubWinNode(w) {}


void DerWinNode::init(Geometry d)
{
   if (pointerIsValid()) delwin(getCPointer());
   setPointer(derwin(getParent()->getCPointer(), d[0], d[1], d[2], d[3]));
   if (! pointerIsValid()) {
      std::stringstream buf;
      buf << __PRETTY_FUNCTION__ << ": derwin() fehlgeschlagen mit Parametern " << d << endl;
      throw BadDesign(buf.str());
   }
   initChildren();
}


/*------------------------/ SubWinNode: /------------------------*/

SubWinNode::SubWinNode(AbstractWindowNode* w): AbstractSubWinNode(w) {}


void SubWinNode::init(Geometry d)
{
   if (pointerIsValid()) delwin(getCPointer());
   setPointer(subwin(getParent()->getCPointer(), d[0], d[1], d[2], d[3]));
   if (! pointerIsValid()) {
      std::stringstream buf;
      buf << __PRETTY_FUNCTION__ << ": subwin() fehlgeschlagen mit Parametern " << d << endl;
      throw BadDesign(buf.str());
   }
   initChildren();
}


/*-------------------/ MenuWindow: /-----------------------*/

/* statische Klassenvariable müssen so initialisiert werden: */
int MenuWindow::lastID = 0;


MenuWindow::~MenuWindow()
{
   deleteSource();
}



void MenuWindow::addItem(const char* name, const char* description)
{
   /* name und description müssen const char* sein. Bei
      std::string.c_str() werden rätselhafterweise
      fehlerhafte Zeichen angezeigt.                 */
   if (pointerIsValid())
      throw BadDesign(__PRETTY_FUNCTION__, " Menü ist bereits gepostet.");
   ItemPair p(name, description);
   itempairs.push_back(p);
}


void MenuWindow::setItems(std::initializer_list<const char*> li)
{
   for (const char* str : li) addItem(str);
}

void MenuWindow::setMenuFormat()
{
   TextUtil::Dimension format{getItemCount(),1};
   if (menu_align == Align::horizontal) format.swap_values();
   set_menu_format(menunode.getPointer(), format.height, format.width);
}


void MenuWindow::setMenuSpacing()
{
   /* Zu den Maximalwerten siehe menu_spacing(3) */
   static constexpr int maxrow       {3};
   static const int maxcol {TABSIZE};
   int spc_rows   {0}; /* Setze Defaults... */
   int spc_columns{0};
   if (menu_align == Align::vertikal) {
      spc_rows = std::min(item_spacing, maxrow);
   }
   else {
      spc_columns = std::min(item_spacing, maxcol);
   }
   int s = set_menu_spacing(menunode.getPointer(), 0, spc_rows, spc_columns);
   if (s != E_OK) throw BadDesign("__PRETTY_FUNCTION__", "Fehler in set_menu_spacing");
}


void MenuWindow::init(Dimension position)
{
   /*----------------- MENU einrichten: ---------------------*/
   if (pointerIsValid())
      throw BadDesign(__PRETTY_FUNCTION__, " WINDOW schon vorhanden.");
   int size = itempairs.size();
   if (size == 0)
      throw BadDesign(__PRETTY_FUNCTION__, " Keine Items definiert.");
   itemArray = new ITEM*[size + 1];
   int index = 0;
   for (ItemPair p : itempairs) itemArray[index++] = new_item(p.first, p.second);
   itemArray[index] = nullptr; /* Null-Terminieren! */
   /* TODO: set_menu_format() allenfalls implementieren! */
   menunode.setPointer(new_menu(itemArray));
   if (! menunode.hasValidPointer()) throw BadDesign(__PRETTY_FUNCTION__, "new_menu(FIELDS**) mißlungen.");
   if (! show_description) menu_opts_off(menunode.getPointer(), O_SHOWDESC); /* Description ausschalten */
   set_menu_mark(menunode.getPointer(), menu_mark.c_str());
   setMenuFormat();
   setMenuSpacing();

   /*----------------- WINDOW einrichten: ---------------------*/
   Dimension msize;
   scale_menu(menunode.getPointer(), &(msize.height), &(msize.width));
   DerWinNode::init(Geometry{msize.height + 2, msize.width + 2, position.height, position.width});
   int result = set_menu_win(menunode.getPointer(), getCPointer());
   if (result != E_OK)
      throw BadDesign(__PRETTY_FUNCTION__, " set_menu_win() mißlungen.");
   result = set_menu_sub(menunode.getPointer(), derwin(getCPointer(), msize.height, msize.width,1,1));
   if (result != E_OK)
      throw BadDesign(__PRETTY_FUNCTION__, " set_menu_sub() mißlungen.");

   /*--------------- Menü posten: ----------------------*/
   result = post_menu(menunode.getPointer());
   if (result != E_OK) {
      std::string errstr = (result == E_NO_ROOM) ?
                           "Menü paßt nicht ins Fenster. You should consider to use set_menu_format() to solve the problem." :
                           "post_menu mißlungen";
      throw BadDesign(__PRETTY_FUNCTION__, errstr);
   }
   setKeypad(); /* Funktionstasten erkennen */
}


void MenuWindow::setBackground(ColorPair& cp)
{
   DerWinNode::setBackground(cp);
   set_menu_back(menunode.getPointer(), cp.getAttr());
}


void MenuWindow::setForeground(ColorPair& cp)
{
   set_menu_fore(menunode.getPointer(), cp.getAttr());
}


void MenuWindow::showDescription()
{
   if (menunode.hasValidPointer())
      throw BadDesign(__PRETTY_FUNCTION__, " MENU ist bereits initiiert.");
   show_description = true;
}


void MenuWindow::setMenuMark( const std::string& mark )
{
   if (menunode.hasValidPointer())
      throw BadDesign(__PRETTY_FUNCTION__, " MENU ist bereits initiiert.");
   menu_mark = mark;
}


Dimension MenuWindow::getMenuSize() const
{
   MENU* m = menunode.getPointer();
   return Dimension(m->height, m->width);
}


int MenuWindow::getItemCount() const
{
   return item_count(menunode.getPointer());
}


int MenuWindow::menuDriver(int command) const
{
   /* siehe dazu man menu_driver: */
   int ret = menu_driver(menunode.getPointer(), command);
   if (ret == E_OK) return ret;
   std::stringstream buf;
   buf << "menu_driver() konnte nicht ausgeführt werden: ";
   switch (ret) {
   case E_BAD_ARGUMENT:
      buf << "Bad Argument";
      break;
   case E_BAD_STATE:
      buf << "Bad State";
      break;
   case E_NOT_POSTED:
      buf << "post_menu(MENU) noch nicht ausgeführt";
      break;
   case E_NO_MATCH:
      buf << "No Match";
      break;
   case E_REQUEST_DENIED:
      buf << "Request denied";
      break;
   case E_SYSTEM_ERROR:
      buf << "SYSTEM ERROR";
      break;
   case E_UNKNOWN_COMMAND:
      buf << "Unbekannter Befehl";
      break;
   default:
      buf << "Das darf nicht sein";
   }
   throw BadDesign(__PRETTY_FUNCTION__, buf.str());
   return ret;
}


/* Bei Fehler wird -1 zurückgegeben. */
int MenuWindow::getActiveIndex() const
{
   ITEM* f = current_item(menunode.getPointer());
   return (f == nullptr) ? -1 : item_index(f);
}


int MenuWindow::setActive(int index) const
{
   if ((index < 0) || (index >= getItemCount())) {
      std::stringstream buf;
      buf << __PRETTY_FUNCTION__ << ": Ungültiger Index " << index << ".";
      throw BadDesign(buf.str());
   }
   ITEM* it = itemArray[index];
   return set_current_item(menunode.getPointer(), it);
}

Ocurses::WindowResponse MenuWindow::readKey(int ch)
{
   switch (ch) {
   case KEY_DOWN:
      if (getActiveIndex() < getLastItemIndex()) menuDriver(REQ_DOWN_ITEM);
      return CONTINUE_LISTENING;
   case KEY_UP:
      if (getActiveIndex() > 0) menuDriver(REQ_UP_ITEM);
      return CONTINUE_LISTENING;
   case KEY_RETURN:
      if (elist != nullptr) { /* Es ist ein EventListener definiert */
         MenuEvent ev(getID());
         ev.setSelectedIndex(getActiveIndex());
         elist->eventTriggered(ev);
      }
      return QUIT_WINDOW;
   case KEY_ESC: /* 27 = ESCAPE */
      return QUIT_WINDOW;
   default: /* Unbekannte Taste... */
      return CONTINUE_LISTENING;
   }
}


void MenuWindow::deleteSource()
{
   MENU* pt = menunode.getPointer();
   unpost_menu(pt);
   free_menu(pt);
   ITEM** ptr = itemArray;
   while (ptr != nullptr) {
      delete ptr++;
   }
   delete[] itemArray;
   menunode.setPointer(nullptr);
   itemArray = nullptr;
   DerWinNode::deleteSource();
}




/*-------------------/ MenuBar: /-----------------------*/


Ocurses::WindowResponse MenuBar::readKey(int ch)
{
   switch (ch) {
   case KEY_RIGHT:
      if (getActiveIndex() < getLastItemIndex()) menuDriver(REQ_RIGHT_ITEM);
//      MESSAGE.stream() << "StartX: " << getStartX(getActiveIndex()) << std::endl;
      return CONTINUE_LISTENING;
   case KEY_LEFT:
      if (getActiveIndex() > 0) menuDriver(REQ_LEFT_ITEM);
      return CONTINUE_LISTENING;
   case KEY_RETURN: {
      EventListener* elist = getEventListener();
      if (elist != nullptr) { /* Es ist ein EventListener definiert */
         MenuEvent ev(getID());
         ev.setSelectedIndex(getActiveIndex());
         elist->eventTriggered(ev);
      }
      return QUIT_WINDOW;
   }
   case KEY_ESC: /* 27 = ESCAPE */
      return QUIT_WINDOW;
   default: /* Unbekannte Taste... */
      return CONTINUE_LISTENING;
   }
}


void MenuBar::addMenu(Menu& m)
{
   if (static_cast<int>(mvec.size()) >= getItemCount())
      throw BadDesign(__PRETTY_FUNCTION__, "Hinzufügen eines überzähligen Menü");
   mvec.push_back(&m);
}


int MenuBar::getStartX(int item) const
{
   const ItemVec& iv = getItems();
   if (item >= static_cast<int>(iv.size()))
      throw BadDesign("__PRETTY_FUNCTION__", "Ungültiger Item-Index");
   if (item == 0) return 0;
   int ret = getItemSpacing() * item;
   for (int i = 0; i < item; ++i) {
      ret += strlen(iv[i].first);
   }
   return ret;
}


/*------------------------/ ScreenNode: /------------------------*/

void ScreenNode::startColor()
{
   if (! has_colors()) throw ColorException("Dieses Terminal hat keine Farben.");
   int err = start_color();
   if (err != OK) throw ColorException("start_color() mißlungen.");
   imFarbmodus = true;
}

ScreenNode::ScreenNode()
{
   setPointer(initscr());
}

ScreenNode::~ScreenNode()
{
   endwin();
   setPointer(nullptr);
}

void ScreenNode::initDefaults()
{
   startColor();
   cbreak(); /* Keine Pufferung bei Texteingabe */
//	nonl(); /* nl() [=default] und nonl() betreffen die Enter-Taste */
   noecho();
   curs_set(0);
   keypad(getCPointer(), TRUE); /* getCPointer() == stdscr */
}




/*------------------------/ SimpleMessage: /------------------------*/

SimpleMessage::SimpleMessage(const std::wstring& mess, AbstractWindowNode* parent) : PanelWinNode(parent), message(mess)
{
   Ocurses::ROT_WEISS.init();
}

void SimpleMessage::init()
{
   int linewidth = COLS - (2 - MARGIN_X) * 2;
   messvec = StringUtil::wrap_vec(message, linewidth);
   int textwidth = 0;
   for (std::wstring str : messvec) {
      textwidth = std::max((int)str.length(), textwidth);
   }
   int windowwidth = textwidth + (MARGIN_X * 2);
   int windowheight = messvec.size() + (2 * MARGIN_Y); /* messvec.size() == Texthöhe */
   PanelWinNode::init({windowheight, windowwidth, MARGIN_Y, MARGIN_X});
}

void SimpleMessage::draw()
{
   setBackground(Ocurses::ROT_WEISS);
   drawBox();
   int line = MARGIN_Y;
   StringUtil::WC_Converter conv;
   for (std::wstring str : messvec) {
      println(line++, conv.to_bytes(str), MARGIN_X);
   }
   mvprintf(Dimension(line + 1, getSize().width - 9), "< OK >");
}

Ocurses::WindowResponse SimpleMessage::readKey(int ch)
{
   return (ch == '\n') ? Ocurses::QUIT_WINDOW : Ocurses::CONTINUE_LISTENING;
}

void SimpleMessage::gotFocus()
{
   oldcurs = showCursor(false);
}

void SimpleMessage::lostFocus()
{
   Ocurses::showCursor(oldcurs);
}




/*------------------------/ statische Methoden: /------------------------*/

bool operator==(const AbstractWindowNode& left, const AbstractWindowNode& right)
{
   return (&left == &right);
}




} // Ende namespace Ocurses






