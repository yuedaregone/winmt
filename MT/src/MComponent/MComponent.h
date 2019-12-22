#pragma once

class MDrawProtocol;
class MComponent
{
public:
	virtual void Init(MDrawProtocol* draw) { m_draw = draw; }
	
	virtual void OnUpdate() {}
	
protected:
	MDrawProtocol* m_draw;
};
