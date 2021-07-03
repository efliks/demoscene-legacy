/*
	Make 3D String 1.0 (12/02/02)

	Coded by Mikolaj Felix a.k.a. Majuma
	Web: www.ps.nq.pl / www.naag.prv.pl
	Mail: mfelix@polbox.com
*/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>

#define MAX_LEN 16
#define ADD_X 20.0

#define SCALE_LETTER 0.5

typedef struct
{
	char filename[11];
	unsigned short n_vertices, n_faces;
	float width;
}FileInfo;


long MakeString3d(FileInfo *src_table, int n_files,
	char *output)
{
	FILE *in, *out;
	unsigned short i, j, k, max_v, max_f, t16, add_v;
	float t32, move_x;

	out = fopen(output, "w+b");

	fputc('M', out);
	fputc('F', out);
	fputc('3', out);
	fputc('D', out);

	max_v = max_f = 0;
	for (i=0; i<n_files; i++)
	{
		max_v += src_table[i].n_vertices;
		max_f += src_table[i].n_faces;
	}

	fwrite(&max_v, 2, 1, out);
	fwrite(&max_f, 2, 1, out);

	move_x = 0;
	for (i=0; i<n_files; i++)
	{
		in = fopen(src_table[i].filename, "r+b");
		fseek(in, 8, SEEK_SET);

		for (j=0; j<src_table[i].n_vertices; j++)
		{
			fread(&t32, 4, 1, in);
			t32 += move_x;
			t32 *= SCALE_LETTER;
			fwrite(&t32, 4, 1, out);

			fread(&t32, 4, 1, in);
			t32 *= SCALE_LETTER;
			fwrite(&t32, 4, 1, out);

			fread(&t32, 4, 1, in);
			t32 *= SCALE_LETTER;
			fwrite(&t32, 4, 1, out);
		}

		fclose(in);
		move_x += (src_table[i].width + ADD_X);
	}

	add_v = 0;
	for (i=0; i<n_files; i++)
	{
		in = fopen(src_table[i].filename, "r+b");
		fseek(in, (src_table[i].n_vertices * 12) + 8, SEEK_SET);

		for (j=0; j<src_table[i].n_faces; j++)
		{
			for (k=0; k<3; k++)
			{
				fread(&t16, 2, 1, in);
				t16 += add_v;
				fwrite(&t16, 2, 1, out);
			}
		}

		fclose(in);
		add_v += src_table[i].n_vertices;
	}

	fclose(out);

	/* high word = max vertices
		low word = max faces */
	return ((max_v << 16) | (max_f & 0xffff));
}

int GetFile(FileInfo *str, char *filename)
{
	FILE *p;
	float min_x, max_x, curr_x;
	unsigned short i;

	p = fopen(filename, "r+b");
	if (!p) return 0;

	if (fgetc(p) != 'M' || fgetc(p) != 'F' ||
		fgetc(p) != '3' || fgetc(p) != 'D')
	{
		fclose(p);
		return 0;
	}

	strcpy(str->filename, filename);
	fread(&str->n_vertices, 2, 1, p);
	fread(&str->n_faces, 2, 1, p);

	min_x = max_x = 0;
	for (i=0; i<str->n_vertices; i++)
	{
		fread(&curr_x, 4, 1, p);
		fseek(p, 8, SEEK_CUR);

		if (curr_x < min_x)
			min_x = curr_x;

		if (curr_x > max_x)
			max_x = curr_x;
	}

	str->width = abs(max_x - min_x);

	fclose(p);
	return 1;
}

int main(int argc, char **argv)
{
	int i, err_counter, n_files;
	char *ptr, txt_buffer[11];
	FileInfo list[MAX_LEN];
	long ret_val;

	printf("%s", "Make 3D String 1.0 by Majuma\n");
	printf("%s", "Mail: mfelix@polbox.com\nWeb: www.ps.nq.pl / www.naag.prv.pl\n");
	printf("%s%s%s%s%s", "Build: ", __DATE__, " ", __TIME__, "\n\n");

	if (argc < 3)
	{
		printf("%s", "Syntax: makestr string output_file\n\n");
		getch();
		return 1;
	}

	i = strlen(argv[1]);
	if (i > MAX_LEN)
	{
		printf("%s", "String is too long!\n\n");
		getch();
		return 2;
	}

	ptr = argv[1];
	err_counter = n_files = 0;

	printf("%s", "Making list of letters... ");

	for (; i>0; i--, ptr++)
	{
		itoa((int)*ptr, txt_buffer, 10);
		strcat(txt_buffer, ".3d");

		if(!GetFile(&list[n_files], txt_buffer))
		{
			err_counter++;
			printf("%s%s", "\nUnable to open file: ", txt_buffer);
		}
		else
			n_files++;
	}

	if(!err_counter)
	{
		printf("%s%d%s", "Done ! (Letters: ", n_files, ")\n");
		printf("%s", "Making 3d string... ");

		ret_val = MakeString3d(list, n_files, argv[2]);

		printf("%s%d%s%d%s", "Done ! (Vertices: ", ret_val >> 16,
			", Faces: ", ret_val & 0xffff, ")\n\n");
	}
	else
		printf("%s%d%s", "\nErrors: ", err_counter, "\n\n");

	getch();

	return 0;
}
