#pragma once
#include <vector>
#include "MSingleton.h"

class MTimer;
class MTimerManager : public MSingleton<MTimerManager>
{
public:
	void RegisterTimer(MTimer* timer);

	void UnregisterTimer(MTimer* timer);

	void Update();

protected:
	void OnUpdateEnd();

private:
	std::vector<MTimer*> m_timers;
	std::vector<MTimer*> m_willAdd;
	std::vector<MTimer*> m_willRmove;
};

#define g_mTimerManager MTimerManager::getInstance()
