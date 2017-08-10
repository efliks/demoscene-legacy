/*
	FlatGrd2 06/02/02
	Mikolaj Felix a.k.a. Majuma
	mfelix@polbox.com
*/

void random_phong_palette(void)
{
	int i;
	float angle,illumination;
	float diff_r,spec_r;
	float diff_g,spec_g;
	float diff_b,spec_b;
	unsigned char r,g,b;

	//static n_file;
	//FILE *p;
	//char txt_buffer[32];


	//p=fopen("palettes.inc","a");
	//itoa(n_file,txt_buffer,10);
	//fprintf(p,"%s%s%s","pal",txt_buffer,"_params:\n");


	srand( (int)rawclock() );

	diff_r = (float)(rand() % 20);
	//fprintf(p, "%s%f%s", "\tdd ", diff_r, "\n");

	spec_r = (float)(rand() % 50);
	//fprintf(p, "%s%f%s", "\tdd ", spec_r, "\n");

	diff_g = (float)(rand() % 20);
	//fprintf(p, "%s%f%s", "\tdd ", diff_g, "\n");

	spec_g = (float)(rand() % 50);
	//fprintf(p, "%s%f%s", "\tdd ", spec_g, "\n");

	diff_b = (float)(rand() % 20);
	//fprintf(p, "%s%f%s", "\tdd ", diff_b, "\n");

	spec_b = (float)(rand() % 50);
	//fprintf(p, "%s%f%s", "\tdd ", spec_b, "\n");

	//fputc('\n',p);
	//fclose(p);
	//n_file++;


	angle=M_PI/2;
	for(i=0;i<=MAX_PHONG_COLORS*3;i+=3)
	{
		illumination=diff_r*cos(angle)+
			spec_r*pow(cos(angle),PHONG_SPEC_EXPONENT);
		r=(unsigned char)illumination;

		illumination=diff_g*cos(angle)+
			spec_g*pow(cos(angle),PHONG_SPEC_EXPONENT);
		g=(unsigned char)illumination;

		illumination=diff_b*cos(angle)+
			spec_b*pow(cos(angle),PHONG_SPEC_EXPONENT);
		b=(unsigned char)illumination;

		if(r>31) r=31;
		if(g>63) g=63;
		if(b>31) b=31;

		palette[i+0]=r<<1;
		palette[i+1]=g;
		palette[i+2]=b<<1;

		angle-=PHONG_DELTA_ANGLE;
	}
}

int load_palette(char *filename)
{
	FILE *p;

	p=fopen(filename,"r+b");
	if(!p) return 0;
	fread(palette,1,768,p);
	fclose(p);
	return 1;
}

void save_palette(char *filename)
{
	FILE *p;

	p=fopen(filename,"w+b");
	fwrite(palette,1,768,p);
	fclose(p);
}

void make_font_palette(void)
{
	int i;
	unsigned char b;

	for(i=(MAX_PHONG_COLORS+1)*3;i<(MAX_PHONG_COLORS+9)*3;i+=3)
	{
		palette[i+0]=(b=(i-MAX_PHONG_COLORS)+30);
		palette[i+1]=b;
		palette[i+2]=b;
	}
}