#include "background.h"

#include "tower.h"

using namespace OSS;
using namespace Classic;





//----------------------------------------------------------------------------------------------------
#pragma mark -
#pragma mark Construction
//----------------------------------------------------------------------------------------------------

TowerBackground::TowerBackground(Tower * tower) : tower(tower),
updateSkyIfNeeded(this, &TowerBackground::updateSky, &updateIfNeeded),
updateSkyTexturesIfNeeded(this, &TowerBackground::updateSkyTextures, &updateIfNeeded),
updateGroundTexturesIfNeeded(this, &TowerBackground::updateGroundTextures, &updateIfNeeded)
{
	OSSObjectLog << "loaded event handlers:" << std::endl;
	for (map<string, Pointer<AbstractEventHandler> >::iterator it = eventHandlers.begin(); it != eventHandlers.end(); it++)
		std::cout << it->first << std::endl;
	
	//Initialize the sound effects
	cockSound = new SoundEffect();
	cockSound->sound = Sound::named("simtower/background/cock");
	cockSound->layer = SoundEffect::kTopLayer;
	
	//Initialize the birds in the morning
	birdsMorningSound = new SoundEffect();
	birdsMorningSound->sound = Sound::named("simtower/background/birds/morning");
	birdsMorningSound->layer = SoundEffect::kTopLayer;
	birdsMorningSound->loopCount = 3;
	birdsMorningSound->copyBeforeUse = true;
	
	//Initialize the bells that ring at 10:00
	bellsAt10Sound = new SoundEffect();
	bellsAt10Sound->sound = Sound::named("simtower/background/bells");
	bellsAt10Sound->layer = SoundEffect::kTopLayer;
	
	//Initialize the birds in the evening
	birdsEveningSound = new SoundEffect();
	birdsEveningSound->sound = Sound::named("simtower/background/birds/evening");
	birdsEveningSound->layer = SoundEffect::kTopLayer;
	
	//Initialize the rain sound to be playing in an infinite loop so we're able to stop it manually.
	rainSound = new SoundEffect();
	rainSound->sound = Sound::named("simtower/background/rain");
	rainSound->layer = SoundEffect::kTopLayer;
	rainSound->loopInfinitely = true;
	
	//Initialize the thunder
	thunderSound = new SoundEffect();
	thunderSound->sound = Sound::named("simtower/background/thunder");
	thunderSound->layer = SoundEffect::kTopLayer;
	thunderSound->copyBeforeUse = true;
}





//----------------------------------------------------------------------------------------------------
#pragma mark -
#pragma mark Simulation
//----------------------------------------------------------------------------------------------------

double TowerBackground::getRainAnimation()
{
	return rainAnimation;
}

void TowerBackground::setRainAnimation(double animation)
{
	if (rainAnimation != animation) {
		rainAnimation = animation;
		setRainAnimationFrame(rainAnimation * 2);
	}
}

bool TowerBackground::isRainyDay()
{
	return rainyDay;
}

void TowerBackground::setRainyDay(bool rainy)
{
	if (rainyDay != rainy) {
		rainyDay = rainy;
		if (rainyDay)
			OSSObjectLog << "I've heard we're in for some bad weather..." << std::endl;
	}
}

void TowerBackground::advance(double dt)
{
	SceneObject::advance(dt);
	
	//Advance the rain animation
	if (getSkyState() == Rain)
		setRainAnimation(fmod(getRainAnimation() + dt, 1));
	
	//Play the sound effects
	if (tower->time->checkDaily(5.5)) Audio::shared()->play(cockSound);
	if (tower->time->checkDaily(6)) Audio::shared()->play(birdsMorningSound);
	if (tower->time->checkDaily(10)) Audio::shared()->play(bellsAt10Sound);
	if (tower->time->checkDaily(18)) Audio::shared()->play(birdsEveningSound);
	
	//Play additional thunder sounds for rainy days
	if (isRainyDay() && getSkyState() == Rain) {
		
		//Decrease the thunder countdown
		nextThunderCountdown -= dt;
		
		//If it reached 0 play the thunder and reset the countdown
		if (nextThunderCountdown <= 0) {
			Audio::shared()->play(thunderSound);
			nextThunderCountdown = randd(3, 15);
		}
	}
}





//----------------------------------------------------------------------------------------------------
#pragma mark -
#pragma mark State
//----------------------------------------------------------------------------------------------------

void TowerBackground::update()
{
	updateSkyIfNeeded();
	updateGroundTexturesIfNeeded();
}

