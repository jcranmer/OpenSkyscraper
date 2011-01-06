#ifndef OSS_JANITOR_H
#define OSS_JANITOR_H


#include "../../general.h"
#include "timedperson.h"


namespace OSS {
	class HousekeepingItem;
	class HotelItem;
	
	class Janitor : public TimedPerson {
	public:
		const Pointer<HousekeepingItem> housekeeping;
		
		//Initialization
		Janitor(Tower * tower, HousekeepingItem * housekeeping);
		
		//Hotel
	private:
		Pointer<HotelItem> assignedHotel;
	public:
		HotelItem * getAssignedHotel();
		void setAssignedHotel(HotelItem * hotel);
		bool hasAssignedHotel();
		void onChangeAssignedHotel();
		
		//Intelligence
		void think();
		
		//Animation Sprite
		void initAnimationSprite();
		void updateAnimationSprite();
		
		bool shouldAnimate();
		void shuffleAnimation();
	};
}


#endif
