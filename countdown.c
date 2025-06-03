#include <SDL.h>
#include <SDL_ttf.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 700
#define TITLE_FONT_SIZE 48
#define NUMBER_FONT_SIZE 80
#define LABEL_FONT_SIZE 24
#define QUOTE_FONT_SIZE 24
#define PADDING 30
#define SQUARE_SIZE 15
#define SQUARE_SPACING 5
#define GRID_COLS 20
#define GRID_ROWS 10
#define TOTAL_SQUARES (GRID_COLS * GRID_ROWS)
#define SEGMENT_OUTLINE_THICKNESS 3
#define DIGIT_SPACING 5
#define QUOTE_DISPLAY_TIME 10000

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
    if (sscanf(dateStr, "%d-%d-%d", &date.year, &date.month, &date.day) == 3) {
        return date;
    } else {
        date.year = 0; date.month = 0; date.day = 0; // Indicate parse error
        return date;
    }
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

void renderWrappedText(SDL_Renderer* renderer, TTF_Font* font, const char* text, Color color, int x, int y, int maxWidth) {
    std::stringstream ss(text);
    std::string word;
    std::string line = "";
    std::vector<std::string> lines;
    int spaceWidth;
    TTF_SizeText(font, " ", &spaceWidth, NULL);

    while (ss >> word) {
        std::string testLine = line;
        if (!testLine.empty()) {
            testLine += " ";
        }
        testLine += word;

        int testWidth;
        TTF_SizeText(font, testLine.c_str(), &testWidth, NULL);

        if (testWidth > maxWidth && !line.empty()) {
            lines.push_back(line);
            line = word;
        } else {
            line = testLine;
        }
    }
    if (!line.empty()) {
        lines.push_back(line);
    }

    int lineHeight = TTF_FontHeight(font);
    for (size_t i = 0; i < lines.size(); ++i) {
        int lineWidth;
        TTF_SizeText(font, lines[i].c_str(), &lineWidth, NULL);
        renderText(renderer, font, lines[i].c_str(), color, x + (maxWidth - lineWidth) / 2, y + i * lineHeight);
    }
}

void drawNumber(SDL_Renderer* renderer, int x, int y, int segmentWidth, int segmentHeight, Color textColor, TTF_Font* font, const char* numberStr) {
    int digitWidth;
    TTF_SizeText(font, "0", &digitWidth, NULL);
    int totalDigitWidth = digitWidth * 2 + DIGIT_SPACING;
    int startDigitX = x + (segmentWidth - totalDigitWidth) / 2;
    int textY = y + (segmentHeight - (TTF_FontHeight(font) - TTF_FontDescent(font))) / 2;

    char digit1[2] = {numberStr[0], '\0'};
    char digit2[2] = {numberStr[1], '\0'};

    renderText(renderer, font, digit1, textColor, startDigitX, textY);
    renderText(renderer, font, digit2, textColor, startDigitX + digitWidth + DIGIT_SPACING, textY);
}

void drawColon(SDL_Renderer* renderer, int x, int y, int w, int h, Color textColor, TTF_Font* font) {
     int textWidth, textHeight;
     TTF_SizeText(font, ":", &textWidth, &textHeight);
     renderText(renderer, font, ":", textColor, x + (w - textWidth) / 2, y + (h - textHeight) / 2);
}

void drawDaySquares(SDL_Renderer* renderer, int daysRemaining, Color filledColor, Color unfilledColor) {
    int gridWidth = GRID_COLS * SQUARE_SIZE + (GRID_COLS - 1) * SQUARE_SPACING;
    int gridHeight = GRID_ROWS * SQUARE_SIZE + (GRID_ROWS - 1) * SQUARE_SPACING;
    int startX = (WINDOW_WIDTH - gridWidth) / 2;
    int startY = WINDOW_HEIGHT - gridHeight - PADDING;

    for (int i = 0; i < TOTAL_SQUARES; ++i) {
        int row = i / GRID_COLS;
        int col = i % GRID_COLS;
        int squareX = startX + col * (SQUARE_SIZE + SQUARE_SPACING);
        int squareY = startY + row * (SQUARE_SIZE + SQUARE_SPACING);

        Color color = (i < daysRemaining) ? filledColor : unfilledColor;
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_Rect squareRect = {squareX, squareY, SQUARE_SIZE, SQUARE_SIZE};
        SDL_RenderFillRect(renderer, &squareRect);
    }
}

std::vector<std::string> loadQuotes(const char* filename) {
    std::vector<std::string> quotes;
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        quotes.push_back(line);
    }
    return quotes;
}

Date loadDeadlineFromConfig(const char* filename) {
    std::ifstream file(filename);
    std::string line;
    Date deadlineDate = {0, 0, 0};

    if (file.is_open()) {
        while (std::getline(file, line)) {
            size_t eqPos = line.find('=');
            if (eqPos != std::string::npos) {
                std::string key = line.substr(0, eqPos);
                std::string value = line.substr(eqPos + 1);
                if (key == "deadline") {
                    deadlineDate = parseDate(value.c_str());
                    break;
                }
            }
        }
        file.close();
    }
    return deadlineDate;
}

