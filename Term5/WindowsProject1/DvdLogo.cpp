#include "framework.h"
#include "DvdLogo.h"
#include <iostream>
#define MAX_LOADSTRING 100


// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
HBITMAP hImage;
BITMAP bImage;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
	hImage = LoadBitmapW(hInstance, MAKEINTRESOURCE(IDB_BITMAP2));
	GetObject(hImage, sizeof(bImage), &bImage);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }
   SetTimer(hWnd, NULL, 1, NULL);
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LONG    x = 1;
LONG    y = 1;
LONG step = 5;
LONG xDirection = 0;
RECT oldRect;
void BouncingMovement(HWND hWnd, HDC hdc);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
			BouncingMovement(hWnd, hdc);
            EndPaint(hWnd, &ps);
        }
        break;
	case WM_TIMER: {
		InvalidateRect(hWnd, &oldRect, FALSE);
		break;
	}
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
LONG RectWidth = 182;
LONG RectHeight = 123;
LONG xSpeed = 1;
LONG ySpeed = 1;

void PaintBitmap(HWND hWnd, HDC hdc, int x, int y);

void BouncingMovement(HWND hWnd, HDC hdc) {
	RECT windowRect;

	if (GetWindowRect(hWnd, &windowRect)) {
		int width = windowRect.right - windowRect.left;
		int height = windowRect.bottom - windowRect.top;

		if ((x + RectWidth) >= width || x <= 0) {
			xSpeed = -xSpeed;
		}

		if ((y + RectHeight) >= height || y <= 0) {
			ySpeed = -ySpeed;
		}

		x += xSpeed;
		y += ySpeed;

	}
	oldRect = { x, y, x + RectWidth, y + RectHeight };
	PaintBitmap(hWnd, hdc, x, y);
}

void PaintBitmap(HWND hWnd, HDC hdc, int x, int y) {
	/*HDC             hdcMem;
	HGDIOBJ         oldBitmap;

	hdcMem = CreateCompatibleDC(hdc);
	oldBitmap = SelectObject(hdcMem, hImage);

	BitBlt(hdc, x, y, bImage.bmWidth, bImage.bmHeight, hdcMem, 0, 0, SRCCOPY);

	SelectObject(hdcMem, oldBitmap);
	DeleteDC(hdcMem);*/

	RECT rc;
	HDC hdcMem;
	HBITMAP hbmMem;
	HGDIOBJ hbmOld;
	HBRUSH hbrBkGnd;

	//
	// Get the size of the client rectangle.
	//

	GetClientRect(hWnd, &rc);

	//
	// Create a compatible DC.
	//

	hdcMem = CreateCompatibleDC(hdc);

	//
	// Create a bitmap big enough for our client rectangle.
	//

	hbmMem = CreateCompatibleBitmap(hdc,
		rc.right - rc.left,
		rc.bottom - rc.top);

	//
	// Select the bitmap into the off-screen DC.
	//

	hbmOld = SelectObject(hdcMem, hbmMem);

	//
	// Erase the background.
	//

	hbrBkGnd = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
	FillRect(hdcMem, &rc, hbrBkGnd);
	DeleteObject(hbrBkGnd);

	//
	// Render the image into the offscreen DC.
	//
	HDC             dvdHdcMem;
	HGDIOBJ         oldBitmap;

	dvdHdcMem = CreateCompatibleDC(hdcMem);
	oldBitmap = SelectObject(dvdHdcMem, hImage);

	BitBlt(hdcMem, x, y, bImage.bmWidth, bImage.bmHeight, dvdHdcMem, 0, 0, SRCCOPY);

	SelectObject(dvdHdcMem, oldBitmap);
	DeleteDC(dvdHdcMem);
	//
	// Blt the changes to the screen DC.
	//

	BitBlt(hdc,
		rc.left, rc.top,
		rc.right - rc.left, rc.bottom - rc.top,
		hdcMem,
		0, 0,
		SRCCOPY);

	//
	// Done with off-screen bitmap and DC.
	//

	SelectObject(hdcMem, hbmOld);
	DeleteObject(hbmMem);
	DeleteDC(hdcMem);
}