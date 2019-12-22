#include "MGLLayer.h"
#include <GL/glew.h>
#include <GL/wglew.h>
#include "MTime.h"
#include "shader.h"
#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "stb_image.h"
#include "MTool.h"

const char* verStr = "\
			#version 120													\n\
			attribute vec3 vertexPosition_modelspace;						\n\
			attribute vec4 vertextColor;									\n\
			attribute vec2 uv;												\n\
			varying vec4 vColor;											\n\
			varying vec2 fragCoord;											\n\
			uniform mat4 MVP;												\n\
			void main(){													\n\
				gl_Position = MVP*vec4(vertexPosition_modelspace, 1.0);		\n\
				fragCoord =	uv;												\n\
				fragCoord.y = 1 - fragCoord.y; \n\
				vColor = vertextColor;										\n\
			}																\n\
		";

const char* fragStr1 = "\n\
			#version 120											\n\
			varying vec4 vColor;									\n\
			varying vec2 fragCoord;											\n\
			uniform float iTime;							\n\
			uniform sampler2D myTextureSampler;						\n\
			float noise(vec3 p)																												\n\
			{																												                \n\
				vec3 i = floor(p);																											\n\
				vec4 a = dot(i, vec3(1., 57., 21.)) + vec4(0., 57., 21., 78.);																\n\
				vec3 f = cos((p - i)*acos(-1.))*(-.5) + .5;																					\n\
				a = mix(sin(cos(a)*a), sin(cos(1. + a)*(1. + a)), f.x);																		\n\
				a.xy = mix(a.xz, a.yw, f.y);																								\n\
				return mix(a.x, a.y, f.z);																									\n\
			}																												                \n\
			float sphere(vec3 p, vec4 spr)																									\n\
			{																												                \n\
				return length(spr.xyz - p) - spr.w;																							\n\
			}																												                \n\
			float flame(vec3 p)																												\n\
			{																												                \n\
				float d = sphere(p*vec3(1., .5, 1.), vec4(.0, -1., .0, 1.));																\n\
				return d + (noise(p + vec3(.0, iTime*2., .0)) + noise(p*3.)*.5)*.25*(p.y);													\n\
			}																												                \n\
			float scene(vec3 p)																												\n\
			{																												                \n\
				return min(100. - length(p), abs(flame(p)));																				\n\
			}																												                \n\
			vec4 raymarch(vec3 org, vec3 dir)																								\n\
			{																												                \n\
				float d = 0.0, glow = 0.0, eps = 0.02;																						\n\
				vec3  p = org;																												\n\
				bool glowed = false;																										\n\
																																			\n\
				for (int i = 0; i < 64; i++)																								\n\
				{																												            \n\
					d = scene(p) + eps;																										\n\
					p += d * dir;																											\n\
					if (d > eps)																											\n\
					{																												        \n\
						if (flame(p) < .0)																									\n\
							glowed = true;																									\n\
						if (glowed)																											\n\
							glow = float(i) / 64.;																							\n\
					}																												        \n\
				}																												            \n\
				return vec4(p, glow);																										\n\
			}																												                \n\
			void main()																														\n\
			{																												                \n\
				vec2 v = fragCoord;																		\n\
				v.y -= 0.8;									                                                    \n\
				vec3 org = vec3(0., -2., 4.);																								\n\
				vec3 dir = normalize(vec3(v.x*1.6, -v.y, -1.5));																			\n\
				vec4 p = raymarch(org, dir);																								\n\
				float glow = p.w;							                                                                                \n\
				vec4 col = mix(vec4(1., .5, .1, 1.), vec4(0.1, .5, 1., 1.), p.y*.02 + .4);				                                    \n\
				gl_FragColor = mix(vec4(0.), col, pow(glow*2., 4.));													                        \n\
			}";

