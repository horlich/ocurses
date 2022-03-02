#ifndef PMANAGER_H_INCLMUDED
#define PMANAGER_H_INCLMUDED

#include "omemory.h"
#include "konstanten.h"
#include "nodes/window.h"

namespace Ocurses {

class PanelWinNode;
class AbstractWindowNode;



/*
                         AbstractPanelManager:
*/

class AbstractPanelManager {
   /* Unterklassen müssen die Methode
      readKey(int) implementieren     */
private:
   Memory::StackPointer<PanelWinNode> topWindow;

public:
   AbstractPanelManager() : topWindow("Top Window") {}

   /* Kopier- und Zuweisschutz: */
   AbstractPanelManager(const AbstractPanelManager&) = delete;
   AbstractPanelManager(const AbstractPanelManager&&) = delete;
   AbstractPanelManager& operator=(const AbstractPanelManager&) = delete;
   AbstractPanelManager& operator=(const AbstractPanelManager&&) = delete;

   virtual ~AbstractPanelManager() = default;

   /* Führt auch draw() aus und updatet: */
   virtual void setTop(PanelWinNode* pn);

   /* Gibt nullptr zurück, wenn topWindow nicht definiert ist: */
   virtual PanelWinNode* getTopWindow() const
   {
      return topWindow.getPointer();
   }

   /* Unterklassen können bestimmen, welches
      Fenster zu Programmstart angezeigt werden soll: */
   virtual PanelWinNode* getStartWindow()
   {
      return nullptr;
   }

   /* Code: KEY_RESIZE */
   virtual void keyResizePressed() const;

   virtual Ocurses::WindowResponse readKey(int ch) = 0;

   Ocurses::WindowResponse myReadKey(int ch);
};


void keyResizePressed(AbstractWindowNode* win);













} // Ende Namespace Ocurses

#endif // PMANAGER_H_INCLMUDED
