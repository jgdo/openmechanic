#include "DriverSeatControl.h"

#include "GamePhysics/PlayerBody.h"

#include <algorithm>


DriverSeatControl::DriverSeatControl(RID rid, ControlEngine *eng):
	BlockConctrolObject(rid, eng), _currentPower(0), _currentSteering(0)
{
	for (size_t i = 0; i < MAX_NUM_POWER; i++)
	{
		_powerCEPs.emplace_back(std::make_shared<DriverSeatCEPPower>(this));
	}

	for (size_t i = 0; i < MAX_NUM_STEERING; i++)
	{
		_steeringCEPs.emplace_back(std::make_shared<DriverSeatCEPSteering>(this));
	}
}


DriverSeatControl::~DriverSeatControl()
{
	if(_attachedPlayer) {
		_attachedPlayer->detachDriverSeat(false);
	}
}

std::vector<ConnectionEndpoint*> DriverSeatControl::getPowerEPs()
{
	std::vector<ConnectionEndpoint*> eps(_powerCEPs.size());
	std::transform(_powerCEPs.begin(), _powerCEPs.end(), eps.begin(), [](std::shared_ptr<ConnectionEndpoint> const& ptr) {return ptr.get();  });

	return eps;
}

std::vector<ConnectionEndpoint*> DriverSeatControl::getSteeringEPs()
{
	std::vector<ConnectionEndpoint*> eps(_steeringCEPs.size());
	std::transform(_steeringCEPs.begin(), _steeringCEPs.end(), eps.begin(), [](std::shared_ptr<ConnectionEndpoint> const& ptr) {return ptr.get();  });

	return eps;
}

void DriverSeatControl::setControlValues(float power, float steering)
{
	_currentPower = power;
	_currentSteering = steering;
}

std::vector<ConnectionEndpoint*> DriverSeatControl::getConnectionEndpoints() {
	std::vector<ConnectionEndpoint*> eps(_powerCEPs.size() + _steeringCEPs.size());
	std::transform(_powerCEPs.begin(), _powerCEPs.end(), eps.begin(), [](std::shared_ptr<ConnectionEndpoint> const& ptr) {return ptr.get();  });
	std::transform(_steeringCEPs.begin(), _steeringCEPs.end(), eps.begin()+ _powerCEPs.size(), [](std::shared_ptr<ConnectionEndpoint> const& ptr) {return ptr.get();  });

	return eps;
}

void DriverSeatControl::attachPlayerBody(PlayerBody *player) {
	_attachedPlayer = player;
}

void DriverSeatControl::detachPlayerBody() {
	_attachedPlayer = nullptr;
}


void DriverSeatCEPPower::setConnectionState(int state)
{
	if (state == NO_CONNECTION)
		_connected = false;
	else if (state == SIGNAL_SOURCE)
		_connected = true;
	else
		throw std::runtime_error("DriverSeatCEPPower::setConnectionState(): only NO_CONNECTION and SIGNAL_SOURCE allowed");
}

int DriverSeatCEPPower::getConnectionState() const
{
	return _connected ? SIGNAL_SOURCE : NO_CONNECTION;
}

float DriverSeatCEPPower::computeOutput()
{
	// std::cout << "power = " << _parentObject->getPowerValue() << std::endl;
	return _parentObject->getPowerValue();
}

void DriverSeatCEPPower::applyInput(float in)
{
	throw std::runtime_error("DriverSeatCEPPower::applyInput(): DriverSeatCEPPower is not an input, check connection ep matching");
}

void DriverSeatCEPSteering::setConnectionState(int state)
{
	if (state == NO_CONNECTION)
		_connected = false;
	else if (state == SIGNAL_SOURCE)
		_connected = true;
	else
		throw std::runtime_error("DriverSeatCEPSteering::setConnectionState(): only NO_CONNECTION and SIGNAL_SOURCE allowed");
}

int DriverSeatCEPSteering::getConnectionState() const
{
	return _connected ? SIGNAL_SOURCE : NO_CONNECTION;
}

float DriverSeatCEPSteering::computeOutput()
{
	// std::cout << "steering = " << _parentObject->getSteeringValue() << std::endl;
	return _parentObject->getSteeringValue();
}

void DriverSeatCEPSteering::applyInput(float in)
{
	throw std::runtime_error("DriverSeatCEPSteering::applyInput(): DriverSeatCEPPower is not an input, check connection ep matching");
}
