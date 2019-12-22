#include "MTouchControl.h"
#include "MTouchProtocol.h"

MTouchControl::MTouchControl()
:m_touches(NULL)
, m_curTouch(NULL)
{
}

MTouchControl::~MTouchControl()
{
	if (m_touches)
	{
		delete m_touches;
		m_touches = NULL;
	}
}

void MTouchControl::RegisterTouchCallback(int priority, MTouchProtocol* _touch)
{
	if (!m_touches)
	{
		m_touches = new std::map<int, std::vector<MTouchProtocol*> >();
	}
	std::map<int, std::vector<MTouchProtocol*> >::iterator it = m_touches->find(priority);
	if (it != m_touches->end())
	{
		it->second.push_back(_touch);
	}
	else
	{
		std::vector<MTouchProtocol*> tempVTouch;
		tempVTouch.push_back(_touch);
		m_touches->insert(std::map<int, std::vector<MTouchProtocol*> >::value_type(priority, tempVTouch));
	}
}

void MTouchControl::UnregisterTouch(MTouchProtocol* _touch)
{
	if (m_touches)
	{
		std::map<int, std::vector<MTouchProtocol*> >::iterator it = m_touches->begin();
		for (;it != m_touches->end();)
		{
			std::vector<MTouchProtocol*>::iterator itr = it->second.begin();
			for (; itr != it->second.end(); ++itr)
			{
				if ((*itr)->GetId() == _touch->GetId())
				{
					it->second.erase(itr);
					break;
				}
			}
			if (it->second.empty())
			{
				it = m_touches->erase(it);
				break;
			}
		}
	}
}

bool MTouchControl::OnTouchBegin(int x, int y)
{
	if (m_touches)
	{
		std::map<int, std::vector<MTouchProtocol*> >::iterator it = m_touches->begin();
		for (; it != m_touches->end(); ++it)
		{
			std::vector<MTouchProtocol*>::iterator itr = it->second.begin();
			for (; itr != it->second.end(); ++itr)
			{
				bool isTouch = (*itr)->OnTouchBegin(x, y);
				if (isTouch)
				{
					m_curTouch = *itr;
					return true;
				}
			}
		}
	}
	return false;
}

void MTouchControl::OnTouchMove(int x, int y)
{
	if (m_curTouch)
	{
		m_curTouch->OnTouchMove(x, y);
	}
}

void MTouchControl::OnTouchEnd(int x, int y)
{
	if (m_curTouch)
	{
		m_curTouch->OnTouchEnd(x, y);
		m_curTouch = NULL;
	}
}