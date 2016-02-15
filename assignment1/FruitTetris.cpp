/*
CMPT 361 Assignment 1 - FruitTetris implementation Sample Skeleton Code

- This is ONLY a skeleton code showing:
How to use multiple buffers to store different objects
An efficient scheme to represent the grids and blocks

- Compile and Run:
Type make in terminal, then type ./FruitTetris

This code is extracted from Connor MacLeod's (crmacleo@sfu.ca) assignment submission
by Rui Ma (ruim@sfu.ca) on 2014-03-04. 

Modified in Sep 2014 by Honghua Li (honghual@sfu.ca).
*/

#include "include/Angel.h"
#include <cstdlib>
#include <iostream>
#include <ctime>

using namespace std;


// xsize and ysize represent the window size - updated if window is reshaped to prevent stretching of the game
int xsize = 400; 
int ysize = 720;

// current tile
vec2 tile[4]; // An array of 4 2d vectors representing displacement from a 'center' piece of the tile, on the grid
vec2 tilepos = vec2(5, 19); // The position of the current tile using grid coordinates ((0,0) is the bottom left corner)

// An array storing all possible orientations of all possible tiles
// The 'tile' array will always be some element [i][j] of this array (an array of vec2)
// 0 - 3	= Right "L"
// 4 - 7	= Left "L"
// 8 - 11	= Right "S" x 2
// 12 - 15	= Left "S" x 2
// 16 - 19	= "T"
// 20 - 23	= "I" x 2
vec2 AllRotations[24][4] = 
{
	{vec2(-1,-1), vec2(-1,0), vec2(0,0), vec2(1,0)},
	{vec2(1,-1), vec2(0,-1), vec2(0,0), vec2(0,1)},     
	{vec2(1,1), vec2(1,0), vec2(0,0), vec2(-1,0)},  
	{vec2(-1,1), vec2(0,1), vec2(0,0), vec2(0,-1)},

	{vec2(1,-1),vec2(1,0),vec2(0,0),vec2(-1,0)},
	{vec2(1,1),vec2(0,1),vec2(0,0),vec2(0,-1)},
	{vec2(-1,1),vec2(-1,0),vec2(0,0),vec2(1,0)},
	{vec2(-1,-1),vec2(0,-1),vec2(0,0),vec2(0,1)},

	{vec2(-1,0),vec2(0,0),vec2(0,-1),vec2(1,-1)},
	{vec2(0,-1),vec2(0,0),vec2(1,0),vec2(1,1)},
	{vec2(1,-1),vec2(0,-1),vec2(0,0),vec2(-1,0)},
	{vec2(1,1),vec2(1,0),vec2(0,0),vec2(0,-1)},

	{vec2(-1,-1),vec2(0,-1),vec2(0,0),vec2(1,0)},
	{vec2(1,-1),vec2(1,0),vec2(0,0),vec2(0,1)},
	{vec2(1,0),vec2(0,0),vec2(0,-1),vec2(-1,-1)},
	{vec2(0,1),vec2(0,0),vec2(1,0),vec2(1,-1)},

	{vec2(-1,0),vec2(0,0),vec2(1,0),vec2(0,-1)},
	{vec2(0,-1),vec2(0,0),vec2(0,1),vec2(1,0)},
	{vec2(1,0),vec2(0,0),vec2(-1,0),vec2(0,1)},
	{vec2(0,1),vec2(0,0),vec2(0,-1),vec2(-1,0)},

	{vec2(-2,0),vec2(-1,0),vec2(0,0),vec2(1,0)},
	{vec2(0,-2),vec2(0,-1),vec2(0,0),vec2(0,1)},
	{vec2(1,0),vec2(0,0),vec2(-1,0),vec2(-2,0)},
	{vec2(0,1),vec2(0,0),vec2(0,-1),vec2(0,-2)}
};

// colors
vec4 purple = 	vec4(0.5, 0.0, 1.0, 1.0);
vec4 red 	= 	vec4(1.0, 0.1, 0.1, 1.0);
vec4 yellow = 	vec4(0.8, 0.8, 0.0, 1.0);
vec4 green 	= 	vec4(0.1, 0.8, 0.1, 1.0);
vec4 orange = 	vec4(1.0, 0.5, 0.0, 1.0); 
vec4 white  = 	vec4(1.0, 1.0, 1.0, 1.0);
vec4 black  = 	vec4(0.0, 0.0, 0.0, 1.0);
vec4 grey 	= 	vec4(0.5, 0.5, 0.5, 1.0);

