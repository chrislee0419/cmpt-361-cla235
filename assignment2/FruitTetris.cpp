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
vec4 boardcolours[7200];

// location of vertex attributes in the shader program
GLuint vPosition;
GLuint vColor;

// locations of uniform variables in shader program
GLuint locxsize;
GLuint locysize;

GLuint mvp;

// VAO and VBO
// 0	- grid
// 1	- current piece
// 2 	- board
GLuint vaoIDs[3]; // One VAO for each object: the grid, the board, the current piece
// 0 to 1	- grid
// 2 to 3	- current piece
// 4 to 5 	- board
GLuint vboIDs[6]; // Two Vertex Buffer Objects for each VAO (specifying vertex positions and colours, respectively)

// trigger to pause game when it ends
bool endgame;

// used to tag fruits for deletion
bool fruittag[10][20];


//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------

// colours all faces of the block at position (x, y)
void colourBlock(int x, int y, vec4 colour) {
	for (int i = 0; i < 6; i++) {
		boardcolours[x*6 + 60*y + i] = colour;
		boardcolours[x*6 + 60*y + i + 1200] = colour;
		boardcolours[x*6 + 60*y + i + 2400] = colour;
		boardcolours[x*6 + 60*y + i + 3600] = colour;
		boardcolours[x*6 + 60*y + i + 4800] = colour;
		boardcolours[x*6 + 60*y + i + 6000] = colour;
	}
}

//-------------------------------------------------------------------------------------------------------------------

// When the current tile is moved or rotated (or created), update the VBO containing its vertex position data
void updatetile()
{
	// Bind the VBO containing current tile vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[2]); 

	// For each of the 4 'cells' of the tile,
	for (int i = 0; i < 4; i++) 
	{
		// Calculate the grid coordinates of the cell
		GLfloat x = tilepos.x + tile[i].x; 
		GLfloat y = tilepos.y + tile[i].y;

		// Create the 8 corners of the cube - these vertices are using location in pixels
		// These vertices are later converted by the vertex shader
		vec4 p1 = vec4(x * 1.0, y * 1.0, 0.0, 1); 
		vec4 p2 = vec4(x * 1.0, 1.0 + (y * 1.0), 0.0, 1);
		vec4 p3 = vec4(1.0 + (x * 1.0), y * 1.0, 0.0, 1);
		vec4 p4 = vec4(1.0 + (x * 1.0), 1.0 + (y * 1.0), 0.0, 1);
		vec4 p5 = vec4(x * 1.0, y * 1.0, 1.0, 1); 
		vec4 p6 = vec4(x * 1.0, 1.0 + (y * 1.0), 1.0, 1);
		vec4 p7 = vec4(1.0 + (x * 1.0), y * 1.0, 1.0, 1);
		vec4 p8 = vec4(1.0 + (x * 1.0), 1.0 + (y * 1.0), 1.0, 1);

		// Two points are used by two triangles each
		vec4 newpoints[36] = {
			p1, p2, p3, p2, p3, p4,
			p5, p6, p7, p6, p7, p8, 
			p5, p6, p1, p6, p1, p2, 
			p3, p4, p7, p4, p7, p8,
			p2, p6, p4, p6, p4, p8,
			p5, p1, p7, p1, p7, p3}; 

		// Put new data in the VBO
		glBufferSubData(GL_ARRAY_BUFFER, i*36*sizeof(vec4), 36*sizeof(vec4), newpoints); 
	}

	glBindVertexArray(0);
}

//-------------------------------------------------------------------------------------------------------------------

// Returns largest continuous space
// Should not return 0, as that means the row should be cleared
int checkrowspaces(int row)
{
	int max = 0, count = 0;

	for (int i = 0; i < 10; i++) {
		if (board[i][row])
			count = 0;
		else
			count++;
		if (count > max)
			max = count;
	}

	return max;
}

//-------------------------------------------------------------------------------------------------------------------

