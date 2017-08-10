
/*
	Majuma's Raytracer MRT 1.1
	Coded by Mikolaj 'Majuma' Feliks
	
	Mail : majuma@wp.pl
	WWW  : http://www.majuma.xt.pl/
	
	Modified : Tue Sep 13 13:00:50 2005
	Tab spacing : 4
	
	Compilation
	-----------
	BCC 5.5   : bcc32 -Lpath_to_libraries -Ipath_to_includes mrt.c
	GCC 3.4.3 : gcc -lm -o mrt mrt.c 
*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#ifndef __GNUC__
#include <alloc.h>
#include <mem.h>
#endif

#include "newtypes.h"
#include "mrt.h"
#include "font.h"
#include "pcxfiles.h"


#include "font.c"
#include "pcxfiles.c"



//------------------------------------------------------------
//------------------------------------------------------------

int LoadPalette(SCREEN *s, char *PalFile) {
	FILE	*p;
	char	line[32], c;
	int		eof = 0;
	int		curr_char = 0, curr_line = 0;
	int		i, j, n_num, num_char = 0;
	char	num[4], cc;
	int		ln;
	int		color;
	BYTE	*pal_ptr = s->Palette;
	
	
	p = fopen(PalFile, "r");
	if (!p) {
		return PAL_FILE;
	}
	
	while (!eof) {
		c   = fgetc(p);
		eof = feof(p);
		
		if (c == 0x0a || eof) {
			if (!curr_char) continue;
			line[curr_char] = 0;
			curr_char = 0;
			
			ln = strlen(line);
			num_char = 0;
			n_num = 0;
			
			for (i = 0; i < ln; i++) {
				cc = line[i];
				if (cc == ',' || i == (ln - 1)) {
					if (i == (ln - 1)) {
						num[num_char++] = cc;
						if (num_char == 3) {
							fclose(p);
							return PAL_NUMLEN;
						}
					}
				
					num[num_char] = 0;
					num_char = 0;
					
					for (j = 0; j < strlen(num); j++) {
						if (num[j] < '0' || num[j] > '9') {
							fclose(p);
							return PAL_NOTNUM;
						}
					}
					
					color = atoi(num);
					if (color < 0 || color > 63) {
						fclose(p);
						return PAL_RANGE;
					}
					
					*pal_ptr = (BYTE)color;
					pal_ptr++;
					
					n_num++;
					if (n_num > 3) {
						fclose(p);
						return PAL_NUMNUM;
					}
				
				}
				else {
					num[num_char++] = cc;
					if (num_char == 3) {
						fclose(p);
						return PAL_NUMLEN;
					}		
				}
			}
			
			
			if ((++curr_line) == 256) break;
		}
		else if (c >= '!') {
			line[curr_char++] = c;
			if (curr_char == 32) {
				fclose(p);
				return PAL_LINELEN;
			}
		}
	}
	
	fclose(p);
	
	if (curr_line != 256) {
		return PAL_LINES;
	}


	return PAL_OK;
}

//------------------------------------------------------------

DWORD SetScreen(SCREEN *s, int w, int h) {
	DWORD size = w * h;

	s->Width  = w;
	s->Height = h;

	s->Buffer = (BYTE *)malloc(size);
	if (!s->Buffer) return 0;

	s->Palette = (BYTE *)malloc(768);
	if (!s->Palette) {
		free(s->Buffer);
		return 0;
	}

	return (size + 768);
}

//------------------------------------------------------------

void FreeScreen(SCREEN *s) {
	free(s->Buffer);
	free(s->Palette);
}

//------------------------------------------------------------

/*
void ClearScreen(SCREEN *s, BYTE c) {
	memset(s->Buffer, c, s->Width * s->Height);
}
*/

//------------------------------------------------------------

SPHERE *add_sphere(WORLD *w) {
	SPHERE *s, *ns;

	if (!w->n_spheres) {
		w->sphere_tab = (SPHERE *)malloc(sizeof(SPHERE));
		if (!w->sphere_tab) return (SPHERE *)0;

		w->sphere_tab->prev = w->sphere_tab->next = (SPHERE *)0;
		ns = w->sphere_tab;
	}
	else {
		s = w->sphere_tab;
		while (s->next) s = s->next;

		s->next = (SPHERE *)malloc(sizeof(SPHERE));
		if (!s->next) return (SPHERE *)0;

		ns = s->next;
		ns->prev = s;
		ns->next = (SPHERE *)0;
	}

	
	w->n_spheres++;

	return ns;
}

PLANE *add_plane(WORLD *w) {
	PLANE *p, *np;

	if (!w->n_planes) {
		w->plane_tab = (PLANE *)malloc(sizeof(PLANE));
		if (!w->plane_tab) return (PLANE *)0;

		w->plane_tab->prev = w->plane_tab->next = (PLANE *)0;
		np = w->plane_tab;
	}
	else {
		p = w->plane_tab;
		while (p->next) p = p->next;

		p->next = (PLANE *)malloc(sizeof(PLANE));
		if (!p->next) return (PLANE *)0;

		np = p->next;
		np->prev = p;
		np->next = (PLANE *)0;
	}

	
	w->n_planes++;

	return np;
}

CYLINDER *add_cylinder(WORLD *w) {
	CYLINDER *c, *nc;

	if (!w->n_cylinders) {
		w->cylinder_tab = (CYLINDER *)malloc(sizeof(CYLINDER));
		if (!w->cylinder_tab) return (CYLINDER *)0;

		w->cylinder_tab->prev = w->cylinder_tab->next = (CYLINDER *)0;
		nc = w->cylinder_tab;
	}
	else {
		c = w->cylinder_tab;
		while (c->next) c = c->next;

		c->next = (CYLINDER *)malloc(sizeof(CYLINDER));
		if (!c->next) return (CYLINDER *)0;

		nc = c->next;
		nc->prev = c;
		nc->next = (CYLINDER *)0;
	}

	
	w->n_cylinders++;

	return nc;
}