int main(int argc, char* argv[]) {
    Date targetDate = loadDeadlineFromConfig("config.txt");

    if (targetDate.year == 0) {
        printf("Error: Could not read or parse deadline from config.txt. Ensure the file exists and has a line like 'deadline=YYYY-MM-DD'\n");
        return 1;
    }

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

    TTF_Font* titleFont = TTF_OpenFont("Technology.ttf", TITLE_FONT_SIZE);
    TTF_Font* numberFont = TTF_OpenFont("Technology.ttf", NUMBER_FONT_SIZE);
    TTF_Font* labelFont = TTF_OpenFont("Technology.ttf", LABEL_FONT_SIZE);
    TTF_Font* quoteFont = TTF_OpenFont("Technology.ttf", QUOTE_FONT_SIZE);
    if (!titleFont || !numberFont || !labelFont || !quoteFont) {
        printf("Font loading failed: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    Color darkBackground = {0x10, 0x10, 0x10, 0xff};
    Color neonBlue = {0x00, 0xf0, 0xff, 0xff};
    Color neonMagenta = {0xff, 0x00, 0xff, 0xff};
    Color darkNeonBlue = {0x00, 0x70, 0x90, 0xff};

    std::vector<std::string> quotes = loadQuotes("quotes.txt");
    srand(time(NULL));
    int currentQuoteIndex = rand() % quotes.size();
    Uint32 lastQuoteChangeTime = SDL_GetTicks();

    SDL_bool running = SDL_TRUE;
    char daysStr[10];
    char hoursStr[10];
    char minutesStr[10];
    int daysRemaining = 0;

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = SDL_FALSE;
            }
        }

        Uint32 currentTimeTicks = SDL_GetTicks();
        if (!quotes.empty() && currentTimeTicks - lastQuoteChangeTime > QUOTE_DISPLAY_TIME) {
            currentQuoteIndex = rand() % quotes.size();
            lastQuoteChangeTime = currentTimeTicks;
        }

        time_t currentTime = time(NULL);
        time_t remaining = targetTime - currentTime;

        if (remaining <= 0) {
            strcpy(daysStr, "00");
            strcpy(hoursStr, "00");
            strcpy(minutesStr, "00");
            daysRemaining = 0;
        } else {
            daysRemaining = remaining / (24 * 3600);
            remaining %= (24 * 3600);
            int hours = remaining / 3600;
            remaining %= 3600;
            int minutes = remaining / 60;
            
            sprintf(daysStr, "%02d", daysRemaining > 99 ? 99 : daysRemaining);
            sprintf(hoursStr, "%02d", hours);
            sprintf(minutesStr, "%02d", minutes);
        }

        SDL_SetRenderDrawColor(renderer, darkBackground.r, darkBackground.g, darkBackground.b, darkBackground.a);
        SDL_RenderClear(renderer);

        drawDaySquares(renderer, daysRemaining > TOTAL_SQUARES ? TOTAL_SQUARES : daysRemaining, neonBlue, darkNeonBlue);

        int titleWidth, titleHeight;
        TTF_SizeText(titleFont, "Countdown", &titleWidth, &titleHeight);
        renderText(renderer, titleFont, "Countdown", neonMagenta, (WINDOW_WIDTH - titleWidth) / 2, PADDING);

        int digitWidth, numberHeight;
        TTF_SizeText(numberFont, "0", &digitWidth, &numberHeight);
        int colonWidth;
        TTF_SizeText(numberFont, ":", &colonWidth, NULL);

        int numberDisplayWidth = digitWidth * 2 + DIGIT_SPACING;
        int colonDisplayWidth = colonWidth;
        int displayHeight = numberHeight;

        int totalDisplayWidth = numberDisplayWidth * 3 + colonDisplayWidth * 2 + PADDING * 4;
        int startX = (WINDOW_WIDTH - totalDisplayWidth) / 2;
        int numberY = PADDING * 2 + titleHeight;
        int labelY = numberY + displayHeight + PADDING / 2;

        drawNumber(renderer, startX, numberY, numberDisplayWidth, displayHeight, neonBlue, numberFont, daysStr);
        int daysLabelWidth;
        TTF_SizeText(labelFont, "Days", &daysLabelWidth, NULL);
        renderText(renderer, labelFont, "Days", neonBlue, startX + numberDisplayWidth/2 - daysLabelWidth/2, labelY);
        startX += numberDisplayWidth + PADDING;
        
        drawColon(renderer, startX, numberY, colonDisplayWidth, displayHeight, neonBlue, numberFont);
        startX += colonDisplayWidth + PADDING;

        drawNumber(renderer, startX, numberY, numberDisplayWidth, displayHeight, neonBlue, numberFont, hoursStr);
        int hoursLabelWidth;
        TTF_SizeText(labelFont, "Hours", &hoursLabelWidth, NULL);
        renderText(renderer, labelFont, "Hours", neonBlue, startX + numberDisplayWidth/2 - hoursLabelWidth/2, labelY);
        startX += numberDisplayWidth + PADDING;

        drawColon(renderer, startX, numberY, colonDisplayWidth, displayHeight, neonBlue, numberFont);
        startX += colonDisplayWidth + PADDING;

        drawNumber(renderer, startX, numberY, numberDisplayWidth, displayHeight, neonBlue, numberFont, minutesStr);
        int minutesLabelWidth;
        TTF_SizeText(labelFont, "Minutes", &minutesLabelWidth, NULL);
        renderText(renderer, labelFont, "Minutes", neonBlue, startX + numberDisplayWidth/2 - minutesLabelWidth/2, labelY);

        if (!quotes.empty()) {
            const std::string& quote = quotes[currentQuoteIndex];
            renderWrappedText(renderer, quoteFont, quote.c_str(), neonMagenta, PADDING, labelY + LABEL_FONT_SIZE + PADDING * 2, WINDOW_WIDTH - PADDING * 2);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    TTF_CloseFont(titleFont);
    TTF_CloseFont(numberFont);
    TTF_CloseFont(labelFont);
    TTF_CloseFont(quoteFont);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}