// Returns a pointer to a boolean array of size 11
// The first index is true if there is at least one valid position
// The next ten indices are true if the orientation will fit
// for that respective column
bool* checkorientationfit(int orientation)
{
	vec2 rotation[4];
	vec2 pos = vec2(0,19);
	int x, y, i, j;
	bool *result = new bool[11];

	// initialize first value of result to default
	result[0] = false;

	// copy the orientation you want to test
	for (i = 0; i < 4; i++)
		rotation[i] = AllRotations[orientation][i];

	// try to find a valid position for the specific orientation
	for (i = 0; i < 10; i++) {
		// test the orientation centred at (i, 19)
		pos[0] = i;
		for (j = 0; j < 4; j++) {
			x = rotation[j][0] + pos[0];
			y = rotation[j][1] + pos[1];

			// if there is an overlap with an existing block, position is not valid
			if (y < 20 && board[x][y])
				break;
			else if (x < 0 || x > 9)
				break;
		}

		// if the orientation can be placed without overlap, update the result array
		if (j == 4) {
			result[i+1] = true;
			result[0] = true;
		}
		else
			result[i+1] = false;
	}

	return result;

}

//-------------------------------------------------------------------------------------------------------------------

// Returns true if a new block is chosen, false indicates game end
bool chooseorientation() {
	// first, determine how wide the new piece can be
	int maxwidth = checkrowspaces(19);
	bool rotations[24];

	// if maxspace = 1, then only vertical "I" blocks can fit
	// so just test if it can even fit at all
	if (maxwidth == 1) {
		bool* check = checkorientationfit(21);

		// if it can't fit anywhere on the board, end the game and return;
		if ( !check[0] ) {
			delete[] check;
			return false;
		}

		// find a valid position
		int pos = rand() % 10;
		while ( !check[pos + 1] )
			pos = rand() % 10;

		// set the position
		tilepos[0] = pos;
		tilepos[1] = 19;

		// copy orientation to current tile
		for (int i = 0; i < 4; i++)
			tile[i] = AllRotations[21][i];

		delete[] check;
		return true;
	}

	// initializing rotations
	for (int i = 0; i < 24; i++)
		rotations[24] = true;

	// eliminate orientations that are too wide
	if (maxwidth < 4) {
		rotations[20] = false;
		rotations[22] = false;
	}
	if (maxwidth < 3) {
		for (int i = 0; i < 10; i++)
			rotations[2*i] = false;
	}
	// if both these if statements are triggered, maxwidth should be 2
	// (maxwidth = 1 was dealt with earlier)

	int pos, orientation, i;
	bool *check;

	while (true) {
		// choose a random orientation as the candidate new piece
		orientation = rand() % 24;
		// if the orientation chosen cannot fit, continue
		if ( !rotations[orientation] )
			continue;

		check = checkorientationfit(orientation);
		// if the candidate piece cannot fit, flag and continue
		if ( !check[0] ) {
			rotations[orientation] = false;
			delete[] check;

			// check if there are anymore valid orientations
			// if not, return with false;
			for (i = 0; i < 24; i++) {
				// valid rotation found
				if ( rotations[i] )
					break;
			}
			// no valid orientations
			if (i == 24)
				return false;

		}
		// otherwise, if it does fit, find a valid position
		else {
			pos = rand() % 10;
			while ( !check[pos + 1] )
				pos = rand() % 10;

			tilepos[0] = pos;
			tilepos[1] = 19;

			// copy orientation to current tile
			for (i = 0; i < 4; i++)
				tile[i] = AllRotations[orientation][i];

			delete[] check;
			return true;
		}
	}

}

//-------------------------------------------------------------------------------------------------------------------

