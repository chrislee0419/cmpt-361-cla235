#include <stdio.h>
#include <GL/glut.h>
#include <math.h>
#include "global.h"
#include "sphere.h"

//
// Global variables
//
extern int win_width;
extern int win_height;

extern GLfloat frame[WIN_HEIGHT][WIN_WIDTH][3];  

extern float image_width;
extern float image_height;

extern Point eye_pos;
extern float image_plane;
extern RGB_float background_clr;
extern RGB_float null_clr;

extern Spheres *scene;

// light 1 position and color
extern Point light1;
extern float light1_intensity[3];

// global ambient term
extern float global_ambient[3];

// light decay parameters
extern float decay_a;
extern float decay_b;
extern float decay_c;

extern int shadow_on;
extern int reflect_on;
extern int refract_on;
extern int chess_on;
extern int stochastic_on;
extern int supersample_on;
extern int step_max;

extern Spheres *chessboard;

/////////////////////////////////////////////////////////////////////

// returns a vector that is <90 degrees from the normal
Vector random_ray(Vector n)
{
	Vector random = {-n.x, -n.y, -n.z};
	while (vec_dot(random, n) < 0)
	{
		random.x = rand();
		random.y = rand();
		random.z = rand();
	}

	return random;
}


/*********************************************************************
 * Phong illumination - you need to implement this!
 * q = point on surface
 * v = vector from point q to the eye
 * n = normal vector of point q
 * sph = sphere containing point q
 *********************************************************************/
RGB_float phong(Point q, Vector v, Vector n, Spheres *sph) {
	RGB_float final_colour, amb, spec, dif;
	Vector l, r;

	// vector setup
	l = get_vec(q, light1);
	normalize(&l);
	normalize(&v);
	normalize(&n);
	r = vec_minus(vec_scale(n, 2*vec_dot(n, l)), l);
	normalize(&r);

	amb.r = sph->mat_ambient[0] * global_ambient[0];
	amb.g = sph->mat_ambient[1] * global_ambient[1];
	amb.b = sph->mat_ambient[2] * global_ambient[2];

	float dis = vec_len(get_vec(q, light1));
	float decay = 1.0 / (decay_a + decay_b*dis + decay_c*dis*dis);
	float shadow = 1.0;
	if (shadow_on == 1)
	{
		if (intersect_scene(q, l, scene, NULL, sph->index) != NULL)
			shadow = 0.0;
	}

	float n_dot_l = vec_dot(n, l);
	if (n_dot_l < 0.0) n_dot_l = 0.0;
	dif.r = sph->mat_diffuse[0] * n_dot_l;
	dif.g = sph->mat_diffuse[1] * n_dot_l;
	dif.b = sph->mat_diffuse[2] * n_dot_l;

	float v_dot_r = vec_dot(v, r);
	float spec_term = 0.0;
	if (v_dot_r > 0.0)
		spec_term = pow(v_dot_r, sph->mat_shineness);
	spec.r = sph->mat_specular[0] * spec_term;
	spec.g = sph->mat_specular[1] * spec_term;
	spec.b = sph->mat_specular[2] * spec_term;

	final_colour.r = amb.r + decay * shadow * light1_intensity[0] * (dif.r + spec.r);
	final_colour.g = amb.g + decay * shadow * light1_intensity[1] * (dif.g + spec.g);
	final_colour.b = amb.b + decay * shadow * light1_intensity[2] * (dif.b + spec.b);

	return final_colour;
}

/************************************************************************
 * This is the recursive ray tracer - you need to implement this!
 * You should decide what arguments to use.
 ************************************************************************/
RGB_float recursive_ray_trace(Point origin, Vector ray, int recursion, Spheres *sph_origin) {
	RGB_float colour = {0};
	Point hit;
	Spheres *sph;

	if (recursion > step_max)
		return colour;
	else if (recursion == 0)
		sph = intersect_scene(origin, ray, scene, &hit, -1);
	else if (sph_origin != NULL)
		sph = intersect_scene(origin, ray, scene, &hit, sph_origin->index);
	else
		return colour;

	if (sph != NULL)
	{
		Vector norm;
		if (sph->index == 0)
		{
			norm = board_normal();
			board_colour(sph, hit);
		}
		else
			norm = sphere_normal(hit, sph);

		RGB_float phong_colour = phong(hit, get_vec(hit, eye_pos), norm, sph);

		colour.r += phong_colour.r;
		colour.g += phong_colour.g;
		colour.b += phong_colour.b;

		// Reflections
		if (reflect_on == 1)
		{
			Vector reversed_ray = vec_scale(ray, -1.0);
			Vector r = vec_minus(vec_scale(norm, 2*vec_dot(norm, reversed_ray)), reversed_ray);
			RGB_float reflect_colour = recursive_ray_trace(hit, r, recursion+1, sph);
			reflect_colour.r *= sph->reflectance;
			reflect_colour.g *= sph->reflectance;
			reflect_colour.b *= sph->reflectance;
			colour.r += reflect_colour.r;
			colour.g += reflect_colour.g;
			colour.b += reflect_colour.b;
		}

		// Stochasitic rays
		if (stochastic_on == 1)
		{
			RGB_float stoc_colour;

			

			colour.r += stoc_colour.r;
			colour.g += stoc_colour.g;
			colour.b += stoc_colour.b;
		}
	}
	else
		colour = background_clr;


	return colour;
}