// colours array
vec4 allColours[5] = {purple, red, yellow, green, orange};
 
//board[x][y] represents whether the cell (x,y) is occupied
bool board[10][20]; 

//An array containing the colour of each of the 10*20*2*3 vertices that make up the board
//Initially, all will be set to black. As tiles are placed, sets of 6 vertices (2 triangles; 1 square)
//will be set to the appropriate colour in this array before updating the corresponding VBO
vec4 boardcolours[1200];

// location of vertex attributes in the shader program
GLuint vPosition;
GLuint vColor;

// locations of uniform variables in shader program
GLuint locxsize;
GLuint locysize;

// VAO and VBO
GLuint vaoIDs[3]; // One VAO for each object: the grid, the board, the current piece
GLuint vboIDs[6]; // Two Vertex Buffer Objects for each VAO (specifying vertex positions and colours, respectively)

// trigger to pause game when it ends
bool endgame;


//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------

// When the current tile is moved or rotated (or created), update the VBO containing its vertex position data
void updatetile()
{
	// Bind the VBO containing current tile vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[4]); 

	// For each of the 4 'cells' of the tile,
	for (int i = 0; i < 4; i++) 
	{
		// Calculate the grid coordinates of the cell
		GLfloat x = tilepos.x + tile[i].x; 
		GLfloat y = tilepos.y + tile[i].y;

		// Create the 4 corners of the square - these vertices are using location in pixels
		// These vertices are later converted by the vertex shader
		vec4 p1 = vec4(33.0 + (x * 33.0), 33.0 + (y * 33.0), .4, 1); 
		vec4 p2 = vec4(33.0 + (x * 33.0), 66.0 + (y * 33.0), .4, 1);
		vec4 p3 = vec4(66.0 + (x * 33.0), 33.0 + (y * 33.0), .4, 1);
		vec4 p4 = vec4(66.0 + (x * 33.0), 66.0 + (y * 33.0), .4, 1);

		// Two points are used by two triangles each
		vec4 newpoints[6] = {p1, p2, p3, p2, p3, p4}; 

		// Put new data in the VBO
		glBufferSubData(GL_ARRAY_BUFFER, i*6*sizeof(vec4), 6*sizeof(vec4), newpoints); 
	}

	glBindVertexArray(0);
}

//-------------------------------------------------------------------------------------------------------------------

// Called at the start of play and every time a tile is placed
void newtile()
{
	srand(time(NULL));		// seed the rand() function

	int orientation = rand() % 24;
	int pos = -1;
	bool overlap;

	// Update the geometry VBO of current tile
	for (int i = 0; i < 4; i++)
		tile[i] = AllRotations[orientation][i];

	// randomize starting position
	while (pos < 0) {
		pos = rand() % 10;
		tilepos = vec2(pos , 19); // Put the tile at the top of the board

		overlap = false;

		// check if pos causes any new tile to go over borders or existing blocks
		for (int i = 0; i < 4; i++) {
			int x = tilepos[0] + tile[i][0];
			int y = tilepos[1] + tile[i][1];

			// if a block is out of bounds
			if (x < 0 || x > 9) {
				pos = -1;
				break;
			}
			// if a block overlaps with an existing block, record overlap
			else if (y < 20 && board[x][y]) {
				overlap = true;
			}
			// tile[i] is within bounds
		}
		// current block is within bounds, but overlaps with an existing block
		if (pos > -1 && overlap)
			endgame = true;
	}

	updatetile(); 

	// Update the color VBO of current tile
	vec4 newcolours[24];
	vec4 blockcolour;
	for (int i = 0; i < 24; i+=6) {
		if (endgame)
			blockcolour = grey;
		else
			blockcolour = allColours[rand() % 5];
		newcolours[i] = blockcolour;
		newcolours[i+1] = blockcolour;
		newcolours[i+2] = blockcolour;
		newcolours[i+3] = blockcolour;
		newcolours[i+4] = blockcolour;
		newcolours[i+5] = blockcolour;
	}

	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]); // Bind the VBO containing current tile vertex colours
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(newcolours), newcolours); // Put the colour data in the VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

//-------------------------------------------------------------------------------------------------------------------

