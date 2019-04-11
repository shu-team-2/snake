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
const char WALL('#');   //border

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

string playerName; // global string for holding player names

struct Item
{
	int x, y;
	char symbol;

    void place(char grid[][SIZEX]) { grid[y][x] = symbol; }
};


//---------------------------------------------------------------------------
//----- run game
//---------------------------------------------------------------------------

int main()
{
	//function declarations (prototypes)
	void initialiseGame(char g[][SIZEX], char m[][SIZEX], vector<Item> &snake, Item &mouse, Item &pill, int &score, int &mouseCount, int &mousePill);
	void renderGame(const char g[][SIZEX], const string &mess, const int &score);
    void updateGame(char grid[][SIZEX], const char maze[][SIZEX], const int keyCode, string& mess, vector<Item> &snake, Item &mouse, Item &pill, int &score, int &mouseCount, int &mousePill, int &snakeSize);
	bool wantsToQuit(const int key);
	bool isArrowKey(const int k);
	bool isCheatKey(const int key); // creating cheat keyCode prototype
	int getKeyPress();
	void endProgram();

	void activateCheat(char g[][SIZEX], vector<Item> &snake, int &snakeSize, bool &cheatMode);
    void deactivateCheat(char g[][SIZEX], vector<Item> &snake, int &snakeSize, bool &cheatMode);
	void checkPowerPill(char g[][SIZEX], Item &pill, vector<Item> &snake);
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
    int snakeSize(0);      // storing the snake length, so correct amount of body parts can be added to snake when cheat is deactivated
    int score(0);          // storing score
    int mouseCount(0);     // storing the amount of mice collected
    int mousePill(0);      // storing the counter of mice collected (resets at 2)

	string message("LET'S START..."); // current message to player

	//action...
	getPlayerData(); // get player name function call

	seed(); // seed the random number generator
	SetConsoleTitle("FoP 2018-19 - Task 1c - Game Skeleton");
	initialiseGame(grid, maze, snake, mouse, pill, score, mouseCount, mousePill); // initialise grid (incl. walls and spot)
	
    int keyCode; // current keyCode selected by player

	do // game loop
	{
		renderGame(grid, message, score); // display game info, modified grid and messages

		keyCode = getKeyPress(); // read in  selected keyCode: arrow or letter command
		if (isArrowKey(keyCode))
		{
            // char grid[][SIZEX], const char maze[][SIZEX], const int keyCode, string& mess, vector<Item> &snake, Item &mouse, Item &pill, int &score, int &mouseCount, int &mousePill, int &snakeSize
            updateGame(grid, maze, keyCode, message, snake, mouse, pill, score, mouseCount, mousePill, snakeSize);
			checkPowerPill(grid, pill, snake); // checking to see if there is 2 mice collected
		}
		else if (isCheatKey(keyCode) && cheatMode) // if the user presses 'C' to deactivate cheat mode
			deactivateCheat(grid, snake, snakeSize, cheatMode);		// calling deactivate cheat function
		else if (isCheatKey(keyCode)) // if user presses 'C' keyCode and cheat mode is disabled
		{
			activateCheat(grid, snake, snakeSize, cheatMode);		// calling the cheat function
		}
		else
			message = "INVALID KEY!"; // set 'Invalid keyCode' message
	} while (toupper(keyCode) != QUIT);	  // while user does not want to quit

	renderGame(grid, message, score); // display game info, modified grid and messages
	endProgram();			   // display final message
	return 0;
}

//---------------------------------------------------------------------------
//----- initialise game state
//---------------------------------------------------------------------------

void initialiseGame(char grid[][SIZEX], char maze[][SIZEX], vector<Item> &snake, Item &mouse, Item &pill, int &score, int &mouseCount, int &mousePill)
{
	// initialise grid and place spot in middle
	void setInitialMazeStructure(char maze[][SIZEX]);
	void updateGrid(
        char grid[][SIZEX], const char maze[][SIZEX],
        vector<Item> &snake, Item &mouse, Item &pill,
        int &mousePill);

	setInitialMazeStructure(maze); // initialise maze

    // init snake, mouse, pill positions
	// TODO: Ensure Items cannot spwan on inner walls
    snake.front().x = random(SIZEX - 2);
    snake.front().y = random(SIZEY - 2);

    mouse.x = random(SIZEX - 2);
    mouse.y = random(SIZEY - 2);

    pill.x = random(SIZEX - 2);
    pill.y = random(SIZEY - 2);

	updateGrid(grid, maze, snake, mouse, pill, mousePill); // prepare grid
}

