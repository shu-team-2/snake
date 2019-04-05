//---------------------------------------------------------------------------
//Program: Skeleton for Task 1c � group assignment
//Author: Pascale Vacher
//Last updated: 23 February 2018
//---------------------------------------------------------------------------

//Go to 'View > Task List' menu to open the 'Task List' pane listing the initial amendments needed to this program

//---------------------------------------------------------------------------
//----- include libraries
//---------------------------------------------------------------------------

//include standard libraries
#include <iostream>
#include <iomanip>
#include <conio.h>
#include <cassert>
#include <string>
#include <sstream>
#include <time.h> 
using namespace std;

//include our own libraries
#include "RandomUtils.h"  //for seed, random
#include "ConsoleUtils.h" //for clrscr, gotoxy, etc.
#include "TimeUtils.h"	//for getSystemTime, timeToString, etc.

//---------------------------------------------------------------------------
//----- define constants
//---------------------------------------------------------------------------

//defining the size of the grid
const int SIZEX(15); //horizontal dimension (length of border)
const int SIZEY(11); //vertical dimension (height of border)
//defining symbols used for display of the grid and content
const char SPOT('@');   //spot
const char TUNNEL(' '); //tunnel
const char WALL('#');   //border
const char HEAD('0');  // head of snake
const char BODY('o');  // body of snake
//defining the command letters to move the spot on the maze
const int UP(72);	//up arrow
const int DOWN(80);  //down arrow
const int RIGHT(77); //right arrow
const int LEFT(75);  //left arrow
//defining the other command letters
const char QUIT('Q'); //to end the game
const char CHEAT('C'); // to cheat 
string playerName; // global string for holding player names
bool cheatMode = false;
struct Item
{
	int x, y;
	char symbol;
};

//---------------------------------------------------------------------------
//----- run game
//---------------------------------------------------------------------------

int main()
{
	//function declarations (prototypes)
	void initialiseGame(char g[][SIZEX], char m[][SIZEX], Item &spot);
	void renderGame(const char g[][SIZEX], const string &mess);
	void updateGame(char g[][SIZEX], const char m[][SIZEX], Item &s, const int kc, string &mess);
	bool wantsToQuit(const int key);
	bool isArrowKey(const int k);
	bool isCheatKey(const int key); // creating cheat key prototype
	int getKeyPress();
	void endProgram();
	
	void activateCheat();
	void deactivateCheat();
		
	void getPlayerData(); // get player name prototype
	getPlayerData(); // get player name function call

	// local variable declarations
	char grid[SIZEY][SIZEX];		  // grid for display
	char maze[SIZEY][SIZEX];		  // structure of the maze
	Item spot = { 0, 0, SPOT };		  // spot's position and symbol
	string message("LET'S START..."); // current message to player

	//action...
	seed(); // seed the random number generator
	SetConsoleTitle("FoP 2018-19 - Task 1c - Game Skeleton");
	initialiseGame(grid, maze, spot); // initialise grid (incl. walls and spot)
	int key;						  // current key selected by player

	do // game loop
	{
		renderGame(grid, message); // display game info, modified grid and messages

		key = getKeyPress(); // read in  selected key: arrow or letter command
		if (isArrowKey(key))
			updateGame(grid, maze, spot, key, message);
		else if (isCheatKey(key) && cheatMode == true) // if the user presses 'C' to deactivate cheat mode
			deactivateCheat();
		else if (isCheatKey(key))
			activateCheat();		// calling the cheat function
		else
			message = "INVALID KEY!"; // set 'Invalid key' message
	} while (toupper(key) != QUIT);	  // while user does not want to quit

	renderGame(grid, message); // display game info, modified grid and messages
	endProgram();			   // display final message
	return 0;
}

//---------------------------------------------------------------------------
//----- initialise game state
//---------------------------------------------------------------------------

void initialiseGame(char grid[][SIZEX], char maze[][SIZEX], Item &spot)
{
	

	// initialise grid and place spot in middle
	void setInitialMazeStructure(char maze[][SIZEX]);
	void setSpotInitialCoordinates(Item & spot);
	void updateGrid(char g[][SIZEX], const char m[][SIZEX], const Item &i);

	setInitialMazeStructure(maze); // initialise maze
	setSpotInitialCoordinates(spot);
	updateGrid(grid, maze, spot); // prepare grid
}

