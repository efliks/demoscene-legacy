/*
	"Mroczna Harmonia"
	Mikolaj Felix
	mfelix@polbox.com
*/

void interrupt (*old_keyboard)();

void interrupt my_keyboard(void)
{
	int key=inportb(0x60);
	outportb(0x20,0x20);
	if(key&0x80) 
		keytab[key-0x80]=0;
	else 
		keytab[key]=1;
}

void init_keyboard(void)
{
	int x;
	old_keyboard=getvect(9);
	setvect(9,my_keyboard);
	for(x=0;x<256;x++) keytab[x]=0;
}

void restore_keyboard(void)
{
   setvect(9,old_keyboard);
}