void TowerBackground::updateSky()
{
	double time = tower->time->getTimeOfDay();
	
	//Night
	if (time < 5 || time >= 19)
		setSky(Night, 0);
	
	//Dawn
	else if (time >= 5 && time < 6)
		setSky(EarlyDawn, (time - 5));
	else if (time >= 6 && time < 7)
		setSky(LateDawn, (time - 6));
	
	//Dusk
	else if (time >= 17 && time < 18)
		setSky(EarlyDusk, (time - 17));
	else if (time >= 18 && time < 19)
		setSky(LateDusk, (time - 18));
	
	//Day
	else if (!isRainyDay())
		setSky(Day, 0);
	
	//Rain
	else if (time >= 7 && time < 8)
		setSky(Worsening, (time - 7));
	else if (time >= 16 && time < 17)
		setSky(Improving, (time - 16));
	else
		setSky(Rain, 0);
	
	//Update the textures if required
	updateSkyTexturesIfNeeded();
}

void TowerBackground::updateSkyTextures()
{
	//Decide what variants of the sky should be interpolated
	string currentVariant = "";
	string targetVariant = "";
	
	switch (skyState) {
		case Night: {
			currentVariant = "night";
		} break;
			
		case EarlyDawn: {
			currentVariant = "night";
			targetVariant = "twilight";
		} break;
		case LateDawn: {
			currentVariant = "twilight";
			targetVariant = "day";
		} break;
			
		case Day: {
			currentVariant = "day";
		} break;
			
		case Worsening: {
			currentVariant = "day";
			targetVariant = "overcast";
		} break;
		case Rain: {
			stringstream s;
			s << "rain/" << getRainAnimationFrame();
			currentVariant = s.str();
		} break;
		case Improving: {
			currentVariant = "overcast";
			targetVariant = "day";
		} break;
			
		case EarlyDusk: {
			currentVariant = "day";
			targetVariant = "twilight";
		} break;
		case LateDusk: {
			currentVariant = "twilight";
			targetVariant = "night";
		} break;
	}
	
	//Load the required sky textures
	for (unsigned int i = 0; i < 10; i++) {
		
		//Current
		if (currentVariant.size())
			currentSkyTextures[i] = Texture::named(getSkyTextureName(i, currentVariant));
		else
			currentSkyTextures[i] = NULL;
		
		//Target
		if (targetVariant.size())
			targetSkyTextures[i] = Texture::named(getSkyTextureName(i, targetVariant));
		else
			targetSkyTextures[i] = NULL;
	}
	
	//Clouds don't have rain animation, so they're always showing overcast
	if (skyState == Rain)
		currentVariant = "overcast";
	
	//Load the required cloud textures
	for (unsigned int i = 0; i < 4; i++) {
		
		//Current
		if (currentVariant.size())
			currentCloudTextures[i] = Texture::named(getCloudTextureName(i, currentVariant));
		else
			currentCloudTextures[i] = NULL;
		
		//Target
		if (targetVariant.size())
			targetCloudTextures[i] = Texture::named(getCloudTextureName(i, targetVariant));
		else
			targetCloudTextures[i] = NULL;
	}
}

void TowerBackground::updateGroundTextures()
{
	cityTexture = Texture::named(getCityTextureName());
	groundTexture = Texture::named(getGroundTextureName());
}





//----------------------------------------------------------------------------------------------------
#pragma mark -
#pragma mark Drawing
//----------------------------------------------------------------------------------------------------

void TowerBackground::draw(rectd dirtyRect)
{
	drawSky(dirtyRect);
	drawGround(dirtyRect);
}

void TowerBackground::drawSky(rectd dirtyRect)
{
	//Find the lowest and hightest sky indices to draw
	unsigned int lowest = clampi(floor(dirtyRect.minY() / 360), 0, /*9*/10);
	unsigned int highest = clampi(ceil(dirtyRect.maxY() / 360), 0, /*9*/10);
	
	//Create the interpolated texture quad and find the basic rectangle of one sky slice
	InterpolatedTextureQuad quad;
	quad.rect = dirtyRect;
	quad.rect.size.y = 360;	
	
	//Draw the slices
	for (unsigned int y = lowest; y <= highest; y++) {
		unsigned int slice = std::min<unsigned int>(y, 9);
		quad.rect.origin.y = y * quad.rect.size.y;
		quad.state0.texture = currentSkyTextures[slice];
		quad.state1.texture = targetSkyTextures[slice];
		quad.interpolation = getSkyInterpolation();
		quad.autogenerateTextureRect(true, false);
		quad.draw();
	}
	
	//Draw the clouds
	const int2 cloudGrid(250, 100);
	for (int x = dirtyRect.minX() / cloudGrid.x - 1; x <= dirtyRect.maxX() / cloudGrid.x + 1; x++) {
		for (int y = dirtyRect.minY() / cloudGrid.y - 1; y <= dirtyRect.maxY() / cloudGrid.y + 1; y++) {
			
			//Reproduce the world location
			double2 worldLocation = int2(x, y) * cloudGrid;
			
			//Decide whether to draw a cloud here at all
			if (!hasCloudAt(worldLocation))
				continue;
			
			//Decide what cloud texture to use
			double textureNoise = cloudNoise(worldLocation * 467.248);
			unsigned int textureIndex = (textureNoise + 1.0) * 2;
			
			Texture * texture0 = currentCloudTextures[textureIndex];
			Texture * texture1 = targetCloudTextures[textureIndex];
			if (!texture0)
				continue;
			
			//Calculate an offset for the cloud
			double2 offset(cloudNoise(worldLocation * 0.941), cloudNoise(worldLocation * 12.786));
			offset *= 50;
			
			//Draw the quad
			InterpolatedTextureQuad quad;
			quad.rect = rectd(worldLocation + offset, texture0->size);
			quad.rect.origin -= quad.rect.size / 2;
			quad.state0.texture = texture0;
			quad.state1.texture = texture1;
			quad.interpolation = getSkyInterpolation();
			quad.draw();
		}
	}
}

