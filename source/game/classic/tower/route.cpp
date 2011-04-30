#include "route.h"

using namespace OSS;
using namespace Classic;





//----------------------------------------------------------------------------------------------------
#pragma mark -
#pragma mark Node
//----------------------------------------------------------------------------------------------------

Route::Node::Node(Route * route, recti start, TransportItem * transport, recti end)
{
	this->route = route;
	this->start = start;
	this->transport = transport;
	this->end = end;
}





//----------------------------------------------------------------------------------------------------
#pragma mark -
#pragma mark Initialization
//----------------------------------------------------------------------------------------------------

Route::Route(Tower * tower) : tower(tower)
{
}

Route::Route(const Route & route) : tower(route.tower)
{
	origin = route.origin;
	destination = route.destination;
	for (Nodes::const_iterator it = route.nodes.begin(); it != route.nodes.end(); it++)
		nodes.push_back(*it);
}

string Route::description()
{
	string r = "route (\n";
	r += "\t"; r += origin.description() + "\n";
	for (Nodes::iterator node = nodes.begin(); node != nodes.end(); node++) {
		if (!(*node)) continue;
		r += "\t";
		r += (*node)->start.description();
		r += " >-- ";
		r += (*node)->transport->description();
		r += " --> ";
		r += (*node)->end.description();
		r += "\n";
	}
	r += "\t"; r += destination.description() + "\n";
	r += ")";
	return r;
}





//----------------------------------------------------------------------------------------------------
#pragma mark -
#pragma mark Nodes
//----------------------------------------------------------------------------------------------------

const Route::Nodes & Route::getNodes() const
{
	return nodes;
}

void Route::addNode(Node * node)
{
	if (!node)
		return;
	nodes.push_back(node);
}

void Route::addNode(recti start, TransportItem * transport, recti end)
{
	addNode(new Node(this, start, transport, end));
}

Route::Node * Route::nextNode()
{
	if (nodes.empty())
		return NULL;
	return nodes.front();
}

void Route::popNode()
{
	assert(!nodes.empty());
	nodes.pop_front();
}





//----------------------------------------------------------------------------------------------------
#pragma mark -
#pragma mark Distance
//----------------------------------------------------------------------------------------------------

const unsigned int Route::getDistance()
{
	recti offset = origin;
	double distance = 0;
	for (Nodes::const_iterator it = nodes.begin(); it != nodes.end(); it++) {
		distance += offset.distanceX((*it)->start);
		offset = (*it)->end;
	}
	distance += offset.distanceX(destination);
	return distance;
}





//----------------------------------------------------------------------------------------------------
#pragma mark -
#pragma mark Operators
//----------------------------------------------------------------------------------------------------

Route & Route::operator= (const Route & route)
{
	assert(route.tower == tower);
	origin = route.origin;
	destination = route.destination;
	//nodes = route.nodes;
	nodes.clear();
	for (Nodes::const_iterator it = route.nodes.begin(); it != route.nodes.end(); it++)
		nodes.push_back(*it);
	return *this;
}





//----------------------------------------------------------------------------------------------------
#pragma mark -
#pragma mark Validation
//----------------------------------------------------------------------------------------------------

bool Route::isValid()
{
	for (Nodes::iterator node = nodes.begin(); node != nodes.end(); node++) {
		if (!(*node)) continue;
		if (!(*node)->transport->isInTower())
			return false;
		if (!(*node)->transport->connectsToFloor((*node)->start.minY()))
			return false;
		if (!(*node)->transport->connectsToFloor((*node)->end.minY()))
			return false;
	}
	return true;
}





//----------------------------------------------------------------------------------------------------
#pragma mark -
#pragma mark Pathfinder
//----------------------------------------------------------------------------------------------------

Route * Route::findRoute(Tower * tower, recti origin, recti destination, unsigned int options)
{
	Route * route = new Route(tower);
	route->origin = origin;
	route->destination = destination;
	//route->autorelease();
	
	if (!findRoute(tower, origin, destination, options, NULL, UsedTransportsSet(),
				   PathfinderStats(), route))
		route = NULL;
	
	return route;
}

bool Route::findRoute(Tower * tower, recti origin, recti destination, unsigned int options,
					  TransportItem * transport, UsedTransportsSet usedTransports,
					  PathfinderStats stats, Route * route)
{
	//If there is no transport and the origin and destination are on the same floor, we already
	//have a route.
	if (!transport && origin.minY() == destination.minY())
		return true;
	
	//Check whether the transport connects directly to the destination floor
	if (transport && transport->connectsToFloor(destination.minY())) {
		route->addNode(origin, transport, transport->getFloorRect(destination.minY()));
		return true;
	}
	
	//Fetch the connection floors
	std::set<int> connectionFloors;
	if (transport) {
		connectionFloors = transport->getConnectionFloors();
		connectionFloors.erase(origin.minY());
	} else {
		connectionFloors.insert(origin.minY());
	}
	
	//Update the stats and add this transport to the used ones
	if (transport) {
		if (transport->isElevator()) stats.elevatorsUsed++;
		if (transport->isStairs()) stats.stairsUsed++;
		if (transport->isEscalator()) stats.escalatorsUsed++;
		usedTransports.insert(transport);
	}
	
	//Abort if we exceed the stats limits
	if (stats.elevatorsUsed >= 2 || stats.stairsUsed >= 4 || stats.escalatorsUsed >= 6)
		return false;
	
	//Iterate through the connection floors
	Pointer<Route> shortestRoute;
	
	for (std::set<int>::iterator floor = connectionFloors.begin(); floor != connectionFloors.end(); floor++) {
		
		//Find the transports on this floor
		TowerStructure::ItemSet items = tower->structure->getItems(*floor, kTransportCategory);
		for (TowerStructure::ItemSet::iterator item = items.begin(); item != items.end(); item++) {
			TransportItem * t = (TransportItem *)((Item *)*item);
			
			//Avoid service elevators if demanded
			if (options & kNoServiceElevators && t->getType() == kServiceElevatorType)
				continue;
			
			//Avoid anything but service elevators if demanded
			if (options & kOnlyServiceElevators && t->getType() != kServiceElevatorType)
				continue;
			
			//Skip transports we've already used
			if (usedTransports.count(t))
				continue;
			
			//Skip elevators if we aren't on a lobby and connecting from a previous transport
			if (t->isElevator() && (*floor % 15) != 0 && transport)
				continue;
			
			//Create a new temporary route
			Pointer<Route> newRoute = new Route(*route);
			if (transport)
				newRoute->addNode(origin, transport, transport->getFloorRect(*floor));
			
			//Find routes using this transport
			//OSSObjectLog << "finding routes using " << t->description() << std::endl;
			if (findRoute(tower, t->getFloorRect(*floor), destination, options, t, usedTransports,
						  stats, newRoute))
				if (!shortestRoute || newRoute->getDistance() < shortestRoute->getDistance())
					shortestRoute = newRoute;
		}
	}
	
	//If we have found a route, copy it to the old one
	if (shortestRoute) {
		*route = *shortestRoute;
		return true;
	}
	
	return false;
}