void setSpotInitialCoordinates(Item &spot)
{
	// set spot coordinates inside the grid at random at beginning of game
	// TODO: Ensure Spot does not spwan on inner walls

	spot.y = random(SIZEY - 2); // vertical coordinate in range [1..(SIZEY - 2)]
	spot.x = random(SIZEX - 2); // horizontal coordinate in range [1..(SIZEX - 2)]
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

void updateGame(char grid[][SIZEX], const char maze[][SIZEX], Item &spot, const int keyCode, string &mess)
{
	// update game
	void updateGameData(const char g[][SIZEX], Item &s, const int kc, string &m);
	void updateGrid(char g[][SIZEX], const char maze[][SIZEX], const Item &s);
	updateGameData(grid, spot, keyCode, mess); //move spot in required direction
	updateGrid(grid, maze, spot);			   //update grid information
}
void updateGameData(const char g[][SIZEX], Item &spot, const int key, string &mess)
{
	// move spot in required direction
	bool isArrowKey(const int k);
	void setKeyDirection(int k, int &dx, int &dy);
	assert(isArrowKey(key));

	// reset message to blank
	mess = "";

	// calculate direction of movement for given key
	int dx(0), dy(0);
	setKeyDirection(key, dx, dy);

	// check new target position in grid and update game data (incl. spot coordinates) if move is possible
	switch (g[spot.y + dy][spot.x + dx])
	{				  //...depending on what's on the target position in grid...
	case TUNNEL:	  //can move
		spot.y += dy; //go in that Y direction
		spot.x += dx; //go in that X direction
		break;
	case WALL:		  //hit a wall and stay there
		mess = "CANNOT GO THERE!";
		break;
	}
}
void updateGrid(char grid[][SIZEX], const char maze[][SIZEX], const Item &spot)
{
	// update grid configuration after each move
	void placeMaze(char g[][SIZEX], const char b[][SIZEX]);
	void placeItem(char g[][SIZEX], const Item &spot);

	placeMaze(grid, maze); //reset the empty maze configuration into grid
	placeItem(grid, spot); //set spot in grid
}

void placeMaze(char grid[][SIZEX], const char maze[][SIZEX])
{
	// reset the empty/fixed maze configuration into grid
	for (int row(0); row < SIZEY; ++row)
		for (int col(0); col < SIZEX; ++col)
			grid[row][col] = maze[row][col];
}

void placeItem(char g[][SIZEX], const Item &item)
{
	// place item at its new position in grid
	g[item.y][item.x] = item.symbol;
}
//---------------------------------------------------------------------------
//----- process key
//---------------------------------------------------------------------------
void setKeyDirection(const int key, int &dx, int &dy)
{
	// calculate direction indicated by key
	bool isArrowKey(const int k);
	assert(isArrowKey(key));

	switch (key) //...depending on the selected key...
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
	// get key or command selected by user
	// KEEP THIS FUNCTION AS GIVEN
	int keyPressed;
	keyPressed = _getch();	//read in the selected arrow key or command letter
	while (keyPressed == 224) //ignore symbol following cursor key
		keyPressed = _getch();
	return keyPressed;
}

bool isArrowKey(const int key)
{
	// check if the key pressed is an arrow key (also accept 'K', 'M', 'H' and 'P')
	return (key == LEFT) || (key == RIGHT) || (key == UP) || (key == DOWN);
}
bool isCheatKey(const int key)
{
	// check if the key pressed is cheat key
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
void renderGame(const char g[][SIZEX], const string &mess)
{
	// display game title, messages, maze, spot and other items on screen
	string tostring(char x);
	string tostring(int x);
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string &message);
	void paintGrid(const char g[][SIZEX]);


	//TODO: Change the colour of the messages
	//display game title

	showMessage(clBlack, clYellow, 0, 0, "___GAME___");

	//TODO: Display date and time from the system
	showMessage(clWhite, clRed, 40, 0, "FoP Task 1c - February 2019   ");
	showMessage(clWhite, clRed, 40, 1, getTime()); //outputting current time
	
	showMessage(clWhite, clRed, 40, 2, getDate()); // outputting current date

	
	/*time_t now = time(0); // getting current system time
	tm *ltm = localtime(&now); // storing local time in tm attribute
	showMessage(clWhite, clRed, 40, 1, "Date: "); // showing the message that displays the current date
	cout << ltm->tm_mday << "/" << 1 + ltm->tm_mon << "/";
	cout << 1900 + ltm->tm_year << "\n";*/

	// displaying group and group members
	showMessage(clDarkBlue, clWhite, 40, 3, "SE3_8");
	showMessage(clDarkBlue, clWhite, 40, 4, "Joshua Sexton-Jones");
	showMessage(clDarkBlue, clWhite, 40, 5, "Chris Brewster");

	//display menu options available
	//TODO: Show other options availables when ready...
	showMessage(clRed, clYellow, 40, 6, "TO MOVE - USE KEYBOARD ARROWS ");
	showMessage(clRed, clYellow, 40, 7, "TO QUIT - ENTER 'Q'           ");
	showMessage(clRed, clYellow, 40, 8, "TO CHEAT - ENTER 'C'           ");
	//showMessage(clDarkBlue, clWhite, 40, 11, "CHEAT MODE: DISABLED.");

	
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

void activateCheat()
{
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);
	cheatMode = true;
	showMessage(clDarkBlue, clWhite, 40, 10, "CHEAT MODE: ENABLED.");
}

void deactivateCheat()
{
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);
	cheatMode = false;
	showMessage(clDarkBlue, clWhite, 40, 10, "CHEAT MODE: DISABLED.");

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
	showMessage(clRed, clYellow, 40, 8, "");
	system("pause"); //hold output screen until a keyboard key is hit
}