const char* fragStr = "\
			#version 120											\n\
			varying vec4 vColor;									\n\
			varying vec2 fragCoord;											\n\
			uniform float iTime;							\n\
			uniform sampler2D myTextureSampler;						\n\
			void main(){											\n\
				vec2 p = fragCoord;  \n\
				//p.y = 1 - p.y;	\n\
				p.y -= 1.25;																		\n\
				vec2 p1 = p;			\n\
				// background color																	\n\
				vec3 bcol = vec3(1.0, 0.8, 0.7 - 0.07*p.y)*(1.0 - 0.25*length(p));					\n\
				// animate																			\n\
				float tt = mod(iTime, 1.5) / 1.5;													\n\
				float ss = pow(tt, .2)*0.5 + 0.5;													\n\
				ss = 1.0 + ss * 0.5*sin(tt*6.2831*3.0 + p.y*0.5)*exp(-tt * 4.0);					\n\
				p *= vec2(0.5, 1.5) + ss * vec2(0.5, -0.5);												\n\
				// shape																					\n\
				float a = atan(p.x, p.y) / 3.141593;																			\n\
				float r = length(p);																\n\
				float h = abs(a);																				\n\
				float d = (13.0*h - 22.0*h*h + 10.0*h*h*h) / (6.0 - 5.0*h);							\n\
				// color																				\n\
				float s = 1.0 - 0.5*clamp(r / d, 0.0, 1.0);												\n\
				s = 0.75 + 0.75*p.x;																				\n\
				s *= 1.0 - 0.25*r;																		\n\
				s = 0.5 + 0.6*s;																					\n\
				s *= 0.5 + 0.5*pow(1.0 - clamp(r / d, 0.0, 1.0), 0.1);												\n\
				vec3 hcol = vec3(1.0, 0.5*r, 0.3)*s;												\n\
				vec3 col = mix(bcol, hcol, smoothstep(-0.01, 0.01, d - r));										\n\
				p1.y += 0.15; \n\
				float ba = 1 - length(p1) * 1.4;																		\n\
				float ha = 1.0;																		\n\
				float fa = mix(ba, ha, smoothstep(-0.01, 0.01, d - r));									\n\
				gl_FragColor = vec4(col * fa, fa);								\n\
			}														\n\
		";
const char* startStr = "\
			#version 120											\n\
			varying vec4 vColor;									\n\
			varying vec2 fragCoord;									\n\
			uniform vec2 iResolution;								\n\
			uniform vec2 iMouse;								\n\
			uniform float iTime; \n";
const char* endStr = "\
		void main() {												\n\
			vec2 p = fragCoord;										\n\
			p = p * iResolution;									\n\
			vec4 fragColor = vec4(fragCoord.x,fragCoord.y,0,1);		\n\
			mainImage(fragColor, p);								\n\
			gl_FragColor = fragColor;								\n\
		}															\n\
		";
			

void init();

HRESULT MGLLayer::Create()
{
	if (FAILED(Init()))
	{
		return -1;
	}

	UINT width = GetWidth();
	UINT height = GetHeight();

	DWORD style = WS_EX_LAYERED | WS_EX_TOOLWINDOW;
	m_hwnd = CreateWindowEx(style, m_name.c_str(), m_name.c_str(), WS_VISIBLE | WS_SYSMENU,
		m_rect.left, m_rect.top, width, height, NULL, NULL, m_instance, NULL);

	if (m_hwnd == NULL)
		return -1;	

	m_backBuff = (UINT*)malloc(width * height * sizeof(UINT));
	memset(m_backBuff, 0, width * height * sizeof(UINT));

	m_wndDc = ::GetDC(m_hwnd);

	
	m_layerDc = ::CreateCompatibleDC(m_wndDc);
	m_bitMap = ::CreateCompatibleBitmap(m_wndDc, width, height);

	m_bmpInfo = (SBmpInfo*)malloc(sizeof(SBmpInfo));
	BITMAPINFO& bmpInfo = m_bmpInfo->m_BitmapInfo;
	bmpInfo.bmiHeader.biSize = sizeof(bmpInfo.bmiHeader);
	bmpInfo.bmiHeader.biWidth = width;
	bmpInfo.bmiHeader.biHeight = -(int)height;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_BITFIELDS;

	*(UINT*)(bmpInfo.bmiColors + 0) = 0xFF0000;
	*(UINT*)(bmpInfo.bmiColors + 1) = 0x00FF00;
	*(UINT*)(bmpInfo.bmiColors + 2) = 0x0000FF;

	::SelectObject(m_layerDc, m_bitMap);
	
	if (!CreateHGLRC()) return -1;	

	InitGL();

	ResizeGL();

	init();

	return S_OK;
}

