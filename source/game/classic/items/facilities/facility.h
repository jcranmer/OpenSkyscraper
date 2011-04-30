#ifndef OSS_CLASSIC_ITEMS_FACILITIES_FACILITY_H
#define OSS_CLASSIC_ITEMS_FACILITIES_FACILITY_H

#include "../item.h"
#include "../../people/person.h"


namespace OSS {
	namespace Classic {
		class FacilityItem : public Item {
			
			/**
			 * Initialization
			 */
		public:
			FacilityItem(Tower * tower, ItemDescriptor * descriptor);
			
			
			/**
			 * Ceiling
			 */
		private:
			bool ceiling;
			Pointer<Texture> ceilingTexture;
			
		public:
			bool hasCeiling() const;
			void setCeiling(bool flag);
			
			rectd getCeilingRect();
			
			
			/**
			 * Variant
			 */
		private:
			unsigned int variant;
			
		public:
			unsigned int getVariant() const;
			void setVariant(const unsigned int variant);
			
			virtual void willChangeVariant(unsigned int newVariant) {}
			virtual void didChangeVariant() {}
			
			
			/**
			 * Lighting
			 */
		private:
			bool lightsOn;
			
		public:
			bool areLightsOn();
			void setLightsOn(bool lo);
			
			bool shouldBeLitDueToTime();
			bool isLit();
			
			
			/**
			 * People
			 */
			virtual void didAddPerson(Person * person);
			
			
			/**
			 * Reachability
			 */
		private:
			Pointer<Route> routeFromLobby;
			
		public:
			bool isReachableFromLobby();
			virtual void didChangeReachability() {}
			
			virtual void eventTransportIncreased(ItemEvent<TransportItem> * event);
			virtual void eventTransportDecreased(ItemEvent<TransportItem> * event);
			
			
			/**
			 * Simulation
			 */
		public:
			virtual void advance(double dt);
			
			
			/**
			 * State
			 */
		private:
			bool autoTextured;
			unsigned int autoTextureSlice;
			
		public:
			bool isAutoTextured();
			void setAutoTextured(bool at);
			
			unsigned int getAutoTextureSlice();
			void setAutoTextureSlice(unsigned int slice);
			
			virtual string getAutoTextureName(int floor, unsigned int slice) { return ""; }
			virtual rectd getAutoTextureRect(int floor, unsigned int slice) { return rectd(); }
			
			virtual void update();
			virtual void updateBackground();
			virtual void updateCeiling();
			virtual void updateLighting() {}
			virtual void updateReachability();
			
			Updatable::Conditional<FacilityItem> updateCeilingIfNeeded;
			Updatable::Conditional<FacilityItem> updateLightingIfNeeded;
			Updatable::Conditional<FacilityItem> updateReachabilityIfNeeded;
			
			
			/**
			 * Drawing
			 */
		public:
			virtual void draw(rectd dirtyRect);
			virtual void drawCeiling(rectd dirtyRect);
		};
	}
}


#endif