//------------------------------------------------------------

int set_world(WORLD *w, int scr_w, int scr_h, double fov,
	DWORD *allocated_mem) {

	int		j;
	BYTE	*p;

	if (!(*allocated_mem = SetScreen(&w->cam.scr, scr_w, scr_h))) {
		return ERR_SCRMEM;
	}
	
	
	w->sphere_tab   = (SPHERE *)0;
	w->n_spheres	= 0;

	w->plane_tab    = (PLANE *)0;
	w->n_planes		= 0;

	w->cylinder_tab = (CYLINDER *)0;
	w->n_cylinders	= 0;


	p = w->cam.scr.Palette;

	// COLOR 1 (WHITE)
	for (j = 0; j < 32; j++, p += 3) {
		*(p + 0) = j << 1;
		*(p + 1) = j << 1;
		*(p + 2) = j << 1;
	}

	// COLOR 2 (RED)
	for (j = 0; j < 32; j++, p += 3) {
		*(p + 0) = j << 1;
		*(p + 1) = 0;
		*(p + 2) = 0;
	}

	// COLOR 3 (GREEN)
	for (j = 0; j < 32; j++, p += 3) {
		*(p + 0) = 0;
		*(p + 1) = j << 1;
		*(p + 2) = 0;
	}

	// COLOR 4 (BLUE)
	for (j = 0; j < 32; j++, p += 3) {
		*(p + 0) = 0;
		*(p + 1) = 0;
		*(p + 2) = j << 1;
	}

	// COLOR 5 (YELLOW)
	for (j = 0; j < 32; j++, p += 3) {
		*(p + 0) = j << 1;
		*(p + 1) = j << 1;
		*(p + 2) = 0;
	}

	// COLOR 6 (CYAN)
	for (j = 0; j < 32; j++, p += 3) {
		*(p + 0) = 0;
		*(p + 1) = j << 1;
		*(p + 2) = j << 1;
	}

	// COLOR 7 (MAGENTA)
	for (j = 0; j < 32; j++, p += 3) {
		*(p + 0) = j << 1;
		*(p + 1) = 0;
		*(p + 2) = j << 1;
	}

	// COLOR 8 (BLUE 2)
	for (j = 0; j < 32; j++, p += 3) {
		*(p + 0) = 0;
		*(p + 1) = j;
		*(p + 2) = j << 1;
	}

	w->cam.fov = fov;
	w->cam.w_div_h = (double)scr_w / (double)scr_h;
	w->cam.persp = w->cam.w_div_h / (2.0 * tan(w->cam.fov / 2.0 * M_PI / 180.0));

	return ERR_OK;
}

//------------------------------------------------------------

void free_world(WORLD *w) {
	SPHERE		*s;
	PLANE		*p;
	CYLINDER	*c;

	s = w->sphere_tab;
	if (s) {
		while (s->next) s = s->next;
		s = s->prev;
		while (s) {
			free(s->next);
			s = s->prev;
		}
		free(w->sphere_tab);
	}

	p = w->plane_tab;
	if (p) {
		while (p->next) p = p->next;
		p = p->prev;
		while (p) {
			free(p->next);
			p = p->prev;
		}
		free(w->plane_tab);
	}

	c = w->cylinder_tab;
	if (c) {
		while (c->next) c = c->next;
		c = c->prev;
		while (c) {
			free(c->next);
			c = c->prev;
		}
		free(w->cylinder_tab);
	}


	FreeScreen(&w->cam.scr);
}

//------------------------------------------------------------

double vector_len(VECTOR *v) {
	return sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}

//------------------------------------------------------------

void cross_product(VECTOR *v1, VECTOR *v2, VECTOR *result) {
	result->x = v1->y * v2->z - v1->z * v2->y;
	result->y = v1->z * v2->x - v1->x * v2->z;
	result->z = v1->x * v2->y - v1->y * v2->x;
}

//------------------------------------------------------------

double dot_product(VECTOR *v1, VECTOR *v2) {
	return (v1->x * v2->x + v1->y * v2->y + v1->z * v2->z);
}

//------------------------------------------------------------

int solve_plane(PLANE *p, VECTOR *o, VECTOR *d,
	double min_dist, double max_dist, double *dist) {

	double alfa  = o->x - p->x0;
	double beta  = o->y - p->y0;
	double gamma = o->z - p->z0;
	double t;

	t = - ((p->A * alfa + p->B * beta + p->C * gamma) /
		(p->A * d->x + p->B * d->y + p->C * d->z));

	if ((t < min_dist) || (t > max_dist))
		return 0;

	*dist = t;

	return 1;
}

//------------------------------------------------------------

int solve_sphere(SPHERE *s, VECTOR *o, VECTOR *d,
	double min_dist, double max_dist, double *dist) {
	
	double alfa  = o->x - s->x0;
	double beta  = o->y - s->y0;
	double gamma = o->z - s->z0;
	PUT_VARS();

	a = d->x * d->x + d->y * d->y + d->z * d->z;
	b = 2.0 * (alfa * d->x + beta * d->y + gamma * d->z);
	c = alfa * alfa + beta * beta + gamma * gamma - s->R * s->R;

	GET_ROOTS();
}

//------------------------------------------------------------

