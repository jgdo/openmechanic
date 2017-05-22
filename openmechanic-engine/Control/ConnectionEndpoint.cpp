#include "ConnectionEndpoint.h"


ConnectionEndpoint::ConnectionEndpoint():
	_currentConnection(nullptr)
{
}


void ConnectionEndpoint::connectDisconnect(int state, Connection* connection)
{
	if (_currentConnection && connection) 
		throw std::runtime_error("ConnectionEndpoint::connect(): already connected");

	_currentConnection = connection;
	setConnectionState(state);
}

ConnectionEndpoint::~ConnectionEndpoint()
{
}
