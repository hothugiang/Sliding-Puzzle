#include "button.h"

Button::Button(const SDL_Rect& rect, const SDL_Color& colour, TTF_Font* const font, const SDL_Color& fontColour)
    : UserInterface(rect, colour, font, fontColour) {

}

bool Button::isMouseInside(const int x, const int y) const {
    // Assume mouse is inside tile
    bool inside = true;

   	// Mouse is left of the button
	if (x < mRect.x)
	{
		inside = false;
	}
	// Mouse is right of the button
	else if (x > mRect.x + mRect.w)
	{
		inside = false;
	}
	// Mouse above the button
	else if (y < mRect.y)
	{
		inside = false;
	}
	// Mouse below the button
	else if (y > mRect.y + mRect.h)
	{
		inside = false;
	}

    return inside;
}

void Button::changeColourTo(const SDL_Color& colour) {
    mColour = colour;
}