int solve_cylinder(CYLINDER *cyl, VECTOR *o, VECTOR *d,
	double min_dist, double max_dist, double *dist) {
	
	double alfa = -cyl->m;
	double beta = -cyl->n;
	PUT_VARS();
	
	switch (cyl->type) {
		case CYL_XY:
			alfa += o->x;
			beta += o->y;
			a = d->x * d->x + d->y * d->y;
			b = 2.0 * (alfa * d->x + beta * d->y);
			break;
		case CYL_XZ:
			alfa += o->x;
			beta += o->z;
			a = d->x * d->x + d->z * d->z;
			b = 2.0 * (alfa * d->x + beta * d->z);
			break;
		case CYL_YZ:
			alfa += o->y;
			beta += o->z;
			a = d->y * d->y + d->z * d->z;
			b = 2.0 * (alfa * d->y + beta * d->z);
	}
	
	c = alfa * alfa + beta * beta - cyl->R * cyl->R;
	
	
	GET_ROOTS();
}

//------------------------------------------------------------

BYTE shoot_ray(WORLD *w, VECTOR *ro, VECTOR *rd) {
	double		curr_dist = MAX_DIST, dist;
	SPHERE		*s;
	PLANE		*p;
	CYLINDER	*c;
	VECTOR		hit;
	double		t;
	SPHERE		*hit_s;
	PLANE		*hit_p;
	CYLINDER	*hit_c;
	int			found = ID_NOTHING, found_shadow = 0;
	BYTE		color;
	VECTOR		normal, to_light;
	int			tx, ty;
	int			r;
	double		shadow_mul, v_len;
	VECTOR		h, rx, ry;
	double		dot_val;
	double		fade_color;
	int			icolor;


// search for spheres
	s = w->sphere_tab;
	while (s) {
		r = solve_sphere(s, ro, rd, 0.0, MAX_DIST, &dist);
		if (r && (dist < curr_dist)) {
			curr_dist = dist;
			hit_s = s;
			found = ID_SPHERE;
		}

		s = s->next;
	}

// search for planes
	p = w->plane_tab;
	while (p) {
		r = solve_plane(p, ro, rd, 0.0, MAX_DIST, &dist);
		if (r && (dist < curr_dist)) {
			curr_dist = dist;
			hit_p = p;
			found = ID_PLANE;
		}

		p = p->next;
	}
	
// search for cylinders
	c = w->cylinder_tab;
	while (c) {
		r = solve_cylinder(c, ro, rd, 0.0, MAX_DIST, &dist);
		if (r && (dist < curr_dist)) {
			curr_dist = dist;
			hit_c = c;
			found = ID_CYLINDER;
		}

		c = c->next;
	}


	if (found != ID_NOTHING) {
		hit.x = ro->x + rd->x * curr_dist;
		hit.y = ro->y + rd->y * curr_dist;
		hit.z = ro->z + rd->z * curr_dist;

		if (found == ID_SPHERE) {
			normal.x = hit.x - hit_s->x0;
			normal.y = hit.y - hit_s->y0;
			normal.z = hit.z - hit_s->z0;
		}
		else if (found == ID_PLANE) {
			normal.x = hit_p->A;
			normal.y = hit_p->B;
			normal.z = hit_p->C;
		}
		else /*if (found == ID_CYLINDER)*/ {
			switch (hit_c->type) {
				case CYL_XY:
					normal.x = hit.x - hit_c->m;
					normal.y = hit.y - hit_c->n;
					normal.z = 0.0;
					break;
				case CYL_XZ:
					normal.x = hit.x - hit_c->m;
					normal.y = 0.0;
					normal.z = hit.z - hit_c->n;
					break;
				default: //case CYL_YZ:
					normal.x = 0.0;
					normal.y = hit.y - hit_c->m;
					normal.z = hit.z - hit_c->n;
			}
		}


		if (found != ID_PLANE) {
			t = 1.0 / vector_len(&normal);
			normal.x *= t;
			normal.y *= t;
			normal.z *= t;
		}

		to_light.x = w->light.pos.x - hit.x;
		to_light.y = w->light.pos.y - hit.y;
		to_light.z = w->light.pos.z - hit.z;
		t = 1.0 / (v_len = vector_len(&to_light));
		to_light.x *= t;
		to_light.y *= t;
		to_light.z *= t;

		t = dot_product(&normal, &to_light);
		if (t < 0.0)
			color = C_EMPTY;
		else {
			//check for shadow
			s = w->sphere_tab;
			while (s) {
				if (found == ID_SPHERE && s == hit_s) {
					s = s->next;
					continue;
				}

				if (solve_sphere(s, &hit, &to_light, 0.0, v_len, &dist)) {
					found_shadow = 1;
					break;
				}

				s = s->next;
			}

			if (!found_shadow) {
				p = w->plane_tab;
				while (p) {
					if (found == ID_PLANE && p == hit_p) {
						p = p->next;
						continue;
					}

					if (solve_plane(p, &hit, &to_light, 0.0, v_len, &dist)) {
						found_shadow = 1;
						break;
					}

					p = p->next;
				}
			}
			
			if (!found_shadow) {
				c = w->cylinder_tab;
				while (c) {
					if (found == ID_CYLINDER && c == hit_c) {
						c = c->next;
						continue;
					}

					if (solve_cylinder(c, &hit, &to_light, 0.0, v_len, &dist)) {
						found_shadow = 1;
						break;
					}

					c = c->next;
				}
			}

			if (!found_shadow)
				shadow_mul = 1.0;
			else
				shadow_mul = 0.5;
				
			fade_color = 31.0 / (MAX_DIST * MAX_DIST) * curr_dist * curr_dist;

			icolor = (int)(t * shadow_mul * 31.0 - fade_color);
			if (icolor < 0) icolor = 0;
			color = (BYTE)icolor;
		}

		if (found == ID_SPHERE) {
			color += hit_s->color;
		}
		else if (found == ID_PLANE) {
			h.x = hit.x - hit_p->x0;
			h.y = hit.y - hit_p->y0;
			h.z = hit.z - hit_p->z0;
			
			dot_val = dot_product(&h, &hit_p->tex_x);
			rx.x = hit_p->tex_x.x * dot_val;
			rx.y = hit_p->tex_x.y * dot_val;
			rx.z = hit_p->tex_x.z * dot_val;
			tx = (int)(vector_len(&rx) * hit_p->tex_scale);
			
			dot_val = dot_product(&h, &hit_p->tex_y);
			ry.x = hit_p->tex_y.x * dot_val;
			ry.y = hit_p->tex_y.y * dot_val;
			ry.z = hit_p->tex_y.z * dot_val;
			ty = (int)(vector_len(&ry) * hit_p->tex_scale);
		
			if (h.x < hit_p->A) tx--;
			
			if (h.y < hit_p->B) ty--;
			
			
			if (!(tx & 1)) {
				if (!(ty & 1))
					color += hit_p->color1;
				else
					color += hit_p->color2;
			}
			else {
				if (!(ty & 1))
					color += hit_p->color2;
				else
					color += hit_p->color1;
			}
		
			
		}
		else /*if (found == ID_CYLINDER)*/ {
			color += hit_c->color;
		}
	}
	else
		color = C_EMPTY;

	return color;
}

