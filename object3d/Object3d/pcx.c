/*
	PCX
	Mikolaj Felix 13/07/01
	mfelix@polbox.com
*/

typedef struct
{
	unsigned width,height;
	unsigned char *data;
}PCX;

char error_message[128];

int load_pcx(PCX *pcx,char *filename,int load_palette)
{
	FILE *p;
	unsigned i;
	int j;
	unsigned char b;

	if((p=fopen(filename,"r+b"))==NULL)
	{
		strcpy(error_message,"Unable to open file!");
		return NULL;
	}

	if(fgetc(p)!=0x0a)
	{
		fclose(p);
		strcpy(error_message,"Incorrect file format!");
		return NULL;
	}

	fseek(p,2,SEEK_CUR);
	if(fgetc(p)!=8)
	{
		fclose(p);
		strcpy(error_message,"Incorrect color depth. Must by 8-bit!");
		return NULL;
	}

	fread(&i,2,1,p);
	pcx->width=i;
	fread(&i,2,1,p);
	pcx->height=i;
	fread(&i,2,1,p);
	pcx->width=i-pcx->width+1;
	fread(&i,2,1,p);
	pcx->height=i-pcx->height+1;

	if(pcx->width>320 || pcx->height>200)
	{
		fclose(p);
		strcpy(error_message,"Image is too big!");
		return NULL;
	}

	if((pcx->data=(unsigned char *)malloc(pcx->width*pcx->height))==NULL)
	{
		fclose(p);
		strcpy(error_message,"Not enough memory!");
		return NULL;
	}

	fseek(p,128,SEEK_SET);
	i=0;
	while(i!=(pcx->width*pcx->height))
	{
		b=fgetc(p);
		if(b>=192)
		{
			j=b-192;
			b=fgetc(p);
			while(j-->0) pcx->data[i++]=b;
		}
		else
			pcx->data[i++]=b;
	}

	if(load_palette)
	{
		fseek(p,-768L,SEEK_END);
		for(i=0;i<768;i++) palette[i]=fgetc(p)>>2;
	}

	fclose(p);
	return 1;
}

void unload_pcx(PCX *pcx)
{
	free(pcx->data);
}

/*
void draw_pcx(PCX *pcx,int x,int y,unsigned char *buffer)
{
	asm {
		push ds
		push es

		les di,buffer
		mov ax,y
		mov bx,ax
		shl ax,6
		shl bx,8
		add ax,bx
		add ax,x
		add di,ax

		lds si,pcx
		lodsw
		mov bx,ax
		lodsw
		mov dx,ax
		lds si,dword ptr [si]

		mov ax,320
		sub ax,bx
		cld

		shr bx,1
		jnc ver_even }
ver:
	asm {
		mov cx,bx
		movsb
		rep movsw
		add di,ax
		dec dx
		jnz ver
		jmp quit }

ver_even:
	asm {
		mov cx,bx
		rep movsw
		add di,ax
		dec dx
		jnz ver_even}
quit:
	asm {
		pop es
		pop ds }
}*/