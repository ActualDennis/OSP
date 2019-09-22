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
	void Move();
	void OnMouseMove(int x, int y);
	void OnMouseClicked();
	void BouncingMovement();
	int xPos = 50;
	int yPos = 50;
private:
	int xSpeed;
	int ySpeed;
	HWND hWindow;
	long RectHeight;
	long RectWidth;
	bool MouseMoveActivated = true;
	int xDest = 0;
	int yDest = 0;
	int xStep = 0;
	int yStep = 0;
};