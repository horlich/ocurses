#ifndef WINDOW_IMPL_H_INCLUXDED
#define WINDOW_IMPL_H_INCLUXDED


namespace Ocurses {




/*
                      SimpleMessage:
*/

class SimpleMessage : public Ocurses::PanelWinNode {

   std::wstring message;
   std::vector<std::wstring> messvec;
   /* Abstände zwischen Fensterrahmen und Text */
   static constexpr int MARGIN_X = 3;
   static constexpr int MARGIN_Y = 2;
   bool oldcurs = false; /* das Ergebnis von showCursor() */

public:
   SimpleMessage(const std::wstring& mess, AbstractWindowNode* parent);

   virtual ~SimpleMessage() = default;

   void init();

   void draw() override;

   Ocurses::WindowResponse readKey(int ch) override;

   void gotFocus() override;

   void lostFocus() override;
};











} // Ende Namespace Ocurses



#endif // WINDOW_IMPL_H_INCLUXDED