//------------------------------------------------------------

void render(WORLD *w, int v) {
	VECTOR	a, b, c, new_c;
	VECTOR	ro, rd;
	VECTOR	so;
	VECTOR	ver1, ver2, ver3;
	VECTOR	vw, vh;
	int		wi, hi;
	BYTE	*scr_ptr;
	BYTE	color;
	double	t;
	double	sina, cosa;
	long	n_pixels = w->cam.scr.Width * w->cam.scr.Height;
	long	pix_counter = 0, pix_put_dot = n_pixels >> 6;
	char	out_str[MAX_FILENAME_LEN + 16];


	if (v) {
		printf("%s", "rendering");
	}


	a.x = w->cam.look_at.x - w->cam.pos.x;
	a.y = w->cam.look_at.y - w->cam.pos.y;
	a.z = w->cam.look_at.z - w->cam.pos.z;
	t = 1.0 / vector_len(&a);
	a.x *= t;
	a.y *= t;
	a.z *= t;
	
	if (a.x == 0.0 && a.y == 0.0) return;
	
	c.x = 0.0;
	c.y = 0.0;
	c.z = 1.0;
	cross_product(&a, &c, &b);
	cross_product(&a, &b, &c);
	t = 1.0 / vector_len(&b);
	b.x *= t;
	b.y *= t;
	b.z *= t;
	t = -1.0 / vector_len(&c);
	c.x *= t;
	c.y *= t;
	c.z *= t;
	
	sina = -sin(w->cam.rot);
	cosa = cos(w->cam.rot);
	new_c.x = sina * b.x + cosa * c.x;
	new_c.y = sina * b.y + cosa * c.y;
	new_c.z = sina * b.z + cosa * c.z;
	cross_product(&a, &new_c, &b);
	cross_product(&a, &b, &c);
	t = 1.0 / vector_len(&b);
	b.x *= t;
	b.y *= t;
	b.z *= t;
	t = -1.0 / vector_len(&c);
	c.x *= t;
	c.y *= t;
	c.z *= t;
	
	so.x = w->cam.pos.x + a.x * w->cam.persp;
	so.y = w->cam.pos.y + a.y * w->cam.persp;
	so.z = w->cam.pos.z + a.z * w->cam.persp;
	t = -w->cam.w_div_h * 0.5;
	b.x *= t;
	b.y *= t;
	b.z *= t;
	c.x *= 0.5;
	c.y *= 0.5;
	c.z *= 0.5;
	ver1.x =  b.x + c.x + so.x;
	ver1.y =  b.y + c.y + so.y;
	ver1.z =  b.z + c.z + so.z;
	ver2.x = -b.x + c.x + so.x;
	ver2.y = -b.y + c.y + so.y;
	ver2.z = -b.z + c.z + so.z;
	ver3.x =  b.x - c.x + so.x;
	ver3.y =  b.y - c.y + so.y;
	ver3.z =  b.z - c.z + so.z;
	vw.x = ver2.x - ver1.x;
	vw.y = ver2.y - ver1.y;
	vw.z = ver2.z - ver1.z;
	vw.x /= w->cam.scr.Width;
	vw.y /= w->cam.scr.Width;
	vw.z /= w->cam.scr.Width;
	vh.x = ver3.x - ver1.x;
	vh.y = ver3.y - ver1.y;
	vh.z = ver3.z - ver1.z;
	vh.x /= w->cam.scr.Height;
	vh.y /= w->cam.scr.Height;
	vh.z /= w->cam.scr.Height;


	scr_ptr = w->cam.scr.Buffer;
	for (hi = 0; hi < w->cam.scr.Height; hi++) {

		for (wi = 0; wi < w->cam.scr.Width; wi++, scr_ptr++) {
			ro.x = ver1.x + vw.x * wi + vh.x * hi;
			ro.y = ver1.y + vw.y * wi + vh.y * hi;
			ro.z = ver1.z + vw.z * wi + vh.z * hi;

			rd.x = ro.x - w->cam.pos.x;
			rd.y = ro.y - w->cam.pos.y;
			rd.z = ro.z - w->cam.pos.z;
			t = 1.0 / vector_len(&rd);
			rd.x *= t;
			rd.y *= t;
			rd.z *= t;

			color = shoot_ray(w, &ro, &rd);
			*scr_ptr = color;

			if (v) {
				if ((++pix_counter) == pix_put_dot) {
					pix_counter = 0;
					printf("%s", "."); fflush(stdout);
				}
			}
		}
	}

	strcpy(out_str, "MRT ");
	strcat(out_str, VERSION);
	strcat(out_str, " [");
	strcat(out_str, w->file);
	strcat(out_str, "]");
	PutString(&w->cam.scr, out_str, 2, 2, C_WHITE + 16, 2);

	if (v) {
		printf("%s\n", " OK");
	}
}