void initGrid()
{
	// ***Generate geometry data
	vec4 gridpoints[64]; // Array containing the 64 points of the 32 total lines to be later put in the VBO
	vec4 gridcolours[64]; // One colour per vertex
	// Vertical lines 
	for (int i = 0; i < 11; i++){
		gridpoints[2*i] = vec4((33.0 + (33.0 * i)), 33.0, 0, 1);
		gridpoints[2*i + 1] = vec4((33.0 + (33.0 * i)), 693.0, 0, 1);
		
	}
	// Horizontal lines
	for (int i = 0; i < 21; i++){
		gridpoints[22 + 2*i] = vec4(33.0, (33.0 + (33.0 * i)), 0, 1);
		gridpoints[22 + 2*i + 1] = vec4(363.0, (33.0 + (33.0 * i)), 0, 1);
	}
	// Make all grid lines white
	for (int i = 0; i < 64; i++)
		gridcolours[i] = white;


	// *** set up buffer objects
	// Set up first VAO (representing grid lines)
	glBindVertexArray(vaoIDs[0]); // Bind the first VAO
	glGenBuffers(2, vboIDs); // Create two Vertex Buffer Objects for this VAO (positions, colours)

	// Grid vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[0]); // Bind the first grid VBO (vertex positions)
	glBufferData(GL_ARRAY_BUFFER, 64*sizeof(vec4), gridpoints, GL_STATIC_DRAW); // Put the grid points in the VBO
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0); 
	glEnableVertexAttribArray(vPosition); // Enable the attribute
	
	// Grid vertex colours
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[1]); // Bind the second grid VBO (vertex colours)
	glBufferData(GL_ARRAY_BUFFER, 64*sizeof(vec4), gridcolours, GL_STATIC_DRAW); // Put the grid colours in the VBO
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor); // Enable the attribute
}


void initBoard()
{
	// *** Generate the geometric data
	vec4 boardpoints[1200];
	for (int i = 0; i < 1200; i++)
		boardcolours[i] = black; // Let the empty cells on the board be black
	// Each cell is a square (2 triangles with 6 vertices)
	for (int i = 0; i < 20; i++){
		for (int j = 0; j < 10; j++)
		{		
			vec4 p1 = vec4(33.0 + (j * 33.0), 33.0 + (i * 33.0), .5, 1);
			vec4 p2 = vec4(33.0 + (j * 33.0), 66.0 + (i * 33.0), .5, 1);
			vec4 p3 = vec4(66.0 + (j * 33.0), 33.0 + (i * 33.0), .5, 1);
			vec4 p4 = vec4(66.0 + (j * 33.0), 66.0 + (i * 33.0), .5, 1);
			
			// Two points are reused
			boardpoints[6*(10*i + j)    ] = p1;
			boardpoints[6*(10*i + j) + 1] = p2;
			boardpoints[6*(10*i + j) + 2] = p3;
			boardpoints[6*(10*i + j) + 3] = p2;
			boardpoints[6*(10*i + j) + 4] = p3;
			boardpoints[6*(10*i + j) + 5] = p4;
		}
	}

	// Initially no cell is occupied
	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 20; j++)
			board[i][j] = false; 


	// *** set up buffer objects
	glBindVertexArray(vaoIDs[1]);
	glGenBuffers(2, &vboIDs[2]);

	// Grid cell vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[2]);
	glBufferData(GL_ARRAY_BUFFER, 1200*sizeof(vec4), boardpoints, GL_STATIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	// Grid cell vertex colours
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[3]);
	glBufferData(GL_ARRAY_BUFFER, 1200*sizeof(vec4), boardcolours, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);
}

// No geometry for current tile initially
void initCurrentTile()
{
	glBindVertexArray(vaoIDs[2]);
	glGenBuffers(2, &vboIDs[4]);

	// Current tile vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[4]);
	glBufferData(GL_ARRAY_BUFFER, 24*sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	// Current tile vertex colours
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]);
	glBufferData(GL_ARRAY_BUFFER, 24*sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);
}

