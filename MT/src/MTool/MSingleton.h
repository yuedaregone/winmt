#ifndef __MSINGLETON_H__
#define __MSINGLETON_H__
#include <stdio.h>
template<class T>
class MSingleton
{
public:
	static T* getInstance()
	{
		if (!m_pInstance)
		{
			m_pInstance = new T();
		}
		return m_pInstance;
	}

	static void destroyInstance()
	{
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = NULL;
		}
	}
protected:
	static T* m_pInstance;
protected:
	MSingleton() {};
	virtual ~MSingleton() {};
};

template<class T>
T* MSingleton<T>::m_pInstance = NULL;

#endif