//------------------------------------------------------------

DWORD write_to_file(WORLD *w) {
	int		i;
	char	filename[MAX_FILENAME_LEN];
	DWORD	size;

	for (i = 0; i < (int)(strlen(w->file)); i++) {
		if (w->file[i] == '.')
			break;
		else
			filename[i] = w->file[i];
	}

	if (i > (MAX_FILENAME_LEN - 3 - 1)) {
		i -= 4;
	}

	filename[i + 0] = '.';
	filename[i + 1] = 'p';
	filename[i + 2] = 'c';
	filename[i + 3] = 'x';
	filename[i + 4] = 0;

	size = SavePCX(filename, w->cam.scr.Buffer, w->cam.scr.Palette,
		w->cam.scr.Width, w->cam.scr.Height);

	return size;
}

//------------------------------------------------------------

int is_double(char *s) {
	int		ln = strlen(s);
	int		i;
	int		n_dots = 0;

	if (ln < 3) return 0;

	if ((s[0] != '-') && ((s[0] < '0') || (s[0] > '9')))
		return 0;

	for (i = 1; i < ln; i++) {
		if (((s[i] < '0') || (s[i] > '9')) && (s[i] != '.'))
			return 0;

		if (s[i] == '.') {
			if ((++n_dots) > 1)
				return 0;
		}
	}

	if (s[i] == '.') return 0;

	if (!n_dots) return 0;

	return 1;
}

//------------------------------------------------------------

int parse_line(char *line, int *id, int *cyl_type, double *d_tab, BYTE *b_tab) {
	typedef struct tS {
		char label[MAX_LABEL_LEN];
		char data_type; // 'd' = double, 's' = string
		BYTE index; // index in d_tab or in b_tab
		BYTE ok;
	} S;
	
	typedef struct tCOL {
		char name[10], alt_name[10];
		int  id;
	} COL;
	

	COL	col_tab[8] = {{"white", "c1", C_WHITE}, {"red", "c2", C_RED}, \
		{"green", "c3", C_GREEN}, {"blue", "c4", C_BLUE}, \
		{"yellow", "c5", C_YELLOW}, {"cyan", "c6", C_CYAN}, \
		{"magenta", "c7", C_MAGENTA}, {"blue2", "c8", C_BLUE2}};

	S	s_sph[5] = {{"x0", 'd', 0, 0}, {"y0", 'd', 1, 0}, {"z0", 'd', 2, 0}, \
		{"r", 'd', 3, 0}, {"color", 's', 0, 0}};

	S	s_pln[9] = {{"x0", 'd', 0, 0}, {"y0", 'd', 1, 0}, {"z0", 'd', 2, 0}, \
		{"a", 'd', 3, 0}, {"b", 'd', 4, 0}, {"c", 'd', 5, 0}, \
		{"color1", 's', 0, 0}, {"color2", 's', 1, 0}, {"texscale", 'd', 6, 0}};

	S	s_cxy[4] = {{"x0", 'd', 0, 0}, {"y0", 'd', 1, 0}, {"r", 'd', 2, 0}, \
		{"color", 's', 0, 0}};

	S	s_cxz[4] = {{"x0", 'd', 0, 0}, {"z0", 'd', 1, 0}, {"r", 'd', 2, 0}, \
		{"color", 's', 0, 0}};

	S	s_cyz[4] = {{"y0", 'd', 0, 0}, {"z0", 'd', 1, 0}, {"r", 'd', 2, 0}, \
		{"color", 's', 0, 0}};

	S	s_lig[3] = {{"x", 'd', 0, 0}, {"y", 'd', 1, 0}, {"z", 'd', 2, 0}};

	S	s_cam[7] = {{"x", 'd', 0, 0}, {"y", 'd', 1, 0}, {"z", 'd', 2, 0}, \
		{"lx", 'd', 3, 0}, {"ly", 'd', 4, 0}, {"lz", 'd', 5, 0}, \
		{"rot", 'd', 6, 0}};
		
	
	

	int		ln = strlen(line);
	int		i, j, k;
	char	name[MAX_NAME_LEN], c;
	S		*s_tab;
	S		*s;
	char	str[MAX_LABEL_LEN + MAX_VAL_LEN + 1];
	int		str_i = 0;
	char	label[MAX_LABEL_LEN];
	char	val[MAX_VAL_LEN];
	int		mode;
	int		found_label;
	BYTE	col;
	int		num_of_s;
	int		ci, col_found = 0;


	if (line[ln - 1] != ')') return ERR_LINEEND;

	for (i = 0; ; i++) {
		if (i == MAX_NAME_LEN) return ERR_NAMELEN;

		c = line[i];
		if (c == '(') break;

		if (c < 'a' || c > 'z') return ERR_NAMECHAR;

		name[i] = c;
	}
	name[i++] = 0;
	//printf("name: %s\n", name);

	if (strcmp(name, "sphere") == 0) {
		*id = ID_SPHERE;
		s_tab = s_sph;
		num_of_s = 5;
	}
	else if (strcmp(name, "plane") == 0) {
		*id = ID_PLANE;
		s_tab = s_pln;
		num_of_s = 9;
	}
	else if (strcmp(name, "camera") == 0) {
		*id = ID_CAMERA;
		s_tab = s_cam;
		num_of_s = 7;
	}
	else if (strcmp(name, "light") == 0) {
		*id = ID_LIGHT;
		s_tab = s_lig;
		num_of_s = 3;
	}
	else if (strncmp(name, "cylinder", 8) == 0) {
		if (strcmp(name, "cylinderxy") == 0) {
			*cyl_type = CYL_XY;
			s_tab = s_cxy;
		}
		else if (strcmp(name, "cylinderxz") == 0) {
			*cyl_type = CYL_XZ;
			s_tab = s_cxz;
		}
		else if (strcmp(name, "cylinderyz") == 0) {
			*cyl_type = CYL_YZ;
			s_tab = s_cyz;
		}
		else
			return ERR_BADNAME;
						
		*id = ID_CYLINDER;
		num_of_s = 4;
	}
	else
		return ERR_BADNAME;


	for (; i < ln; i++) {
		c = line[i];

		if (c == ';' || ((c == ')') && (i == (ln - 1)))) {
			str[str_i] = 0;
			str_i = 0;
			//printf("str: %s\n", str);

			if (!strlen(str) || (str[0] == '=')) return ERR_NOLABEL;

			mode = 0;
			for (j = 0; j < strlen(str); j++) {
				c = str[j];

				if (mode == 0) {
					if (j == MAX_LABEL_LEN) return ERR_LABELLEN;

					if (c == '=') {
						mode++;
						k = 0;
						label[j] = 0;
						//printf("label: %s\n", label);
					}
					else
						label[j] = c;
				}
				else {
					val[k++] = c;
					if (k == MAX_VAL_LEN) return ERR_VALLEN;
				}
			}
			val[k] = 0;
			//printf("val: %s\n", val);

			s = s_tab;
			found_label = 0;
			for (j = 0; j < num_of_s; j++, s++) {
				if (strcmp(label, s->label) == 0) {
					found_label = 1;
					break;
				}
			}

			if (!found_label) return ERR_LABEL;

			if (s->ok) return ERR_LABELDBL;

			if (s->data_type == 'd') {
				if (!is_double(val)) return ERR_NUM;
				d_tab[s->index] = atof(val);
			}
			else /* if (s->data_type == 's') */ {
				for (ci = 0; ci < 8; ci++) {
					if (strcmp(val, col_tab[ci].name) == 0 ||
						strcmp(val, col_tab[ci].alt_name) == 0) {
						col = col_tab[ci].id;
						col_found = 1;
						break;	
					}
				}
			
				if (!col_found) return ERR_COLOR;

				b_tab[s->index] = col;
			}

			s->ok = 1;
		}
		else {
			str[str_i++] = c;
			if (str_i == (MAX_LABEL_LEN + MAX_VAL_LEN + 1)) return ERR_CHARS;
		}
	}

	s = s_tab;
	for (i = 0; i < num_of_s; i++, s++) {
		if (!s->ok) return ERR_NUMARGS;
	}


	return ERR_OK;
}

