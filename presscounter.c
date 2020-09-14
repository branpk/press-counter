
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <windows.h>


static HWND hWindow;
static int pressed = 0;
static int pressCount = 0;


static LRESULT CALLBACK keyboardHookProc(
  int nCode,
  WPARAM wParam,
  LPARAM lParam)
{
  if (nCode == HC_ACTION) {
    KBDLLHOOKSTRUCT *info = (KBDLLHOOKSTRUCT *) lParam;

    if (info->vkCode == VK_HOME) {
      if (wParam == WM_KEYDOWN && !pressed) {
        pressed = 1;
        pressCount++;
        InvalidateRect(hWindow, NULL, TRUE);
      }
      if (wParam == WM_KEYUP) {
        pressed = 0;
        InvalidateRect(hWindow, NULL, TRUE);
      }
    }
  }

  return CallNextHookEx(NULL, nCode, wParam, lParam);
}


static void draw(HWND hWnd) {
  PAINTSTRUCT paintStruct;
  HDC hDC = BeginPaint(hWnd, &paintStruct);

  HFONT hFont = CreateFont(
    -MulDiv(108, GetDeviceCaps(hDC, LOGPIXELSY), 72),
    0,
    0,
    0,
    FW_NORMAL,
    FALSE,
    FALSE,
    FALSE,
    ANSI_CHARSET,
    OUT_DEFAULT_PRECIS,
    CLIP_DEFAULT_PRECIS,
    ANTIALIASED_QUALITY,
    DEFAULT_PITCH,
    "Arial");

  HFONT oldFont = SelectObject(hDC, hFont);
  DeleteObject(oldFont);

  SetTextAlign(hDC, TA_RIGHT);
  SetBkColor(hDC, RGB(0, 0, 0));
  if (pressed)
    SetTextColor(hDC, RGB(255, 255, 255));
  else
    SetTextColor(hDC, RGB(200, 200, 200));

  static char buffer[256];
  sprintf(&buffer[0], "%dxA", pressCount);
  TextOut(hDC, 360, 100, &buffer[0], strlen(buffer));

  EndPaint(hWnd, &paintStruct);
}


static LRESULT CALLBACK windowProc(
  HWND hWnd,
  UINT uMsg,
  WPARAM wParam,
  LPARAM lParam)
{
  switch (uMsg) {
    case WM_KEYDOWN:
      if (wParam == VK_NUMPAD3) {
        pressCount = 0;
        InvalidateRect(hWindow, NULL, TRUE);
      }
      break;

    case WM_PAINT:
      draw(hWnd);
      break;

    case WM_DESTROY:
      PostQuitMessage(0);
      break;

    default:
      return DefWindowProc(hWnd, uMsg, wParam, lParam);
  }

  return 0;
}


int CALLBACK WinMain(
  HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPSTR lpCmdLine,
  int nCmdShow)
{
  WNDCLASSEX classEx;
  classEx.cbSize = sizeof(WNDCLASSEX);
  classEx.style = CS_HREDRAW | CS_VREDRAW;
  classEx.lpfnWndProc = &windowProc;
  classEx.cbClsExtra = 0;
  classEx.cbWndExtra = 0;
  classEx.hInstance = hInstance;
  classEx.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
  classEx.hCursor = LoadCursor(NULL, IDC_ARROW);
  classEx.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
  classEx.lpszMenuName = NULL;
  classEx.lpszClassName = "Press Counter";
  classEx.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

  RegisterClassEx(&classEx);

  hWindow = CreateWindow(
    classEx.lpszClassName,
    "Press Counter",
    WS_SYSMENU,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    400,
    400,
    NULL,
    NULL,
    hInstance,
    NULL);

  HHOOK hHook = SetWindowsHookEx(
    WH_KEYBOARD_LL,
    &keyboardHookProc,
    hInstance,
    0);

  ShowWindow(hWindow, nCmdShow);
  UpdateWindow(hWindow);

  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  UnhookWindowsHookEx(hHook);
  return (int) msg.wParam;
}
