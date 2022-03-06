/*
 * CPolledTimer.cpp
 *
 *  Created on: 01.10.2020
 *      Author: mnl
 */

#include "mbed.h"
#include "CPolledTimer.h"

CPolledTimer::CPolledTimer(int32_t waitTime) {
	m_waitTime = waitTime;
	m_targetTime = millisSinceStart() + m_waitTime;
}

int64_t CPolledTimer::millisSinceStart() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(
			HighResClock::now().time_since_epoch()).count();
}

bool CPolledTimer::timeReached() {
	if (millisSinceStart()>=m_targetTime)
	{
		m_targetTime = m_targetTime + m_waitTime;
		return true;
	}
	else
	{
	return false;
	}
}

void CPolledTimer::updateWaitTime(int32_t waitTime) {
	m_waitTime = waitTime;
	m_targetTime = millisSinceStart()+ m_waitTime;

}
