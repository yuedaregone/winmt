#include "MTimerManager.h"
#include "MTimer.h"



void MTimerManager::RegisterTimer(MTimer* timer)
{
	m_willAdd.push_back(timer);
}

void MTimerManager::UnregisterTimer(MTimer* timer)
{
	m_willRmove.push_back(timer);
}

void MTimerManager::Update()
{
	std::vector<MTimer*>::iterator it;
	for (it = m_timers.begin(); it != m_timers.end(); ++it)
	{
		(*it)->OnUpdate();
	}

	this->OnUpdateEnd();
}

void MTimerManager::OnUpdateEnd()
{
	if (!m_willAdd.empty())
	{
		std::vector<MTimer*>::iterator it;
		for (it = m_willAdd.begin(); it != m_willAdd.end(); ++it)
		{
			m_timers.push_back(*it);
		}
		m_willAdd.clear();
	}
	if (!m_willRmove.empty())
	{
		std::vector<MTimer*>::iterator it;
		for (it = m_willRmove.begin(); it != m_willRmove.end(); ++it)
		{
			std::vector<MTimer*>::iterator t = std::find(m_timers.begin(), m_timers.end(), *it);
			if (t != m_timers.end())
			{
				m_timers.erase(t);
			}
		}
		m_willRmove.clear();
	}
}

