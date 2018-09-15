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
		targetBox = { _xPos, 50, TARGET_WIDTH, TARGET_HEIGHT };
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

	void resetTarget() {
		targetBox.y = 50;
		targetBox.x = rand() % (SCREEN_WIDTH - TARGET_WIDTH);
		targetSpeed = (rand() % 3) + 2;
		setRandomColor();
	}

	void moveTarget(int bottom) {
		targetBox.y += targetSpeed;
		if (targetBox.y > bottom) {
			resetTarget();
		}

	}
};


//------------------------

class Player {
public:
	SDL_Rect playerBox;
	int playerSpeed;
	SDL_Color playerColor;

public:
	Player(int _xPos, int _yPos, int _width, int _height, int speed, SDL_Color color) {
		playerBox = { _xPos, _yPos, _width, _height };
		playerSpeed = speed;
		playerColor = color;
	}

	void moveRight() {
		playerBox.x += playerSpeed;
		if (playerBox.x + playerBox.w > SCREEN_WIDTH) {
			playerBox.x -= playerSpeed;
		}
	}

	void moveLeft() {
		playerBox.x -= playerSpeed;
		if (playerBox.x < 0) {
			playerBox.x += playerSpeed;
		}
	}
};

//---------------------------

//color pallette setup
SDL_Color Red = { 255,0,0,255 };
SDL_Color Green = { 0,255,0,255 };
SDL_Color Blue = { 0,0,255,255 };
SDL_Color Black = { 0,0,0,0 };
int score = 0;
int highScore = 0;
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
		gWindow = SDL_CreateWindow("Catch", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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
					Times = TTF_OpenFont("times.ttf", 60);
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
	TTF_Quit();
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
	SDL_Rect scoreRect = { 0,0,150,50 };
	SDL_Surface* scoreSurface = TTF_RenderText_Blended(Times, ("Score: " + std::to_string(score)).c_str(), Black);
	SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(gRenderer, scoreSurface);
	SDL_RenderCopy(gRenderer, scoreTexture, NULL, &scoreRect);
	SDL_FreeSurface(scoreSurface);
	SDL_DestroyTexture(scoreTexture);
}

void displayLives(int lives) {
	SDL_Rect livesRect = { 170,0,150,50 };
	SDL_Surface* livesSurface = TTF_RenderText_Blended(Times, ("Lives: " + std::to_string(lives)).c_str(), Black);
	SDL_Texture* livesTexture = SDL_CreateTextureFromSurface(gRenderer, livesSurface);
	SDL_RenderCopy(gRenderer, livesTexture, NULL, &livesRect);
	SDL_FreeSurface(livesSurface);
	SDL_DestroyTexture(livesTexture);
}

void displayMenuText(int highScore) {
	SDL_Rect menuRect = { 70,215,500,50 };
	SDL_Surface* menuSurface = TTF_RenderText_Blended(Times, ("Press enter to play, high score: " + std::to_string(highScore)).c_str(), Black);
	SDL_Texture* menuTexture = SDL_CreateTextureFromSurface(gRenderer, menuSurface);
	SDL_RenderCopy(gRenderer, menuTexture, NULL, &menuRect);
	SDL_FreeSurface(menuSurface);
	SDL_DestroyTexture(menuTexture);
}

void displayQuitText() {
	SDL_Rect quitRect = { 340, 0, 300, 50 };
	SDL_Surface* quitSurface = TTF_RenderText_Blended(Times, "Press BACKSPACE to quit", Black);
	SDL_Texture* quitTexture = SDL_CreateTextureFromSurface(gRenderer, quitSurface);
	SDL_RenderCopy(gRenderer, quitTexture, NULL, &quitRect);
	SDL_FreeSurface(quitSurface);
	SDL_DestroyTexture(quitTexture);
}

void displayScoreSplashScreen(int score) {
	SDL_Rect splashRect = { 220, 215, 200, 50 };
	SDL_Surface* splashSurface = TTF_RenderText_Blended(Times, ("Final score: " + std::to_string(score)).c_str(), Black);
	SDL_Texture* splashTexture = SDL_CreateTextureFromSurface(gRenderer, splashSurface);
	SDL_RenderCopy(gRenderer, splashTexture, NULL, &splashRect);
	SDL_FreeSurface(splashSurface);
	SDL_DestroyTexture(splashTexture);
}

