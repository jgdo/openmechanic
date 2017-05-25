#pragma once

#include "ConnectionEndpoint.h"

#include "Utils/GameObject.h"

#include <memory>

class Connection: public GameObject
{
public:
	typedef std::shared_ptr<Connection> Ptr;
	typedef std::shared_ptr<Connection const> ConstPtr;

	typedef Connection BaseResourceType;

	template<typename ...Arg> Ptr static create(Arg&&...arg) {
		struct EnableMakeShared : public Connection {
			EnableMakeShared(Arg&&...arg) :Connection(std::forward<Arg>(arg)...) {}
		};
		return std::make_shared<EnableMakeShared>(std::forward<Arg>(arg)...);
	}

	~Connection();

	inline ConnectionEndpoint* getSource() {
		return _source;
	}

	inline ConnectionEndpoint* getSink() {
		return _sink;
	}

protected:
	Connection(RID rid, ConnectionEndpoint* source, ConnectionEndpoint* sink);
private:
	ConnectionEndpoint* _source;
	ConnectionEndpoint* _sink;
};

typedef Connection::Ptr ConnectionPtr;
typedef Connection::ConstPtr ConnectionConstPtr;

