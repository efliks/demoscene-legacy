/*
	FlatGrd2 06/02/02
	Mikolaj Felix a.k.a. Majuma
	mfelix@polbox.com
*/

/*
unsigned char bmp_header[BMP_HEADER_SIZE]={0x42,0x4d,0x36,0xfe,0,0,0,0,0,0,0x36,
	0x4,0,0,0x28,0,0,0,0x40,0x1,0,0,0xc8,0,0,0,0x1,0,0x8,0,0,0,0,0,0,0xfa,0,0,0xc4,0xe,0,0,
	0xc4,0xe,0,0,0,0,0,0,0,0,0,0};
*/

unsigned char pcx_header[PCX_HEADER_SIZE]={10,5,1,8,0,0,0,0,63,1,199,0,64,1,200,
	0,254,36,242,191,119,115,70,0,0,0,0,0,118,7,0,0,124,6,0,0,144,244,124,0,172,
	240,124,0,24,0,0,0,197,185,247,191,24,0,0,0,0,0,0,0,2,0,0,0,1,1,64,1,1,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

char frame_file[16];
long bytes_written=0;

long dump_pcx(char *filename)
{
	FILE *p;
	long file_size;
	unsigned char *buf_off,curr_b;
	long buf_counter,i,bytes_found;

	p=fopen(filename,"w+b");
	if(!p) return 0;

	fwrite(&pcx_header,1,PCX_HEADER_SIZE,p);

	buf_off=frame_buffer;
	buf_counter=0;

	while(buf_counter<64000)
	{
		curr_b=*buf_off;

		bytes_found=0;
		for(i=0;i<63;i++)
		{
			if((buf_counter+i)>63999) break;

			if(*(buf_off+i)==curr_b)
				bytes_found++;
			else
				break;
		}

		if(curr_b>=192) curr_b=191;

		if(!bytes_found)
		{
			fputc(curr_b,p);
			buf_counter++;
			buf_off++;
		}
		else
		{
			fputc(bytes_found|192,p);
			fputc(curr_b,p);
			buf_counter+=bytes_found;
			buf_off+=bytes_found;
		}
	}

	buf_off=palette;
	for(i=0;i<768;i++,buf_off++) fputc((*buf_off)<<2,p);
	
	file_size=ftell(p);
	fclose(p);
	return file_size;
}

/*
long dump_bmp(char *filename)
{
	FILE *p;
	long i;
	unsigned char *off=frame_buffer;

	p=fopen(filename,"w+b");
	if(!p) return 0;

	fwrite(&bmp_header,1,BMP_HEADER_SIZE,p);

	for(i=0;i<768;i+=3)
	{
		fputc(palette[i+2]<<2,p);
		fputc(palette[i+1]<<2,p);
		fputc(palette[i+0]<<2,p);
		fputc(0,p);
	}

	for(i=199;i>=0;i--)
		fwrite(off+(i*320),1,320,p);

	fclose(p);
	return 64000+BMP_HEADER_SIZE+1024;
}
*/

int save_frame(void)
{
	static unsigned long frame_nr=1;
	char btxt1[32],btxt2[32];
	long file_size;

	strcpy(btxt1,"dump\\");

	if(frame_nr<10)
		strcat(btxt1,"0000");
	else if(frame_nr<100)
		strcat(btxt1,"000");
	else if(frame_nr<1000)
		strcat(btxt1,"00");
	else if(frame_nr<10000)
		strcat(btxt1,"0");
	else
		return 0;

	strcat(btxt1,itoa(frame_nr,btxt2,10));
	strcat(btxt1,".pcx");

	file_size=dump_pcx(btxt1);
	if(!file_size) return 0;

	strcpy(frame_file,btxt1);
	bytes_written+=file_size;

	frame_nr++;
	return 1;
}