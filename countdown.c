#include <SDL.h>
#include <SDL_ttf.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 450
#define TITLE_FONT_SIZE 48
#define NUMBER_FONT_SIZE 80
#define LABEL_FONT_SIZE 24
#define PADDING 20

typedef struct {
    int year;
    int month;
    int day;
} Date;

typedef struct {
    Uint8 r, g, b, a;
} Color;

Date parseDate(const char* dateStr) {
    Date date;
    sscanf(dateStr, "%d-%d-%d", &date.year, &date.month, &date.day);
    return date;
}

time_t getTargetTime(Date date) {
    struct tm target = {0};
    target.tm_year = date.year - 1900;
    target.tm_mon = date.month - 1;
    target.tm_mday = date.day;
    return mktime(&target);
}

void renderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, Color color, int x, int y) {
    SDL_Color sdlColor = {color.r, color.g, color.b, color.a};
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, sdlColor);
    if (!surface) return;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return;
    }
    SDL_Rect rect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void drawSegment(SDL_Renderer* renderer, int x, int y, int w, int h, Color bgColor, Color textColor, TTF_Font* font, const char* text) {
    SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_Rect bgRect = {x, y, w, h};
    SDL_RenderFillRect(renderer, &bgRect);

    int textWidth, textHeight;
    TTF_SizeText(font, text, &textWidth, &textHeight);
    renderText(renderer, font, text, textColor, x + (w - textWidth) / 2, y + (h - textHeight) / 2);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s YYYY-MM-DD\n", argv[0]);
        return 1;
    }

    Date targetDate = parseDate(argv[1]);
    time_t targetTime = getTargetTime(targetDate);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() < 0) {
        printf("TTF initialization failed: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Countdown", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                        WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Window creation failed: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        printf("Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    TTF_Font* titleFont = TTF_OpenFont("arial.ttf", TITLE_FONT_SIZE);
    TTF_Font* numberFont = TTF_OpenFont("arial.ttf", NUMBER_FONT_SIZE);
    TTF_Font* labelFont = TTF_OpenFont("arial.ttf", LABEL_FONT_SIZE);
    if (!titleFont || !numberFont || !labelFont) {
        printf("Font loading failed: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    Color blueBackground = {0x1a, 0x44, 0x8a, 0xff};
    Color whiteBackground = {0xff, 0xff, 0xff, 0xff};
    Color darkBlueText = {0x1a, 0x44, 0x8a, 0xff};
    Color whiteText = {0xff, 0xff, 0xff, 0xff};

    SDL_bool running = SDL_TRUE;
    char daysStr[10];
    char hoursStr[10];
    char minutesStr[10];

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = SDL_FALSE;
            }
        }

        time_t currentTime = time(NULL);
        time_t remaining = targetTime - currentTime;

        if (remaining <= 0) {
            strcpy(daysStr, "00");
            strcpy(hoursStr, "00");
            strcpy(minutesStr, "00");
        } else {
            int days = remaining / (24 * 3600);
            remaining %= (24 * 3600);
            int hours = remaining / 3600;
            remaining %= 3600;
            int minutes = remaining / 60;
            
            sprintf(daysStr, "%02d", days);
            sprintf(hoursStr, "%02d", hours);
            sprintf(minutesStr, "%02d", minutes);
        }

        SDL_SetRenderDrawColor(renderer, blueBackground.r, blueBackground.g, blueBackground.b, blueBackground.a);
        SDL_RenderClear(renderer);

        int titleWidth, titleHeight;
        TTF_SizeText(titleFont, "Countdown", &titleWidth, &titleHeight);
        renderText(renderer, titleFont, "Countdown", whiteText, (WINDOW_WIDTH - titleWidth) / 2, PADDING);

        int daysNumberWidth, numberHeight;
        TTF_SizeText(numberFont, daysStr, &daysNumberWidth, &numberHeight);
        int hoursNumberWidth;
        TTF_SizeText(numberFont, hoursStr, &hoursNumberWidth, NULL);
        int minutesNumberWidth;
        TTF_SizeText(numberFont, minutesStr, &minutesNumberWidth, NULL);
        int colonWidth;
        TTF_SizeText(numberFont, ":", &colonWidth, NULL);

        int segmentWidth = (WINDOW_WIDTH - PADDING * 2 - colonWidth * 2) / 3 - PADDING * 2; // Adjusted calculation for segment width
        int segmentHeight = numberHeight + PADDING; // Give some padding around the number

        int totalSegmentsWidth = segmentWidth * 3 + colonWidth * 2 + PADDING * 4; // 3 segments + 2 colons + spacing
        int startX = (WINDOW_WIDTH - totalSegmentsWidth) / 2;
        int numberY = WINDOW_HEIGHT / 2 - segmentHeight / 2;
        int labelY = numberY + segmentHeight + PADDING / 2;

        drawSegment(renderer, startX, numberY, segmentWidth, segmentHeight, whiteBackground, darkBlueText, numberFont, daysStr);
        int daysLabelWidth;
        TTF_SizeText(labelFont, "Days", &daysLabelWidth, NULL);
        renderText(renderer, labelFont, "Days", whiteText, startX + segmentWidth/2 - daysLabelWidth/2, labelY);
        startX += segmentWidth + PADDING;
        
        drawSegment(renderer, startX, numberY, colonWidth, segmentHeight, whiteBackground, darkBlueText, numberFont, ":");
        startX += colonWidth + PADDING;

        drawSegment(renderer, startX, numberY, segmentWidth, segmentHeight, whiteBackground, darkBlueText, numberFont, hoursStr);
        int hoursLabelWidth;
        TTF_SizeText(labelFont, "Hours", &hoursLabelWidth, NULL);
        renderText(renderer, labelFont, "Hours", whiteText, startX + segmentWidth/2 - hoursLabelWidth/2, labelY);
        startX += segmentWidth + PADDING;

        drawSegment(renderer, startX, numberY, colonWidth, segmentHeight, whiteBackground, darkBlueText, numberFont, ":");
        startX += colonWidth + PADDING;

        drawSegment(renderer, startX, numberY, segmentWidth, segmentHeight, whiteBackground, darkBlueText, numberFont, minutesStr);
        int minutesLabelWidth;
        TTF_SizeText(labelFont, "Minutes", &minutesLabelWidth, NULL);
        renderText(renderer, labelFont, "Minutes", whiteText, startX + segmentWidth/2 - minutesLabelWidth/2, labelY);

        SDL_RenderPresent(renderer);
        SDL_Delay(1000);
    }

    TTF_CloseFont(titleFont);
    TTF_CloseFont(numberFont);
    TTF_CloseFont(labelFont);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
} 