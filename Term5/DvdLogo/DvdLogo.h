#pragma once
#include "framework.h"

enum Direction {
	Left,
	Right,
	Bottom,
	Top
};

class DvdLogo {
public :
	DvdLogo(HWND hWnd, long xSpeed, long ySpeed, long height, long width) {
		this->xSpeed = xSpeed;
		this->ySpeed = ySpeed;
		this->hWindow = hWnd;
		this->RectHeight = height;
		this->RectWidth = width;
	}

	~DvdLogo() {
		delete hWindow;
	}

	void Move(Direction dir);
	void BouncingMovement();
	int xPos = 50;
	int yPos = 50;
private:
	int xSpeed;
	int ySpeed;
	HWND hWindow;
	long RectHeight;
	long RectWidth;
};