void setInitialMazeStructure(char maze[][SIZEX])
{
	// set the position of the walls in the maze
	// TODO: Amend initial maze configuration (change size changed and inner walls)
	// initialise maze configuration
	char initialMaze[SIZEY][SIZEX] 	//local array to store the maze structure
		= { { '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#' },
		{ '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		{ '#', ' ', ' ', '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		{ '#', ' ', ' ', ' ', ' ', '#', ' ', ' ', '#', ' ', ' ', ' ', '#', ' ', '#' },
		{ '#', ' ', ' ', ' ', ' ', '#', ' ', ' ', '#', ' ', ' ', ' ', ' ', ' ', '#' },
		{ '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		{ '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		{ '#', ' ', '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#', ' ', ' ', '#' },
		{ '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		{ '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		{ '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#' } };

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

void updateGame(char grid[][SIZEX], const char maze[][SIZEX], const int keyCode, string& mess, vector<Item> &snake, Item &mouse, Item &pill, int &score, int &mouseCount, int &mousePill, int &snakeSize)
{
	void updateGameData(const char g[][SIZEX], const int kc, string& m, vector<Item> &sn, Item &mouse, Item &pill, int &score, int &mouseCount, int &mousePill, int &snakeSize);
    void updateGrid(
        char grid[][SIZEX], const char maze[][SIZEX],
        vector<Item> &snake, Item &mouse, Item &pill,
        int &mousePill);

	updateGameData(grid, keyCode, mess, snake, mouse, pill, score, mouseCount, mousePill, snakeSize); // move spot in required direction
    updateGrid(grid, maze, snake, mouse, pill, mousePill);	 // update grid information
}
void updateGameData(const char g[][SIZEX], const int key, string &mess, vector<Item> &snake, Item &mouse, Item &pill, int &score, int &mouseCount, int &mousePill, int &snakeSize)
{
	// move spot in required direction
	bool wantsToQuit(int key); // prototype of wants to quit function
	bool isArrowKey(const int k);
	void setKeyDirection(int k, int &dx, int &dy);
	void eatMouse(vector<Item> &snake, Item &mouse, int &score, int &mouseCount, int &mousePill);
	void eatPill(vector<Item> &snake, Item &pill, int &snakeSize, int &mousePill);

	assert(isArrowKey(key));
	void endProgram();

	int size = static_cast<int>(snake.size());	 // getting the size of the snake
	// reset message to blank
	mess = "";

	// calculate direction of movement for given keyCode
	int dx(0), dy(0);
	setKeyDirection(key, dx, dy);

	// check new target position in grid and update game data (incl. spot coordinates) if move is possible
	switch (g[snake[0].y + dy][snake[0].x + dx])
	{				  //...depending on what's on the target position in grid...
	case TUNNEL:	  //can move

		for (int i = size - 1; i > 0; i--)
		{
			snake[i].y = snake[i - 1].y;
			snake[i].x = snake[i - 1].x;
		}

		snake[0].y += dy;
		snake[0].x += dx;

		break;
	case WALL:		  //hit a wall and stay there
		//mess = "CANNOT GO THERE!";
		// wantsToQuit(QUIT); // asking if user wants to quit after they hit wall 
		endProgram();
		break;
	case MOUSE:
		snake[0].y += dy;	//go in that Y direction
		snake[0].x += dx;	//go in that X direction
		eatMouse(snake, mouse, score, mouseCount, mousePill);
		break;
	case POWERPILL:
		snake[0].y += dy;	//go in that Y direction
		snake[0].x += dx;	//go in that X direction
		eatPill(snake, pill, snakeSize, mousePill);
		break;
	}
}

void updateGrid(char grid[][SIZEX], const char maze[][SIZEX], vector<Item> &snake, Item &mouse, Item &pill, int &mousePill)
{
	// update grid configuration after each move
	void placeMaze(char g[][SIZEX], const char b[][SIZEX]);
    void placeSnake(char grid[][SIZEX], vector <Item> &snake);

	placeMaze(grid, maze); //reset the empty maze configuration into grid
    placeSnake(grid, snake);
    mouse.place(grid); // place mouse
	if (mousePill == 2)
	{
        pill.place(grid);
	}
	else if (mousePill > 2)
	{
		mousePill = 0; // resetting mousepill when 3 mice are collected (since pill comes after 2 mice)
	}
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
    for (Item &i : snake) { i.place(grid); } // place snake
}

// function for eating the mouse
void eatMouse(vector<Item> &snake, Item &mouse, int &score, int &mouseCount, int &mousePill)
{
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);

	if (snake[0].x == mouse.x && snake[0].y == mouse.y)
	{
        snake.push_back( {0, 0, BODY} ); // new body
		//g[snake.back().y][snake.back().x] = snake.back().symbol;

		showMessage(clRed, clYellow, 40, 16, "MOUSE CAUGHT!");

        mouse.y = random(SIZEY - 2); // new mouse pos
        mouse.x = random(SIZEX - 2);
        
		++score; // increment values
		++mouseCount;
		++mousePill; 

	}
}
// function for eating the pill
void eatPill(vector<Item> &snake, Item &pill, int &snakeSize, int &mousePill)
{
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);
	snakeSize = static_cast<int>(snake.size());	 // getting the size of the snake

	if (snake[0].x == pill.x && snake[0].y == pill.y) // if the snake head eats the pill
	{
		mousePill = 0; // resetting value of mouse pill
		showMessage(clDarkBlue, clWhite, 40, 15, "POWERPILL: CONSUMED");

		snake.erase(snake.begin() + 1, snake.begin() + snakeSize -3); // erasing the body parts
	}

}

void checkPowerPill(char g[][SIZEX], Item &pill, vector<Item> &snake)
{
	// TODO implement
}

//---------------------------------------------------------------------------
//----- process keyCode
//---------------------------------------------------------------------------
void setKeyDirection(const int key, int &dx, int &dy)
{
	// calculate direction indicated by keyCode
	bool isArrowKey(const int k);
	assert(isArrowKey(key));

	switch (key) //...depending on the selected keyCode...
	{
	case LEFT:   //when LEFT arrow pressed...
		dx = -1; //decrease the X coordinate
		dy = 0;
		break;
	case RIGHT:  //when RIGHT arrow pressed...
		dx = +1; //increase the X coordinate
		dy = 0;
		break;
	case UP:	 // when UP arrow pressed...
		dx = 0;
		dy = -1; // decrease the Y coordinate
		break;
	case DOWN:	 // when DOWN arrow pressed...
		dx = 0;
		dy = +1; // increase the Y coordinate
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
void renderGame(const char g[][SIZEX], const string &mess, const int &score)
{
	// display game title, messages, maze, spot and other items on screen
	string tostring(char x);
	string tostring(int x);

	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string &message);
	void paintGrid(const char g[][SIZEX]);


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
	showMessage(clRed, clYellow, 40, 14, "Score: " + tostring(score));	

	//print auxiliary messages if any
	showMessage(clBlack, clWhite, 40, 11, mess); //display current message

	//display grid contents
	paintGrid(g);
}

void getPlayerData()
{
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);

	string name; // local variable for holding player name
	cout << "Enter your name (Max 20 characters): ";
	cin >> name;
	name.resize(20); // resizing the string to first 20 characters
	showMessage(clDarkGreen, clWhite, 40, 9, "PLAYER: " + name);
}

void activateCheat(char grid[][SIZEX], vector<Item> &snake, int &snakeSize, bool &cheatMode)
{
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);
    void placeSnake(char grid[][SIZEX], vector <Item> &snake);

	snakeSize = static_cast<int>(snake.size());	 // getting the size of the snake

	cheatMode = true; // setting global cheat bool to true
	showMessage(clDarkBlue, clWhite, 40, 10, "CHEAT MODE: ENABLED.");

	snake.erase(snake.begin() + 1, snake.begin() + snakeSize); // erasing the body parts
    
    placeSnake(grid, snake);
}

void deactivateCheat(char grid[][SIZEX], vector<Item> &snake, int &snakeSize, bool &cheatMode)
{
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);
    void placeSnake(char grid[][SIZEX], vector <Item> &snake);

	cheatMode = false;
	showMessage(clDarkBlue, clWhite, 40, 10, "CHEAT MODE: DISABLED.");
	//TODO: Deactivate Cheat Mode core functionality
	for (int i = 0; i < snakeSize - 1; i++)
	{
        snake.push_back( {0, 0, BODY} );
	}
    placeSnake(grid, snake);
}

void paintGrid(const char g[][SIZEX])
{
	// display grid content on screen
	selectBackColour(clBlack);
	selectTextColour(clWhite);
	gotoxy(0, 2);
	//TODO: Give a diferent colour to the symbol representing Spot
	for (int row(0); row < SIZEY; ++row)
	{
		for (int col(0); col < SIZEX; ++col)
			cout << g[row][col]; //output cell content
		cout << endl;
	}
}

void endProgram()
{
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string &message);

	//TODO: Display a message when user chooses to quit
	showMessage(clRed, clYellow, 40, 17, "");
	system("pause"); //hold output screen until a keyboard keyCode is hit
}