//------------------------------------------------------------

int set_object(WORLD *w, int id, int type, double *dbl_nums, BYTE *byte_nums) {
	SPHERE		*sp;
	PLANE		*pl;
	CYLINDER	*cl;
	VECTOR		z_vec, t_vec;
	double		t;
	
	switch (id) {
		case ID_CAMERA:
			w->cam.pos.x     = dbl_nums[0];
			w->cam.pos.y     = dbl_nums[1];
			w->cam.pos.z     = dbl_nums[2];
			w->cam.look_at.x = dbl_nums[3];
			w->cam.look_at.y = dbl_nums[4];
			w->cam.look_at.z = dbl_nums[5];
			w->cam.rot       = dbl_nums[6];
			
			t = floor(w->cam.rot / 360.0);
			if (w->cam.rot > 360.0) t = -t;
			w->cam.rot += t * 360;
			w->cam.rot *= M_PI / 180.0;
			
			break;
			
		case ID_LIGHT:
			w->light.pos.x   = dbl_nums[0];
			w->light.pos.y   = dbl_nums[1];
			w->light.pos.z   = dbl_nums[2];
			break;
			
		case ID_SPHERE:
			sp = add_sphere(w);
			if (!sp) return ERR_SPHMEM;

			sp->x0   = dbl_nums[0];
			sp->y0   = dbl_nums[1];
			sp->z0   = dbl_nums[2];
			sp->R    = dbl_nums[3];
			if (sp->R < 0.0) return ERR_RADIUS;

			sp->color = byte_nums[0];
			
			break;
			
		case ID_PLANE:
			pl = add_plane(w);
			if (!pl) return ERR_PLNMEM;
			
			pl->x0     = dbl_nums[0];
			pl->y0     = dbl_nums[1];
			pl->z0     = dbl_nums[2];
			pl->A      = dbl_nums[3];
			pl->B      = dbl_nums[4];
			pl->C      = dbl_nums[5];
			pl->color1 = byte_nums[0];
			pl->color2 = byte_nums[1];
				
			pl->tex_scale = dbl_nums[6];
			if (pl->tex_scale <= 0.0) return ERR_NEGSCALE;
			pl->tex_scale = 1.0 / pl->tex_scale;
				
			z_vec.x = pl->A;
			z_vec.y = pl->B;
			z_vec.z = pl->C;
			t = 1.0 / vector_len(&z_vec);
			pl->A *= t;
			pl->B *= t;
			pl->C *= t;
				
			if (pl->A == 0.0 && pl->B == 0.0) {
				pl->tex_x.x = 1.0;
				pl->tex_x.y = 0.0;
				pl->tex_x.z = 0.0;
				pl->tex_y.x = 0.0;
				pl->tex_y.y = 1.0;
				pl->tex_y.z = 0.0;
			}
			else {
				t_vec.x = 0.0;
				t_vec.y = 0.0;
				t_vec.z = 1.0;
				cross_product(&z_vec, &t_vec, &pl->tex_y);
				cross_product(&z_vec, &pl->tex_y, &pl->tex_x);
				t = 1.0 / vector_len(&pl->tex_x);
				pl->tex_x.x = -pl->tex_x.x * t;
				pl->tex_x.y = -pl->tex_x.y * t;
				pl->tex_x.z = -pl->tex_x.z * t;
				t = 1.0 / vector_len(&pl->tex_y);
				pl->tex_y.x *= t;
				pl->tex_y.y *= t;
				pl->tex_y.z *= t;
			}
				
			break;
			
		case ID_CYLINDER:
			cl = add_cylinder(w);
			if (!cl) return ERR_CYLMEM;

			cl->m = dbl_nums[0];
			cl->n = dbl_nums[1];
			cl->R = dbl_nums[2];
			if (cl->R < 0.0) return ERR_RADIUS;

			cl->color = byte_nums[0];
			cl->type  = (BYTE)(type);
	}

	return ERR_OK;
}

