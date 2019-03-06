#include "pump.h"

#include <avr/io.h>

Pump::Pump()
{
	init();
}


Pump::~Pump()
{

}


void Pump::init()
{
	mIsActive = false;
	mTime = 0.0;
	mMinSpeed = 5.0;
	mMaxSpeed = 1.0;
	mCurrentSpeed = 2.0;
	mIsOn = false;

	DDRB |= (1<<PB2);
}


void Pump::setSpeed(uint8_t aSpeed)
{
	float range = mMaxSpeed - mMinSpeed;
	mCurrentSpeed =  (aSpeed/255.0) * range;
	mCurrentSpeed += mMinSpeed;
}


void Pump::setMinInterval(float aSpeed)
{
	mMinSpeed = aSpeed;
}


void Pump::setMaxInterval(float aSpeed)
{
	mMaxSpeed = aSpeed;
}


void Pump::update(float aEt)
{
	if(!mIsActive)
	{
		PORTB &= ~(1<<PB2);
		return;
	}

	mTime += aEt;
	
	if(mIsOn &&  mTime > 0.5)
	{
		mIsOn = false;
		// turn off
		PORTB &= ~(1<<PB2);
	}
	if(!mIsOn && mTime >= mCurrentSpeed)
	{
		mIsOn = true;
		mTime = 0.0;
		// turn on
		PORTB |= (1<<PB2);
	}


	if(mTime > mMinSpeed)
		mTime = 0.0;
}


void Pump::start()
{
	mIsActive = true;
}


void Pump::stop()
{
	mIsActive = false;
}

