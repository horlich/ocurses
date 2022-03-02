#ifndef EVENTS_H_INWCLUDED
#define EVENTS_H_INWCLUDED

#include <string>

namespace Ocurses {




class Event {
   //
   int triggerID = 0; /* ID des auslösenden Objekts, z.B. MenuNode */

public:
   Event(int trigger_id) : triggerID(trigger_id) {}

   /* Kopier- und Zuweisschutz: */
   Event(const Event&) = delete;
   Event(const Event&&) = delete;
   Event& operator=(const Event&) = delete;
   Event& operator=(const Event&&) = delete;

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

   /* Kopier- und Zuweisschutz: */
   MenuEvent(const MenuEvent&) = delete;
   MenuEvent(const MenuEvent&&) = delete;
   MenuEvent& operator=(const MenuEvent&) = delete;
   MenuEvent& operator=(const MenuEvent&&) = delete;

   virtual ~MenuEvent() = default;

   virtual std::string getClass() const override { return "MenuEvent"; }

   inline int getSelectedIndex() const { return selectedIndex; }

   inline void setSelectedIndex(int i) { selectedIndex = i; }
};


class EventListener {
   //
public:
   EventListener() = default;

   /* Kopier- und Zuweisschutz: */
   EventListener(const EventListener&) = delete;
   EventListener(const EventListener&&) = delete;
   EventListener& operator=(const EventListener&) = delete;
   EventListener& operator=(const EventListener&&) = delete;

   virtual ~EventListener() = default;

   virtual void eventTriggered(const Event&) = 0;
};









} // Ende Namespace Ocurses



#endif // EVENTS_H_INWCLUDED