void init()
{
	// Load shaders and use the shader program
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	// Get the location of the attributes (for glVertexAttribPointer() calls)
	vPosition = glGetAttribLocation(program, "vPosition");
	vColor = glGetAttribLocation(program, "vColor");

	// Create 3 Vertex Array Objects, each representing one 'object'. Store the names in array vaoIDs
	glGenVertexArrays(3, &vaoIDs[0]);

	// Initialize the grid, the board, and the current tile
	initGrid();
	initBoard();
	initCurrentTile();

	// The location of the uniform variables in the shader program
	locxsize = glGetUniformLocation(program, "xsize"); 
	locysize = glGetUniformLocation(program, "ysize");

	// Game initialization
	endgame = false;
	newtile(); // create new next tile

	// set to default
	glBindVertexArray(0);
	glClearColor(0, 0, 0, 0);
}

//-------------------------------------------------------------------------------------------------------------------

// Checks if the specified row (0 is the bottom 19 the top) is full
// If every cell in the row is occupied, it will clear that cell and everything above it will shift down one row
void checkfullrow(int row)
{
	int i;

	for (i = 0; i < 10; i++) {
		if ( !board[i][row] )
			break;
	}
	// if the row is not full, return
	if (i != 10)
		return;

	// shift down all rows above the specified row
	for (i = row; i < 20; i++) {
		// for each block in the row
		for (int j = 0; j < 10; j++) {
			// for every row except for the top
			if (i != 19) {
				board[j][i] = board[j][i+1];

				// replace row with colours of row above
				for (int k = 0; k < 6; k++)
					boardcolours[j*6 + i*60 + k] = boardcolours[j*6 + (i+1)*60 + k];
			}
			// for top row
			else {
				board[j][i] = false;

				// replace top row with black
				for (int k = 0; k < 6; k++)
					boardcolours[j*6 + i*60 + k] = black;
			}
		}
	}

	// refresh the colours in the buffer
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[3]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec4)*1200, boardcolours);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

//-------------------------------------------------------------------------------------------------------------------

// Places the current tile - update the board vertex colour VBO and the array maintaining occupied cells
void settile()
{
	int x, y, currentblock;

	// retrieve colours from current piece VBO
	vec4 colours[24] = {0};
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]);
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(colours), colours);
	
	for (int i = 0; i < 4; i++) {
		x = tilepos[0] + tile[i][0];
		y = tilepos[1] + tile[i][1];

		// make sure the current block does not overflow off the top
		// if game has ended, allow overflow to prevent infinite loop
		if (y < 20 || endgame) {
			if (y < 20)
				board[x][y] = true;

			currentblock = x*6 + y*60;
			if (!endgame) {
				for (int j = 0; j < 6; j++)
					boardcolours[currentblock + j] = colours[i*6];
			}
		}
		// if it does overflow, end the game
		else {
			endgame = true;
			i = -1;
		}
	}

	// set current tile to grey if endgame was triggered (show losing block)
	if (endgame) {
		vec4 greycolour[24];
		for (int i = 0; i < 24; i++)
			greycolour[i] = grey;
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(greycolour), greycolour);
	}

	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[3]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 1200*sizeof(vec4), boardcolours);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

//-------------------------------------------------------------------------------------------------------------------

// Given (x,y), tries to move the tile x squares to the right and y squares down
// Returns true if the tile was successfully moved, or false if there was some issue
bool movetile(vec2 direction)
{
	vec2 newblockpos;
	for (int i = 0; i < 4; i++) {
		newblockpos = tilepos + tile[i] + direction;

		// check if it has hit a block
		if (board[(int)(newblockpos[0])][(int)(newblockpos[1])])
			return false;

		// check if it has hit the side of the board
		else if (newblockpos[0] < 0 || newblockpos[0] > 9)
			return false;

		// check if it has hit the bottom of the board
		else if (newblockpos[1] < 0)
			return false;
	}
	tilepos += direction;
	updatetile();
	return true;
}

//-------------------------------------------------------------------------------------------------------------------

// Rotates the current tile, if there is room
void rotate()
{      
	int i, j;
	// check to see what block and orientation it currently is
	for (i = 0; i < 24; i++) {
		for (j = 0; j < 4; j++) {
			if (AllRotations[i][j][0] != tile[j][0]) break;
			else if (AllRotations[i][j][1] != tile[j][1]) break;
		}
		if (j == 4) break;
	}
	if (i == 24) return;

	// find the next orientation
	i++;
	if (i % 4 == 0)
		i -= 4;

	// copy arrays
	vec2 oldorientation[4];
	for (j = 0; j < 4; j++) {
		oldorientation[j] = tile[j];
		tile[j] = AllRotations[i][j];
	}

	// check if anything obstructs the rotation
	if ( !movetile(vec2(0,0)) ) {
		for (j = 0; j < 4; j++)
			tile[j] = oldorientation[j];
	}
}

