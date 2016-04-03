
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "chessboard.h"

// board attributes
Point board_centre = {0, 0, 0};			// centre position
float board_size = 2.0;					// length and width
float board_amb[] = {0.2, 0.2, 0.2};	// ambient
float board_dif[] = {0.3, 0.3, 0.3};	// diffuse
float board_spec[] = {0.9, 0.9, 0.9};	// specular
float board_shine = 20.0;				// shininess
float board_refl = 0.3;					// reflectance


// returns a board object with Spheres attributes
Spheres *create_board()
{
	Spheres *board;
	board = add_sphere(	board, board_centre, board_size,
						board_amb, board_dif, board_spec,
						board_shine, board_refl, 0);
	return board;
}

int check_inbounds(Spheres *board, Point hit)
{
	float centre = board->center;
	float size = board->radius;

	
}

// returns the t-parameter of the intersection with the board
float intersect_board(Point orig, Vector ray, Spheres *board, Point *hit)
{
	if (board == NULL)
		return -1.0;
	else if (board->index != 0)
		return -1.0;

	Vector board_norm = board_normal();
	float a = board_norm.x;
	float b = board_norm.y;
	float c = board_norm.z;
	float d = board->center.y;

	float numer = -( (orig.x*a) + (orig.y*b) + (orig.z*c) + d);
	float denom = 1.0 / vec_dot(board_norm, ray);

	float t = numer * denom;

	if (t < 0)
		return -1;
	else if()
	{

	}
	else
	{
		Point intersect = get_point(orig, vec_scale(ray, t));
		hit->x = intersect.x;
		hit->y = intersect.y;
		hit->z = intersect.z;
		return t;
	}
}

// returns the normal vector for the board;
Vector board_normal()
{
	Vector norm;
	norm.x = 0.0;
	norm.y = 1.0;
	norm.z = 0.0;
	return norm;
}
