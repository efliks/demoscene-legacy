/*
	FlatGrd2 06/02/02
	Mikolaj Felix a.k.a. Majuma
	mfelix@polbox.com
*/

unsigned char *bumpmap;

int enable_bumpmap(void)
{
	int i,j;
	unsigned char *ptr;

	bumpmap = (unsigned char *)malloc(128*128);
	if(!bumpmap) return 0;

	srand((int)rawclock());

	ptr = bumpmap;
	for(i=0;i<128*128;i++, ptr++) *ptr = rand() % 255;

	for(i=0;i<6;i++)
	{
		ptr = bumpmap;
		for(j=0;j<16384;j++)
		{
			*ptr = (bumpmap[(j-1) & 16383] +
					bumpmap[(j+1) & 16383] +
					bumpmap[(j-128) & 16383] +
					bumpmap[(j+128) & 16383]) >> 2;

			ptr++;
		}
	}

	return 1;
}

void disable_bumpmap(void)
{
	free(bumpmap);
}