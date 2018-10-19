#include "VayoWin32Device.h"

#ifdef _WIN32

#include "VayoRoot.h"
#include "VayoInput.h"
#include "VayoRenderSystem.h"

namespace
{
	// This is just used to forward Windows messages from a global window
	// procedure to our member function window procedure because we cannot
	// assign a member function to WNDCLASS::lpfnWndProc.
	Vayo::Win32Device* gWin32Device = NULL;
	Vayo::KeypadDispatcher* gKeypadDispatcher = NULL;
	Vayo::TouchDispatcher* gTouchDispatcher = NULL;
	HKL KEYBOARD_INPUT_HKL = 0;
	unsigned int KEYBOARD_INPUT_CODEPAGE = 1252;
}

NS_VAYO_BEGIN

// Get the codepage from the locale language id
// Based on the table from http://www.science.co.il/Language/Locale-Codes.asp?s=decimal
static unsigned int LocaleIdToCodepage(unsigned int lcid)
{
	switch (lcid)
	{
	case 1098:  // Telugu
	case 1095:  // Gujarati
	case 1094:  // Punjabi
	case 1103:  // Sanskrit
	case 1111:  // Konkani
	case 1114:  // Syriac
	case 1099:  // Kannada
	case 1102:  // Marathi
	case 1125:  // Divehi
	case 1067:  // Armenian
	case 1081:  // Hindi
	case 1079:  // Georgian
	case 1097:  // Tamil
		return 0;
	case 1054:  // Thai
		return 874;
	case 1041:  // Japanese
		return 932;
	case 2052:  // Chinese (PRC)
	case 4100:  // Chinese (Singapore)
		return 936;
	case 1042:  // Korean
		return 949;
	case 5124:  // Chinese (Macau S.A.R.)
	case 3076:  // Chinese (Hong Kong S.A.R.)
	case 1028:  // Chinese (Taiwan)
		return 950;
	case 1048:  // Romanian
	case 1060:  // Slovenian
	case 1038:  // Hungarian
	case 1051:  // Slovak
	case 1045:  // Polish
	case 1052:  // Albanian
	case 2074:  // Serbian (Latin)
	case 1050:  // Croatian
	case 1029:  // Czech
		return 1250;
	case 1104:  // Mongolian (Cyrillic)
	case 1071:  // FYRO Macedonian
	case 2115:  // Uzbek (Cyrillic)
	case 1058:  // Ukrainian
	case 2092:  // Azeri (Cyrillic)
	case 1092:  // Tatar
	case 1087:  // Kazakh
	case 1059:  // Belarusian
	case 1088:  // Kyrgyz (Cyrillic)
	case 1026:  // Bulgarian
	case 3098:  // Serbian (Cyrillic)
	case 1049:  // Russian
		return 1251;
	case 8201:  // English (Jamaica)
	case 3084:  // French (Canada)
	case 1036:  // French (France)
	case 5132:  // French (Luxembourg)
	case 5129:  // English (New Zealand)
	case 6153:  // English (Ireland)
	case 1043:  // Dutch (Netherlands)
	case 9225:  // English (Caribbean)
	case 4108:  // French (Switzerland)
	case 4105:  // English (Canada)
	case 1110:  // Galician
	case 10249:  // English (Belize)
	case 3079:  // German (Austria)
	case 6156:  // French (Monaco)
	case 12297:  // English (Zimbabwe)
	case 1069:  // Basque
	case 2067:  // Dutch (Belgium)
	case 2060:  // French (Belgium)
	case 1035:  // Finnish
	case 1080:  // Faroese
	case 1031:  // German (Germany)
	case 3081:  // English (Australia)
	case 1033:  // English (United States)
	case 2057:  // English (United Kingdom)
	case 1027:  // Catalan
	case 11273:  // English (Trinidad)
	case 7177:  // English (South Africa)
	case 1030:  // Danish
	case 13321:  // English (Philippines)
	case 15370:  // Spanish (Paraguay)
	case 9226:  // Spanish (Colombia)
	case 5130:  // Spanish (Costa Rica)
	case 7178:  // Spanish (Dominican Republic)
	case 12298:  // Spanish (Ecuador)
	case 17418:  // Spanish (El Salvador)
	case 4106:  // Spanish (Guatemala)
	case 18442:  // Spanish (Honduras)
	case 3082:  // Spanish (International Sort)
	case 13322:  // Spanish (Chile)
	case 19466:  // Spanish (Nicaragua)
	case 2058:  // Spanish (Mexico)
	case 10250:  // Spanish (Peru)
	case 20490:  // Spanish (Puerto Rico)
	case 1034:  // Spanish (Traditional Sort)
	case 14346:  // Spanish (Uruguay)
	case 8202:  // Spanish (Venezuela)
	case 1089:  // Swahili
	case 1053:  // Swedish
	case 2077:  // Swedish (Finland)
	case 5127:  // German (Liechtenstein)
	case 1078:  // Afrikaans
	case 6154:  // Spanish (Panama)
	case 4103:  // German (Luxembourg)
	case 16394:  // Spanish (Bolivia)
	case 2055:  // German (Switzerland)
	case 1039:  // Icelandic
	case 1057:  // Indonesian
	case 1040:  // Italian (Italy)
	case 2064:  // Italian (Switzerland)
	case 2068:  // Norwegian (Nynorsk)
	case 11274:  // Spanish (Argentina)
	case 1046:  // Portuguese (Brazil)
	case 1044:  // Norwegian (Bokmal)
	case 1086:  // Malay (Malaysia)
	case 2110:  // Malay (Brunei Darussalam)
	case 2070:  // Portuguese (Portugal)
		return 1252;
	case 1032:  // Greek
		return 1253;
	case 1091:  // Uzbek (Latin)
	case 1068:  // Azeri (Latin)
	case 1055:  // Turkish
		return 1254;
	case 1037:  // Hebrew
		return 1255;
	case 5121:  // Arabic (Algeria)
	case 15361:  // Arabic (Bahrain)
	case 9217:  // Arabic (Yemen)
	case 3073:  // Arabic (Egypt)
	case 2049:  // Arabic (Iraq)
	case 11265:  // Arabic (Jordan)
	case 13313:  // Arabic (Kuwait)
	case 12289:  // Arabic (Lebanon)
	case 4097:  // Arabic (Libya)
	case 6145:  // Arabic (Morocco)
	case 8193:  // Arabic (Oman)
	case 16385:  // Arabic (Qatar)
	case 1025:  // Arabic (Saudi Arabia)
	case 10241:  // Arabic (Syria)
	case 14337:  // Arabic (U.A.E.)
	case 1065:  // Farsi
	case 1056:  // Urdu
	case 7169:  // Arabic (Tunisia)
		return 1256;
	case 1061:  // Estonian
	case 1062:  // Latvian
	case 1063:  // Lithuanian
		return 1257;
	case 1066:  // Vietnamese
		return 1258;
	}
	return 65001;   // utf-8
}

