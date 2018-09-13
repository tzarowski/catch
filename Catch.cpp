#include <SDL.h>
#include <stdio.h>
#include <string>
#include <time.h>
#include <SDL_ttf.h>

const int TARGET_WIDTH = 32;
const int TARGET_HEIGHT = 12;


const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//------------------------

class Target {
public:
	SDL_Rect targetBox;
	int targetSpeed;
	SDL_Color targetColor;

public:
	Target(int _xPos, int speed, SDL_Color color) {
		targetBox = { _xPos, 0, TARGET_WIDTH, TARGET_HEIGHT };
		targetSpeed = speed;
		targetColor = color;
	}

	void setColor(SDL_Color newColor) {
		targetColor = newColor;
	}

	void setRandomColor() {
		SDL_Color possibleColors[] = { { 255,0,0,255 },{ 0,0,255,255 } };
		targetColor = possibleColors[rand() % 2];
	}
};


//------------------------

//color pallette setup
SDL_Color Red = { 255,0,0,255 };
SDL_Color Green = { 0,255,0,255 };
SDL_Color Blue = { 0,0,255,255 };
SDL_Color Black = { 0,0,0,0 };
bool init();

void close();


SDL_Window* gWindow = NULL;

SDL_Renderer* gRenderer = NULL;

TTF_Font* Times = NULL;

bool init() {
	bool success = true;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not be initialised! Error %s\n", SDL_GetError());
		success = false;
	}
	else {
		gWindow = SDL_CreateWindow("Pong", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL) {
			printf("Window could not be created! Error: %s\n", SDL_GetError());
			success = false;
		}
		else {
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gRenderer == NULL) {
				printf("Renderer could not be created!");
				success = false;
			}
			else {
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				if (TTF_Init() < 0) {
					printf("Text renderer could not be created!");
					success = false;
				}
				else {
					Times = TTF_OpenFont("times.ttf", 50);
					if (Times == NULL) {
						printf("Font could not be loaded");
						success = false;
					}
				}
			}
		}
	}
	return success;
}

void close() {
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	SDL_Quit();
}

bool sameColor(SDL_Color color1, SDL_Color color2) {
	if (color1.r == color2.r &&
		color1.g == color2.g &&
		color1.b == color2.b &&
		color1.a == color2.a) {
		return true;
	}
	else {
		return false;
	}
}

void displayScore(int score) {
	SDL_Rect scoreRect = { 0,0,50,25 };
	SDL_Surface* scoreSurface = TTF_RenderText_Solid(Times, ("Score: " + std::to_string(score)).c_str(), Black);
	SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(gRenderer, scoreSurface);
	SDL_RenderCopy(gRenderer, scoreTexture, NULL, &scoreRect);
	SDL_FreeSurface(scoreSurface);
	SDL_DestroyTexture(scoreTexture);
}

void displayLives(int lives) {
	SDL_Rect livesRect = { 0,50,50,25 };
	SDL_Surface* livesSurface = TTF_RenderText_Solid(Times, ("Lives: " + std::to_string(lives)).c_str(), Black);
	SDL_Texture* livesTexture = SDL_CreateTextureFromSurface(gRenderer, livesSurface);
	SDL_RenderCopy(gRenderer, livesTexture, NULL, &livesRect);
	SDL_FreeSurface(livesSurface);
	SDL_DestroyTexture(livesTexture);
}

Target resetTarget(Target target) {
	target.targetBox.y = 0;
	target.targetBox.x = rand() % (SCREEN_WIDTH - TARGET_WIDTH);
	target.targetSpeed = (rand() % 3) + 2;
	target.setRandomColor();
	return target;
}
void playGame() {

	bool quit = false;

	SDL_Event e;

	//player setup
	int playerX = SCREEN_WIDTH * 2 / 5;
	int playerY = SCREEN_HEIGHT - 40;
	int playerWidth = SCREEN_WIDTH / 5;
	int playerHeight = SCREEN_HEIGHT / 20;
	int playerSpeed = 10;
	int score = 0;
	int lives = 3;


	int frameNumber = 0;

	srand(time(NULL));

	Target currentTargets[] = { Target(20,3, Blue), Target(60,2, Red) , Target(100, 1, Blue), Target(140,4,Green) };
	int numberOfTargets = 4;

	//game loop

	while (!quit) {
		frameNumber++;
		//check for quit and other input
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				quit = true;
			}
			else if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
				case SDLK_RIGHT:
					playerX += playerSpeed;
					if (playerX + playerWidth > SCREEN_WIDTH) {
						playerX -= playerSpeed;
					}
					break;
				case SDLK_LEFT:
					playerX -= playerSpeed;
					if (playerX < 0) {
						playerX += playerSpeed;
					}
					break;
				default:
					break;
				}
			}
		}

		//reset screen
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(gRenderer);

		displayScore(score);
		displayLives(lives);

		//draw and display player
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
		SDL_Rect playerRect = { playerX, playerY, playerWidth, playerHeight };
		SDL_RenderFillRect(gRenderer, &playerRect);

		//for each target
		for (int i = 0; i < numberOfTargets; i++) {

			SDL_Color currentTargetColor = currentTargets[i].targetColor;

			//move target down and check if at bottom of screen
			if (frameNumber % 100 == 0) {
				currentTargets[i].targetBox.y += currentTargets[i].targetSpeed;
				if (currentTargets[i].targetBox.y > playerY) {
					currentTargets[i] = resetTarget(currentTargets[i]);
					//currentTargets[i].setRandomColor();
				}
			}

			//check collision with player
			if (SDL_HasIntersection(&currentTargets[i].targetBox, &playerRect)) {

				currentTargets[i] = resetTarget(currentTargets[i]);

				//check color
				if (sameColor(currentTargetColor, Blue)) {
					score++;

					//check if green extra life brick should spawn
					if (score % 10 == 0) {
						currentTargets[i].setColor(Green);
					}
					//else {
						//currentTargets[i].setRandomColor();
					//}

				}

				if (sameColor(currentTargetColor, Red)) {
					lives--;
					//currentTargets[i].setRandomColor();
				}

				if (sameColor(currentTargetColor, Green)) {
					lives++;
					//currentTargets[i].setRandomColor();
				}
			}

			SDL_SetRenderDrawColor(gRenderer, currentTargetColor.r, currentTargetColor.g, currentTargetColor.b, currentTargetColor.a);
			SDL_RenderFillRect(gRenderer, &currentTargets[i].targetBox);
		}

		SDL_RenderPresent(gRenderer);

	}
}

int main(int argc, char* args[]) {
	if (!init()) {
		printf("Failed to initialize!");
	}
	else {
		playGame();

	}
	close();
	return 0;


}