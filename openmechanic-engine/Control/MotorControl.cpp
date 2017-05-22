#include "MotorControl.h"

#include  <algorithm>

MotorControl::MotorControl(RID rid, ControlEngine *eng):
	BlockConctrolObject(rid, eng), _currentSpeed(15), _muliplier(1)
{
	for (size_t i = 0; i < MAX_NUM_CONNECTIONS; i++)
	{
		_mcCEPs.emplace_back(std::make_shared<MotorControlCEPOut>(this, 
			[](MotorControlCEPOut* self) {return self->getParentObject()->getSpeed() * self->getParentObject()->getMultiplier(); },
			[](MotorControlCEPOut* self, bool connected) {}));
	}

	_signalInCEP = std::make_shared<MotorControlCEPSignalIn>(this,
		[](MotorControlCEPSignalIn* self, float in) { self->getParentObject()->setMultiplier(in); },
		[](MotorControlCEPSignalIn* self, bool connected) {if (!connected) self->getParentObject()->setMultiplier(1); });
}

void MotorControl::setSpeed(float speed)
{
	_currentSpeed = speed;
}

MotorControl::~MotorControl()
{
	disconnectControl();
}

std::vector<ConnectionEndpoint*> MotorControl::getConnectionEndpoints() {
	std::vector<ConnectionEndpoint*> eps(_mcCEPs.size()+1);
	std::transform(_mcCEPs.begin(), _mcCEPs.end(), eps.begin(), [](std::shared_ptr<ConnectionEndpoint> const& ptr) {return ptr.get();  });
	eps.back() = _signalInCEP.get();
	return eps;
}
