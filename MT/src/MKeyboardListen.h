#pragma once
#include "MSingleton.h"
#include <vector>
#include "MApplication.h"

#define KEY_DOWN 1
#define KEY_UP 0

typedef bool(*KeyEvent)(int state, int code);


class MKeyboardListen : public MSingleton<MKeyboardListen>
{
public:
	void RegisterKeyListen(KeyEvent ev)
	{
		if (ev == NULL) return;

		if (MApplaction::GetCurrent()->GetHHOOK() == NULL)
			MApplaction::GetCurrent()->StartHOOK();

		std::vector<KeyEvent>::iterator it = std::find(m_events.begin(), m_events.end(), ev);
		if (it == m_events.end())
		{
			m_events.push_back(ev);
		}
	}

	void UnregisterKeyListen(KeyEvent ev)
	{
		std::vector<KeyEvent>::iterator it = std::find(m_events.begin(), m_events.end(), ev);
		if (it != m_events.end())
		{
			m_events.erase(it);
		}

		if (m_events.empty())
		{
			m_downKeys.clear();
			MApplaction::GetCurrent()->StopHOOK();
		}
	}

	bool IsClickKey(int keycode)
	{
		return std::find(m_downKeys.begin(), m_downKeys.end(), keycode)
			!= m_downKeys.end();
	}

private:
	bool OnKeyEvent(int state, int code)
 	{
		if (state == KEY_DOWN)
		{
			std::vector<int>::iterator it = std::find(m_downKeys.begin(), m_downKeys.end(), code);
			if (it == m_downKeys.end())
			{
				m_downKeys.push_back(code);
			}			
		}
		else if (state == KEY_UP)
		{
			std::vector<int>::iterator it = std::find(m_downKeys.begin(), m_downKeys.end(), code);
			if (it != m_downKeys.end())
			{
				m_downKeys.erase(it);
			}
		}

		bool ret = false;
		std::vector<KeyEvent>::iterator it = m_events.begin();
		for (; it != m_events.end(); ++it)
		{
			ret = (**it)(state, code) || ret;
		}
		return ret;
	}

	std::vector<KeyEvent> m_events;
	std::vector<int> m_downKeys;

	friend MApplaction;
};

#define g_mKeyboardListen MKeyboardListen::getInstance()
