#include <Windows.h>
#include "MyApp.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	EnableMemLeakCheck();
	Root::Config configData;
	configData.MainDeviceAttrib.ScreenSize = Dimension2di(1280, 720);
	configData.MainDeviceAttrib.WndCaption = L"Ä£ÐÍ²âÊÔ";
	configData.RendererName = L"RenderSystem_GL";
	configData.MainDeviceAttrib.BgClearColor = 0xff0f0f0f;
	ModelViewer viewer;
	viewer.init<Root>(&configData);
	viewer.run();
	return 0;
}