/*********************************************************************
 * This function traverses all the pixels and cast rays. It calls the
 * recursive ray tracer and assign return color to frame
 *
 * You should not need to change it except for the call to the recursive
 * ray tracer. Feel free to change other parts of the function however,
 * if you must.
 *********************************************************************/
void ray_trace() {
	int i, j;
	float x_grid_size = image_width / float(win_width);
	float y_grid_size = image_height / float(win_height);
	float x_start = -0.5 * image_width;
	float y_start = -0.5 * image_height;
	RGB_float ret_colour;
	Point cur_pixel_pos;
	Vector ray;

	// ray is cast through center of pixel
	cur_pixel_pos.x = x_start + 0.5 * x_grid_size;
	cur_pixel_pos.y = y_start + 0.5 * y_grid_size;
	cur_pixel_pos.z = image_plane;

	for (i=0; i<win_height; i++) {
		for (j=0; j<win_width; j++) {
			ray = get_vec(eye_pos, cur_pixel_pos);

			ret_colour = recursive_ray_trace(cur_pixel_pos, ray, 0, NULL);
			// Parallel rays can be cast instead using below
			// ray.x = ray.y = 0;
			// ray.z = -1.0;
			// ret_color = recursive_ray_trace(cur_pixel_pos, ray, 1);

			if (supersample_on == 1)
			{
				Point nw_pos, ne_pos, sw_pos, se_pos;
				RGB_float temp_col;
				// NW
				nw_pos.x = cur_pixel_pos.x - 0.5 * x_grid_size;
				nw_pos.y = cur_pixel_pos.y + 0.5 * y_grid_size;
				nw_pos.z = cur_pixel_pos.z;
				// NE
				ne_pos.x = cur_pixel_pos.x + 0.5 * x_grid_size;
				ne_pos.y = cur_pixel_pos.y + 0.5 * y_grid_size;
				ne_pos.z = cur_pixel_pos.z;
				// SW
				sw_pos.x = cur_pixel_pos.x - 0.5 * x_grid_size;
				sw_pos.y = cur_pixel_pos.y - 0.5 * y_grid_size;
				sw_pos.z = cur_pixel_pos.z;
				// SE
				se_pos.x = cur_pixel_pos.x + 0.5 * x_grid_size;
				se_pos.y = cur_pixel_pos.y - 0.5 * y_grid_size;
				se_pos.z = cur_pixel_pos.z;

				// get colour
				temp_col = recursive_ray_trace(nw_pos, get_vec(eye_pos, nw_pos), 0, NULL);
				ret_colour.r += temp_col.r;
				ret_colour.g += temp_col.g;
				ret_colour.b += temp_col.b;
				temp_col = recursive_ray_trace(ne_pos, get_vec(eye_pos, ne_pos), 0, NULL);
				ret_colour.r += temp_col.r;
				ret_colour.g += temp_col.g;
				ret_colour.b += temp_col.b;
				temp_col = recursive_ray_trace(sw_pos, get_vec(eye_pos, sw_pos), 0, NULL);
				ret_colour.r += temp_col.r;
				ret_colour.g += temp_col.g;
				ret_colour.b += temp_col.b;
				temp_col = recursive_ray_trace(se_pos, get_vec(eye_pos, se_pos), 0, NULL);
				ret_colour.r += temp_col.r;
				ret_colour.g += temp_col.g;
				ret_colour.b += temp_col.b;

				// average
				ret_colour.r /= 5.0;
				ret_colour.g /= 5.0;
				ret_colour.b /= 5.0;
			}

			frame[i][j][0] = GLfloat(ret_colour.r);
			frame[i][j][1] = GLfloat(ret_colour.g);
			frame[i][j][2] = GLfloat(ret_colour.b);

			cur_pixel_pos.x += x_grid_size;
		}

		cur_pixel_pos.y += y_grid_size;
		cur_pixel_pos.x = x_start;
	}
}