//------------------------------------------------------------

int build_world(WORLD *w, char *cfgfile, int res, int *err_line, int *err_char,
	DWORD *amem) {
	FILE	*p;
	char	line[MAX_LINE_LEN], c;
	int		n_lines = 0, curr_char = 0, in_comment = 0;
	int		f_line = 1, f_char = 0;
	int		i, j, r, id;
	double	dbl_nums[16];
	BYTE	byte_nums[16];
	int		is_camera = 0, is_light = 0;
	int		n_spheres = 0, n_planes = 0, n_cylinders = 0;
	int		hor, ver;
	int		eof_detected = 0;
	int		type;
	

	switch (res) {
		case RES_320X240:
			hor = 320;
			ver = 240;
			break;
		case RES_400X300:
			hor = 400;
			ver = 300;
			break;
		case RES_640X480:
			hor = 640;
			ver = 480;
			break;
		case RES_800X600:
			hor = 800;
			ver = 600;
			break;
		case RES_1024X768:
			hor = 1024;
			ver = 768;
			break;
		case RES_1600X1200:
			hor = 1600;
			ver = 1200;
			break;
		default:
			return ERR_RES;
	}

	r = set_world(w, hor, ver, ANGLE_OF_VIEW, amem);
	if (r != ERR_OK) return r;

	p = fopen(cfgfile, "r");
	if (!p) {
		free_world(w);
		return ERR_NOFILE;
	}

	for (i = 0; i < (int)(strlen(cfgfile)); i++) {
		c = cfgfile[i];
		if (c == '.' || i == MAX_FILENAME_LEN) break;

		w->file[i] = c;
	}
	w->file[i] = 0;


	while (!eof_detected) {
		if (feof(p)) {
		  eof_detected = 1;
		  f_line++;
		}
	
		c = tolower(getc(p));
		if (c == 0x0a) {
			f_line++;
			f_char = 0;
		}
		else if (c != 0x0d)
			f_char++;

		if (!in_comment) {
			if (c < '!') {
				if (c == 0x0a || eof_detected) {
					if (curr_char) {
						line[curr_char] = 0;
						curr_char = 0;
						n_lines++;


						r = parse_line(line, &id, &type, dbl_nums, byte_nums);
						if (r != ERR_OK) {
							*err_line = f_line - 1;
							free_world(w);
							return r;
						}
						
						r = set_object(w, id, type, dbl_nums, byte_nums);
						if (r != ERR_OK) {
							*err_line = f_line - 1;
							free_world(w);
							return r;
						}

						switch (id) {
							case ID_CAMERA:
								if ((++is_camera) > 1) {
									free_world(w);
									return ERR_CAMERA;
								}
								break;
								
							case ID_LIGHT:
								if ((++is_light) > 1) {
									free_world(w);
									return ERR_LIGHT;
								}
								break;
								
							case ID_SPHERE:
								*amem += sizeof(SPHERE);
								n_spheres++;
								break;
								
							case ID_PLANE:
								*amem += sizeof(PLANE);
								n_planes++;
								break;
								
							case ID_CYLINDER:
								*amem += sizeof(CYLINDER);
								n_cylinders++;
						}
					}
				}
			}
			else {
				if (c == '{') {
					if (!in_comment && curr_char) {
						line[curr_char] = 0;
						curr_char = 0;
					}
					in_comment++;
				}
				else if (c == '}') {
					fclose(p);
					*err_line = f_line;
					*err_char = f_char;
					free_world(w);
					return ERR_COMMENT;
				}
				else
					line[curr_char++] = c;

					if (curr_char == MAX_LINE_LEN) {
						fclose(p);
						*err_line = f_line;
						free_world(w);
						return ERR_CHARS;
					}
			}
		}
		else {
			if (c == '}') {
				if ((--in_comment) < 0) {
					fclose(p);
					*err_line = f_line;
					*err_char = f_char;
					free_world(w);
					return ERR_COMMENT;
				}
			}
			else if (c == '{')
				in_comment++;
		}
	}

	fclose(p);

	if (in_comment) {
		free_world(w);
		return ERR_COMMENT2;
	}

	if (!n_lines) {
		free_world(w);
		return ERR_EMPTY;
	}

	if (!is_camera) {
		free_world(w);
		return ERR_NOCAMERA;
	}
	
	if (!is_light) {
		free_world(w);
		return ERR_NOLIGHT;
	}
		
	if (!n_planes && !n_spheres && !n_cylinders) {
		free_world(w);
		return ERR_NOOBJ;
	}

	return ERR_OK;
}

//------------------------------------------------------------

