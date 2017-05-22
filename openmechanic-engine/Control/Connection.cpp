#include "Connection.h"

Connection::Connection(RID rid, ConnectionEndpoint* source, ConnectionEndpoint* sink):
	Resource(rid), _source(source), _sink(sink)
{
}


Connection::~Connection()
{
}
