#pragma once
#include "MTool.h"
#include "MComponent.h"
#include <vector>
#include "MTimer.h"
enum
{
	None                = 0,
	DrawScale			= 0x1,
	DrawRotation		= 0x2,
};

class MComponent;
class MDrawProtocol : public MTimer
{
public:
	MDrawProtocol() : m_rotation(0) {
		m_rect = GetRect(0,0,0,0);
		m_drawState = None;
	}
	virtual ~MDrawProtocol() {}
	virtual void Draw() = 0;

	void SetPosition(uint32_t x, uint32_t y) 
	{
		uint32_t w = GetW();
		uint32_t h = GetH();

		m_rect.left = (LONG)x;
		m_rect.top = (LONG)y;
		m_rect.right = (LONG)x + w;
		m_rect.bottom = (LONG)y + h;
	}

	void SetRotation(float rot) { 
		m_drawState |= DrawRotation;
		m_rotation = rot; 
	}

	void SetScale(float x, float y) { 
		m_drawState |= DrawScale;
		m_scaleX = x; m_scaleY = y; 
	}

	uint32_t GetW() { return m_rect.right - m_rect.left; }
	uint32_t GetH() { return m_rect.bottom - m_rect.top; }

	void AddComponent(MComponent* comp) 
	{ 
		comp->Init(this);
		m_comps.push_back(comp); 
	}

	void RemoveComponent(MComponent* comp)
	{
		std::vector<MComponent*>::iterator it = m_comps.begin();
		for (; it != m_comps.end(); ++it)
		{
			if (*it == comp)
			{
				m_comps.erase(it);
				break;
			}
		}
	}

	virtual void OnUpdate()
	{
		std::vector<MComponent*>::iterator it = m_comps.begin();
		for (; it != m_comps.end(); ++it)
		{
			(*it)->OnUpdate();
		}
	}
protected:
	RECT m_rect;
	float m_rotation;
	float m_scaleX;
	float m_scaleY;
	int m_drawState;
	std::vector<MComponent*> m_comps;
};
