/*
	3D Letters Generator 1.0 (11/02/02, 16/02/02, 19/03/02)
	Converts standard system font to 3d objects in *.3d format.

	Coded by Mikolaj Felix a.k.a. Majuma
	Web: www.ps.nq.pl / www.naag.prv.pl
	Mail: mfelix@polbox.com
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#include "letgen.h"

#define ASM_OUTPUT
#define LOG_FILENAME "fontz.txt"

long SaveLetter3d(tiny_struct3d *b, char *filename)
{
	FILE *p;
	int i;
	short temp;

	p = fopen(filename, "w+b");
	if (!p) return 0;
	
	fputc('M', p);
	fputc('F', p);
	fputc('3', p);
	fputc('D', p);

	temp = (short)b->n_vertices;
	fwrite(&temp, 2, 1, p);
	temp = (short)b->n_faces;
	fwrite(&temp, 2, 1, p);

	fwrite(b->vertices, sizeof(vertex3d), b->n_vertices, p);
	fwrite(b->faces, sizeof(face), b->n_faces, p);

	fclose(p);
	return 1;
}

int main(void)
{
	tiny_struct3d *Buffer3d = EnableBuffer3d();
	unsigned i, j;
	char txt_buffer[16];
	FILE *p;
	long ret_val;

	printf("%s", "*** 3D Letters Generator 1.0 ***\n");
	printf("%s", "Coded by Mikolaj Felix a.k.a. Majuma\n");
	printf("%s", "Web: www.ps.nq.pl / www.naag.prv.pl\nMail: mfelix@polbox.com\n");
	printf("%s%s%s%s%s", "Build: ", __DATE__, " ", __TIME__, "\n\n");

	printf("%s", "Generating font files: ");

	p = fopen(LOG_FILENAME, "w");

	for (i = 0; i < 256; i++)
	{
		MakeLetter3d(i);

		itoa(i, txt_buffer, 10);
		strcat(txt_buffer, ".3d");

		SaveLetter3d(Buffer3d, txt_buffer);
	
		#ifndef ASM_OUTPUT
		fprintf(p, "%s%s%s%d%s%d%s", "File: ", txt_buffer, " -> ",
			Buffer3d->n_vertices, " vertices, ", Buffer3d->n_faces, " faces\n");
		#else
		fprintf(p, "%s%d%s%d%s%d%s", "dw ", Buffer3d->n_vertices, ", ",
			Buffer3d->n_faces, "\t; ", i, "\n");
		#endif

		
		itoa((int)((float)i / 256.0 * 100.0), txt_buffer, 10);
		printf("%s%s", txt_buffer, "%");

		for (j = 0; j < strlen(txt_buffer) + 1; j++)
			printf("%s", "\b");
	}

	fclose(p);

	printf("%s", "100% Done !\n\n");
	getch();

	return 0;
}
