#ifndef EVENTS_H_INWCLUDED
#define EVENTS_H_INWCLUDED

#include <string>

namespace Ocurses {




class Event {
   //
   int triggerID = 0; /* ID des auslösenden Objekts, z.B. MenuNode */

public:
   Event(int trigger_id) : triggerID(trigger_id) {}

   virtual ~Event() = default;

   virtual std::string getClass() const { return "Event"; }

   inline int getTriggerID() const { return triggerID; }

   void setTriggerID(int id) { triggerID = id; }
};


class MenuEvent : public Event {
   //
   int selectedIndex = -1;

public:
   using Event::Event;

   virtual ~MenuEvent() = default;

   virtual std::string getClass() const override { return "MenuEvent"; }

   inline int getSelectedIndex() const { return selectedIndex; }

   inline void setSelectedIndex(int i) { selectedIndex = i; }
};


class EventListener {
public:
   virtual ~EventListener() = default;

   virtual void eventTriggered(const Event&) = 0;
};









} // Ende Namespace Ocurses



#endif // EVENTS_H_INWCLUDED
