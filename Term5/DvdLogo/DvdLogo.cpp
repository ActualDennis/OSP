#include "DvdLogo.h"

void DvdLogo::Move(Direction dir)
{
	RECT windowRect;

	if (GetWindowRect(hWindow, &windowRect)) {
		int width = windowRect.right - windowRect.left;
		int height = windowRect.bottom - windowRect.top;

		switch (dir) {
			case Left: {
				if (xPos - xSpeed > 0) {
					xPos -= xSpeed;
				}
				break;
			}
			case Right: {
				if (xPos + xSpeed + RectWidth < width) {
					xPos += xSpeed;
				}
				break;
			}
			case Top: {
				if (yPos - ySpeed > 0) {
					yPos -= ySpeed;
				}
				break;
			}
			case Bottom: {
				if (yPos + RectHeight + ySpeed < height) {
					yPos += ySpeed;
				}
				break;
			}
		}
	}
}

void DvdLogo::Move() {
	RECT windowRect;

	if (GetWindowRect(hWindow, &windowRect)) {
		int width = windowRect.right - windowRect.left;
		int height = windowRect.bottom - windowRect.top;

		if (xPos + RectWidth >= width) {
			xDest = width - width / 5 - RectWidth;
		}
		else if (xPos <= 0) {
			xDest = width / 5 + RectWidth;
		}

		if ((yPos + RectHeight) >= height) {
			yDest = height - height / 4 - RectHeight;
		}
		else if (yPos <= 0) {
			yDest = height / 4 + RectHeight;
		}
	}

	xStep = (int)((xDest - xPos))  * 0.15;
	yStep = (int)((yDest - yPos)) * 0.15;

	xPos += xStep;
	yPos += yStep;
}

void DvdLogo::OnMouseMove(int x, int y) {
	if (!this->MouseMoveActivated) {
		return;
	}

	xDest = x * 1.2;
	yDest = y * 1.2;
}

void DvdLogo::OnMouseClicked()
{
	this->MouseMoveActivated = !this->MouseMoveActivated;

	/*if (!this->MouseMoveActivated) {
		xDest *= 1.5;
		yDest *= 1.5;
	}*/
}

void DvdLogo::BouncingMovement()
{
	RECT windowRect;

	if (GetWindowRect(hWindow, &windowRect)) {
		int width = windowRect.right - windowRect.left;
		int height = windowRect.bottom - windowRect.top;

		if ((xPos + RectWidth) >= width || xPos <= 0) {
			xSpeed = -xSpeed;
		}

		if ((yPos + RectHeight) >= height || yPos <= 0) {
			ySpeed = -ySpeed;
		}

		xPos += xSpeed;
		yPos += ySpeed;
	}
}
