#define PHONG_SPEC_EXPONENT 12

void random_phong_palette(short *palette,int n_colors)
{
	int i;
	float delta_angle,angle,illumination;
	float diff_r,spec_r;
	float diff_g,spec_g;
	float diff_b,spec_b;
	unsigned char r,g,b;
	
	static n_file;
	FILE *p;
	char txt_buffer[32];
	float *p_ptr;
	long temp;

	delta_angle=(M_PI/2)/(float)n_colors;
	angle=M_PI/2;

	diff_r=(float)(rand()%20);
	spec_r=(float)(rand()%50);
	diff_g=(float)(rand()%20);
	spec_g=(float)(rand()%50);
	diff_b=(float)(rand()%20);
	spec_b=(float)(rand()%50);

	/* write phong params */
	p=fopen("palettes.inc","a");
	itoa(n_file,txt_buffer,10);
	fprintf(p,"%s%s%s","pal",txt_buffer,"_params:\n");

	p_ptr=&diff_r;
	for(i=0;i<3*2;i++)
	{
		temp=*p_ptr;
		itoa(temp,txt_buffer,10);
		strcat(txt_buffer,".0");
		fprintf(p,"%s%s%s","\tdd ",txt_buffer,"\n");

		p_ptr++;
	}
	fputc('\n',p);

	fclose(p);
	n_file++;

	for(i=0;i<n_colors;i++)
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

		palette[i]=COLOR16(r,g,b);
		angle-=delta_angle;
	}
}