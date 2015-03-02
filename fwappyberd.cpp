#include <iostream>
#include <time.h>
#include <SDL.h>
#include <SDL_image.h>
#include <gfx/SDL2_gfxPrimitives.h>
#include <vector>
#include <cmath>

const float PI = 3.14159265;

using namespace std;

#include "boxCollision.hpp"

//ADAPTIVE??
const int windowWidth = 1000;//1000
const int windowHeight = 600;//600
const int FRAMERATE = 60;
const double FRAME_TIME = 1000/FRAMERATE;
const int pipeWidth = 0.75*windowWidth/5;//150
const int numberOfPipes = (windowWidth/(pipeWidth*3)) + 1;
int minPipeHeight = 40;//40
int gapSize = windowHeight/3;//200
int floorHeight = 40;//40
double gravity = 0.6; //0.6
double impulse  = -(gapSize/25);//-8
double impulseMax = gapSize/40;//5
int shipWidth = pipeWidth/2;//75
int shipHeight = gapSize/4;//50
int pipeSpeed = 6; //5

//SINGLE-FACTOR CHANGES/WHATEVER THE OPPOSITE OF ADAPTIVE IS
//const int windowWidth = 1000;
//const int windowHeight = 600;
//const int FRAMERATE = 60;
//const double FRAME_TIME = 1000/FRAMERATE;
//const int pipeWidth = 150;
//const int numberOfPipes = (windowWidth/(pipeWidth*3)) + 1;
//int minPipeHeight = 40;
//int gapSize = 200;
//int floorHeight = 40;
//double gravity = 0.6;
//double impulse  = -8;
//double impulseMax = 5;
//int shipWidth = 75;
//int shipHeight = 50;
//int pipeSpeed = 5;

struct ship {SDL_Rect rekt; double vSpeed;};


void applyGravity(ship* playerShip)
{
	playerShip->rekt.y += playerShip->vSpeed;
	playerShip->vSpeed += gravity;
}

void applyImpulse(ship* playerShip)
{
	playerShip->vSpeed = impulse;
	playerShip->rekt.y += playerShip->vSpeed;
}

SDL_Rect pipes[numberOfPipes*2];

SDL_Window *win = NULL;
SDL_Renderer *ren = NULL;

SDL_Texture *berdTex = NULL;
SDL_Texture *fwappyberdTex = NULL;
SDL_Texture *pipeTex = NULL;
SDL_Texture *bgTex = NULL;
SDL_Texture *groundTex = NULL;

SDL_Texture *loadTexture(string path)
{
	SDL_Texture *newTexture = NULL;

	SDL_Surface *loadedSurface = IMG_Load( path.c_str() );
    if( loadedSurface == NULL )
    {
        printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
    }
    else
    {
        //Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface(ren, loadedSurface );
        if( newTexture == NULL )
        {
            printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
        }

        //Get rid of old loaded surface
        SDL_FreeSurface( loadedSurface );
    }

	return newTexture;
}

bool init()
{
    bool initSuccess = true;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_EVERYTHING ) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        initSuccess = false;
    }
    else
    {
        //Create window
        win = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN );
        if( win == NULL )
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
            initSuccess = false;
        }
        else
        {
			ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (ren == NULL)
			{
				printf( "Renderer could not be created! SDL_Error: %s\n", SDL_GetError() );
				initSuccess = false;
			}
			else
			{
				SDL_SetRenderDrawColor(ren,0xff,0x00,0xff,0x00);

				int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					initSuccess = false;
				}		
			}
        }
    }

    return initSuccess;
}

bool loadMedia()
{
    //Loading success flag
    bool success = true;

 	berdTex = loadTexture("airship.png"); //use airshipBounded for visible boundaries
    if( berdTex == NULL )
    {
        printf( "Failed to load texture image!\n" );
        success = false;
    }

	fwappyberdTex = loadTexture("damShip.bmp");
	if( fwappyberdTex == NULL )
    {
        printf( "Failed to load texture image!\n" );
        success = false;
    }

	pipeTex = loadTexture("pipes.png");
	if( pipeTex == NULL )
    {
        printf( "Failed to load texture image!\n" );
        success = false;
    }

	bgTex = loadTexture("bg.jpg");
	if( bgTex == NULL )
    {
        printf( "Failed to load texture image!\n" );
        success = false;
    }

	groundTex = loadTexture("floor.png");
	if( groundTex == NULL )
    {
        printf( "Failed to load texture image!\n" );
        success = false;
    }

    return success;
}

void close()
{
    //Destroy textures
	SDL_DestroyTexture(berdTex);
	berdTex = NULL;

	SDL_DestroyTexture(fwappyberdTex);
	fwappyberdTex = NULL;

	SDL_DestroyTexture(pipeTex);
	pipeTex = NULL;

	SDL_DestroyTexture(bgTex);
	bgTex = NULL;

	SDL_DestroyTexture(groundTex);
	groundTex = NULL;

	//Destroy Renderer
	SDL_DestroyRenderer(ren);
	ren = NULL;

    //Destroy window
    SDL_DestroyWindow( win );
    win = NULL;

    //Quit SDL subsystems
	IMG_Quit();
    SDL_Quit();
}

float fRand()
{
	//dont forget to add srand(time(NULL)); to the start of the main method
	return rand()/(float)RAND_MAX;
}

