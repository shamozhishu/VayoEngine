#include "VayoWin32Device.h"

#ifdef _WIN32

#include "VayoCore.h"
#include "VayoUtils.h"
#include <WindowsX.h>

namespace
{
	// This is just used to forward Windows messages from a global window
	// procedure to our member function window procedure because we cannot
	// assign a member function to WNDCLASS::lpfnWndProc.
	HKL KEYBOARD_INPUT_HKL = 0;
	unsigned int KEYBOARD_INPUT_CODEPAGE = 1252;

	struct SEnvMapper
	{
		HWND hWnd;
		Vayo::Win32Device* win32Dev;
	};
	list<SEnvMapper> g_envMapper;
}

NS_VAYO_BEGIN

static KeypadDispatcher* s_keypadDispatcher = NULL;

static SEnvMapper* getEnvMapperFromHWnd(HWND hWnd)
{
	list<SEnvMapper>::iterator it = g_envMapper.begin();
	for (; it != g_envMapper.end(); ++it)
		if ((*it).hWnd == hWnd)
			return &(*it);
	return NULL;
}


static Win32Device* getDeviceFromHWnd(HWND hWnd)
{
	list<SEnvMapper>::iterator it = g_envMapper.begin();
	for (; it != g_envMapper.end(); ++it)
		if ((*it).hWnd == hWnd)
			return (*it).win32Dev;
	return NULL;
}

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

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Forward hwnd on because we can get messages (e.g., WM_CREATE)
	// before CreateWindow returns, and thus before mhMainWnd is valid.
	Win32Device* dev = getDeviceFromHWnd(hwnd);
	if (dev)
		return dev->windowProc(hwnd, msg, wParam, lParam);
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT Win32Device::windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
		injectDestroy();
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
		return 0;
	case WM_LBUTTONDOWN:
		injectMouseDown(EMK_LEFT, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_RBUTTONDOWN:
		injectMouseDown(EMK_RIGHT, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MBUTTONDOWN:
		injectMouseDown(EMK_MIDDLE, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONUP:
		injectMouseUp(EMK_LEFT, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_RBUTTONUP:
		injectMouseUp(EMK_RIGHT, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MBUTTONUP:
		injectMouseUp(EMK_MIDDLE, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		injectMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_KEYDOWN:
	case WM_KEYUP:
	{
		bool keyDown = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
		injectKeyboard(wParam, lParam, keyDown);
		if (msg == WM_SYSKEYDOWN || msg == WM_SYSKEYUP)
			return DefWindowProc(hwnd, msg, wParam, lParam);
		else
			return 0;
	}
	break;
	case WM_PAINT:
		injectPaint();
		return 0;
	case WM_INPUTLANGCHANGE:
		injectInputLanguageChange();
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////
Win32Device::Win32Device(EDeviceID deviceID, const Attrib& attrib)
	: Device(deviceID, attrib)
	, _wndHandle(NULL)
	, _externalWindow(false)
{
	s_keypadDispatcher = KeypadDispatcher::getSingletonPtr();
}

Win32Device::~Win32Device()
{
	if (!_externalWindow && _wndHandle)
		UnregisterClass(getDeviceCode().c_str(), GetModuleHandle(NULL));

	list<SEnvMapper>::iterator it = g_envMapper.begin();
	for (; it != g_envMapper.end(); ++it)
	{
		if ((*it).hWnd == _wndHandle)
		{
			g_envMapper.erase(it);
			break;
		}
	}
}

bool Win32Device::init()
{
	if (_attribute.WndHandle)
	{
		_wndHandle = reinterpret_cast<HWND>(_attribute.WndHandle);
		RECT rc;
		GetClientRect(_wndHandle, &rc);
		setScreenSize(Dimension2di(rc.right - rc.left, rc.bottom - rc.top));
		_attribute.FullScreen = false;
		_externalWindow = true;
	}
	else
	{
		wstring strClassName = getDeviceCode();

		HINSTANCE hInst = GetModuleHandle(NULL);
		WNDCLASS wc;
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc = WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInst;
		wc.hIcon = LoadIcon(0, IDI_APPLICATION);
		wc.hCursor = LoadCursor(0, IDC_ARROW);
		wc.hbrBackground = NULL;
		wc.lpszMenuName = 0;
		wc.lpszClassName = strClassName.c_str();

		if (!RegisterClass(&wc))
		{
			printLastError(L"×¢²á´°¿ÚÊ§°Ü!");
			return false;
		}

		DWORD style = WS_POPUP;
		if (!_attribute.FullScreen)
			style = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
		// Compute window rectangle dimensions based on requested client area dimensions.
		RECT R = { 0, 0, _attribute.ScreenSize._width, _attribute.ScreenSize._height };
		AdjustWindowRect(&R, style, FALSE);
		int realWidth = R.right - R.left;
		int realHeight = R.bottom - R.top;

		int windowLeft = (GetSystemMetrics(SM_CXSCREEN) - realWidth) / 2;
		int windowTop = (GetSystemMetrics(SM_CYSCREEN) - realHeight) / 2;

		if (windowLeft < 0)
			windowLeft = 0;
		if (windowTop < 0)
			windowTop = 0;

		if (_attribute.FullScreen)
		{
			windowLeft = 0;
			windowTop = 0;
			realWidth = GetSystemMetrics(SM_CXSCREEN);
			realHeight = GetSystemMetrics(SM_CYSCREEN);
		}

		_wndHandle = CreateWindow(strClassName.c_str(), _attribute.WndCaption.c_str(), style,
			windowLeft, windowTop, realWidth, realHeight, NULL, NULL, hInst, NULL);
		if (!_wndHandle)
		{
			UnregisterClass(strClassName.c_str(), hInst);
			printLastError(L"CreateWindow Failed!");
			return false;
		}

		MoveWindow(_wndHandle, windowLeft, windowTop, realWidth, realHeight, TRUE);
		ShowWindow(_wndHandle, SW_SHOW);
		UpdateWindow(_wndHandle);
		SetActiveWindow(_wndHandle);
		SetForegroundWindow(_wndHandle);
		_attribute.WndHandle = _wndHandle;
	}

	KEYBOARD_INPUT_HKL = GetKeyboardLayout(0);
	KEYBOARD_INPUT_CODEPAGE = LocaleIdToCodepage(LOWORD(KEYBOARD_INPUT_HKL));
	SetWindowText(_wndHandle, _attribute.WndCaption.c_str());

	SEnvMapper em;
	em.win32Dev = this;
	em.hWnd = _wndHandle;
	g_envMapper.push_back(em);

	return true;
}

void* Win32Device::getWndHandle() const
{
	return _wndHandle;
}

bool Win32Device::handleEvents(bool& idle)
{
	MSG msg = { 0 };
	if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
			return false;

		if (_externalWindow && msg.hwnd == _wndHandle)
			windowProc(msg.hwnd, msg.message, msg.wParam, msg.lParam);
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

void Win32Device::sleep(unsigned int milliSeconds, bool pauseTimer)
{
	Timer& timer = Core::getCore().getTimer();
	const bool wasStopped = timer.isStopped();
	if (pauseTimer && !wasStopped)
		timer.stop();

	Sleep(milliSeconds);

	if (pauseTimer && !wasStopped)
		timer.start();
}

void Win32Device::setWndCaption(const wstring& wndCaption)
{
	if (wndCaption != _attribute.WndCaption)
	{
		Device::setWndCaption(wndCaption);
		if (_wndHandle)
			SetWindowText(_wndHandle, _attribute.WndCaption.c_str());
	}
}

void Win32Device::injectMouseDown(EMouseKeys mouseKey, int x, int y)
{
	SetCapture(_wndHandle);
	Device::injectMouseDown(mouseKey, x, y);
	if (_attribute.WndPaint)
		InvalidateRect(_wndHandle, NULL, FALSE);
}

void Win32Device::injectMouseUp(EMouseKeys mouseKey, int x, int y)
{
	Device::injectMouseUp(mouseKey, x, y);
	ReleaseCapture();
	if (_attribute.WndPaint)
		InvalidateRect(_wndHandle, NULL, FALSE);
}

void Win32Device::injectMouseMove(int x, int y)
{
	Device::injectMouseMove(x, y);
	if (_attribute.WndPaint)
		InvalidateRect(_wndHandle, NULL, FALSE);
}

void Win32Device::injectMouseWheel(float wheel)
{
	Device::injectMouseWheel(wheel);
	if (_attribute.WndPaint)
		InvalidateRect(_wndHandle, NULL, FALSE);
}

void Win32Device::injectKeyboard(unsigned int keyCode, unsigned int scanCode, bool keyDown)
{
	Device::injectKeyboard(keyCode, scanCode, keyDown);
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

	s_keypadDispatcher->handleKeyClicked(evt);
	if (_attribute.WndPaint)
		InvalidateRect(_wndHandle, NULL, FALSE);
}

void Win32Device::injectPaint()
{
	Device::injectPaint();
	PAINTSTRUCT ps;
	BeginPaint(_wndHandle, &ps);
	Core& core = Core::getCore();
	if (_attribute.WndPaint && core.isLaunched())
		core.renderOneFrame(this);
	EndPaint(_wndHandle, &ps);
}

void Win32Device::injectDestroy()
{
	Device::injectDestroy();
	if (_attribute.WndQuit)
		PostQuitMessage(0);
}

void Win32Device::injectActivate()
{
	Device::injectActivate();
	if (_attribute.WndPaint)
		InvalidateRect(_wndHandle, NULL, FALSE);
}

void Win32Device::injectInactive()
{
	Device::injectInactive();
	if (_attribute.WndPaint)
		InvalidateRect(_wndHandle, NULL, FALSE);
}

void Win32Device::injectInputLanguageChange()
{
	Device::injectInputLanguageChange();
	// get the new codepage used for keyboard input
	KEYBOARD_INPUT_HKL = GetKeyboardLayout(0);
	KEYBOARD_INPUT_CODEPAGE = LocaleIdToCodepage(LOWORD(KEYBOARD_INPUT_HKL));
}

NS_VAYO_END

#endif // _WIN32
