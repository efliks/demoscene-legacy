#define set_vga() { asm { mov ax,13h; int 10h } }
#define set_text() { asm { mov ax,03h; int 10h } }

unsigned char *frame_buffer,palette[768];

void clear_buffer(void)
{
	asm {
		push es
		les di,frame_buffer
		db 66h
		xor ax,ax
		mov cx,16000
		cld
		db 66h
		rep stosw
		pop es }
}

void copy_buffer(void)
{
	asm {
		push ds
		push es
		lds si,frame_buffer
		mov ax,0a000h
		mov es,ax
		xor di,di
		mov cx,16000
		cld
		db 66h
		rep movsw
		pop es
		pop ds }
}

void retrace(void)
{
	while(!(inp(0x03da)&8));
	while((inp(0x03da)&8));
}

void set_palette(void)
{
	int i;

	outp(0x03c8,0);
	for(i=0;i<768;i++) outp(0x03c9,palette[i]);
}

void get_palette(void)
{
	int i;

	outp(0x03c7,0);
	for(i=0;i<768;i++) palette[i]=inp(0x03c9);
}