// greys all laid pieces on the board to denote game end
// used only in newtile()
void greyboard() {
	int x, y;
	for (int i = 0; i < 200; i++) {
		x = i % 10;
		y = i / 10;
		if (board[x][y]) {
			colourBlock(x, y, grey);
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 7200*sizeof(vec4), boardcolours);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

//-------------------------------------------------------------------------------------------------------------------

// Called at the start of play and every time a tile is placed
void newtile()
{

	if ( !chooseorientation() ) {
		// no valid orientation, end the game
		endgame = true;
		greyboard();
	}

	updatetile(); 

	// Update the color VBO of current tile
	vec4 newcolours[144];
	vec4 blockcolour;
	for (int i = 0; i < 144; i+=36) {
		if (endgame)
			blockcolour = grey;
		else
			blockcolour = allColours[rand() % 5];
		for (int j = i; j < i+36; j++)
			newcolours[j] = blockcolour;
	}

	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[3]); // Bind the VBO containing current tile vertex colours
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(newcolours), newcolours); // Put the colour data in the VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

//-------------------------------------------------------------------------------------------------------------------

void initGrid()
{
	// ***Generate geometry data
	vec4 gridpoints[590];
	vec4 gridcolours[590]; // One colour per vertex

	// Front Side
	// Vertical lines 
	for (int i = 0; i < 11; i++){
		gridpoints[2*i] = vec4(1.0 * i, 0, 0, 1);
		gridpoints[2*i + 1] = vec4(1.0 * i, 20.0, 0, 1);
	}
	// Horizontal lines
	for (int i = 0; i < 21; i++){
		gridpoints[22 + 2*i] = vec4(0, 1.0 * i, 0, 1);
		gridpoints[22 + 2*i + 1] = vec4(10.0, 1.0 * i, 0, 1);
	}

	// Back Side
	// Vertical lines 
	for (int i = 0; i < 11; i++){
		gridpoints[64 + 2*i] = vec4(1.0 * i, 0, 0, 1);
		gridpoints[64 + 2*i + 1] = vec4(1.0 * i, 20.0, 0, 1);
	}
	// Horizontal lines
	for (int i = 0; i < 21; i++){
		gridpoints[86 + 2*i] = vec4(0, 1.0 * i, 0, 1);
		gridpoints[86 + 2*i + 1] = vec4(10.0, 1.0 * i, 0, 1);
	}

	// Z-lines
	for (int i = 0; i < 12; i++) {
		for (int j = 0; i < 22; i++) {
			gridpoints[128 + 22*i + 2*j] = vec4(1.0 * i, 1.0 * j, 0, 1);
			gridpoints[128 + 22*i + 2*j + 1] = vec4(1.0 * i, 1.0 * j, 1, 1);
		}
	}

	// Make all grid lines white
	for (int i = 0; i < 590; i++)
		gridcolours[i] = white;


	// *** set up buffer objects
	// Set up first VAO (representing grid lines)
	glBindVertexArray(vaoIDs[0]); // Bind the first VAO
	glGenBuffers(2, vboIDs); // Create two Vertex Buffer Objects for this VAO (positions, colours)

	// Grid vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[0]); // Bind the first grid VBO (vertex positions)
	glBufferData(GL_ARRAY_BUFFER, 590*sizeof(vec4), gridpoints, GL_STATIC_DRAW); // Put the grid points in the VBO
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0); 
	glEnableVertexAttribArray(vPosition); // Enable the attribute
	
	// Grid vertex colours
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[1]); // Bind the second grid VBO (vertex colours)
	glBufferData(GL_ARRAY_BUFFER, 590*sizeof(vec4), gridcolours, GL_STATIC_DRAW); // Put the grid colours in the VBO
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor); // Enable the attribute
}


