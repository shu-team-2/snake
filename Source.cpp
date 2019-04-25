//include standard libraries
#include <iostream>
#include <iomanip>
#include <conio.h>
#include <cassert>
#include <string>
#include <sstream>
#include <time.h> 
#include <vector> // including the vector library for snake 
#include <algorithm>
#include <fstream> // including file streaming library for writing/reading data to .txt file

using namespace std;

//include our own libraries
#include "RandomUtils.h"  //for seed, random
#include "ConsoleUtils.h" //for clrscr, gotoxy, etc.
#include "TimeUtils.h"	//for getSystemTime, timeToString, etc.

//---------------------------------------------------------------------------
//----- define constants
//---------------------------------------------------------------------------

// defining the size of the grid
const int SIZEX(15); //horizontal dimension (length of border)
const int SIZEY(11); //vertical dimension (height of border)

// defining symbols used for display of the grid and content
const char HEAD('0');   //spot
const char TUNNEL(' '); //tunnel
const char WALL('#');   //wall
const char BORDER = char(254); // border
const char BODY('o');  // body of snake
const char MOUSE('@'); // mouse

const char POWERPILL('+'); // powerpill

// defining the command letters to move the spot on the maze
const int UP(72);	//up arrow
const int DOWN(80);  //down arrow
const int RIGHT(77); //right arrow
const int LEFT(75);  //left arrow

// defining the other command letters
const char QUIT('Q'); //to end the game
const char CHEAT('C'); // to cheat 
const char SAVE('S'); // to save
int miceCollected;

string playerName; // global string for holding player names

struct Item
{
	int x, y;
	char symbol;

	void place(char grid[][SIZEX]) { grid[y][x] = symbol; } // place item on grid

	void randomise(const char maze[][SIZEX]) // random spawn
	{
		do
		{
			x = random(SIZEX - 2); // exclude walls
			y = random(SIZEY - 2);
		} while (maze[y][x] != TUNNEL);
	}
};

//---------------------------------------------------------------------------
//----- run game
//---------------------------------------------------------------------------

int main()
{
	//function declarations (prototypes)
	void initialiseGame(char g[][SIZEX], char m[][SIZEX], vector<Item> &snake, Item &mouse, Item &pill, int &score, int &pillCounter);
	void renderGame(const char g[][SIZEX], const string &mess, const int &score, bool &invincible, vector<Item> &snake);
	void updateGame(char grid[][SIZEX], const char maze[][SIZEX], const int keyCode, string& mess, vector<Item> &snake, Item &mouse, Item &pill, int &score, int &pillCounter, int &snakeSize, bool &cheatMode, bool &invincible, int &invincibleCounter);
	bool wantsToQuit(const int key);
	bool isArrowKey(const int k);
	bool isCheatKey(const int key); // creating cheat keyCode prototype
	void saveGame(int &score);
	int getKeyPress();
	void endProgram();

	void activateCheat(char g[][SIZEX], vector<Item> &snake, int &snakeSize, bool &cheatMode);
	void deactivateCheat(char g[][SIZEX], vector<Item> &snake, int &snakeSize, bool &cheatMode);
	void checkPowerPill(char g[][SIZEX], Item &pill, vector<Item> &snake, bool &invincible);
	void getPlayerData(); // get player name prototype

	// local variable declarations
	char grid[SIZEY][SIZEX];		  // grid for display
	char maze[SIZEY][SIZEX];		  // structure of the maze

	// game items
	vector<Item> snake = {
		{0, 0, HEAD},
		{0, 0, BODY},
		{0, 0, BODY},
		{0, 0, BODY},
	};
	Item pill = { 0, 0, POWERPILL };
	Item mouse = { 0, 0, MOUSE };

	bool cheatMode(false); // detecting cheatmode
	bool invincible(false);; // detecting invincible mode
	int snakeSize(0);      // storing the snake length, so correct amount of body parts can be added to snake when cheat is deactivated
	int score(0);          // storing score
	int pillCounter(0);      // storing the counter of mice collected (resets at 2)
	int invincibleCounter(0); // storing the counter of the steps taken while invincible
	string message("LET'S START..."); // current message to player

	//action...
	getPlayerData(); // get player name function call

	seed(); // seed the random number generator
	SetConsoleTitle("FoP 2018-19 - Task 1c - Game Skeleton");
	initialiseGame(grid, maze, snake, mouse, pill, score, pillCounter); // initialise grid (incl. walls and spot)

	int keyCode; // current keyCode selected by player

	do // game loop
	{
		renderGame(grid, message, score, invincible, snake); // display game info, modified grid and messages

		keyCode = getKeyPress(); // read in  selected keyCode: arrow or letter command
		if (isArrowKey(keyCode))
		{
			updateGame(grid, maze, keyCode, message, snake, mouse, pill, score, pillCounter, snakeSize, cheatMode, invincible, invincibleCounter);
			checkPowerPill(grid, pill, snake, invincible); // checking to see if there is 2 mice collected
		}
		else if (isCheatKey(keyCode) && cheatMode) // if the user presses 'C' to deactivate cheat mode
			deactivateCheat(grid, snake, snakeSize, cheatMode);		// calling deactivate cheat function
		else if (isCheatKey(keyCode)) // if user presses 'C' keyCode and cheat mode is disabled
		{
			activateCheat(grid, snake, snakeSize, cheatMode);		// calling the cheat function
		}
		else if (keyCode == 'S') // if user chooses to save game
		{
			saveGame(score);
		}
			message = "INVALID KEY!"; // set 'Invalid keyCode' message
	} while (toupper(keyCode) != QUIT);	  // while user does not want to quit

	renderGame(grid, message, score, invincible, snake); // display game info, modified grid and messages
	endProgram();			   // display final message
	return 0;
}