int main(int argc, char **argv) {
	WORLD	world;
	int		r, el, ec;
	clock_t	tp = clock(), tk;
	DWORD	file_size, memory;
	int		i;
	int		input_err = 0, silent = 0;
	int		res = -1, file_index = -1, pal_index = -1;
#ifdef __GNUC__
	DWORD	div_clock = CLOCKS_PER_SEC;
#else
	DWORD	div_clock = CLK_TCK;
#endif


	if (argc < 2) {
		input_err = 1;
	}
	
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--res") == 0) {
			r = atoi(argv[i + 1]);
			if (r < 1 || r > 6 || res != -1) {
				input_err = 1;
				break;
			}
			res = r;
			i++;
		}
		else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--silent") == 0) {
			if (silent) {
				input_err = 1;
				break;
			}
			silent = 1;
		}
		else if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--pal") == 0) {
			if (pal_index != -1) {
				input_err = 1;
				break;
			}
			pal_index = i + 1;
			i++;
		}
		else {
			if (file_index != -1 || argv[i][0] == '-') {
				input_err = 1;
				break;
			}
			file_index = i;
		}
	}
	
	if (file_index == -1) input_err = 1;
	
	if (!silent || input_err) {
		printf("%s%s%s%s%s%s\n", "MRT ", VERSION, " -- ", __DATE__, " -- ", __TIME__);
		printf("%s\n", "Idea & coding by Mikolaj 'Majuma' Feliks, see http://www.majuma.xt.pl");
	}
	
	if (input_err) {
		printf("%s\n", "USAGE: mrt [-r <res>] [-s] [-p <file>] file");
		printf("%s\n", " -r --res     resolution (1 = 320x240  2 = 400x300   3 = 640x480");
		printf("%s\n", "                          4 = 800x600  5 = 1024x768  6 = 1600x1200, default = 4)");
		printf("%s\n", " -s --silent  silent mode (no messages)");
		printf("%s\n", " -p --pal     palette file");
		return ERR_SYNTAX;
	}

	if (res == -1) res = RES_800X600;

	r = build_world(&world, argv[file_index], res, &el, &ec, &memory);
	if (r != ERR_OK) {
		if (silent) return r;
	
		switch (r) {
		case ERR_NOFILE:
			printf("%s\n", "file not found");
			return r;
		case ERR_COMMENT:
			printf("%s%d%s%d\n", "parse error in line ", el, " char ", ec);
			return r;
		case ERR_CHARS:
			printf("%s%d%s\n", "line ", el, " is too long");
			return r;
		case ERR_COMMENT2:
			printf("%s\n", "open comment, } expected");
			return r;
		case ERR_EMPTY:
			printf("%s\n", "file is empty");
			return r;
		case ERR_NAMECHAR:
			printf("%s%d%s\n", "line ", el, ": illegal chars in object name");
			return r;
		case ERR_NAMELEN:
			printf("%s%d%s\n", "line ", el, ": object name is too long");
			return r;
		case ERR_NONAME:
			printf("%s%d%s\n", "line ", el, ": object has no name");
			return r;
		case ERR_BADNAME:
			printf("%s%d%s\n", "line ", el, ": unrecognized object name");
			return r;
		case ERR_NUMLEN:
			printf("%s%d%s\n", "line ", el, ": number is too long");
			return r;
		case ERR_NUM:
			printf("%s%d%s\n", "line ", el, ": illegal number or no number");
			return r;
		case ERR_LINEEND:
			printf("%s%d%s\n", "line ", el, ": illegal end of line");
			return r;
		case ERR_NUMARGS:
			printf("%s%d%s\n", "line ", el, ": incorrect number of arguments");
			return r;
		case ERR_CAMERA:
			printf("%s\n", "only one camera is allowed");
			return r;
		case ERR_LIGHT:
			printf("%s\n", "only one light is allowed");
			return r;
		case ERR_SCRMEM:
			printf("%s\n", "unable to alloc screen");
			return r;
		case ERR_SPHMEM:
			printf("%s\n", "unable to alloc sphere table");
			return r;
		case ERR_PLNMEM:
			printf("%s\n", "unable to alloc plane table");
			return r;
		/*case ERR_RES:
			printf("%s\n", "incorrect resolution");
			return r;*/
		case ERR_COLOR:
			printf("%s%d%s\n", "line ", el, ": incorrect color");
			return r;
		case ERR_RADIUS:
			printf("%s%d%s\n", "line ", el, ": negative radius");
			return r;
		case ERR_NOCAMERA:
			printf("%s\n", "no camera defined");
			return r;
		case ERR_NOLIGHT:
			printf("%s\n", "no light defined");
			return r;
		case ERR_NOOBJ:
			printf("%s\n", "world is empty");
			return r;
		case ERR_CYLMEM:
			printf("%s\n", "unable to alloc cylinder table");
			return r;
		case ERR_NEGSCALE:
			printf("%s%d%s\n", "line ", el, ": scale must be above zero");
			return r;
		case ERR_LABELLEN:
			printf("%s%d%s\n", "line ", el, ": label is too long");
			return r;
		case ERR_VALLEN:
			printf("%s%d%s\n", "line ", el, ": value is too long");
			return r;
		case ERR_LABEL:
			printf("%s%d%s\n", "line ", el, ": incorrect label");
			return r;
		case ERR_LABELDBL:
			printf("%s%d%s\n", "line ", el, ": multiple occurrance of label");
			return r;
		case ERR_NOLABEL:
			printf("%s%d%s\n", "line ", el, ": no label");
			return r;
		}
	}

	if (!silent) printf("%s%d%s\n", "world OK, allocated ", (memory >> 10), " kB");
	
	
	if (pal_index != -1) {
		if ((r = LoadPalette(&world.cam.scr, argv[pal_index])) != PAL_OK)
			if (!silent) printf("%s%d%s\n", "unable to load palette (", r, "); default palette will be used");
	}
	

	render(&world, (silent + 1) & 1);

	r = ERR_OK;
	file_size = write_to_file(&world);
	if (!file_size) {
		if (!silent) printf("%s\n", "unable to write file");
		r = ERR_WRITE;
	}
	else {
		if (!silent) printf("%s%d%s\n", "file size: ", (file_size >> 10), " kB");
	}

	free_world(&world);


	tk = clock();
	if (tk >= 0)
		if (!silent) printf("%s%.4f%s\n", "total time: ", ((tk - tp) /
			(double)(div_clock)), " sec");
	
	return r;
}
