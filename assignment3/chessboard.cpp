
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "chessboard.h"

// board attributes
Point board_centre = {0, -1.75, -3};	// centre position
float board_size = 2.0;					// length and width
float board_amb[] = {0.2, 0.2, 0.2};	// ambient
float board_dif[] = {0.3, 0.3, 0.3};	// diffuse
float board_spec[] = {0.9, 0.9, 0.9};	// specular
float board_shine = 20.0;				// shininess
float board_refl = 0.3;					// reflectance

float board_amb_alt[] = {0.1, 0.1, 0.1};	// ambient
float board_dif_alt[] = {0.15, 0.15, 0.15};	// diffuse
float board_spec_alt[] = {0.5, 0.5, 0.5};	// specular


// returns a board object with Spheres attributes to the scene
Spheres *create_board(Spheres *scene)
{
	Spheres *board;
	board = add_sphere(	scene, board_centre, board_size,
						board_amb, board_dif, board_spec,
						board_shine, board_refl, 0);
	return board;
}

int check_inbounds(Spheres *board, Point hit)
{
	Point centre = board->center;
	float size = board->radius;

	if (hit.x < centre.x-size || hit.x > centre.x+size)
		return 0;
	else if (hit.z < centre.z-size || hit.z > centre.z+size)
		return 0;
	else if (hit.y != centre.y)
		return 0;
	else
		return 1;
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

	Point intersect = get_point(orig, vec_scale(ray, t));
	if( check_inbounds(board, intersect) == 0 )
		return -1;
	else
	{
		hit->x = intersect.x;
		hit->y = intersect.y;
		hit->z = intersect.z;
		return t;
	}
}

// changes the colour of the board depending on hit location
void board_colour(Spheres *board, Point hit)
{
	if (board->index != 0)
		return;
	if ( check_inbounds(board, hit) == 0 )
		return;

	Point centre = board->center;
	float size = board->radius;

	Point origin = {centre.x-size, centre.y, centre.x+size};

	int i;
	for (i = 0; i < 64; i++)
	{
		int x = i % 8;
		int x1 = origin.x + (x*2*size);
		int x2 = x1 + size/4.0;

		int z = i / 8;
		int z1 = origin.z - (z+1*2*size);

		if (hit.z < z1)
		{
			i += 7;
			continue;
		}
		else if (hit.x < x1 || hit.x > x2)
			continue;
		else
			break;
	}

	// even rows
	if ( (i/8)%2 == 0 )
	{
		if (i%2 == 0)
		{
			(board->mat_ambient)[0] = board_amb[0];
			(board->mat_ambient)[1] = board_amb[1];
			(board->mat_ambient)[2] = board_amb[2];
			(board->mat_diffuse)[0] = board_dif[0];
			(board->mat_diffuse)[1] = board_dif[1];
			(board->mat_diffuse)[2] = board_dif[2];
			(board->mat_specular)[0] = board_spec[0];
			(board->mat_specular)[1] = board_spec[1];
			(board->mat_specular)[2] = board_spec[2];
		}
		else
		{
			(board->mat_ambient)[0] = board_amb_alt[0];
			(board->mat_ambient)[1] = board_amb_alt[1];
			(board->mat_ambient)[2] = board_amb_alt[2];
			(board->mat_diffuse)[0] = board_dif_alt[0];
			(board->mat_diffuse)[1] = board_dif_alt[1];
			(board->mat_diffuse)[2] = board_dif_alt[2];
			(board->mat_specular)[0] = board_spec_alt[0];
			(board->mat_specular)[1] = board_spec_alt[1];
			(board->mat_specular)[2] = board_spec_alt[2];
		}
	}
	else
	{
		if (i%2 == 1)
		{
			(board->mat_ambient)[0] = board_amb[0];
			(board->mat_ambient)[1] = board_amb[1];
			(board->mat_ambient)[2] = board_amb[2];
			(board->mat_diffuse)[0] = board_dif[0];
			(board->mat_diffuse)[1] = board_dif[1];
			(board->mat_diffuse)[2] = board_dif[2];
			(board->mat_specular)[0] = board_spec[0];
			(board->mat_specular)[1] = board_spec[1];
			(board->mat_specular)[2] = board_spec[2];
		}
		else
		{
			(board->mat_ambient)[0] = board_amb_alt[0];
			(board->mat_ambient)[1] = board_amb_alt[1];
			(board->mat_ambient)[2] = board_amb_alt[2];
			(board->mat_diffuse)[0] = board_dif_alt[0];
			(board->mat_diffuse)[1] = board_dif_alt[1];
			(board->mat_diffuse)[2] = board_dif_alt[2];
			(board->mat_specular)[0] = board_spec_alt[0];
			(board->mat_specular)[1] = board_spec_alt[1];
			(board->mat_specular)[2] = board_spec_alt[2];
		}
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
