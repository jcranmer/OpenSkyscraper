#include "facility.h"

using namespace OSS;
using namespace Classic;





//----------------------------------------------------------------------------------------------------
#pragma mark -
#pragma mark Initialization
//----------------------------------------------------------------------------------------------------

FacilityItem::FacilityItem(Tower * tower, ItemDescriptor * descriptor) : Item(tower, descriptor),
updateCeilingIfNeeded(this, &FacilityItem::updateCeiling, &updateIfNeeded),
updateLightingIfNeeded(this, &FacilityItem::updateLighting, &updateBackgroundIfNeeded),
updateReachabilityIfNeeded(this, &FacilityItem::updateReachability, &updateIfNeeded)
{
	constructed = false;
	ceiling = true;
	variant = 0;
	lightsOn = false;
	
	autoTextured = false;
	autoTextureSlice = false;
	
	//Update the reachability..
	//updateReachabilityIfNeeded.setNeeded();
}





//----------------------------------------------------------------------------------------------------
#pragma mark -
#pragma mark Ceiling
//----------------------------------------------------------------------------------------------------

bool FacilityItem::hasCeiling() const
{
	return ceiling;
}

void FacilityItem::setCeiling(bool flag)
{
	if (ceiling != flag) {
		ceiling = flag;
		
		//We have to update the background because we have to cut off a small amount off its top if
		//we have a ceiling.
		updateBackgroundIfNeeded.setNeeded();
		
		//Of course we also have to update the ceiling itself.
		updateCeilingIfNeeded.setNeeded();
	}
}

rectd FacilityItem::getCeilingRect()
{
	rectd r = getWorldRect();
	r.origin.y = r.maxY();
	r.size.y = tower->structure->ceilingHeight;
	r.origin.y -= r.size.y;
	return r;
}





//----------------------------------------------------------------------------------------------------
#pragma mark -
#pragma mark Variant
//----------------------------------------------------------------------------------------------------

unsigned int FacilityItem::getVariant() const
{
	return variant;
}

void FacilityItem::setVariant(const unsigned int variant)
{
	if (this->variant != variant) {
		willChangeVariant(variant);
		this->variant = variant;
		didChangeVariant();
		updateBackgroundIfNeeded.setNeeded();
	}
}





//----------------------------------------------------------------------------------------------------
#pragma mark -
#pragma mark Lighting
//----------------------------------------------------------------------------------------------------

bool FacilityItem::areLightsOn()
{
	return lightsOn;
}

void FacilityItem::setLightsOn(bool lo)
{
	if (lightsOn != lo) {
		lightsOn = lo;
		updateBackgroundIfNeeded.setNeeded();
	}
}



bool FacilityItem::shouldBeLitDueToTime()
{
	return tower->time->isBetween(7, 17);
}

bool FacilityItem::isLit()
{
	return (shouldBeLitDueToTime() || areLightsOn());
}





//----------------------------------------------------------------------------------------------------
#pragma mark -
#pragma mark People
//----------------------------------------------------------------------------------------------------

void FacilityItem::didAddPerson(Person * person)
{
	Item::didAddPerson(person);
	person->setFloor(getRect().minY());
}





//----------------------------------------------------------------------------------------------------
#pragma mark -
#pragma mark Reachability
//----------------------------------------------------------------------------------------------------

bool FacilityItem::isReachableFromLobby()
{
	return routeFromLobby;
}

void FacilityItem::eventTransportIncreased(ItemEvent<TransportItem> * event)
{
	updateReachabilityIfNeeded.setNeeded();
}

void FacilityItem::eventTransportDecreased(ItemEvent<TransportItem> * event)
{
	updateReachabilityIfNeeded.setNeeded();
}





//----------------------------------------------------------------------------------------------------
#pragma mark -
#pragma mark Simulation
//----------------------------------------------------------------------------------------------------

void FacilityItem::advance(double dt)
{
	Item::advance(dt);
	
	//Ask the facility to update its lighting state at twilight
	if (tower->time->checkDaily(7) || tower->time->checkDaily(17))
		updateLightingIfNeeded.setNeeded();
}





//----------------------------------------------------------------------------------------------------
#pragma mark -
#pragma mark State
//----------------------------------------------------------------------------------------------------

bool FacilityItem::isAutoTextured()
{
	return autoTextured;
}

void FacilityItem::setAutoTextured(bool at)
{
	if (autoTextured != at) {
		autoTextured = at;
		updateBackgroundIfNeeded.setNeeded();
	}
}



unsigned int FacilityItem::getAutoTextureSlice()
{
	return autoTextureSlice;
}

void FacilityItem::setAutoTextureSlice(unsigned int slice)
{
	if (autoTextureSlice != slice) {
		autoTextureSlice = slice;
		updateBackgroundIfNeeded.setNeeded();
	}
}



void FacilityItem::update()
{
	Item::update();
	
	//Update the other components if required.
	updateCeilingIfNeeded();
	updateLightingIfNeeded();
	updateReachabilityIfNeeded();
}

void FacilityItem::updateBackground()
{	
	Item::updateBackground();
	
	//Adjust the topmost background's height if we have a ceiling
	if (hasCeiling()) {
		unsigned int index = (getNumFloors() - 1);
		if (backgrounds.count(index))
			backgrounds[index]->rect.size.y -= getCeilingRect().size.y;
	}
	
	//Autotexture if requested
	if (isAutoTextured()) {
		unsigned int slice = getAutoTextureSlice();
		unsigned int numFloors = (hasUnifiedBackground() ? 1 : getNumFloors());
		for (unsigned int i = 0; i < numFloors; i++) {
			backgrounds[i]->texture = Texture::named(getAutoTextureName(i, slice));
			backgrounds[i]->textureRect = getAutoTextureRect(i, slice);
		}
	}
}

void FacilityItem::updateCeiling()
{
	//If we have a ceiling we have to load the appropriate ceiling texture
	if (hasCeiling())
		ceilingTexture = Texture::named("ceiling.png");
	
	//Otherwise we can just get rid of a potential ceiling texture
	else
		ceilingTexture = NULL;
}

void FacilityItem::updateReachability()
{
	bool before = routeFromLobby;
	
	//If the route has become invalid, clear it so it gets recalculated properly.
	if (routeFromLobby && !routeFromLobby->isValid())
		routeFromLobby = NULL;
	
	//If there is no route, try to calculate one.
	if (!routeFromLobby)
		routeFromLobby = Route::findRoute(tower, tower->structure->getFloorRect(0), this->getRect(),
			Route::kNoServiceElevators);
	
	//Subclass notification.
	if ((bool)routeFromLobby != before)
		didChangeReachability();
}





//----------------------------------------------------------------------------------------------------
#pragma mark -
#pragma mark Drawing
//----------------------------------------------------------------------------------------------------

void FacilityItem::draw(rectd dirtyRect)
{
	Item::draw(dirtyRect);
	
	//Draw the ceiling if we have one
	if (hasCeiling())
		drawCeiling(dirtyRect);
}

void FacilityItem::drawCeiling(rectd dirtyRect)
{
	//Create a textured quad which we will use for drawing the ceiling
	TexturedQuad quad;
	quad.rect = getCeilingRect();
	quad.texture = ceilingTexture;
	
	//Automatically calculate the ceiling's texture rect
	quad.autogenerateTextureRect(true, false);
	
	//Draw the ceiling quad
	quad.draw();
}