//---------------------------------------------------------------------------
//----- initialise game state
//---------------------------------------------------------------------------

void initialiseGame(char grid[][SIZEX], char maze[][SIZEX], vector<Item> &snake, Item &mouse, Item &pill, int &score, int &pillCounter)
{
	// initialise grid and place spot in middle
	void setInitialMazeStructure(char maze[][SIZEX]);
	void updateGrid(
		char grid[][SIZEX], const char maze[][SIZEX],
		vector<Item> &snake, Item &mouse, Item &pill,
		int &score, int &pillCounter);

	setInitialMazeStructure(maze); // initialise maze

	for (Item &i : snake) // init snake in center
	{
		i.x = SIZEX / 2;
		i.y = SIZEY / 2;
	}

	mouse.randomise(maze); // random spawns
	pill.randomise(maze);

	updateGrid(grid, maze, snake, mouse, pill, score, pillCounter); // prepare grid
}

void setInitialMazeStructure(char maze[][SIZEX])
{
	// set the position of the walls in the maze
	// TODO: Amend initial maze configuration (change size changed and inner walls)
	// initialise maze configuration
	char initialMaze[SIZEY][SIZEX] 	//local array to store the maze structure
		= { { '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#','#' },
		{ '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		{ '#', ' ', ' ', '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		{ '#', ' ', ' ', ' ', ' ', '#', ' ', ' ', '#', ' ', ' ', ' ', '#', ' ', '#' },
		{ '#', ' ', ' ', ' ', ' ', '#', ' ', ' ', '#', ' ', ' ', ' ', ' ', ' ', '#' },
		{ '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		{ '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		{ '#', ' ', '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#', ' ', ' ', '#' },
		{ '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		{ '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		{ '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#','#' } };

	// with '#' for wall, ' ' for tunnel, etc.
	// copy into maze structure with appropriate symbols
	for (int row(0); row < SIZEY; ++row)
		for (int col(0); col < SIZEX; ++col)
			switch (initialMaze[row][col])
			{
				// not a direct copy, in case the symbols used are changed
			case '#':
				maze[row][col] = WALL;
				break;
			case ' ':
				maze[row][col] = TUNNEL;
				break;
			}
}

//---------------------------------------------------------------------------
//----- Update Game
//---------------------------------------------------------------------------

void updateGame(char grid[][SIZEX], const char maze[][SIZEX], const int keyCode, string& mess, vector<Item> &snake, Item &mouse, Item &pill, int &score, int &pillCounter, int &snakeSize, bool &cheatMode, bool &invincible, int &invincibleCounter)
{
	void updateGameData(const char g[][SIZEX], const int kc, string& m, vector<Item> &sn, Item &mouse, Item &pill, int &score, int &pillCounter, int &snakeSize, bool &cheatMode, bool &invincible, int &invincibleCounter);
	void updateGrid(
	char grid[][SIZEX], const char maze[][SIZEX],
	vector<Item> &snake, Item &mouse, Item &pill,
	int &score, int &pillCounter);

	updateGameData(grid, keyCode, mess, snake, mouse, pill, score, pillCounter, snakeSize, cheatMode, invincible, invincibleCounter); // move spot in required direction
	updateGrid(grid, maze, snake, mouse, pill, score, pillCounter);	 // update grid information
}
void updateGameData(const char grid[][SIZEX], const int key, string &mess, vector<Item> &snake, Item &mouse, Item &pill, int &score, int &pillCounter, int &snakeSize, bool &cheatMode, bool &invincible, int &invincibleCounter)
{
	// move spot in required direction
	bool wantsToQuit(int key); // prototype of wants to quit function
	bool isArrowKey(const int k);
	void setKeyDirection(int k, int &dx, int &dy, int &score);
	void eatMouse(const char maze[][SIZEX], vector<Item> &snake, Item &mouse, int &score, int &pillCounter, const bool &cheatMode);
	void eatPill(const char grid[][SIZEX], vector<Item> &snake, Item &pill, int &pillCounter, const bool &cheatMode, bool &invincible, int &invincibleCounter, int &score);

	assert(isArrowKey(key));
	void endProgram();

	// reset message to blank
	mess = "";
	//invincibleCounter = score; // getting current score, since invincible mode lasts 20 moves
	// calculate direction of movement for given keyCode
	int newX(0), newY(0); // new pos

	setKeyDirection(key, newX, newY, score);
	newX += snake.front().x;
	newY += snake.front().y;

	if (grid[newY][newX] == MOUSE) { eatMouse(grid, snake, mouse, score, pillCounter, cheatMode); }
	else if (grid[newY][newX] == POWERPILL) { 
		eatPill(grid, snake, pill, pillCounter, cheatMode, invincible, invincibleCounter, score); 
		

	}

	if (invincibleCounter + 20 == score)
	{
		invincible = false; // turning off invincible after 20 moves
	}

	if (invincible == true && grid[newY][newX] == WALL)
	{
		snake.front().x = SIZEX - 14; // moving snake to opposite side of grid if wall is hit
	}
	else if (invincible == true && grid[newY][newX] == WALL || grid[newY][newX] == BODY)
	{

	}
	else if (grid[newY][newX] == BORDER || grid[newY][newX] == WALL || grid[newY][newX] == BODY)
	{
		// TODO end game for body
		mess = "CANNOT GO THERE!";
		endProgram();
	}
	else
	{
		for (int i(snake.size() - 1); i > 0; --i) // shift snake 
		{
			snake.at(i).x = snake.at(i - 1).x; // shift item positions
			snake.at(i).y = snake.at(i - 1).y;
		}
		snake.front() = { newX, newY, HEAD }; // new head

		// other options checks
		
	}
}

void updateGrid(char grid[][SIZEX], const char maze[][SIZEX], vector<Item> &snake, Item &mouse, Item &pill, int &score, int &pillCounter)
{
	// update grid configuration after each move
	void placeMaze(char g[][SIZEX], const char b[][SIZEX]);
	void placeSnake(char grid[][SIZEX], vector <Item> &snake);

	placeMaze(grid, maze); //reset the empty maze configuration into grid
	placeSnake(grid, snake);
	mouse.place(grid); // place mouse

	// +1 offset prevents spawn when score==0
	if (pillCounter == 2) { pill.place(grid); }
}

void placeMaze(char grid[][SIZEX], const char maze[][SIZEX])
{
	// reset the empty/fixed maze configuration into grid
	for (int row(0); row < SIZEY; ++row)
		for (int col(0); col < SIZEX; ++col)
			grid[row][col] = maze[row][col];
}

void placeSnake(char grid[][SIZEX], vector <Item> &snake)
{
	// place snake backwards so head is visible on spawn
	for (int i(snake.size() - 1); i >= 0; --i) { snake.at(i).place(grid); }
}

// function for eating the mouse
void eatMouse(const char maze[][SIZEX], vector<Item> &snake, Item &mouse, int &score, int &pillCounter, const bool &cheatMode)
{
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);
	void endProgram();
	mouse.randomise(maze); // new mouse

	if (pillCounter >= 2)
	{
		pillCounter = 0; // resetting pill counter if a 3rd mouse is collected (since the pill needs to spawn after 2 mice are collected)
	}
	else
	{
		++pillCounter;
	}

	if (!cheatMode) // dont change snake when cheating
	{
		// use body explicitly in case of powerpill
		Item newBody = { snake.back().x, snake.back().y, BODY };

		snake.push_back(newBody); // 2 new bodys
		snake.push_back(newBody);
		++miceCollected; // increment values, also making it so the program only increments score when player is not cheating	
		if (miceCollected >= 10)
		{
			endProgram();  // ending the game when 10 mice are eaten
		}
	}
	else
	{
		score = 0;
	}

	showMessage(clRed, clYellow, 40, 16, "MOUSE CAUGHT!");
}
// function for eating the pill
void eatPill(const char grid[][SIZEX], vector<Item> &snake, Item &pill, int &pillCounter, const bool &cheatMode, bool &invincible, int &invincibleCounter, int &score)
{
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);
	showMessage(clDarkBlue, clWhite, 40, 15, "POWERPILL: CONSUMED");
	
	
	pill.randomise(grid); // new pill location
	invincibleCounter = score; // setting the counter to the score, since invicible mode must be deactivated after 20 steps
	invincible = true; // turning on invincible mode when snake eats pill
	// dont change snake when cheating
	if (!cheatMode) { snake.resize(4); } // just head

	pillCounter = 0; // reset pill counter
}

void checkPowerPill(char g[][SIZEX], Item &pill, vector<Item> &snake, bool &invincible)
{
	// TODO implement
}

void saveGame(int &score)
{
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);
	ofstream writeToFile; // creating an output file stream variable that will store the data to be saved
	writeToFile.open("saveFile.txt", ios::out); // opening the text file for saving
	if (writeToFile.fail())
	{
		showMessage(clDarkBlue, clWhite, 40, 20, "SAVE FAILED");
	}
	else
	{
		writeToFile << "Player Name: " << playerName.c_str() << endl;
		writeToFile << "Score: " << score << endl;
		writeToFile << "Mice Collected: " << miceCollected << endl;
	}


	writeToFile.close();
}

