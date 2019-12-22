#pragma once
#include "MLayer.h"

class MGLLayer : public MLayer
{
public:
	MGLLayer(HINSTANCE instance, RECT rect, const char* name) : MLayer(instance, rect, name) {}

	virtual HRESULT Create();

protected:
	virtual void UpdateLayer();

private:
	BOOL CreateHGLRC();

	void InitGL();

	void ResizeGL();

	BOOL InitPBuffer();

	virtual void OnDestroy();
private:
	HGLRC m_glRC;

	HDC m_pbufferDC;
	HGLRC m_pbufferGLRC;
};