void TowerBackground::drawGround(rectd dirtyRect)
{
	//Check whether the ground is visible
	if (dirtyRect.minY() < 0) {
		
		//Create the textured quad and find its rectangle
		TexturedQuad quad;
		quad.rect = dirtyRect;
		quad.rect.size.y = 360;
		quad.rect.origin.y = -quad.rect.size.y;
		
		//Set the texture, generate the texture rect and draw the quad
		quad.texture = groundTexture;
		quad.autogenerateTextureRect(true, false);
		quad.draw();
	}
	
	//Check whether the city is visible
	if (dirtyRect.minY() < 55 && dirtyRect.maxY() > 0) {
		
		//Create the textured quad for the city
		TexturedQuad quad;
		quad.rect = dirtyRect;
		quad.rect.size.y = cityTexture->size.y;
		quad.rect.origin.y = 0;
		
		//Set the texture, generate the coordinates and draw
		quad.texture = cityTexture;
		quad.autogenerateTextureRect(true, false);
		quad.draw();
	}
}





//----------------------------------------------------------------------------------------------------
#pragma mark -
#pragma mark Event Handling
//----------------------------------------------------------------------------------------------------

void TowerBackground::eventTimeChanged(Event * event)
{
	updateSkyIfNeeded.setNeeded();
}

void TowerBackground::eventDayChanged(Event * event)
{
	//Decide whether it will be a rainy day
	setRainyDay(randui(0, 4) == 0);
}





//----------------------------------------------------------------------------------------------------
#pragma mark -
#pragma mark Sky
//----------------------------------------------------------------------------------------------------

string TowerBackground::getSkyTextureName(unsigned int index, string variant)
{
	stringstream name;
	name << "simtower/background/sky/";
	name << index << "/";
	name << variant;
	return name.str();
}

string TowerBackground::getCloudTextureName(unsigned int index, string variant)
{
	stringstream name;
	name << "simtower/background/cloud/";
	name << index << "/";
	name << variant;
	return name.str();
}

double TowerBackground::getSkyInterpolation()
{
	return skyInterpolation;
}

void TowerBackground::setSkyInterpolation(double f)
{
	skyInterpolation = f;
}

TowerBackground::SkyState TowerBackground::getSkyState()
{
	return skyState;
}

void TowerBackground::setSkyState(SkyState state)
{
	if (skyState != state) {
		
		//If we're switching away from rain, also stop the rain sound
		if (skyState == Rain) rainSound->stop();
		
		//Set the new state
		skyState = state;
		updateSkyTexturesIfNeeded.setNeeded();
		
		//If we've switched to rain, start playing the sound
		if (skyState == Rain) {
			rainSound->play();
			nextThunderCountdown = 0;
		}
	}
}

void TowerBackground::setSky(SkyState state, double interpolation)
{
	setSkyState(state);
	setSkyInterpolation(interpolation);
}

unsigned int TowerBackground::getRainAnimationFrame()
{
	return rainAnimationFrame;
}

void TowerBackground::setRainAnimationFrame(unsigned int frame)
{
	if (rainAnimationFrame != frame) {
		rainAnimationFrame = frame;
		updateSkyTexturesIfNeeded.setNeeded();
	}
}

bool TowerBackground::hasCloudAt(double2 location)
{
	return (cloudNoise(location) > 0.6);
}

double TowerBackground::cloudNoise(double2 p)
{
	int n=(int)p.x+(int)p.y*57;
	n=(n<<13)^n;
	int nn=(n*(n*n*60493+19990303)+1376312589)&0x7fffffff;
	return 1.0-((double)nn/1073741824.0);
}





//----------------------------------------------------------------------------------------------------
#pragma mark -
#pragma mark Ground
//----------------------------------------------------------------------------------------------------

string TowerBackground::getCityTextureName()
{
	return "simtower/background/city";
}

string TowerBackground::getGroundTextureName()
{
	return "simtower/background/ground";
}
