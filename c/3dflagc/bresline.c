void bresline(int x1,int y1,int x2,int y2,unsigned char color,
				unsigned char *buffer)
{
	int dx,dy,max,x,y,d,dinc1,dinc2,xinc1,xinc2,yinc1,yinc2,i;

	dx=abs(x2-x1);
	dy=abs(y2-y1);

	if(dx>=dy)
	{
		max=dx+1;
		d=(dy<<1)-dx;
		dinc1=dy<<1;
		dinc2=(dy-dx)<<1;
		xinc1=1;
		xinc2=1;
		yinc1=0;
		yinc2=1;
	}
	else
	{
		max=dy+1;
		d=(dx<<1)-dy;
		dinc1=dx<<1;
		dinc2=(dx-dy)<<1;
		xinc1=0;
		xinc2=1;
		yinc1=1;
		yinc2=1;
	}

	if(x1>x2)
	{
		xinc1=-xinc1;
		xinc2=-xinc2;
	}

	if(y1>y2)
	{
		yinc1=-yinc1;
		yinc2=-yinc2;
	}

	x=x1;
	y=y1;

	for(i=0;i<max;i++)
	{
		if(x>=0 && x<=319 && y>=0 && y<=199)
			buffer[x+(y<<6)+(y<<8)]=color;

		if(d<0)
		{
			d+=dinc1;
			x+=xinc1;
			y+=yinc1;
		}
		else
		{
			d+=dinc2;
			x+=xinc2;
			y+=yinc2;
		}
	}
}