void displayHUD(int score, int lives) {
	displayScore(score);
	displayLives(lives);
	displayQuitText();
}

int playGame() {
	score = 0;
	bool quit = false;

	//softQuit is a "Quit to Menu", hardQuit is a "Quit to Desktop"
	bool softQuit = false;
	bool hardQuit = false;

	SDL_Event e;

	//player setup
	int playerX = SCREEN_WIDTH * 2 / 5;
	int playerY = SCREEN_HEIGHT - 40;
	int playerWidth = SCREEN_WIDTH / 5;
	int playerHeight = SCREEN_HEIGHT / 20;
	int playerSpeed = 10;

	Player player = Player(playerX, playerY, playerWidth, playerHeight, playerSpeed, Black);
	int lives = 3;


	int frameNumber = 0;

	srand(time(NULL));

	Target currentTargets[] = { Target(20,3, Blue), Target(60,2, Red) , Target(100, 1, Blue), Target(140,4,Green) };

	int numberOfTargets = 4;

	//game loop

	while ((!softQuit) && (!hardQuit)) {
		frameNumber++;
		//check for quit and other input
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				hardQuit = true;
			}
			else if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
				case SDLK_RIGHT:
					player.moveRight();
					break;
				case SDLK_LEFT:
					player.moveLeft();
					break;
				case SDLK_BACKSPACE:
					softQuit = true;
				default:
					break;
				}
			}
		}

		//reset screen
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(gRenderer);

		displayHUD(score, lives);

		//draw and display player
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
		SDL_RenderFillRect(gRenderer, &player.playerBox);

		//for each target
		for (int i = 0; i < numberOfTargets; i++) {

			SDL_Color currentTargetColor = currentTargets[i].targetColor;

			//move target down and check if at bottom of screen
			if (frameNumber % 100 == 0) {

				currentTargets[i].moveTarget(player.playerBox.y);

			}

			//check collision with player
			if (SDL_HasIntersection(&currentTargets[i].targetBox, &player.playerBox)) {
				currentTargets[i].resetTarget();

				//check color
				if (sameColor(currentTargetColor, Blue)) {
					score++;

					//check if green extra life brick should spawn
					if (score % 10 == 0) {
						currentTargets[i].setColor(Green);
					}

				}

				if (sameColor(currentTargetColor, Red)) {
					lives--;
					if (lives <= 0) {
						softQuit = true;
					}
				}

				if (sameColor(currentTargetColor, Green)) {
					lives++;
				}
			}

			SDL_SetRenderDrawColor(gRenderer, currentTargetColor.r, currentTargetColor.g, currentTargetColor.b, currentTargetColor.a);
			SDL_RenderFillRect(gRenderer, &currentTargets[i].targetBox);
		}

		SDL_RenderPresent(gRenderer);

	}

	if (hardQuit) {
		return -1;
	}
	if (softQuit) {
		return 1;
	}
	else {
		return 0;
	}
}

void playMenu() {
	bool quit = false;
	bool wantsToPlay = false;
	SDL_Event e1;

	while (!quit) {
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(gRenderer);
		displayMenuText(highScore);
		SDL_RenderPresent(gRenderer);

		while (SDL_PollEvent(&e1) != 0) {
			if (e1.type == SDL_QUIT) {
				quit = true;
			}
			else if (e1.type = SDL_KEYDOWN) {
				switch (e1.key.keysym.sym) {
				case SDLK_RETURN:
					wantsToPlay = true;
					break;
				default:
					break;
				}
				

			}
		}

		if (wantsToPlay) {
			int gameState = playGame();		
			wantsToPlay = false;

			if (gameState < 0) {
				quit = true;
			}
		}

		if (score > highScore) {
			highScore = score;
		}

	}



}

int main(int argc, char* args[]) {
	if (!init()) {
		printf("Failed to initialize!");
	}
	else {
		playMenu();

	}
	close();
	return 0;


}