/*
	"Mroczna Harmonia"
	Mikolaj Felix
	mfelix@polbox.com
*/

#define SB_BASE_ADDRESS 0x220
#define ON_SPEAKER 0x0d1
#define OFF_SPEAKER 0x0d3
#define DSP_OUT 0x10

#define DMA_CHANNEL 0x01
#define DMA_PAGE_PORT 0x83
#define DMA_OFFSET_PORT 0x02
#define DMA_LENGHT_PORT 0x03
#define DMA_MODE 0x48
#define DMA_INT 0x0d

#define WAV_HEADER_SIZE 44
#define SOUND_FREQUENCY 11025

void interrupt (*old_dma_int)(void);
unsigned char end_of_sound=1;

int num_of_loaded_sounds=0;

int sb_init(void)
{
	asm {
		mov al,1
		mov dx,SB_BASE_ADDRESS
		add dx,6
		out dx,al
		in al,dx
		in al,dx
		in al,dx
		in al,dx
		xor al,al
		out dx,al
		add dx,4
		mov cx,100 }
sb_test:
	asm {
		in al,dx
		cmp al,0aah
		je sb_present
		loop sb_test }

	return 0;
sb_present:
	return 1;	
}

void sb_dsp_out(unsigned char value)
{
	asm {
		mov dx,SB_BASE_ADDRESS
		add dx,0ch }
dsp_out_test:
	asm {
		in al,dx
		test al,80h
		jne dsp_out_test
		mov al,value
		out dx,al }
}

void interrupt dma_int(void)
{
	end_of_sound=1;

	asm {
		mov dx,SB_BASE_ADDRESS
		add dx,0eh
		in al,dx
		sub dx,2 }
dma_sb_reset:
	asm {
		in al,dx
		or al,al
		js dma_sb_reset

		mov al,20h
		out 20h,al }
}

void enable_dma_int(void)
{
	old_dma_int=getvect(DMA_INT);
	setvect(DMA_INT,dma_int);

	asm {
		in al,21h
		and al,0dfh
		out 21h,al }
}

void disable_dma_int(void)
{
	setvect(DMA_INT,old_dma_int);
	
	asm {
		in al,21h
		or al,20h
		out 21h,al }
}

void convert_address(unsigned segment,unsigned off1,
			unsigned char *page,unsigned *off2)
{
	asm {
		mov dx,segment
		mov ax,off1
		mov bl,dh
		shr bl,4
		shl dx,4
		add ax,dx
		adc bl,0
		mov dh,bl }

	*page=_DH;
	*off2=_AX;
}

void set_dma(unsigned char page,unsigned off,unsigned lenght)
{
	asm {
		mov al,4
		add al,DMA_CHANNEL
		out 0ah,al

		xor al,al
		out 0ch,al

		mov al,DMA_MODE
		add al,DMA_CHANNEL
		out 0bh,al

		mov dx,DMA_OFFSET_PORT
		mov ax,off
		out dx,al
		mov al,ah
		out dx,al
		inc dx
		mov ax,lenght
		out dx,al
		mov al,ah
		out dx,al

		mov dx,DMA_PAGE_PORT
		mov al,page
		out dx,al

		mov al,DMA_CHANNEL
		out 0ah,al }		
}

void set_freq(unsigned freq)
{
	sb_dsp_out(0x40);
	sb_dsp_out(256-1000000/freq);
}

void play_sound(SOUND *s)
{
	unsigned char page;
	unsigned off;

	if(!end_of_sound) return;

	convert_address(FP_SEG(s->buffer),FP_OFF(s->buffer),&page,&off);
	set_dma(page,off,s->lenght);

	sb_dsp_out(0x14);
	sb_dsp_out(s->lenght&255);
	sb_dsp_out(s->lenght>>8);

	end_of_sound=0;
}

unsigned get_file_size(char *filename)
{
	FILE *p;
	unsigned size=0;

	if((p=fopen(filename,"r+b"))==NULL) return NULL;

	while(!feof(p))
	{
		fgetc(p);
		size++;
	}

	fclose(p);
	return size;
}

int load_sound(SOUND *s,char *filename)
{
	FILE *p;
	unsigned i;
		
	if((s->lenght=get_file_size(filename))==NULL)
	{
		strcpy(missed_file,filename);
		return NULL;
	}
	
	//s->lenght-=WAV_HEADER_SIZE;

	if((s->buffer=(unsigned char *)malloc(s->lenght))==NULL) return NULL;

	p=fopen(filename,"r+b");

	//fseek(p,WAV_HEADER_SIZE,SEEK_SET);
	fgets(s->buffer,s->lenght,p);

	fclose(p);

	num_of_loaded_sounds++;
	return 1;	
}

void unload_sound(SOUND *s)
{
	free(s->buffer);
}

void unload_sounds(void)
{
	int i;

	for(i=0;i<num_of_loaded_sounds;i++) unload_sound(&soundtab[i]);
}