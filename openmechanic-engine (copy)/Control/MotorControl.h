#pragma once
#include "ControlObject.h"


class MotorControl :
	public BlockConctrolObject
{
	typedef SimpleOutputConnectionEndpoint<MotorControl, FORCE_SOURCE> MotorControlCEPOut;
	typedef SimpleInputConnectionEndpoint<MotorControl, SIGNAL_SINK> MotorControlCEPSignalIn;

public:
	static const size_t MAX_NUM_CONNECTIONS = 5;

	typedef std::shared_ptr<MotorControl> Ptr;
	typedef std::shared_ptr<const MotorControl> ConstPtr;

	template<typename ...Arg> Ptr static create(Arg&&...arg) {
		struct EnableMakeShared : public MotorControl {
			EnableMakeShared(Arg&&...arg) :MotorControl(std::forward<Arg>(arg)...) {}
		};
		return std::make_shared<EnableMakeShared>(std::forward<Arg>(arg)...);
	}

	virtual ~MotorControl();

	void setSpeed(float speed);

	inline float getSpeed() const
	{
		return _currentSpeed;
	}

	inline void setMultiplier(float mul)
	{
		_muliplier = mul;
	}

	inline float getMultiplier() const
	{
		return _muliplier;
	}

	virtual std::vector<ConnectionEndpoint*> getConnectionEndpoints() override;

protected:
	MotorControl(RID rid, ControlEngine *eng);

private:
	std::vector<std::shared_ptr<MotorControlCEPOut>> _mcCEPs;
	std::shared_ptr<MotorControlCEPSignalIn> _signalInCEP;

	float _currentSpeed;
	float _muliplier;
};
