#include "BitmapPainter.h"

void BitmapPainter::PaintBitmap(HWND hWnd, HDC hdc, int x, int y)
{
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

void BitmapPainter::PaintBitmap(HWND hWnd, int x, int y)
{
	this->PaintBitmap(hWnd, GetWindowDC(hWnd), x, y);
}