LRESULT CALLBACK mainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Forward hwnd on because we can get messages (e.g., WM_CREATE)
	// before CreateWindow returns, and thus before mhMainWnd is valid.
	return gWin32Device->msgProc(hwnd, msg, wParam, lParam);
}

LRESULT Win32Device::msgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		// WM_ACTIVATE is sent when the window is activated or deactivated.
		// We pause the game when the window is deactivated and unpause it
		// when it becomes active.
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
			injectInactive();
		else
			injectActivate();
		InvalidateRect(_mainWnd, NULL, FALSE);
		return 0;

		// WM_SIZE is sent when the user resizes the window.
	case WM_SIZE:
		// Save the new client area dimensions.
		if (wParam == SIZE_MINIMIZED)
			injectSizeMinimized(LOWORD(lParam), HIWORD(lParam));
		else if (wParam == SIZE_MAXIMIZED)
			injectSizeMaximized(LOWORD(lParam), HIWORD(lParam));
		else if (wParam == SIZE_RESTORED)
			injectSizeRestored(LOWORD(lParam), HIWORD(lParam));
		return 0;

		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
		injectEnterSizeMove();
		return 0;

		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		injectExitSizeMove();
		return 0;

		// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
		onDestroy();
		return 0;

		// The WM_MENUCHAR message is sent when a menu is active and the user presses
		// a key that does not correspond to any mnemonic or accelerator key.
	case WM_MENUCHAR:
		// Don't beep when we alt-enter.
		return MAKELRESULT(0, MNC_CLOSE);

		// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;

	case WM_MOUSEWHEEL:
		injectMouseWheel((float)((short)HIWORD(wParam)) / (float)WHEEL_DELTA);
		InvalidateRect(_mainWnd, NULL, FALSE);
		return 0;
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		injectMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		InvalidateRect(_mainWnd, NULL, FALSE);
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		injectMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		InvalidateRect(_mainWnd, NULL, FALSE);
		return 0;
	case WM_MOUSEMOVE:
		injectMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		InvalidateRect(_mainWnd, NULL, FALSE);
		return 0;
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_KEYDOWN:
	case WM_KEYUP:
	{
		bool keyDown = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
		injectKeyboard(wParam, lParam, keyDown);
		InvalidateRect(_mainWnd, NULL, FALSE);

		if (msg == WM_SYSKEYDOWN || msg == WM_SYSKEYUP)
			return DefWindowProc(hwnd, msg, wParam, lParam);
		else
			return 0;
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		BeginPaint(_mainWnd, &ps);
		Root* pRoot = Root::getSingletonPtr();
		if (pRoot->getConfig().OwnerDraw && pRoot->IsLaunched())
			pRoot->renderOneFrame();
		EndPaint(_mainWnd, &ps);
		return 0;
	}
	case WM_INPUTLANGCHANGE:
		// get the new codepage used for keyboard input
		KEYBOARD_INPUT_HKL = GetKeyboardLayout(0);
		KEYBOARD_INPUT_CODEPAGE = LocaleIdToCodepage(LOWORD(KEYBOARD_INPUT_HKL));
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////