void initBoard()
{
	// *** Generate the geometric data
	vec4 boardpoints[7200];

	for (int i = 0; i < 7200; i++) {
		// Let the empty cells on the board be black
		boardcolours[i] = white;
	}
	// Each cell is a square (2 triangles with 6 vertices)
	for (int i = 0; i < 20; i++){
		for (int j = 0; j < 10; j++)
		{		
			vec4 p1 = vec4(j * 1.0, i * 1.0, 0.0, 1);
			vec4 p2 = vec4(j * 1.0, 1.0 + (i * 1.0), 0.0, 1);
			vec4 p3 = vec4(1.0 + (j * 1.0), i * 1.0, 0.0, 1);
			vec4 p4 = vec4(1.0 + (j * 1.0), 1.0 + (i * 1.0), 0.0, 1);

			vec4 p5 = vec4(j * 1.0, i * 1.0, 1.0, 1);
			vec4 p6 = vec4(j * 1.0, 1.0 + (i * 1.0), 1.0, 1);
			vec4 p7 = vec4(1.0 + (j * 1.0), i * 1.0, 1.0, 1);
			vec4 p8 = vec4(1.0 + (j * 1.0), 1.0 + (i * 1.0), 1.0, 1);
			
			// front side points
			boardpoints[6*(10*i + j)    ] = p1;
			boardpoints[6*(10*i + j) + 1] = p2;
			boardpoints[6*(10*i + j) + 2] = p3;
			boardpoints[6*(10*i + j) + 3] = p2;
			boardpoints[6*(10*i + j) + 4] = p3;
			boardpoints[6*(10*i + j) + 5] = p4;
			// back side points
			boardpoints[6*(10*i + j) + 1200] = p5;
			boardpoints[6*(10*i + j) + 1201] = p6;
			boardpoints[6*(10*i + j) + 1202] = p7;
			boardpoints[6*(10*i + j) + 1203] = p6;
			boardpoints[6*(10*i + j) + 1204] = p7;
			boardpoints[6*(10*i + j) + 1205] = p8;
			// left side points
			boardpoints[6*(10*i + j) + 2400] = p5;
			boardpoints[6*(10*i + j) + 2401] = p6;
			boardpoints[6*(10*i + j) + 2402] = p1;
			boardpoints[6*(10*i + j) + 2403] = p6;
			boardpoints[6*(10*i + j) + 2404] = p1;
			boardpoints[6*(10*i + j) + 2405] = p2;
			// right side points
			boardpoints[6*(10*i + j) + 3600] = p3;
			boardpoints[6*(10*i + j) + 3601] = p4;
			boardpoints[6*(10*i + j) + 3602] = p7;
			boardpoints[6*(10*i + j) + 3603] = p4;
			boardpoints[6*(10*i + j) + 3604] = p7;
			boardpoints[6*(10*i + j) + 3605] = p8;
			// top side points
			boardpoints[6*(10*i + j) + 4800] = p2;
			boardpoints[6*(10*i + j) + 4801] = p5;
			boardpoints[6*(10*i + j) + 4802] = p4;
			boardpoints[6*(10*i + j) + 4803] = p5;
			boardpoints[6*(10*i + j) + 4804] = p4;
			boardpoints[6*(10*i + j) + 4805] = p8;
			// bottom side points
			boardpoints[6*(10*i + j) + 6000] = p5;
			boardpoints[6*(10*i + j) + 6001] = p1;
			boardpoints[6*(10*i + j) + 6002] = p7;
			boardpoints[6*(10*i + j) + 6003] = p1;
			boardpoints[6*(10*i + j) + 6004] = p7;
			boardpoints[6*(10*i + j) + 6005] = p3;
		}
	}

	// Initially no cell is occupied
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 20; j++) {
			board[i][j] = false; 
			fruittag[i][j] = false;
		}
	}

	mat4 m = mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, -5, -10, 0, 1);
	mat4 v = LookAt(vec4(0, 10, -10, 1), vec4(0, 0, 0, 1), vec4(0, 1, 1, 0));
	mat4 p = Perspective(45, (float)xsize/(float)ysize, 1.0, 50.0);
	mat4 mvp_mat = p * v * m;
	for (int i = 0; i < 7200; i+=36) {
		printf("orig [%d]: x=%f, y=%f, z=%f\n",
			i/36, boardpoints[i].x, boardpoints[i].y, boardpoints[i].z);
		printf("\t[%d]: x=%f, y=%f, z=%f\n",
			i/36+1, boardpoints[i+1].x, boardpoints[i+1].y, boardpoints[i+1].z);
		printf("\t[%d]: x=%f, y=%f, z=%f\n",
			i/36+2, boardpoints[i+2].x, boardpoints[i+2].y, boardpoints[i+2].z);
		vec4 res = mvp_mat * boardpoints[i];
		printf("\tnew: x=%f, y=%f, z=%f\n",
			res.x, res.y, res.z);
		res = mvp_mat * boardpoints[i+1];
		printf("\tnew: x=%f, y=%f, z=%f\n",
			res.x, res.y, res.z);
		res = mvp_mat * boardpoints[i+2];
		printf("\tnew: x=%f, y=%f, z=%f\n",
			res.x, res.y, res.z);
	}
	printf("mvp_mat:\n\t{%f, %f, %f, %f}\n\t{%f, %f, %f, %f}\n\t{%f, %f, %f, %f}\n\t{%f, %f, %f, %f}\n",
		mvp_mat[0][0], mvp_mat[0][1], mvp_mat[0][2], mvp_mat[0][3],
		mvp_mat[1][0], mvp_mat[1][1], mvp_mat[1][2], mvp_mat[1][3],
		mvp_mat[2][0], mvp_mat[2][1], mvp_mat[2][2], mvp_mat[2][3],
		mvp_mat[3][0], mvp_mat[3][1], mvp_mat[3][2], mvp_mat[3][3]);
	printf("m:\n\t{%f, %f, %f, %f}\n\t{%f, %f, %f, %f}\n\t{%f, %f, %f, %f}\n\t{%f, %f, %f, %f}\n",
		m[0][0], m[0][1], m[0][2], m[0][3],
		m[1][0], m[1][1], m[1][2], m[1][3],
		m[2][0], m[2][1], m[2][2], m[2][3],
		m[3][0], m[3][1], m[3][2], m[3][3]);
	printf("v:\n\t{%f, %f, %f, %f}\n\t{%f, %f, %f, %f}\n\t{%f, %f, %f, %f}\n\t{%f, %f, %f, %f}\n",
		v[0][0], v[0][1], v[0][2], v[0][3],
		v[1][0], v[1][1], v[1][2], v[1][3],
		v[2][0], v[2][1], v[2][2], v[2][3],
		v[3][0], v[3][1], v[3][2], v[3][3]);
	printf("p:\n\t{%f, %f, %f, %f}\n\t{%f, %f, %f, %f}\n\t{%f, %f, %f, %f}\n\t{%f, %f, %f, %f}\n",
		p[0][0], p[0][1], p[0][2], p[0][3],
		p[1][0], p[1][1], p[1][2], p[1][3],
		p[2][0], p[2][1], p[2][2], p[2][3],
		p[3][0], p[3][1], p[3][2], p[3][3]);


	// *** set up buffer objects
	glBindVertexArray(vaoIDs[2]);
	glGenBuffers(2, &vboIDs[4]);

	// Grid cell vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[4]);
	glBufferData(GL_ARRAY_BUFFER, 7200*sizeof(vec4), boardpoints, GL_STATIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	// Grid cell vertex colours
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]);
	glBufferData(GL_ARRAY_BUFFER, 7200*sizeof(vec4), boardcolours, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);
}

