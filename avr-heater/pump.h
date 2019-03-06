#ifndef PUMP_H
#define PUMP_H

#include <stdlib.h>
#include <stdint.h>

class Pump
{
	public:
		Pump();
		~Pump();
		void init();

		void setSpeed(uint8_t aSpeed); ///< [0-255] scaled to range [min, max]
		void setMinInterval(float aSpeed); ///< stroke interval in sec
		void setMaxInterval(float aSpeed); ///< stroke interval in sec

		void update(float aEt); ///< elapsed time.

		void start();
		void stop();
	private:
		float mMinSpeed;
		float mMaxSpeed;
		float mCurrentSpeed;

		float mTime;
		bool mIsOn;
		bool mIsActive;
};


#endif