Win32Device::Win32Device()
	: _mainWnd(NULL)
	, _externalWindow(false)
{
	gWin32Device = this;
	gKeypadDispatcher = Root::getSingleton().getKeypadDispatcher();
	gTouchDispatcher = Root::getSingleton().getTouchDispatcher();
}

Win32Device::~Win32Device()
{
}

void* Win32Device::getMainWnd() const
{
	return _mainWnd;
}

bool Win32Device::init()
{
	if (Root::getSingleton().getConfig().WndHandle)
	{
		_mainWnd = static_cast<HWND>(Root::getSingleton().getConfig().WndHandle);
		RECT rc;
		GetClientRect(_mainWnd, &rc);
		setScreenSize(Dimension2di(rc.right - rc.left, rc.bottom - rc.top));
		const_cast<bool&>(Root::getSingleton().getConfig().FullScreen) = false;
		_externalWindow = true;
	}
	else
	{
		HINSTANCE hInst = GetModuleHandle(NULL);
		WNDCLASS wc;
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc = mainWndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInst;
		wc.hIcon = LoadIcon(0, IDI_APPLICATION);
		wc.hCursor = LoadCursor(0, IDC_ARROW);
		wc.hbrBackground = NULL;
		wc.lpszMenuName = 0;
		wc.lpszClassName = L"VayoEngine";

		RegisterClass(&wc);

		DWORD style = WS_POPUP;
		if (!Root::getSingleton().getConfig().FullScreen)
			style = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
		// Compute window rectangle dimensions based on requested client area dimensions.
		RECT R = { 0, 0, _screenSize._width, _screenSize._height };
		AdjustWindowRect(&R, style, FALSE);
		int realWidth = R.right - R.left;
		int realHeight = R.bottom - R.top;

		int windowLeft = (GetSystemMetrics(SM_CXSCREEN) - realWidth) / 2;
		int windowTop = (GetSystemMetrics(SM_CYSCREEN) - realHeight) / 2;

		if (windowLeft < 0)
			windowLeft = 0;
		if (windowTop < 0)
			windowTop = 0;

		if (Root::getSingleton().getConfig().FullScreen)
		{
			windowLeft = 0;
			windowTop = 0;
			realWidth = GetSystemMetrics(SM_CXSCREEN);
			realHeight = GetSystemMetrics(SM_CYSCREEN);
		}

		_mainWnd = CreateWindow(L"VayoEngine", _mainWndCaption.c_str(), style,
			windowLeft, windowTop, realWidth, realHeight, NULL, NULL, hInst, NULL);
		if (!_mainWnd)
		{
			MessageBox(0, L"CreateWindow Failed.", 0, 0);
			return false;
		}

		MoveWindow(_mainWnd, windowLeft, windowTop, realWidth, realHeight, TRUE);
		ShowWindow(_mainWnd, SW_SHOW);
		UpdateWindow(_mainWnd);
		const_cast<void*&>(Root::getSingleton().getConfig().WndHandle) = _mainWnd;
	}

	if (!_externalWindow)
	{
		SetActiveWindow(_mainWnd);
		SetForegroundWindow(_mainWnd);
	}

	KEYBOARD_INPUT_HKL = GetKeyboardLayout(0);
	KEYBOARD_INPUT_CODEPAGE = LocaleIdToCodepage(LOWORD(KEYBOARD_INPUT_HKL));

	return true;
}

