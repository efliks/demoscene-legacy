/* supports BMP files in 24-bit or 8-bit format */

typedef struct
{
	long width,height;
	short *data;
}BMP;

char bmp_message[128];

BMP *load_bmp(char *filename)
{
	FILE *p;
	BMP *b;
	unsigned char _r,_g,_b;
	short bpp;
	long i,j;
	short *data_off;
	struct { unsigned char r,g,b; }pal_colors[768];
	int col_num;

	strcpy(bmp_message,filename);
	
	p=fopen(filename,"r+b");
	if(!p)
	{
		strcat(bmp_message,": file not found.");
		return 0;
	}

	b=(BMP *)malloc(sizeof(BMP));
	if(!b)
	{
		strcat(bmp_message,": out of memory.");
		fclose(p);
		return 0;
	}

	fseek(p,18,SEEK_SET);
	fread(&b->width,4,1,p);
	fread(&b->height,4,1,p);

	fseek(p,28,SEEK_SET);
	fread(&bpp,2,1,p);
	
	fseek(p,54,SEEK_SET);

	if(bpp==8)
	{
		for(i=0;i<768;i++)
		{
			_b=fgetc(p)>>3;
			_g=fgetc(p)>>2;
			_r=fgetc(p)>>3;
			fgetc(p);

			pal_colors[i].r=_r;
			pal_colors[i].g=_g;
			pal_colors[i].b=_b;
		}

		fseek(p,1024+54,SEEK_SET);
	}
	else if(bpp!=24)
	{
		strcat(bmp_message,": 24-bit or 8-bit format expected.");
		free(b);
		fclose(p);
		return 0;
	}

	b->data=(short *)malloc((b->width*b->height)<<1);
	if(!b->data)
	{
		strcat(bmp_message,": out of memory.");
		free(b);
		fclose(p);
		return 0;
	}

	data_off=b->data+((b->height-1)*b->width);
	for(i=0;i<b->height;i++)
	{
		for(j=0;j<b->width;j++)
		{
			if(bpp==8)
			{
				col_num=fgetc(p);
				_r=pal_colors[col_num].r;
				_g=pal_colors[col_num].g;
				_b=pal_colors[col_num].b;
			}
			else
			{
				_b=fgetc(p)>>3;
				_g=fgetc(p)>>2;
				_r=fgetc(p)>>3;
			}

			*(data_off+j)=COLOR16(_r,_g,_b);
		}
		
		data_off-=b->width;
	}

	strcat(bmp_message,": ok.");
	fclose(p);
	return b;
}

void unload_bmp(BMP *b)
{
	free(b->data);
	free(b);
}