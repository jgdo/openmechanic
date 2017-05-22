#pragma once

#include <functional>
#include <stdexcept>

class ControlObject;

enum {
	NO_CONNECTION = 0,
	FORCE_SOURCE = (1 << 0),
	FORCE_SINK = (1 << 1),
	SIGNAL_SOURCE = (1 << 2),
	SIGNAL_SINK = (1 << 3)
};

class Connection;

class ConnectionEndpoint
{
public:
	ConnectionEndpoint();
	virtual ~ConnectionEndpoint();

	virtual ControlObject* getParentObject() = 0;

	virtual int getConnectionType() const = 0;

	virtual float computeOutput() = 0;

	virtual void applyInput(float in) = 0;
	
	virtual void setConnectionState(int state) = 0;

	virtual int getConnectionState() const = 0;

	inline bool isConnected() const
	{
		return getConnectionState() != NO_CONNECTION;
	}

	void connectDisconnect(int state, Connection* connection);
	
	inline Connection* getConnection()
	{
		return _currentConnection;
	}

protected:
	Connection* _currentConnection;
};

template<class CO>
class ConnectionEndpointBase: public ConnectionEndpoint
{
public:
	virtual ~ConnectionEndpointBase()
	{
	}

	ConnectionEndpointBase(CO * co): 
		_parentObject(co)
	{
		
	}
	
	virtual CO* getParentObject() override
	{
		return _parentObject;
	}

protected:
	CO* _parentObject;
};


template<class CO, int CType>
class SimpleInputConnectionEndpoint: public ConnectionEndpointBase<CO>{
public:
	SimpleInputConnectionEndpoint(CO * co, std::function<void(SimpleInputConnectionEndpoint*, float)> handler, std::function<void(SimpleInputConnectionEndpoint*, bool)> connectHandler) :
		ConnectionEndpointBase<CO>(co), _inputHandler(handler), _connectHandler(connectHandler), _connected(false)
	{
	}

	virtual ~SimpleInputConnectionEndpoint()
	{
		
	}

	virtual int getConnectionType() const
	{
		return CType;
	}

	virtual float computeOutput()
	{
		throw std::runtime_error("SimpleInputConnectionEndpoint::computeOutput(): cannot compute output of an input connection. Check connection matching in control engine");
	}

	virtual void applyInput(float in)
	{
		_inputHandler(this, in);
	}

	virtual void setConnectionState(int state)
	{
		if(state == NO_CONNECTION)
		{
			_connected = false;
		} else if(state == CType)
		{
			_connected = true;
		} else
		{
			throw std::runtime_error("SimpleInputConnectionEndpoint::setConnectionState(): cannot set connection state to unsupported state. Check connection matching in control engine");
		}

		_connectHandler(this, _connected);
	}

	virtual int getConnectionState() const
	{
		return _connected ? CType : NO_CONNECTION;
	}

private:
	std::function<void(SimpleInputConnectionEndpoint*, float)> _inputHandler;
	std::function<void(SimpleInputConnectionEndpoint*, bool)> _connectHandler;
	bool _connected;
};

template<class CO, int CType>
class SimpleOutputConnectionEndpoint : public ConnectionEndpointBase<CO> {
public:
	SimpleOutputConnectionEndpoint(CO * co, std::function<float(SimpleOutputConnectionEndpoint*)> handler, std::function<void(SimpleOutputConnectionEndpoint*, bool)> connectHandler) :
		ConnectionEndpointBase<CO>(co), _outputHandler(handler), _connectHandler(connectHandler), _connected(false)
	{
	}

	virtual ~SimpleOutputConnectionEndpoint()
	{
		
	}

	virtual int getConnectionType() const
	{
		return CType;
	}

	virtual float computeOutput()
	{
		return _outputHandler(this);
	}

	virtual void applyInput(float in)
	{
		throw std::runtime_error("SimpleOutputConnectionEndpoint::computeOutput(): cannot compute input of an output connection. Check connection matching in control engine");
	}

	virtual void setConnectionState(int state)
	{
		if (state == NO_CONNECTION)
		{
			_connected = false;
		}
		else if (state == CType)
		{
			_connected = true;
		}
		else
		{
			throw std::runtime_error("SimpleOutputConnectionEndpoint::setConnectionState(): cannot set connection state to unsupported state. Check connection matching in control engine");
		}

		_connectHandler(this, _connected);
	}

	virtual int getConnectionState() const
	{
		return _connected ? CType : NO_CONNECTION;
	}

private:
	std::function<float(SimpleOutputConnectionEndpoint*)> _outputHandler;
	std::function<void(SimpleOutputConnectionEndpoint*, bool)> _connectHandler;
	bool _connected;
};
