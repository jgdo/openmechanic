#include "ControlEngine.h"
#include "ConnectionEndpoint.h"

#include "GamePhysics/Joint.h"
#include "DriverSeatControl.h"
#include "MotorControl.h"

ControlEngine::ControlEngine(ResourceManager* resourceMan):
	_resourceManager(resourceMan)
{
}


ControlEngine::~ControlEngine()
{
}

template<class TSouce, class TSink> 
bool getSourceSinkPair(ControlObject* controlA, ControlObject* controlB, TSouce** sourceOut, TSink** sinkOut)
{
	TSouce* source = nullptr;
	TSink* sink = nullptr;

	if (!(source = dynamic_cast<TSouce*>(controlA)) || !(sink = dynamic_cast<TSink*>(controlB)))
	{
		if (!(source = dynamic_cast<TSouce*>(controlB)) || !(sink = dynamic_cast<TSink*>(controlA)))
			return false;
	}

	if (sourceOut)
		*sourceOut = source;

	if (sinkOut)
		*sinkOut = sink;

	return true;
}

bool getFreeEPs(std::vector<ConnectionEndpoint*>&& epsA, std::vector<ConnectionEndpoint*>&& epsB, int typeA, int typeB, ConnectionEndpoint** epAOut, ConnectionEndpoint** epBOut, int* typeAOut, int* typeBOut)
{
	ConnectionEndpoint* epA = nullptr;
	for (ConnectionEndpoint* a : epsA)
	{
		if (!a->isConnected() && (a->getConnectionType() & typeA))
		{
			epA = a;
			break;
		}
	}

	if (!epA)
		return false;

	ConnectionEndpoint* epB = nullptr;
	for (ConnectionEndpoint* b : epsB)
	{
		if (!b->isConnected() && (b->getConnectionType() & typeB))
		{
			epB = b;
			break;
		}
	}

	if (!epB)
		return false;

	if (epAOut)
		*epAOut = epA;

	if (epBOut)
		*epBOut = epB;

	if (typeAOut)
		*typeAOut = typeA;

	if (typeBOut)
		*typeBOut = typeB;

	return true;
}

Connection* ControlEngine::tryCreateConnection(ControlObject* controlA, ControlObject* controlB) {
	std::vector<ConnectionEndpoint*> epsA = controlA->getConnectionEndpoints();
	std::vector<ConnectionEndpoint*> epsB = controlB->getConnectionEndpoints();

	if (epsA.empty() || epsB.empty())
		return nullptr;

	ConnectionEndpoint* source = nullptr, *sink = nullptr;
	int typeSource = NO_CONNECTION, typeSink = NO_CONNECTION;

	// first check some special cases
	{
		// Driver Seat <-> Motor
		DriverSeatControl* sourceControl = nullptr;
		MotorControl* sinkControl = nullptr;
		if(getSourceSinkPair(controlA, controlB, &sourceControl, &sinkControl))
			if(getFreeEPs(sourceControl->getPowerEPs(), sinkControl->getConnectionEndpoints(), SIGNAL_SOURCE, SIGNAL_SINK, &source, &sink, &typeSource, &typeSink))
				goto finished;
	}

	{
		// Driver Seat <-> Joint
		DriverSeatControl* sourceControl = nullptr;
		Joint* sinkControl = nullptr;
		if (getSourceSinkPair(controlA, controlB, &sourceControl, &sinkControl))
			if (getFreeEPs(sourceControl->getSteeringEPs(), sinkControl->getConnectionEndpoints(), SIGNAL_SOURCE, SIGNAL_SINK, &source, &sink, &typeSource, &typeSink))
				goto finished;
	}

	// now just try every combination
	for(ConnectionEndpoint* epA: epsA)
	{
		if (epA->isConnected())
			continue;

		for(ConnectionEndpoint* epB: epsB)
		{
			if (epB->isConnected())
				continue;

			if((epA->getConnectionType() & FORCE_SOURCE) && (epB->getConnectionType() & FORCE_SINK))
			{
				source = epA;
				sink = epB;
				typeSource = FORCE_SOURCE;
				typeSink = FORCE_SINK;
				goto finished;
			}else if ((epB->getConnectionType() & FORCE_SOURCE) && (epA->getConnectionType() & FORCE_SINK))
			{
				source = epB;
				sink = epA;
				typeSource = FORCE_SOURCE;
				typeSink = FORCE_SINK;
				goto finished;
			}

			else if ((epA->getConnectionType() & SIGNAL_SOURCE) && (epB->getConnectionType() & SIGNAL_SINK))
			{
				source = epA;
				sink = epB;
				typeSource = SIGNAL_SOURCE;
				typeSink = SIGNAL_SINK;
				goto finished;
			}
			else if ((epB->getConnectionType() & SIGNAL_SOURCE) && (epA->getConnectionType() & SIGNAL_SINK))
			{
				source = epB;
				sink = epA;
				typeSource = SIGNAL_SOURCE;
				typeSink = SIGNAL_SINK;
				goto finished;
			}
		}
	}

	return nullptr; // no connection found

finished:
	Connection* connection = getResourceManager()->create<Connection>(source, sink);
	_connections.emplace(connection);
	source->connectDisconnect(typeSource, connection);
	sink->connectDisconnect(typeSink, connection);

	if (_listener)
		_listener->onConnectionAdded(connection);

	return connection;
}

void ControlEngine::step()
{
	for(auto& entry: _connections)
	{
		float value = entry->getSource()->computeOutput();
		entry->getSink()->applyInput(value);
	}
}

void ControlEngine::removeAndDeleteConnection(Connection* connection) {
	auto iter = _connections.find(connection);
	if (iter != _connections.end()) {
		if (_listener)
			_listener->onConnectionIsBeeingRemoved(connection);

		connection->getSource()->connectDisconnect(NO_CONNECTION, nullptr);
		connection->getSink()->connectDisconnect(NO_CONNECTION, nullptr);

		_connections.erase(iter);
		getResourceManager()->destroy(connection);
	}

	// FIXME: what to do if connection not present
}

 Connection* ControlEngine::conrolsAreConnected(ControlObject* coA, ControlObject* coB)
{
	for(ConnectionEndpoint* cepA: coA->getConnectionEndpoints())
	{
		Connection* conA = cepA->getConnection();
		if (!conA)
			continue;

		for(ConnectionEndpoint* cepB: coB->getConnectionEndpoints())
		{
			Connection* conB = cepB->getConnection();
			if (!conB)
				continue;

			if (conA == conB)
				return conA;
		}
	}

	return nullptr;
}