#include <SDL.h>
#include <stdio.h>
#include <math.h>

// Initial global variables
const char mapWidth = 24, mapHeight = 24;
const int screenWidth = 1024;
const int screenHeight= 720;

int worldMap[mapWidth][mapHeight] =
{
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

// Window we'll be rendering too
SDL_Window* gWindow = NULL;

// window renderer
SDL_Renderer* gRenderer = NULL;


// Declare functions

// Starts up SDL and creates window
bool init();

// Initialize colors
void initColors();

// Loads media
bool loadMedia();

// Frees media and shuts down SDL
void close();

bool init()
{
	// Initialization flag
	bool success = true;

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		// Create window
		gWindow = SDL_CreateWindow("Raycaster", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			// Create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				// Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
			}
		}
	}
	return success;
}


void close()
{
	SDL_DestroyWindow(gWindow);
	SDL_Quit();
}


int main(int argc, char* args[])
{
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		double posX = 22, posY = 12;		// X and Y start position
		double dirX = -1, dirY = 0;			// Initial direction vector
		double planeX = 0, planeY = 1.0;	// 2D raycaster version of camera plane

		double time = 0;		// time of current frame
		double oldTime = 0;		// time of previous frame

		// Event handling
		bool quit = false;
		SDL_Event e;
		bool forwardPressed = false;
		bool backPressed = false;
		bool leftPressed = false;
		bool rightPressed = false;
		double oldDirX;
		double oldPlaneX;
		
		// Game loop
		while (!quit)
		{
			// Timing for input and FPS counter
			oldTime = time;
			time = SDL_GetTicks();
			double frameTime = (time - oldTime) / 1000.0;	// Time this frame has taken in seconds
			//printf("%f\n", 1.0 / frameTime);

			// speed modifiers
			double moveSpeed = frameTime * 5.0;
			double rotSpeed = frameTime * 3.0;

			while (SDL_PollEvent(&e) != 0)
			{
				switch (e.type)
				{ 
				case SDL_QUIT:
					quit = true;
					break;

				case SDL_KEYDOWN:
					switch (e.key.keysym.sym)
					{
					case SDLK_ESCAPE:
						quit = true;
						break;

					case SDLK_UP:
						forwardPressed = true;
						break; 

					case SDLK_DOWN:
						backPressed = true;
						break;

					case SDLK_RIGHT:
						rightPressed = true;
						break;

					case SDLK_LEFT:
						leftPressed = true;
						break;
					}
					break;

				case SDL_KEYUP:
					switch (e.key.keysym.sym)
					{
					case SDLK_UP:
						forwardPressed = false;
						break;
					
					case SDLK_DOWN:
						backPressed = false;
						break;

					case SDLK_RIGHT:
						rightPressed = false;
						break;

					case SDLK_LEFT:
						leftPressed = false;
						break;
					}
					break;
				}
			} // End of event polling

			// move forward if nothing in front
			if (forwardPressed)
			{
				if (worldMap[int(posY + dirY * moveSpeed)][int(posX)] == false) 
					posX += dirX * moveSpeed;
				if (worldMap[int(posY)][int(posX + dirX * moveSpeed)] == false) 
					posY += dirY * moveSpeed;
			}
			// move back if nothing behind
			if (backPressed)
			{
				if (worldMap[int(posY - dirY * moveSpeed)][int(posX)] == false) 
					posX -= dirX * moveSpeed;
				if (worldMap[int(posY)][int(posX - dirX * moveSpeed)] == false) 
					posY -= dirY * moveSpeed;
			}
			// rotate camera direction and camera plane to the right
			if (rightPressed)
			{
				oldDirX = dirX;
				dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
				dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
				oldPlaneX = planeX;
				planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
				planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
			}
			// rotate camera direction and camera plane to the left
			if (leftPressed)
			{
				oldDirX = dirX;
				dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
				dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
				oldPlaneX = planeX;
				planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
				planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
			}

			// Clear screen
			SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
			SDL_RenderClear(gRenderer);

			// Raycasting loop
			for (int x = 0; x < screenWidth; x++)
			{
				double cameraX = 2 * double(x) / double(screenWidth) - 1;	// map column on camera plane from -1 to 1
				double rayDirX = dirX + planeX * cameraX;		// Ray vector for this part of screen
				double rayDirY = dirY + planeY * cameraX;

				// which box of the map we're in 
				int mapX = int(posX);
				int mapY = int(posY);

				// length of ray from current pos to next x or y side
				double sideDistX, sideDistY;

				// length of ray from one x or y side to the next
				double deltaDistX = fabs(1 / rayDirX);
				double deltaDistY = fabs(1 / rayDirY);
				double perpWallDist;

				// what dir to step in x or y dir (either +1 or -1)
				int stepX, stepY;

				int hit = 0;	// was there a wall hit?
				int side;		// was it a NS wall or an EW wall?

				// Calculate step and initial sideDist
				if (rayDirX < 0)
				{
					stepX = -1;
					sideDistX = (posX - mapX) * deltaDistX;
				}
				else
				{
					stepX = 1;
					sideDistX = (mapX + 1.0 - posX) * deltaDistX;
				}
				if (rayDirY < 0)
				{
					stepY = -1;
					sideDistY = (posY - mapY) * deltaDistY;
				}
				else
				{
					stepY = 1;
					sideDistY = (mapY + 1.0 - posY) * deltaDistY;
				}

				// Perform DDA
				while (hit == 0)
				{
					// jump to next map square, in X or Y dir
					if (sideDistX < sideDistY)
					{
						sideDistX += deltaDistX;
						mapX += stepX;
						side = 0;
					}
					else
					{
						sideDistY += deltaDistY;
						mapY += stepY;
						side = 1;
					}
					// Test if wall is hit
					if (worldMap[mapY][mapX] > 0) hit = 1;
				} // end DDA

				// Calculate distance projected on camera direction
				if (side == 0) perpWallDist = (mapX - posX + (1 - stepX) / 2) / rayDirX;
				else           perpWallDist = (mapY - posY + (1 - stepY) / 2) / rayDirY;

				// Calculate height of line to draw to screen
				int lineHeight = (int)(screenHeight / perpWallDist);

				// calculate lowest and highest pixel to fill in current stripe
				int drawStart = -lineHeight / 2 + screenHeight / 2;
				if (drawStart < 0) drawStart = 0;
				int drawEnd = lineHeight / 2 + screenHeight / 2;
				if (drawEnd >= screenHeight) drawEnd = screenHeight;

				if (side == 1) SDL_SetRenderDrawColor(gRenderer, 120, 120, 120, 255);
				else           SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);

				// Draw vertical line
				SDL_RenderDrawLine(gRenderer, x, drawStart, x, drawEnd);

			} // End raycasting loop

			// Update screen
			SDL_RenderPresent(gRenderer);

		} // End of game loop
	}
	return 0;
}







