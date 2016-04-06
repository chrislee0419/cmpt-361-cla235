#include "sphere.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

// board attributes
extern float board_amb[];	// ambient
extern float board_dif[];	// diffuse
extern float board_spec[];	// specular

extern float board_amb_alt[];	// ambient
extern float board_dif_alt[];	// diffuse
extern float board_spec_alt[];	// specular

extern Point eye_pos;

/**********************************************************************
 * This function intersects a ray with a given sphere 'sph'. You should
 * use the parametric representation of a line and do the intersection.
 * The function should return the parameter value for the intersection, 
 * which will be compared with others to determine which intersection
 * is closest. The value -1.0 is returned if there is no intersection
 *
 * If there is an intersection, the point of intersection should be
 * stored in the "hit" variable
 **********************************************************************/

// returns the normal vector for the board;
Vector board_normal()
{
	Vector norm;
	norm.x = 0.0;
	norm.y = 1.0;
	norm.z = 0.0;
	return norm;
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

	normalize(&ray);

	Vector board_norm = board_normal();
	float a = board_norm.x;
	float b = board_norm.y;
	float c = board_norm.z;
	float d = fabs(board->center.y);

	float numer = -( (orig.x*a) + (orig.y*b) + (orig.z*c) + d);
	float denom = vec_dot(board_norm, ray);

	if (denom == 0.0)
		return -1;

	float t = numer / denom;

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

	Point origin = {centre.x-size, centre.y, centre.z+size};

  //printf("board_colour (%f, %f, %f): ", hit.x, hit.y, hit.z);
	int i;
	for (i = 0; i < 64; i++)
	{
		int x = i % 8;
		float x1 = origin.x + (x*(size/4.0));
		float x2 = x1 + size/4.0;

		int z = i / 8;
		float z1 = origin.z - ((z+1)*(size/4.0));

		if (hit.z < z1)
		{
			i += 7;
			continue;
		}
		else if (hit.x < x1 || hit.x > x2)
    {
			continue;
    }
		else
			break;
	}
  //printf("\n");

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


//////////////////////////////////////////////////////////////////////

float intersect_sphere(Point o, Vector u, Spheres *sph, Point *hit) {
	float epsilon = 1e-6;
	normalize(&u);
	Vector v = get_vec(sph->center, o);
	float a = pow(vec_len(u), 2);
	float b = 2 * (vec_dot(u, v));
	float c = pow(vec_len(v), 2) - pow(sph->radius, 2);

	//printf("a = %f, b = %f, c = %f\n", a, b, c);

	float discrim = pow(b, 2) - 4 * a * c;
	if ( discrim < 0 )
		return -1.0;

	float sqrt_dis = sqrt(discrim);
	float res1 = (-b + sqrt_dis)/(2 * a);
	float res2 = (-b - sqrt_dis)/(2 * a);
	float res;
	if ( res1 > epsilon && (res1 < res2 || res2 < 0) )
	{
		res = res1;
		hit->x = o.x + res * u.x;
		hit->y = o.y + res * u.y;
		hit->z = o.z + res * u.z;
		return res;
	}
	else if ( res2 > epsilon && res2 <= res1)
	{
		res = res2;
		hit->x = o.x + res * u.x;
		hit->y = o.y + res * u.y;
		hit->z = o.z + res * u.z;
		return res;
	}
	return -1.0;
}

/*********************************************************************
 * This function returns a pointer to the sphere object that the
 * ray intersects first; NULL if no intersection. You should decide
 * which arguments to use for the function. For exmaple, note that you
 * should return the point of intersection to the calling function.
 **********************************************************************/
Spheres *intersect_scene(Point o, Vector u, Spheres *sph, Point *hit, int sindex) {
	Spheres *closest_sph = NULL;
	Point closest_hit;
	float min = -1;

	while (sph != NULL)
	{
		// skips this sphere (used for shadows)
		if (sph->index == sindex)
		{
			sph = sph->next;
			continue;
		}

		Point hit;
		float intersect;
		if (sph->index == 0)
			intersect = intersect_board(o, u, sph, &hit);
		else
			intersect = intersect_sphere(o, u, sph, &hit);

		if ( intersect > 0 && (intersect < min || min < 0) )
		{
			min = intersect;
			closest_sph = sph;
			closest_hit = hit;
		}

		sph = sph->next;
	}

	if (hit != NULL)
		*hit = closest_hit;
	return closest_sph;
}

/*****************************************************
 * This function adds a sphere into the sphere list
 *
 * You need not change this.
 *****************************************************/
Spheres *add_sphere(Spheres *slist, Point ctr, float rad, float amb[],
				float dif[], float spe[], float shine, 
				float refl, float refract, float trans, int sindex) {
	Spheres *new_sphere;

	new_sphere = (Spheres *)malloc(sizeof(Spheres));
	new_sphere->index = sindex;
	new_sphere->center = ctr;
	new_sphere->radius = rad;
	(new_sphere->mat_ambient)[0] = amb[0];
	(new_sphere->mat_ambient)[1] = amb[1];
	(new_sphere->mat_ambient)[2] = amb[2];
	(new_sphere->mat_diffuse)[0] = dif[0];
	(new_sphere->mat_diffuse)[1] = dif[1];
	(new_sphere->mat_diffuse)[2] = dif[2];
	(new_sphere->mat_specular)[0] = spe[0];
	(new_sphere->mat_specular)[1] = spe[1];
	(new_sphere->mat_specular)[2] = spe[2];
	new_sphere->mat_shineness = shine;
	new_sphere->reflectance = refl;
	new_sphere->refract = refract;
	new_sphere->trans = trans;
	new_sphere->next = NULL;

	if (slist == NULL) { // first object
		slist = new_sphere;
	} else { // insert at the beginning
		new_sphere->next = slist;
		slist = new_sphere;
	}

	return slist;
}

/******************************************
 * computes a sphere normal - done for you
 ******************************************/
Vector sphere_normal(Point q, Spheres *sph) {
	Vector rc;

	rc = get_vec(sph->center, q);
	normalize(&rc);
	return rc;
}
