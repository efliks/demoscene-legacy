/*
	Adjust Object 3D 1.0 (13/02/02)

	Coded by Mikolaj Felix a.k.a. Majuma
	Web: www.ps.nq.pl / www.naag.prv.pl
	Mail: mfelix@polbox.com
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


typedef struct
{
	float x, y, z;
}vertex3d;

int main(int argc, char **argv)
{
	FILE *p;
	short max_v, max_f, i;
	vertex3d *obj_vertices;
	double ox, oy, oz;
	float radius, temp_r, scale, scale_me;

	if (argc < 3)
	{
		printf("%s", "Syntax: adjust filename scale\n");
		return 1;
	}

	p = fopen(argv[1], "r+b");
	if (!p)
	{
		printf("%s", "Unable to open file!\n");
		return 2;
	}

	if (fgetc(p) != 'M' || fgetc(p) != 'F' ||
		fgetc(p) != '3' || fgetc(p) != 'D')
	{
		printf("%s", "Invalid file format!\n");
		fclose(p);
		return 3;
	}

	fread(&max_v, 2, 1, p);
	fread(&max_f, 2, 1, p);

	obj_vertices = (vertex3d *)malloc(max_v * sizeof(vertex3d));
	if (!obj_vertices)
	{
		printf("%s", "Not enough memory!\n");
		fclose(p);
		return 4;
	}

	fread(obj_vertices, sizeof(vertex3d), max_v, p);
	fclose(p);


	/* center object */
	ox = oy = oz = 0;
	for (i = 0; i < max_v; i++)
	{
		ox += (obj_vertices + i)->x;
		oy += (obj_vertices + i)->y;
		oz += (obj_vertices + i)->z;
	}

	ox /= max_v;
	oy /= max_v;
	oz /= max_v;

	for (i = 0; i < max_v; i++)
	{
		(obj_vertices + i)->x -= ox;
		(obj_vertices + i)->y -= oy;
		(obj_vertices + i)->z -= oz;
	}


	/* normalize and scale object */
	scale_me = atof(argv[2]);

	radius = 0;
	for (i = 0; i < max_v; i++)
	{
		temp_r = pow((obj_vertices + i)->x, 2) +
			pow((obj_vertices + i)->y, 2) +
			pow((obj_vertices + i)->x, 2);

		temp_r = sqrt(temp_r);

		if (temp_r > radius)
			radius = temp_r;
	}
	scale = 1.0 / radius;
	scale *= scale_me;

	for (i = 0; i < max_v; i++)
	{
		(obj_vertices + i)->x *= scale;
		(obj_vertices + i)->y *= scale;
		(obj_vertices + i)->z *= scale;
	}


	p = fopen(argv[1], "r+b");
	fseek(p, 8, SEEK_SET);
	fwrite(obj_vertices, sizeof(vertex3d), max_v, p);
	fclose(p);

	printf("%s", "Done !\n");
	free(obj_vertices);

	return 0;
}