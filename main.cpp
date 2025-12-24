#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include <ctime>
#include <iostream>
#include <math.h>
#include <string>
#include <vector>
using namespace std;

struct TimeElement {
  SDL_Texture *texture; // this will be for our beloved text
  SDL_Rect rect;
};

SDL_Texture *createText(SDL_Renderer *renderer, TTF_Font *font,
                        const string &text, SDL_Color color, SDL_Rect &dest) {
  SDL_Surface *surface = TTF_RenderText_Solid(font, text.c_str(), color);

  if (!surface) {
    cout << "errr " << SDL_GetError() << endl;
    return nullptr;
  }

  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

  dest.w = surface->w;
  dest.h = surface->h;

  SDL_FreeSurface(surface);

  return texture;
}

int main() {

  int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 400;
  TTF_Font *font;

  const char *timeNumbers[] = {"1", "2", "3", "4",  "5",  "6",
                               "7", "8", "9", "10", "11", "12"};
  if (!SDL_Init(SDL_INIT_VIDEO)) {

    cout << "sdl error: " << SDL_GetError() << endl;
  }

  SDL_Window *window = SDL_CreateWindow("analog_clock", SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH,
                                        WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);

  if (!window) {

    cout << "sdl error: " << SDL_GetError() << endl;
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  if (!renderer) {

    cout << "sdl error: " << SDL_GetError() << endl;
  }

  if (!TTF_Init()) {
    cout << "ttf error: " << TTF_GetError() << endl;
  }

  font = TTF_OpenFont("Mojangles.ttf", 36);

  vector<TimeElement> clockNumbers;

  int centerX = 350;
  int centerY = 200;
  int radius = 120;

  for (int i = 0; i < 12; i++) {
    TimeElement elem;

    SDL_Color color = {0, 0, 0, 255};

    elem.texture = createText(renderer, font, timeNumbers[i], color, elem.rect);

    double angle = (i + 1) * (M_PI / 6) - M_PI / 2;

    elem.rect.x = centerX + cos(angle) * radius - elem.rect.w / 2;
    elem.rect.y = centerY + sin(angle) * radius - elem.rect.h / 2;

    clockNumbers.push_back(elem);
  }

  // let's prepare the second hand at first
  //
  //
  //
  //   HOUR HAND

  bool running = true;
  SDL_Event e;

  SDL_Texture *timeTexture = nullptr;
  SDL_Rect timeRect;

  int lastSecond = -1;
  while (running) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        running = false;
      }
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    SDL_RenderClear(renderer);

    filledCircleRGBA(renderer, 350, 200, 150, 0, 0, 0, 255);
    filledCircleRGBA(renderer, 350, 200, 140, 255, 255, 255, 255);

    for (auto &elem : clockNumbers) {
      SDL_RenderCopy(renderer, elem.texture, nullptr, &elem.rect);
    }
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    // SECOND HAND
    time_t now = time(nullptr);
    tm *local = localtime(&now);

    int seconds = local->tm_sec;
    int minutes = local->tm_min;
    int hours = local->tm_hour % 12;

    if (seconds != lastSecond) {
      lastSecond = seconds;

      if (timeTexture) {
        SDL_DestroyTexture(timeTexture);
      }

      char buffer[16]; // TODO: LEARN MORE ABOUT THIS THING

      snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d", local->tm_hour,
               minutes, seconds); // TODO: LEARN MORE ABOUT THIS TOO

      SDL_Color color = {0, 0, 0, 255};

      timeTexture = createText(renderer, font, buffer, color, timeRect);

      timeRect.y = 180;
      timeRect.x = 520;
    }

    int handLength = 130;
    double angle = seconds * (M_PI / 30) - M_PI / 2;

    int x2 = centerX + cos(angle) * handLength;
    int y2 = centerY + sin(angle) * handLength;

    // // SECOND HAND

    SDL_RenderDrawLine(renderer, centerX, centerY, x2, y2);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    // MINUTE HAND

    double minAngle = (minutes + seconds / 60) * (M_PI / 30) - M_PI / 2;

    int minLen = 110;
    filledCircleRGBA(renderer, centerX, centerY, 5, 0, 0, 0, 255);

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    //   MINUTE HAND
    SDL_RenderDrawLine(renderer, centerX, centerY,
                       centerX + cos(minAngle) * minLen,
                       centerY + sin(minAngle) * minLen);

    //    thickLineRGBA(renderer, centerX, centerY, centerX + cos(minAngle) *
    //    minLen,
    //                 sin(minAngle) * minLen, 4, 255, 0, 0, 0255);

    // HOUR HAND
    //
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    double hourAngle = (hours + minutes / 60.0) * (M_PI / 6) - M_PI / 2;

    int hourLen = 80;

    //   HOUR HAND

    SDL_RenderDrawLine(renderer, centerX, centerY,
                       centerX + cos(hourAngle) * hourLen,
                       centerY + sin(hourAngle) * hourLen);

    if (timeTexture) {
      SDL_RenderCopy(renderer, timeTexture, nullptr, &timeRect);
    }

    SDL_RenderPresent(renderer);
    SDL_Delay(16);
  }

  if (timeTexture) {
    SDL_DestroyTexture(timeTexture);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
