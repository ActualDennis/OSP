#pragma once
#include "framework.h"

class BitmapPainter {
public:
	BitmapPainter(HWND hWnd, BITMAP bImage, HBITMAP hImage) {
		this->hWindow = hWnd;
		this->bImage = bImage;
		this->hImage = hImage;
	}

	~BitmapPainter() {
		delete hWindow;
	}
	void PaintBitmap(HWND hWnd, HDC hdc, int x, int y);
	void PaintBitmap(HWND hWnd, int x, int y);
private:
	HWND hWindow;
	HBITMAP hImage;
	BITMAP bImage;
	RECT rc;
	HDC hdcMem;
	HBITMAP hbmMem;
	HGDIOBJ hbmOld;
	HBRUSH hbrBkGnd;
};
