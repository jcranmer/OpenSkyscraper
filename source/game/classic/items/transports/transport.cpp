#include "transport.h"

using namespace OSS;
using namespace Classic;





//----------------------------------------------------------------------------------------------------
#pragma mark -
#pragma mark Connection
//----------------------------------------------------------------------------------------------------

bool TransportItem::connectsToFloor(int floor)
{
	return (floor >= getMinFloor() && floor <= getMaxFloor());
}

std::set<int> TransportItem::getConnectedLobbies()
{
	std::set<int> lobbies;
	
	int minLobby = getRect().minY() / 15;
	int maxLobby = getRect().maxY() / 15;
	for (int lobby = minLobby; lobby <= maxLobby; lobby++)
		if (connectsToFloor(lobby * 15))
			lobbies.insert(lobby);
	
	return lobbies;
}

std::set<int> TransportItem::getConnectionFloors()
{
	return getConnectedLobbies();
}





//----------------------------------------------------------------------------------------------------
#pragma mark -
#pragma mark Identity
//----------------------------------------------------------------------------------------------------

bool TransportItem::isElevator()
{
	return (getGroup() == kElevatorGroup);
}

bool TransportItem::isStairslike()
{
	return (isStairs() || isEscalator());
}

bool TransportItem::isStairs()
{
	return (getType() == kStairsType);
}

bool TransportItem::isEscalator()
{
	return (getType() == kEscalatorType);
}