bool Win32Device::handleEvents(bool& idle)
{
	MSG msg = { 0 };
	if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
			return false;

		if (_externalWindow && msg.hwnd == _mainWnd)
			msgProc(_mainWnd, msg.message, msg.wParam, msg.lParam);
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		idle = false;
	}
	else
		idle = true;

	return true;
}

void Win32Device::onSleep(unsigned int milliSeconds /*= 0*/)
{
	Sleep(milliSeconds);
}

void Win32Device::onDestroy()
{
	if (Root::getSingleton().getConfig().PostQuit)
		PostQuitMessage(0);
}

void Win32Device::setMainWndCaption(const wstring& wndCaption)
{
	if (wndCaption != _mainWndCaption)
	{
		Device::setMainWndCaption(wndCaption);
		if (_mainWnd)
			SetWindowText(_mainWnd, _mainWndCaption.c_str());
	}
}

void Win32Device::injectMouseDown(unsigned int btnState, int x, int y)
{
	_mouseIsDown = btnState;
	SetCapture(_mainWnd);
	if (_mouseIsDown.checkState(MK_LBUTTON))
		gTouchDispatcher->handleTouchBegan(x, y, EMK_LEFT);
	if (_mouseIsDown.checkState(MK_MBUTTON))
		gTouchDispatcher->handleTouchBegan(x, y, EMK_MIDDLE);
	if (_mouseIsDown.checkState(MK_RBUTTON))
		gTouchDispatcher->handleTouchBegan(x, y, EMK_RIGHT);
}

void Win32Device::injectMouseMove(unsigned int btnState, int x, int y)
{
	gTouchDispatcher->setTouchCurPos(x, y);
	if (_mouseIsDown.checkState(MK_LBUTTON))
		gTouchDispatcher->handleTouchMoved(x, y, EMK_LEFT);
	if (_mouseIsDown.checkState(MK_MBUTTON))
		gTouchDispatcher->handleTouchMoved(x, y, EMK_MIDDLE);
	if (_mouseIsDown.checkState(MK_RBUTTON))
		gTouchDispatcher->handleTouchMoved(x, y, EMK_RIGHT);
}

