#pragma once
#include "MTimerManager.h"

class MTimer
{
public:
	MTimer() 
	{
		g_mTimerManager->RegisterTimer(this);
	}

	virtual ~MTimer() 
	{
		g_mTimerManager->UnregisterTimer(this);
	}

	virtual void OnUpdate() = 0;
private:

};