//-------------------------------------------------------------------------------------------------------------------

void timer(int value) {
	// if no top out, continue the game
	if ( !endgame && !movetile(vec2(0,-1)) ) {
		settile();

		// if no partial lock out, check for completed rows, then create a new tile
		if (!endgame) {
			// record which rows to check
			bool checkrow[4] = {false, false, false, false};
			for (int i = 0; i < 4; i++) {
				if (tile[i][1] == 1)
					checkrow[0] = true;
				else if (tile[i][1] == 0)
					checkrow[1] = true;
				else if (tile[i][1] == -1)
					checkrow[2] = true;
				else
					checkrow[3] = true;
			}
			// use checkfullrow(), starting with the topmost block filled
			for (int i = 0; i < 4; i++) {
				if (checkrow[i])
					checkfullrow(tilepos[1] + 1 - i);
			}

			newtile();
		}
	}

	glutPostRedisplay();
	glutTimerFunc(700, timer, 0);
}

//-------------------------------------------------------------------------------------------------------------------

// Starts the game over - empties the board, creates new tiles, resets line counters
void restart()
{
	initBoard();

	endgame = false;
	newtile();
}

//-------------------------------------------------------------------------------------------------------------------

// Draws the game
void display()
{

	glClear(GL_COLOR_BUFFER_BIT);

	glUniform1i(locxsize, xsize); // x and y sizes are passed to the shader program to maintain shape of the vertices on screen
	glUniform1i(locysize, ysize);

	glBindVertexArray(vaoIDs[1]); // Bind the VAO representing the grid cells (to be drawn first)
	glDrawArrays(GL_TRIANGLES, 0, 1200); // Draw the board (10*20*2 = 400 triangles)

	glBindVertexArray(vaoIDs[2]); // Bind the VAO representing the current tile (to be drawn on top of the board)
	glDrawArrays(GL_TRIANGLES, 0, 24); // Draw the current tile (8 triangles)

	glBindVertexArray(vaoIDs[0]); // Bind the VAO representing the grid lines (to be drawn on top of everything else)
	glDrawArrays(GL_LINES, 0, 64); // Draw the grid lines (21+11 = 32 lines)


	glutSwapBuffers();
}

//-------------------------------------------------------------------------------------------------------------------

// Reshape callback will simply change xsize and ysize variables, which are passed to the vertex shader
// to keep the game the same from stretching if the window is stretched
void reshape(GLsizei w, GLsizei h)
{
	xsize = w;
	ysize = h;
	glViewport(0, 0, w, h);
}

//-------------------------------------------------------------------------------------------------------------------

// Handle arrow key keypresses
void special(int key, int x, int y)
{
	// only record these keys if game has not ended
	if (!endgame) {
		switch(key) {
			case GLUT_KEY_UP:
				rotate();
				break;
			case GLUT_KEY_DOWN:
				movetile(vec2(0,-1));
				break;
			case GLUT_KEY_LEFT:
				movetile(vec2(-1,0));
				break;
			case GLUT_KEY_RIGHT:
				movetile(vec2(1,0));
				break;
		}
	}
	glutPostRedisplay();
}

//-------------------------------------------------------------------------------------------------------------------

// Handles standard keypresses
void keyboard(unsigned char key, int x, int y)
{
	switch(key) 
	{
		case 033: // Both escape key and 'q' cause the game to exit
		    exit(EXIT_SUCCESS);
		    break;
		case 'q':
			exit (EXIT_SUCCESS);
			break;
		case 'r': // 'r' key restarts the game
			restart();
			break;
	}
	glutPostRedisplay();
}

//-------------------------------------------------------------------------------------------------------------------

void idle(void)
{
	glutPostRedisplay();
}

//-------------------------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(xsize, ysize);
	glutInitWindowPosition(680, 178); // Center the game window (well, on a 1920x1080 display)
	glutCreateWindow("Fruit Tetris");
	glewInit();
	init();

	// Callback functions
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutSpecialFunc(special);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutTimerFunc(700, timer, 0);

	glutMainLoop(); // Start main loop
	return 0;
}
