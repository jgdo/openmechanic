#include "Connection.h"

Connection::Connection(RID rid, ConnectionEndpoint* source, ConnectionEndpoint* sink):
	GameObject(rid), _source(source), _sink(sink)
{
}


Connection::~Connection()
{
}