// No geometry for current tile initially
void initCurrentTile()
{
	glBindVertexArray(vaoIDs[1]);
	glGenBuffers(2, &vboIDs[2]);

	// Current tile vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[2]);
	glBufferData(GL_ARRAY_BUFFER, 144*sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	// Current tile vertex colours
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[3]);
	glBufferData(GL_ARRAY_BUFFER, 144*sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
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

	// Initialize the grid, board, and current tile
	initGrid();
	initBoard();
	initCurrentTile();

	// The location of the uniform variables in the shader program
	locxsize = glGetUniformLocation(program, "xsize"); 
	locysize = glGetUniformLocation(program, "ysize");
	mvp = glGetUniformLocation(program, "mvp");

	// Game initialization
	endgame = false;
	newtile(); // create new next tile

	// set to default
	glBindVertexArray(0);
	glClearColor(0, 0, 0, 0);

	glEnable(GL_DEPTH_TEST);
	srand(time(NULL));		// seed the rand() function
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
				// replace tagged fruits with tags from above
				fruittag[j][i] = fruittag[j][i+1];

				// replace row with colours of row above
				colourBlock(j, i, boardcolours[j*6 + (i+1)*60]);
			}
			// for top row
			else {
				board[j][i] = false;
				fruittag[j][i] = false;

				// replace top row with black
				colourBlock(j, i, black);
			}
		}
	}

	// refresh the colours in the buffer
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 7200*sizeof(vec4), boardcolours);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

//-------------------------------------------------------------------------------------------------------------------

