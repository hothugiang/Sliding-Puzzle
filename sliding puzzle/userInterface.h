#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>

class UserInterface {
    protected:
        SDL_Rect mRect;
        SDL_Color mColour;

        // Texture to show text
        TTF_Font* mFont;
        SDL_Rect mFontRect;
        SDL_Color mFontColour;
        SDL_Texture* mTexture;

        void centerText();

    public:
        UserInterface(const SDL_Rect& rect, const SDL_Color& colour, TTF_Font* const font, const SDL_Color& fontColour);

        void loadTexture(SDL_Renderer* const renderer, const char* text);
        void loadImage(SDL_Renderer* const renderer, const char* filename);
        void render(SDL_Renderer* const renderer) const;
        void free();

};