//---------------------------------------------------------------------------
//----- process keyCode
//---------------------------------------------------------------------------
void setKeyDirection(const int key, int &dx, int &dy, int &score)
{
	// calculate direction indicated by keyCode
	bool isArrowKey(const int k);
	assert(isArrowKey(key));

	switch (key) //...depending on the selected keyCode...
	{
	case LEFT:   //when LEFT arrow pressed...
		dx = -1; //decrease the X coordinate
		dy = 0;
		score++;
		break;
	case RIGHT:  //when RIGHT arrow pressed...
		dx = +1; //increase the X coordinate
		dy = 0;
		score++;
		break;
	case UP:	 // when UP arrow pressed...
		dx = 0;
		dy = -1; // decrease the Y coordinate
		score++;
		break;
	case DOWN:	 // when DOWN arrow pressed...
		dx = 0;
		dy = +1; // increase the Y coordinate
		score++;
		break;
	}
}

int getKeyPress()
{
	// get keyCode or command selected by user
	// KEEP THIS FUNCTION AS GIVEN
	int keyPressed;
	keyPressed = _getch();	//read in the selected arrow keyCode or command letter
	while (keyPressed == 224) //ignore symbol following cursor keyCode
		keyPressed = _getch();
	return keyPressed;
}

bool isArrowKey(const int key)
{
	// check if the keyCode pressed is an arrow keyCode (also accept 'K', 'M', 'H' and 'P')
	return (key == LEFT) || (key == RIGHT) || (key == UP) || (key == DOWN);
}
bool isCheatKey(const int key)
{

	// check if the keyCode pressed is cheat keyCode
	return (key == CHEAT);
}