void pipeGen()
{
	int pipePair = 0;
	for (int i = 0; i < numberOfPipes*2; i+=2)
	{
		
		pipes[i].x = windowWidth+pipePair*windowWidth/numberOfPipes + pipePair*(pipeWidth/numberOfPipes);
		pipes[i].y = 0;
		pipes[i].w = pipeWidth;
		//pipes[i].h = minPipeHeight + (fRand()*(windowHeight - (floorHeight+2*minPipeHeight))-minPipeHeight);
		pipes[i].h = minPipeHeight + fRand()*(windowHeight - floorHeight - 2*minPipeHeight - gapSize);

		pipes[i+1].x = pipes[i].x;
		pipes[i+1].y = pipes[i].h + gapSize;
		pipes[i+1].w = pipeWidth;
		pipes[i+1].h = windowHeight - gapSize - floorHeight - pipes[i].h;
		pipePair++;
	}
	pipePair = 0;
}

void pipeTransfer(int index)
{
	pipes[index].x = windowWidth+1;
	pipes[index].h = minPipeHeight + fRand()*(windowHeight - floorHeight - 2*minPipeHeight - gapSize);

	pipes[index+1].x = pipes[index].x;
	pipes[index+1].y = pipes[index].h + gapSize;
	pipes[index+1].h = windowHeight - gapSize - floorHeight - pipes[index].h;
}

int main(int argc, char* argv[]) {

	srand(time(NULL));
	
	if( !init() )
    {
        printf( "Failed to initialize!\n" );
		return 0;
    }
	if( !loadMedia() )
    {
        printf( "Failed to load media!\n" );
		return 9;
    }
	
	pipeGen();

	bool running = true;
	bool dead = false;
	bool inDaZone = false;
	bool scoreDisplayed = false;
	int score = 0;
	int impulseCounter = 0;

	ship titanic = {{pipeWidth, windowHeight/2-shipHeight/2,shipWidth,shipHeight},0};
	SDL_Rect ground = {0, windowHeight-floorHeight, windowWidth, floorHeight};
	SDL_Rect ceiling = {0,-1,windowWidth,2};
	applyImpulse(&titanic);
	while (running)	
	{
		int frameTimeStart = SDL_GetTicks();

		//read input
		SDL_Event ev;
		while(SDL_PollEvent(&ev) != 0) {
			if(ev.type == SDL_QUIT) 
			{
				running = false;
			}
			else if (ev.type == SDL_KEYDOWN)
			{
				if (ev.key.keysym.sym == SDLK_SPACE)
				{
					impulseCounter = impulseMax;
				}
				else if (ev.key.keysym.sym == SDLK_b && dead)
				{
					pipeGen();
					dead = false;
					titanic.rekt.x = pipeWidth;
					titanic.rekt.y = windowHeight/2-shipHeight/2;
					titanic.rekt.w = shipWidth;
					titanic.rekt.h = shipHeight;
					titanic.vSpeed = 0;
					score = 0;
					scoreDisplayed = false;
					applyImpulse(&titanic);
				}
				else if (ev.key.keysym.sym == SDLK_ESCAPE)
				{
					running = false;
				}
			}
		}

		//simulate
		if (impulseCounter > 0 && !dead)
		{
			applyImpulse(&titanic);
			impulseCounter--;
		}
		else
		{
			if (collisionDetected(titanic.rekt,2.5*titanic.vSpeed,ceiling,0))
			{
				dead =true;
			}
			if (collisionDetected(titanic.rekt, 2.5*titanic.vSpeed, ground, 0))
			{
				dead = true;
			}
			else //change this to an else
			{
				applyGravity(&titanic);
			}
		}

		//draw
		SDL_RenderClear(ren);
		
		SDL_RenderCopy(ren, bgTex,NULL,NULL);
		SDL_RenderCopy(ren, groundTex,NULL,&ground);
		for (int i = 0; i < numberOfPipes*2; i++)
		{
			SDL_Rect visiblePipe = {0,0,pipes[i].w,pipes[i].h};
			if (pipes[i].x <= 0-pipeWidth)
			{
				pipeTransfer(i);
			}
			if (!dead && collisionDetected(titanic.rekt,2.5*titanic.vSpeed,pipes[i],0))
			{
				dead = true;
				applyImpulse(&titanic);
			}
			if (!dead)
			{
				pipes[i].x-=pipeSpeed;
				if(i%2==0 && pipes[i].x < pipeWidth)
				{
					SDL_Rect pipeZone = {pipes[i].x,pipes[i].y,pipes[i].w,windowHeight};
					if (collisionDetected(titanic.rekt,2.5*titanic.vSpeed,pipeZone,0) && !inDaZone)
					{
						inDaZone = true;
					}
					else if(!collisionDetected(titanic.rekt,2.5*titanic.vSpeed,pipeZone,0) && inDaZone)
					{
						inDaZone = false;
						score++;
						cout << score << endl;
					}
				}
			}
			if (i%2==1)
			{
				SDL_RenderCopy(ren,pipeTex, &visiblePipe, &pipes[i]);
			}
			else
			{
				SDL_RenderCopyEx(ren,pipeTex, &visiblePipe, &pipes[i],0,NULL,SDL_FLIP_VERTICAL);
			}
		}
		if (!dead)
		{
			SDL_RenderCopyEx(ren,berdTex,NULL,&titanic.rekt,2.5*titanic.vSpeed,NULL,SDL_FLIP_NONE);
		}
		else
		{
			SDL_RenderCopyEx(ren,berdTex,NULL,&titanic.rekt,-7.5*titanic.vSpeed,NULL,SDL_FLIP_NONE);

		}

		SDL_RenderPresent(ren);		

		//display score lmao
		if(!scoreDisplayed && dead)
		{
			cout << "Your final score: " << score << endl;
			scoreDisplayed = true;
		}

		//delay
		if ((FRAME_TIME - (SDL_GetTicks() - frameTimeStart)) > 0)
		{
			SDL_Delay(FRAME_TIME - (SDL_GetTicks() - frameTimeStart));
		}
	}

	close();
	return 0;
}