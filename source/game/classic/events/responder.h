#ifndef OSS_CLASSIC_RESPONDER_H
#define OSS_CLASSIC_RESPONDER_H

#include "../../external.h"

#include "event.h"


namespace OSS {
	namespace Classic {
		class TransportItem;
		class HotelItem;
		
		class Responder : public ExtendingResponder {
		public:
			Responder(BasicResponder * base);
			bool handleClassicEvent(OSS::Event * event);
			
			//Time
			virtual void eventTimeChanged(Event * event) {}
			virtual void eventDayChanged(Event * event) {}
			virtual void eventQuarterChanged(Event * event) {}
			virtual void eventYearChanged(Event * event) {}
			
			//Structure
			virtual void eventBoundsChanged(Event * event) {}
			virtual void eventFloorRangeChanged(FloorEvent * event) {}
			
			//UI
			virtual void eventToolChanged(Event * event) {}
			
			//Transport
			virtual void eventTransportIncreased(ItemEvent<TransportItem> * event) {}
			virtual void eventTransportDecreased(ItemEvent<TransportItem> * event) {}
			
			//Hotel
			virtual void eventHotelVacated(ItemEvent<HotelItem> * event) {}
		};
	}
}


#endif