//---------------------------------------------------------------------------
//----- display info on screen
//---------------------------------------------------------------------------

string tostring(int x)
{
	// convert an integer to a string
	std::ostringstream os;
	os << x;
	return os.str();
}
string tostring(char x)
{
	// convert a char to a string
	std::ostringstream os;
	os << x;
	return os.str();
}
void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string &message)
{
	// display a string using specified colour at a given position
	gotoxy(x, y);
	selectBackColour(backColour);
	selectTextColour(textColour);
	cout << message + string(40 - message.length(), ' ');
}
void renderGame(const char g[][SIZEX], const string &mess, const int &score, bool &invincible, vector<Item> &snake)
{
	// display game title, messages, maze, spot and other items on screen
	string tostring(char x);
	string tostring(int x);

	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string &message);
	void paintGrid(const char g[][SIZEX], vector<Item> &snake, bool &invincible);

	//display game title

	showMessage(clBlack, clYellow, 0, 0, "___GAME___");

	showMessage(clWhite, clRed, 40, 0, "FoP Task 1c - February 2019   ");
	showMessage(clWhite, clRed, 40, 1, getTime()); //outputting current time

	showMessage(clWhite, clRed, 40, 2, getDate()); // outputting current date


	// displaying group and group members
	showMessage(clDarkBlue, clWhite, 40, 3, "SE3_8");
	showMessage(clDarkBlue, clWhite, 40, 4, "Joshua Sexton-Jones");
	showMessage(clDarkBlue, clWhite, 40, 5, "Chris Brewster");

	//display menu options available
	showMessage(clRed, clYellow, 40, 6, "TO MOVE - USE KEYBOARD ARROWS ");
	showMessage(clRed, clYellow, 40, 7, "TO QUIT - ENTER 'Q'           ");
	showMessage(clRed, clYellow, 40, 8, "TO CHEAT - ENTER 'C'           ");
	showMessage(clRed, clYellow, 40, 14, "SCORE: " + tostring(score));
	showMessage(clRed, clYellow, 40, 15, "MICE COLLECTED: " + tostring(miceCollected) + "/10");

	if (invincible == true)
	{
		showMessage(clDarkBlue, clWhite, 40, 18, "INVINCIBLE MODE: ACTIVATED");
	}
	else
	{
		showMessage(clDarkBlue, clWhite, 40, 18, "INVINCIBLE MODE: DEACTIVATED");
	}

	//print auxiliary messages if any
	showMessage(clBlack, clWhite, 40, 11, mess); //display current message

	//display grid contents
	paintGrid(g, snake, invincible);
}

