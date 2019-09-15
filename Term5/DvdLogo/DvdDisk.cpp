#include "DvdLogo.h"
#include "framework.h"


void DvdLogo::BouncingMovement() {
	RECT windowRect;
	if (GetWindowRect(this->hWindow, &windowRect)) {
		int width = windowRect.right - windowRect.left;
		int height = windowRect.bottom - windowRect.top;

		if ((xPos + RectWidth) >= width || xPos <= 0) {
			this->xSpeed = -(this->xSpeed);
		}

		if ((yPos + RectHeight) >= height || yPos <= 0) {
			this->ySpeed = -(this->ySpeed);
		}
	}

	this->xPos += this->xSpeed;
	this->yPos += this->ySpeed;
}

void DvdLogo::Move(Direction dir) {
	RECT windowRect;
	
	if (GetWindowRect(this->hWindow, &windowRect)) {
		int width = windowRect.right - windowRect.left;
		int height = windowRect.bottom - windowRect.top;

		switch (dir)
		{
		case Left:
			if (this->xPos - this->xSpeed > 0) {
				this->xPos -= this->xSpeed;
			}
			break;
		case Right:
			if (this->xPos + this->RectWidth + this->xSpeed < width) {
				this->xPos += this->xSpeed;
			}
			break;
		case Bottom:
			if (this->yPos + this->RectHeight + ySpeed < height) {
				this->yPos += ySpeed;
			}
			break;
		case Top:
			if (this->yPos + ySpeed > 0) {
				this->yPos -= ySpeed;
			}
			break;
		default:
			break;
		}
	}

}