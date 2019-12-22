#include "MNetDef.h"
#include "MNetClient.h"
#include "MWindow.h"
#include "MLayer.h"
#include "MApplication.h"
#include "console.h"
#include "MLuaEngine.h"
#include "emt_hp.h"
#include "MTool.h"
#include "MHttpClient.h"
#include "MHttpRequest.h"
#include "MSprite.h"
#include "MGLLayer.h"
#include "MLabel.h"

#include "time.h"
#include <GL/glew.h>

void SetAutoStart()
{
	HKEY key;
	LSTATUS open = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &key);
	char buff[512] = { 0 };
	GetModuleFileName(NULL, buff, 512);
	LSTATUS state = RegSetValueEx(key, TEXT("MT"), 0, REG_SZ, (CONST BYTE *)buff, strlen(buff));
	RegCloseKey(key);	
}

void SetEnv()
{
	char buff[512] = { 0 };
	GetModuleFileName(NULL, buff, 512);

	size_t sz = strlen(buff);
	for (int i = sz - 1; i >= 0; --i)
	{
		if (buff[i] == '\\')
		{
			buff[i] = '\0';
			break;
		}
	}
	SetCurrentDirectory(buff);
}

#define CONSOLE_DEBUG
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);	

	
	//Img_emt_hp i;
	
	//MLayer* l = new MLayer(hInstance, GetRightBottomRect(300, 300), "icon");
	//l->Create();
	//l->DrawImageRGBA(&i, 0, 0);
	//l->UpdateLayer();

	//MSprite* sp = MSprite::CreateBuiltinSprite("Img_emt_hp");

	//l->AddDrawInst(sp);	
	
#ifdef CONSOLE_DEBUG
	start_console();
#endif	
	//SetAutoStart();
	SetEnv();	

	MApplaction app(hInstance);
	app.SetInterval(0.033f);
	app.SetWaitMessageMode(false);
	
	MLayer* l = new MGLLayer(hInstance, GetCenterRect(300, 300), "icon");
	l->Create();
	app.Add(l);
	l->Show(0);
	
	

	g_mLuaEngine->Init();

	//app.Add(l);

	//g_mLuaEngine->Init();

	/*RECT rt; rt.left = 100; rt.right = 400; rt.top = 100; rt.bottom = 400;
	MWindow* mw = new MLayer(hInstance, rt, "MyTest");
	mw->Create();
	app.Add(mw);
	mw->Show(nCmdShow);*/

	//MHttpClient client;
	//client.CreateRequest("https://cn.bing.com/dict/");
	//client.Request();

	//MHttpRequest* req = new MHttpRequest("http://job.xdnice.com/upload/2012-11/1211261702212616.jpg");
	//req->SetHead("Accept:text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8");
	//req->SetHead("Accept-Encoding:gzip, deflate");
	//req->SetHead("Accept-Language:zh-CN,zh;q=0.9,en;q=0.8");
	//req->SetHead("Cache-Control:max-age=0");
	//req->SetHead("Connection:keep-alive");
	//req->SetHead("Host:job.xdnice.com");
	//req->SetHead("If-Modified-Since:Mon, 26 Nov 2012 09:02:06 GMT");
	//req->SetHead("If-None-Match:\"929b72b5b4cbcd1:0\"");
	//req->SetHead("User-Agent:Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/62.0.3202.75 Safari/537.36");
	//req->SetHead("");

	//MHttpRequest* req1 = new MHttpRequest("https://cn.bing.com/az/hprichbg/rb/MGRBerlin_ZH-CN6734108494_1920x1080.jpg");
	//req1->SetHead("Connection:keep-alive");
	//req1->SetHead("Host:cn.bing.com");
	//req1->SetHead("Host:pagead2.googlesyndication.com");
	//req1->SetHead("User-Agent:Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/62.0.3202.75 Safari/537.36");

	//client.CreateRequest("https://cn.bing.com/az/hprichbg/rb/MGRBerlin_ZH-CN6734108494_1920x1080.jpg");
	//client.Request();
	//while (true)
	///{
	//	client.Update();
	//	Sleep(10);
	//}

	/*
	MNetClient client;
	client.Init();
	client.Connet("127.0.0.1", 60320);
	//app.Run();
	long last = clock();
	while (true)
	{
		if (clock() - last > 1200)
		{
			const char* s = "wqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqw";
			client.SendMsg((const uint8_t*)s, strlen(s));
		}

		client.Update();
		Sleep(1);
	}
	*/
	app.Run();
#ifdef CONSOLE_DEBUG
	stop_console();
#endif
	
	return 0;
}
