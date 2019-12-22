#ifndef __MTOUCHPROTOCOL__H__
#define __MTOUCHPROTOCOL__H__
#include "MTouchControl.h"
class MTouchProtocol
{
public:
	MTouchProtocol(int _priority)
	{
		static int m_touchIndex = 0;
		m_id = m_touchIndex++;
		g_mTouchControl->RegisterTouchCallback(_priority, this);
	}
	virtual ~MTouchProtocol() 
	{
		g_mTouchControl->UnregisterTouch(this);
	}	
	virtual bool OnTouchBegin(int x, int y) = 0;
	virtual void OnTouchMove(int x, int y) = 0;
	virtual void OnTouchEnd(int x, int y) = 0;
	int GetId() { return m_id; }
protected:	
	int m_id;
};

#endif