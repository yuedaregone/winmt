#ifndef __MTOUCHCONTROL_H__
#define __MTOUCHCONTROL_H__
#include "MSingleton.h"
#include <map>
#include <vector>

class MTouchProtocol;
class MTouchControl : public MSingleton<MTouchControl>
{
public:
	MTouchControl();
	~MTouchControl();
public:
	void RegisterTouchCallback(int priority, MTouchProtocol* _touch);
	void UnregisterTouch(MTouchProtocol* _touch);

	bool OnTouchBegin(int x, int y);
	void OnTouchMove(int x, int y);
	void OnTouchEnd(int x, int y);
private:
	std::map<int, std::vector<MTouchProtocol*> >* m_touches;
	MTouchProtocol* m_curTouch;
};
#define g_mTouchControl MTouchControl::getInstance()
#endif