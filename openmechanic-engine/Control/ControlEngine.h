#pragma once


#include "ControlObject.h"
#include "Connection.h"
#include <unordered_map>
#include <Utils/ResourceManager.h>
#include <unordered_set>

/*

Manages control elements like motors, driver seats, sensors etc and the connection between them and the physical elements

*/
class ControlEngine
{
public:
	class ConnectionListener {
	public:
		/*
			may not delete connection again
		*/
		virtual void onConnectionAdded(Connection* connection) = 0;
		virtual void onConnectionIsBeeingRemoved(Connection* connection) = 0;
	};

	ControlEngine(ResourceManager* resourceMan);
	virtual ~ControlEngine();

	Connection* tryCreateConnection(ControlObject* controlA, ControlObject* controlB);
	void removeAndDeleteConnection(Connection* connection);

	inline void setListener(ConnectionListener* listener) {
		_listener = listener;
	}

	inline ResourceManager* getResourceManager()
	{
		return _resourceManager;
	}

	// perform a control step
	void step();

	static Connection* conrolsAreConnected(ControlObject* coA, ControlObject* coB);
	
	inline std::unordered_set<Connection*> const& getConnections() const {
		return _connections;
	}

private:
	ResourceManager* _resourceManager;
	std::unordered_set<Connection*> _connections;

	ConnectionListener* _listener = nullptr;
};

