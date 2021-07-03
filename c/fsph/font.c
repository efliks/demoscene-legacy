unsigned font_seg,font_off;

void init_font(void)
{
	asm {
		push bp
		push es
		mov ax,01130h
		mov bh,03h
		int 10h
		mov font_seg,es
		mov font_off,bp
		pop es
		pop bp }
}

void put_char(char ascii_code,int x,int y,
		unsigned char color,unsigned char *where)
{
	asm {
		push ds
		push es
		mov si,font_off
		mov ds,font_seg

		les di,where
		mov ax,y
		mov bx,ax
		shl ax,6
		shl bx,8
		add ax,bx
		add ax,x
		add di,ax

		xor ah,ah
		mov al,ascii_code
		shl ax,3
		add si,ax

		mov dl,color
		mov ch,8 }
ver:
	asm {
		lodsb
		mov cl,8 }
hor:
	asm {
		rcl al,1
		jae next
		mov byte ptr es:[di],dl }
next:
	asm {
		inc di
		dec cl
		jnz hor

		add di,312
		inc dl
		dec ch
		jnz ver
		pop es
		pop ds }
}

void put_string(char *text,int x,int y,
		unsigned char color,unsigned char *where)
{
	int i=0;

	while(text[i])
	{
		put_char(text[i],x,y,color,where);
		i++;
		x+=8;
	}
}