/*
	FlatGrd2 06/02/02
	Mikolaj Felix a.k.a. Majuma
	mfelix@polbox.com
*/

unsigned char *envmap;

int enable_envmap(void)
{
	float x,y,temp_color;
	unsigned char *ptr,color;

	envmap=(unsigned char *)malloc(128*128);
	if(!envmap) return 0;

	ptr=envmap;

	for(y=-64;y<64;y++)
		for(x=-64;x<64;x++)
		{
			temp_color=(62-sqrt(pow(x,2)+pow(y,2)))*1.2;

			if(temp_color<0)
				temp_color=0;
			else if(temp_color>62)
				temp_color=62;

			color=(unsigned char)temp_color;
			*ptr=color;
			ptr++;
		}

	return 1;
}

void disable_envmap(void)
{
	free(envmap);
}