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