void Win32Device::injectMouseUp(unsigned int btnState, int x, int y)
{
	BitState mouseIsUp = btnState;
	if (_mouseIsDown.checkState(MK_LBUTTON) && !mouseIsUp.checkState(MK_LBUTTON))
	{
		_mouseIsDown.eraseState(MK_LBUTTON);
		gTouchDispatcher->handleTouchEnded(x, y, EMK_LEFT);
	}
	if (_mouseIsDown.checkState(MK_MBUTTON) && !mouseIsUp.checkState(MK_MBUTTON))
	{
		_mouseIsDown.eraseState(MK_MBUTTON);
		gTouchDispatcher->handleTouchEnded(x, y, EMK_MIDDLE);
	}
	if (_mouseIsDown.checkState(MK_RBUTTON) && !mouseIsUp.checkState(MK_RBUTTON))
	{
		_mouseIsDown.eraseState(MK_RBUTTON);
		gTouchDispatcher->handleTouchEnded(x, y, EMK_RIGHT);
	}
	ReleaseCapture();
}

void Win32Device::injectKeyboard(unsigned int keyCode, unsigned int scanCode, bool keyDown)
{
	tagKeyInput evt;
	BYTE allKeys[256];

	evt.Key = (EKeyCode)keyCode;
	evt.PressedDown = keyDown;

	const UINT MY_MAPVK_VSC_TO_VK_EX = 3; // MAPVK_VSC_TO_VK_EX should be in SDK according to MSDN, but isn't in mine.
	if (evt.Key == KEY_SHIFT)
	{
		// this will fail on systems before windows NT/2000/XP, not sure _what_ will return there instead.
		evt.Key = (EKeyCode)MapVirtualKey(((scanCode >> 16) & 255), MY_MAPVK_VSC_TO_VK_EX);
	}
	if (evt.Key == KEY_CONTROL)
	{
		evt.Key = (EKeyCode)MapVirtualKey(((scanCode >> 16) & 255), MY_MAPVK_VSC_TO_VK_EX);
		// some keyboards will just return LEFT for both - left and right keys. So also check extend bit.
		if (scanCode & 0x1000000)
			evt.Key = KEY_RCONTROL;
	}
	if (evt.Key == KEY_MENU)
	{
		evt.Key = (EKeyCode)MapVirtualKey(((scanCode >> 16) & 255), MY_MAPVK_VSC_TO_VK_EX);
		if (scanCode & 0x1000000)
			evt.Key = KEY_RMENU;
	}

	GetKeyboardState(allKeys);

	evt.Shift = ((allKeys[VK_SHIFT] & 0x80) != 0);
	evt.Control = ((allKeys[VK_CONTROL] & 0x80) != 0);

	// Handle unicode and deadkeys in a way that works since Windows 95 and nt4.0
	// Using ToUnicode instead would be shorter, but would to my knowledge not run on 95 and 98.
	WORD keyChars[2];
	int conversionResult = ToAsciiEx(keyCode, HIWORD(scanCode), allKeys, keyChars, 0, KEYBOARD_INPUT_HKL);
	if (conversionResult == 1)
	{
		WORD unicodeChar;
		MultiByteToWideChar(
			KEYBOARD_INPUT_CODEPAGE,
			MB_PRECOMPOSED, // default
			(LPCSTR)keyChars,
			sizeof(keyChars),
			(WCHAR*)&unicodeChar,
			1);
		evt.Char = unicodeChar;
	}
	else
		evt.Char = 0;

	// allow composing characters like '@' with Alt Gr on non-US keyboards
	if ((allKeys[VK_MENU] & 0x80) != 0)
		evt.Control = 0;

	gKeypadDispatcher->handleKeyClicked(evt);
}

void Win32Device::injectActivate()
{
	Device::injectActivate();
	RenderSystem* pRenderer = Root::getSingleton().getActiveRenderer();
	if (pRenderer)
		pRenderer->restoreContext();
}

Device* Device::create()
{
	Device* pDevice = NULL;
	// create win32 device.
	pDevice = new Win32Device();
	pDevice->setMainWndCaption(Root::getSingleton().getConfig().WindowName);
	pDevice->setScreenSize(Root::getSingleton().getConfig().WindowSize);
	return pDevice;
}

NS_VAYO_END

#endif // _WIN32
