#pragma once

#include "ControlObject.h"

class DriverSeatCEPPower;
class DriverSeatCEPSteering;

class PlayerBody;

class DriverSeatControl :
	public BlockConctrolObject
{
	static const size_t MAX_NUM_POWER = 5;
	static const size_t MAX_NUM_STEERING = 5;

public:
	typedef std::shared_ptr<DriverSeatControl> Ptr;
	typedef std::shared_ptr<const DriverSeatControl> ConstPtr;

	template<typename ...Arg> Ptr static create(Arg&&...arg) {
		struct EnableMakeShared : public DriverSeatControl {
			EnableMakeShared(Arg&&...arg) :DriverSeatControl(std::forward<Arg>(arg)...) {}
		};
		return std::make_shared<EnableMakeShared>(std::forward<Arg>(arg)...);
	}

	virtual ~DriverSeatControl();

	virtual std::vector<ConnectionEndpoint*> getConnectionEndpoints() override;

	inline float getPowerValue() const
	{
		return _currentPower;
	}

	inline float getSteeringValue() const
	{
		return _currentSteering;
	}

	void setControlValues(float power, float steering);

	std::vector<ConnectionEndpoint*> getPowerEPs();
	std::vector<ConnectionEndpoint*> getSteeringEPs();

	// should be called from PlayerBody only
	void attachPlayerBody(PlayerBody* player);
	void detachPlayerBody();

protected:
	DriverSeatControl(RID rid, ControlEngine *eng);

	std::vector<std::shared_ptr<DriverSeatCEPPower>> _powerCEPs;
	std::vector<std::shared_ptr<DriverSeatCEPSteering>> _steeringCEPs;

	float _currentPower, _currentSteering;

	PlayerBody* _attachedPlayer = nullptr;
};

class DriverSeatCEPPower : public ConnectionEndpointBase<DriverSeatControl> {
public:
	using ConnectionEndpointBase::ConnectionEndpointBase;

	inline virtual int getConnectionType() const {
		return SIGNAL_SOURCE;
	}

	virtual void setConnectionState(int state) override;

	virtual int getConnectionState() const override;

	virtual float computeOutput() override;
	virtual void applyInput(float in) override;

private:
	bool _connected = false;
};

class DriverSeatCEPSteering : public ConnectionEndpointBase<DriverSeatControl> {
public:
	using ConnectionEndpointBase::ConnectionEndpointBase;

	inline virtual int getConnectionType() const {
		return SIGNAL_SOURCE;
	}

	virtual void setConnectionState(int state) override;

	virtual int getConnectionState() const override;

	virtual float computeOutput() override;
	virtual void applyInput(float in) override;

private:
	bool _connected = false;
};
