#ifndef OSS_PERSON_H
#define OSS_PERSON_H


#include "../general.h"
#include "../core/coreobject.h"
#include "item.h"
#include "route.h"


namespace OSS {	
	class Person : public CoreObject {
	protected:
		const Pointer<Tower> tower;
		
	public:
		//Initialization
		Person(Tower * tower);
		virtual void reset();
		
		/**
		 * Attributes
		 */
		
		//Type
	public:
		typedef enum {
			kManType,
			kSalesmanType,
			kChildType,
			kSecurityOfficerType,
			
			kWomanAType,
			kWomanBType,
			kMotherWithChildType,
			kMotherType,
			kHousekeeperType
		} Type;
		
	private:
		Type type;
		
	public:
		Type getType();
		void setType(Type type);
		virtual void onChangeType();
		typedef enum {
			kMale,
			kFemale
		} Gender;
		Gender getGender();
		
		//Stress
	private:
		double stress;
	public:
		double getStress();
		void setStress(double stress);
		virtual void onChangeStress();
		
		
		/**
		 * Managed Sprites
		 */
	public:
		typedef enum {
			kPortraitType,
			kQueuingType,
			kMountingType,
			kDescendingType
		} SpriteType;
		
		typedef enum {
			kHeadingLeft,
			kHeadingRight
		} SpriteHeading;
		
	private:
		typedef std::set< Pointer<Sprite> > SpriteSet;
		SpriteSet managedSprites;
		
		typedef std::map<Sprite *, SpriteType> SpriteTypeMap;
		SpriteTypeMap managedSpriteTypes;
		
		typedef std::map<Sprite *, SpriteHeading> SpriteHeadingMap;
		SpriteHeadingMap managedSpriteHeadings;
		
	public:
		void addManagedSprite(Sprite * sprite, SpriteType type, SpriteHeading heading);
		void removeManagedSprite(Sprite * sprite);
		
		void setManagedSpriteType(Sprite * sprite, SpriteType type);
		void setManagedSpriteHeading(Sprite * sprite, SpriteHeading heading);
		
		void initManagedSprite(Sprite * sprite);
		void updateManagedSprite(Sprite * sprite);
		void updateManagedSprites();
		
		
		/**
		 * Animation Sprite
		 */
	protected:
		Sprite animationSprite;
		double animationTime;
		unsigned int animationIndex;
		int2 animationLocation;
	public:
		Sprite & getAnimationSprite();
		
		unsigned int getAnimationIndex();
		void setAnimationIndex(unsigned int animationIndex);
		virtual void onChangeAnimationIndex();
		
		const int2 & getAnimationLocation();
		void setAnimationLocation(int2 animationLocation);
		virtual void onChangeAnimationLocation();
		
		virtual void initAnimationSprite();
		virtual void updateAnimationSprite();
		
		virtual double getAnimationPeriod();
		virtual bool shouldAnimate();
		void resetAnimation();
		void advanceAnimation(double dt);
		virtual void shuffleAnimation();
		void drawAnimation(rectd visibleRect);
		
		
		/**
		 * Location
		 */
	private:
		int floor;
		Pointer<Item> item;
		double arrivalTime;
	public:
		//Floor
		int getFloor();
		void setFloor(int floor);
		
		//Item
		Item * getItem() const;
		void setItem(Item * item);
		
		//Arrival time
		double getArrivalTime();
		void setArrivalTime(double time);
		
		//Convenience
		double getTimeSinceArrival();
		bool isAt(Item * item);
		bool hasBeenAtFor(Item * item, double duration);
		
		
		/**
		 * Journey
		 */
	private:
		int nextFloor;
		unsigned int nodeIndex;
	public:
		int getNextFloor();
		void setNextFloor(int nextFloor);
		void initJourney();
		void advanceJourney();
		
		
		/**
		 * Route
		 */
	private:
		Pointer<Item> destination;
		Pointer<Route> route;
	public:
		Item * getDestination();
		void setDestination(Item * destination);
		Route * getRoute();
		void setRoute(Route * route);
		void updateRoute();
		
		//Convenience
		bool isAtDestination();
		bool hasRoute();
		
		
		/**
		 * Notifications
		 */
		virtual void onArrivedAtDestination();
		
		
		/**
		 * Intelligence
		 */
		virtual void update();
		
		
		/**
		 * Uncategorized
		 */
		
		//DEBUG: Properties. These are only used for debuggin purposes and ought to be replaced in
		//later versions of the game by a more sophisticated system.
		std::map<std::string, bool> boolProps;
		std::map<std::string, int> intProps;
	};
}


#endif