std::string GetShaderInFile()
{
	std::string path = GetCurrentPath();
	FILE* fp = fopen((path + "\\shader.frag").c_str(), "r");
	if (fp == NULL)
		return fragStr1;

	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char* buf = new char[size + 1];
	memset(buf, 0, size);
	fread(buf, 1, size, fp);

	std::string shader = buf;

	delete[] buf;
	return shader;
}

GLuint vertex = 0;
GLuint color = 0;
GLuint uv = 0;
GLuint shader = 0;
GLuint uniformMVP = 0;
GLuint vertexColorID = 0;
GLuint vertexPosID = 0;
GLuint uvID = 0;
GLuint uniformTimeID = 0;
GLuint resolutionID = 0;

glm::mat4 mvpMatrix;

void init()
{
	glGenBuffers(1, &vertex);
	glBindBuffer(GL_ARRAY_BUFFER, vertex);
	static float v_p[18] = 
	{
		1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,

		1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(v_p), v_p, GL_STATIC_DRAW);
		
	glGenBuffers(1, &uv);
	glBindBuffer(GL_ARRAY_BUFFER, uv);
	static float v_uv[12] =
	{
		1.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,

		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(v_uv), v_uv, GL_STATIC_DRAW);

	glGenBuffers(1, &color);
	glBindBuffer(GL_ARRAY_BUFFER, color);
	static float v_color[24] =
	{
		1.0f, 0.0f, 0.0f, 0.2f,
		0.0f, 1.0f, 0.0f, 0.4f,
		0.0f, 0.0f, 1.0f, 0.6f,

		1.0f, 0.0f, 0.0f, 0.2f,
		0.0f, 1.0f, 0.0f, 0.4f,
		0.0f, 0.0f, 1.0f, 0.6f
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(v_color), v_color, GL_STATIC_DRAW);

	std::string fragShader = GetShaderInFile();
	//std::string fragShader = startStr;
	//fragShader += GetShaderInFile();
	//fragShader += endStr;

	shader = loadShaders(verStr, fragShader.c_str());

	uniformMVP = glGetUniformLocation(shader, "MVP");
	vertexPosID = glGetAttribLocation(shader, "vertexPosition_modelspace");
	uvID = glGetAttribLocation(shader, "uv");
	vertexColorID = glGetAttribLocation(shader, "vertextColor");	
	uniformTimeID = glGetUniformLocation(shader, "iTime");
	resolutionID = glGetUniformLocation(shader, "iResolution");

	glm::mat4 projection = glm::perspective(45.0f, 1.0f / 1.0f, 0.1f, 100.0f);
	glm::mat4 view = glm::lookAt(
		glm::vec3(3, 0, -5), // Camera is at (4,3,3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);
	mvpMatrix = glm::mat4(1);//projection * view;
}

void rd(UINT w, UINT h)
{
	glUseProgram(shader);
	glUniformMatrix4fv(uniformMVP, 1, GL_FALSE, &mvpMatrix[0][0]);
	glUniform1f(uniformTimeID, MTime::time());
	glUniform2f(resolutionID, (GLfloat)w, (GLfloat)h);

	glEnableVertexAttribArray(vertexPosID);
	glBindBuffer(GL_ARRAY_BUFFER, vertex);
	glVertexAttribPointer(
		vertexPosID,		// attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,					// size
		GL_FLOAT,			// type
		GL_FALSE,			// normalized?
		0,					// stride
		(void*)0);			// array buffer offset

	glEnableVertexAttribArray(uvID);
	glBindBuffer(GL_ARRAY_BUFFER, uv);
	glVertexAttribPointer(uvID, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(vertexColorID);
	glBindBuffer(GL_ARRAY_BUFFER, color);
	glVertexAttribPointer(vertexColorID, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glDrawArrays(GL_TRIANGLES, 0, 6); // Starting from vertex 1; 3 vertices total -&gt; 1 triangle
	glDisableVertexAttribArray(vertexPosID);
	glDisableVertexAttribArray(vertexColorID);
}

void MGLLayer::UpdateLayer()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	UINT width = GetWidth();
	UINT height = GetHeight();

	rd(width, height);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(0, 0, width, height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, m_backBuff);

	::SetDIBitsToDevice(m_layerDc,
		0, 0, width, height,
		0, 0, 0, height,
		m_backBuff, &m_bmpInfo->m_BitmapInfo, DIB_RGB_COLORS);
		
	//SwapBuffers(m_layerDc);
	//wglSwapLayerBuffers(m_layerDc, WGL_SWAP_OVERLAY1);

	BLENDFUNCTION blend;
	blend.BlendOp = AC_SRC_OVER;
	blend.BlendFlags = 0;
	blend.AlphaFormat = 1;
	blend.SourceConstantAlpha = 255;

	RECT rect;
	GetWindowRect(m_hwnd, &rect);

	POINT p = { rect.left, rect.top };
	SIZE s = { (LONG)width, (LONG)height };

	POINT m_originPos = { 0, 0 };
	::UpdateLayeredWindow(m_hwnd, m_wndDc, &p, &s, m_layerDc, &m_originPos, 0, &blend, ULW_ALPHA);

}

BOOL MGLLayer::InitPBuffer()
{
	int attribList[] =
	{
		WGL_DRAW_TO_PBUFFER_ARB, TRUE,      // allow rendering to the pbuffer
		WGL_SUPPORT_OPENGL_ARB,  TRUE,      // associate with OpenGL
		WGL_DOUBLE_BUFFER_ARB,   FALSE,     // single buffered
		WGL_RED_BITS_ARB,   8,              // minimum 8-bits for red channel
		WGL_GREEN_BITS_ARB, 8,              // minimum 8-bits for green channel
		WGL_BLUE_BITS_ARB, 8,              // minimum 8-bits for blue channel
		WGL_ALPHA_BITS_ARB, 8,              // minimum 8-bits for alpha channel
		WGL_DEPTH_BITS_ARB, 16,             // minimum 16-bits for depth buffer
		0
	};

	int format = 0;
	UINT matchingFormats = 0;

	if (!wglChoosePixelFormatARB(m_wndDc, attribList, 0, 1, &format, &matchingFormats))
	{
		return FALSE;
	}

	HPBUFFERARB hpBuffer = wglCreatePbufferARB(m_wndDc, format, GetWidth(), GetHeight(), 0);
	if (!hpBuffer)
	{
		return FALSE;
	}

	m_pbufferDC = wglGetPbufferDCARB(hpBuffer);
	if (!m_pbufferDC)
	{
		return FALSE;
	}

	m_pbufferGLRC = wglCreateContext(m_pbufferDC);
	if (!m_pbufferGLRC)
	{
		return FALSE;
	}

	int w, h, channel;
	uint8_t* img = stbi_load(".\\timg.jpg", &w, &h, &channel, 0);
	return true;
}

BOOL MGLLayer::CreateHGLRC()
{
	DWORD dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;// | PFD_DRAW_TO_BITMAP ;
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = dwFlags;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 32;
	pfd.iLayerType = PFD_MAIN_PLANE;
	int pixelFormat = ChoosePixelFormat(m_wndDc, &pfd);
	if (pixelFormat == 0) {		
		return FALSE;
	}
	BOOL bResult = SetPixelFormat(m_wndDc, pixelFormat, &pfd);
	if (bResult == FALSE) {
		return FALSE;
	}
	m_glRC = wglCreateContext(m_wndDc);
	if (!m_glRC) {
		return FALSE;
	}
	if (FALSE == wglMakeCurrent(m_wndDc, m_glRC))
		return FALSE;
	
	//glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) { //init glew
		fprintf(stderr, "Failed to init glew!");
		return FALSE;
	}	

	if (!InitPBuffer())
		return FALSE;

	wglMakeCurrent(m_wndDc, 0);
	//ReleaseDC(m_hwnd, m_wndDc);
	//m_wndDc = NULL;

	wglMakeCurrent(m_pbufferDC, m_pbufferGLRC);

	return TRUE;
}

void MGLLayer::InitGL()
{
	
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0, 0, 0, 0.5);
}

void MGLLayer::ResizeGL()
{
	int width = (int)GetWidth();
	int height = (int)GetHeight();

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void MGLLayer::OnDestroy()
{
	wglMakeCurrent(NULL, NULL);
	if (m_glRC) wglDeleteContext(m_glRC);

	MLayer::OnDestroy();
}