void getPlayerData()
{
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);

	//string name; // local variable for holding player name
	cout << "Enter your name (Max 20 characters): ";
	cin >> playerName;
	playerName.resize(20); // resizing the string to first 20 characters
	showMessage(clDarkGreen, clWhite, 40, 9, "PLAYER: " + playerName);
}

void activateCheat(char grid[][SIZEX], vector<Item> &snake, int &snakeSize, bool &cheatMode)
{
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);

	cheatMode = true; // setting global cheat bool to true

	snakeSize = static_cast<int>(snake.size());	 // getting the size of the snake

	snake.resize(4); // original size

	showMessage(clDarkBlue, clWhite, 40, 10, "CHEAT MODE: ENABLED.");

}

void deactivateCheat(char grid[][SIZEX], vector<Item> &snake, int &snakeSize, bool &cheatMode)
{
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);

	cheatMode = false;

	for (int i(0); i < snakeSize - 1; ++i) { snake.push_back(snake.back()); } // replace body

	showMessage(clDarkBlue, clWhite, 40, 10, "CHEAT MODE: DISABLED.");
}

void paintGrid(const char g[][SIZEX], vector<Item> &snake, bool &invincible)
{
	// display grid content on screen
	selectBackColour(clBlack);
	selectTextColour(clWhite);
	gotoxy(0, 2);
		

	// painting the grid, using different colours for different chars
	for (int row(0); row < SIZEY; ++row)
	{
		for (int col(0); col < SIZEX; ++col)
		{
			selectTextColour(clWhite);
			if (g[row][col] == '0' && invincible == true)
			{
				selectTextColour(clYellow);
			}
			else if (g[row][col] == '0')
			{
				selectTextColour(clRed);
			}
			else if (g[row][col] == 'o' && invincible == true)
			{
				selectTextColour(clRed);
			}
			else if (g[row][col] == 'o')
			{
				selectTextColour(clGreen);
			}
			else if (g[row][col] == '#')
			{
				selectTextColour(clBlue);
			}			
			else if (g[row][col] == '+')
			{
				selectTextColour(clDarkYellow);
			}
			else if (g[row][col] == '@')
			{
				selectTextColour(clMagenta);
			}
			cout << g[row][col]; //output cell content
		}
		cout << endl;
	}
}

void endProgram()
{
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string &message);

	//TODO: Display a message when user chooses to quit
	if (miceCollected == 10)
	{
		showMessage(clRed, clYellow, 40, 17, "YOU WIN!");
	}
	else
	{
		showMessage(clRed, clYellow, 40, 17, "YOU LOSE!");
	}
	system("pause"); //hold output screen until a keyboard keyCode is hit
	exit(0); // quitting the program 
}
