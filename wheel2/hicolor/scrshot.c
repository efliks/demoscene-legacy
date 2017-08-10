/* saves the screen as 24-bit bitmap file */

#define BMP_HEADER_SIZE 54

unsigned char bmp_header[BMP_HEADER_SIZE]={66,77,54,238,2,0,0,0,0,0,54,0,0,0,40,
	0,0,0,64,1,0,0,200,0,0,0,1,0,24,0,0,0,0,0,0,238,2,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0};

int save_screen(char *filename)
{
	FILE *p;
	unsigned i,j;
	short *ofs,color;

	p=fopen(filename,"w+b");
	if(!p) return 0;

	fwrite(&bmp_header,1,BMP_HEADER_SIZE,p);

	ofs=(short *)frame_buffer;
	ofs+=(64000-320);

	for(i=0;i<200;i++)
	{
		for(j=0;j<320;j++)
		{
			color=*(ofs+j);

			fputc(GET_B(color)<<3,p);
			fputc(GET_G(color)<<2,p);
			fputc(GET_R(color)<<3,p);
		}

		ofs-=320;
	}

	fclose(p);
	return 1;
}

int screenshot(void)
{
	static unsigned frame_nr=1;
	char btxt1[32],btxt2[32];

	if(frame_nr<10)
		strcpy(btxt1,"0000");
	else if(frame_nr<100)
		strcpy(btxt1,"000");
	else if(frame_nr<1000)
		strcpy(btxt1,"00");
	else if(frame_nr<10000)
		strcpy(btxt1,"0");
	else
		return 0;

	strcat(btxt1,itoa(frame_nr,btxt2,10));
	strcat(btxt1,".bmp");

	if(!save_screen(btxt1)) return 0;

	frame_nr++;
	return 1;
}