// Places the current tile - update the board vertex colour VBO and the array maintaining occupied cells
void settile()
{
	int x, y;

	// retrieve colours from current piece VBO
	vec4 colours[24] = {0};
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[3]);
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(colours), colours);
	
	for (int i = 0; i < 4; i++) {
		x = tilepos[0] + tile[i][0];
		y = tilepos[1] + tile[i][1];

		// make sure the current block does not overflow off the top
		// if game has ended, allow overflow to prevent infinite loop
		if (y < 20 || endgame) {
			if (y < 20)
				board[x][y] = true;

			if (!endgame) {
				colourBlock(x, y, colours[i*6]);
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

	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 7200*sizeof(vec4), boardcolours);
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

// Compares input colour with board colour at coordinates (x,y)
bool colourcheck(vec4 colour, int x, int y) {
	return 	colour[0] == boardcolours[x*6 + y*60][0] &&
			colour[1] == boardcolours[x*6 + y*60][1] &&
			colour[2] == boardcolours[x*6 + y*60][2] &&
			colour[3] == boardcolours[x*6 + y*60][3];

}

//-------------------------------------------------------------------------------------------------------------------

// Tag consecutive fruits for deletion after checking rows
void tagfruits() {
	vec4 currentcol;
	int x, y;
	bool left, right, up, down;

	for (int i = 0; i < 200; i++) {
		x = i % 10;
		y = i / 10;

		// checks if the current block is on the board
		if ( !board[i%10][i/10] )
			continue;
		
		currentcol = boardcolours[x*6 + y*60];

		// check if near edges
		left = (x == 0);
		right = (x == 9);
		up = (y == 19);
		down = (y == 0);


		// check for threes where the current tile is the centre
		// corner to corner checks
		if ( !(up || down || left || right) ) {
			// NW to SE
			if (colourcheck(currentcol, x-1, y+1) &&
				colourcheck(currentcol, x+1, y-1)) {
				fruittag[x][y] = true;
				fruittag[x-1][y+1] = true;
				fruittag[x+1][y-1] = true;
			}
			// SW to NE
			if (colourcheck(currentcol, x-1, y-1) &&
				colourcheck(currentcol, x+1, y+1)) {
				fruittag[x][y] = true;
				fruittag[x-1][y-1] = true;
				fruittag[x+1][y+1] = true;
			}
		}
		// W to E check
		if (	!(left || right)  &&
				colourcheck(currentcol, x-1, y) &&
				colourcheck(currentcol, x+1, y)) {
			fruittag[x][y] = true;
			fruittag[x-1][y] = true;
			fruittag[x+1][y] = true;
		}
		// N to S check
		if (	!(up || down)  &&
				colourcheck(currentcol, x, y-1) &&
				colourcheck(currentcol, x, y+1)) {
			fruittag[x][y] = true;
			fruittag[x][y-1] = true;
			fruittag[x][y+1] = true;
		}

		// check for 3+ where the current tile is the end point
		// checking northbound
		if (y < 18 &&
			colourcheck(currentcol, x, y+1) &&
			colourcheck(currentcol, x, y+2)) {
			fruittag[x][y] = true;
			fruittag[x][y+1] = true;
			fruittag[x][y+2] = true;

			// if 3+
			for (int k = y+3; k < 20; k++) {
				if (colourcheck(currentcol, x, k))
					fruittag[x][k] = true;
				else
					break;
			}
		}
		// checking northeastbound
		if (y < 18 &&
			x < 8 &&
			colourcheck(currentcol, x+1, y+1) &&
			colourcheck(currentcol, x+2, y+2)) {
			fruittag[x][y] = true;
			fruittag[x+1][y+1] = true;
			fruittag[x+2][y+2] = true;

			// if 3+
			int k = y+3;
			for (int j = x+3; j < 10; j++) {
				if (k < 20) {
					if (colourcheck(currentcol, j, k))
						fruittag[j][k] = true;
					else
						break;
					k++;
				}
				else
					break;
			}
		}
		// checking eastbound
		if (x < 8 &&
			colourcheck(currentcol, x+1, y) &&
			colourcheck(currentcol, x+2, y)) {
			fruittag[x][y] = true;
			fruittag[x+1][y] = true;
			fruittag[x+2][y] = true;

			// if 3+
			for (int j = x+3; j < 10; j++) {
				if (colourcheck(currentcol, j, y))
					fruittag[j][y] = true;
				else
					break;
			}
		}
		// checking southeastbound
		if (y > 1 &&
			x < 8 &&
			colourcheck(currentcol, x+1, y-1) &&
			colourcheck(currentcol, x+2, y-2)) {
			fruittag[x][y] = true;
			fruittag[x+1][y-1] = true;
			fruittag[x+2][y-2] = true;

			// if 3+
			int k = y-3;
			for (int j = x+3; j < 10; j++) {
				if (k >= 0) {
					if (colourcheck(currentcol, j, k))
						fruittag[j][k] = true;
					else
						break;
					k--;
				}
				else
					break;
			}
		}
		// checking southbound
		if (y > 1 &&
			colourcheck(currentcol, x, y-1) &&
			colourcheck(currentcol, x, y-2)) {
			fruittag[x][y] = true;
			fruittag[x][y-1] = true;
			fruittag[x][y-2] = true;

			// if 3+
			for (int k = y-3; k >= 0; k--) {
				if (colourcheck(currentcol, x, k))
					fruittag[x][k] = true;
				else
					break;
			}
		}
		// checking southwestbound
		if (y > 1 &&
			x > 1 &&
			colourcheck(currentcol, x-1, y-1) &&
			colourcheck(currentcol, x-2, y-2)) {
			fruittag[x][y] = true;
			fruittag[x-1][y-1] = true;
			fruittag[x-2][y-2] = true;

			// if 3+
			int k = y-3;
			for (int j = x-3; j >= 0; j--) {
				if (k >= 0) {
					if (colourcheck(currentcol, j, k))
						fruittag[j][k] = true;
					else
						break;
					k--;
				}
				else
					break;
			}
		}
		// checking westbound
		if (x > 1 &&
			colourcheck(currentcol, x-1, y) &&
			colourcheck(currentcol, x-2, y)) {
			fruittag[x][y] = true;
			fruittag[x-1][y] = true;
			fruittag[x-2][y] = true;

			// if 3+
			for (int j = x-3; j >= 0; j--) {
				if (colourcheck(currentcol, j, y))
					fruittag[j][y] = true;
				else
					break;
			}
		}
		// checking northwestbound
		if (y < 18 &&
			x > 1 &&
			colourcheck(currentcol, x-1, y+1) &&
			colourcheck(currentcol, x-2, y+2)) {
			fruittag[x][y] = true;
			fruittag[x-1][y+1] = true;
			fruittag[x-2][y+2] = true;

			// if 3+
			int k = y+3;
			for (int j = x-3; j >= 0; j--) {
				if (k < 20) {
					if (colourcheck(currentcol, j, k))
						fruittag[j][k] = true;
					else
						break;
					k++;
				}
				else
					break;
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------

// Deletes the specified block and moves everything above it down
void deleteblock(int x, int y) {
	// shift down all blocks above the specified block
	for (int i = y; i < 20; i++) {
		// for every row except the top
		if (i != 19) {
			board[x][i] = board[x][i+1];
			// probably redundant, since deletetags() is called from top to bottom
			fruittag[x][i] = fruittag[x][i+1];

			// replace block with colours of block above
			colourBlock(x, y, boardcolours[x*6 + (i+1)*60]);
		}
		// for top row
		else {
			board[x][i] = false;
			fruittag[x][i] = false;

			// replace top block with black
			colourBlock(x, i, black);
		}
	}

	// refresh the colours in the buffer
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 7200*sizeof(vec4), boardcolours);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

//-------------------------------------------------------------------------------------------------------------------

// Remove the tagged fruits
void deletetags() {
	bool doublecheck = false;

	for (int x = 9; x >= 0; x--) {
		for (int y = 19; y >= 0; y--) {
			if (fruittag[x][y]) {
				deleteblock(x,y);
				doublecheck = true;
			}
		}
	}

	// checks whether the moved blocks now create more consecutive fruits
	if (doublecheck) {
		tagfruits();
		deletetags();
	}
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
			// tag fruits for deletion after checkfullrow()
			tagfruits();

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
			// tag fruits again after blocks were moved by checkfullrow()
			tagfruits();
			deletetags();

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

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUniform1i(locxsize, xsize); // x and y sizes are passed to the shader program to maintain shape of the vertices on screen
	glUniform1i(locysize, ysize);

	// projection matrix
	mat4 projection = Perspective(80.0, (float)xsize/(float)ysize, 0.5, 200.0);

	// camera/view matrix
	vec4 eye = vec4(0.0, 10.0, -20.0, 1.0);
	vec4 at = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 up = vec4(0.0, 1.0, 0.0, 0.0);
	mat4 view = LookAt(eye, at, up);

	// model matrix
	mat4 model = mat4(
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0
	);

	// combine matrices
	mat4 mvp_mat = projection * view * model;

	glUniformMatrix4fv(mvp, 1, GL_FALSE, mvp_mat);

	// Bind the VAO representing the grid cells (to be drawn first)
	glBindVertexArray(vaoIDs[2]);
	glDrawArrays(GL_TRIANGLES, 0, 7200);

	glBindVertexArray(vaoIDs[1]); // Bind the VAO representing the current tile (to be drawn on top of the board)
	glDrawArrays(GL_TRIANGLES, 0, 144); // Draw the current tile (8 triangles)

	glBindVertexArray(vaoIDs[0]); // Bind the VAO representing the grid lines (to be drawn on top of everything else)
	glDrawArrays(GL_LINES, 0, 590); // Draw the grid lines (21+11 = 32 lines)


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
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
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
