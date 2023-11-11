#include <stdlib.h>
#include <time.h>

#include "gfx.h"
#include "clock.h"
#include "fontnew.h"

#define NUM_COLUMNS 40

struct
{
    char string[25];
    int pozycja;
    int v;
} kolumna[NUM_COLUMNS];


void inicjuj_palete(void)
{
    int i;
    unsigned char my_pal[768], *ptr;

    dump_palette(my_pal);
    ptr = my_pal;

    for (i = 0; i < 55; i++) {
        *(ptr++) = 0;
        *(ptr++) = i;
        *(ptr++) = i >> 1;
    }
    for (i = 0; i < 9; i++) {
        *(ptr++) = 55 + i;
        *(ptr++) = 55 + i;
        *(ptr++) = 55 + i;
    }

    set_palette(my_pal);
}

void wykonaj_kolumny(unsigned char* buffer)
{
    int i, j;
    unsigned char kolor;
    static licznik;

    for (i = 0; i < NUM_COLUMNS; i++) {
        for (j = kolumna[i].pozycja - 1, kolor = 64 - 8; j >= 0; j--) {
            if (j < 25)
                put_char(kolumna[i].string[j], i << 3, j << 3, kolor, buffer);
            if (kolor > 4)
                kolor -= 4;
        }
    }

    for (i = 0; i < NUM_COLUMNS; i++) {
        switch (kolumna[i].v) {
        case 1:
            kolumna[i].pozycja++;
            break;
        case 2:
            if (licznik == 1 || licznik == 2)
                kolumna[i].pozycja++;
            break;
        case 3:
            if (licznik == 2)
                kolumna[i].pozycja++;
        }
        if (kolumna[i].pozycja == 35) {
            for (j = 0; j < 25; j++)
                kolumna[i].string[j] = rand() % 64 + 32;
            kolumna[i].pozycja = 0;
            kolumna[i].v = rand() % 3 + 1;
        }
    }

    licznik++;
    if (licznik == 3)
       licznik = 0;
}

void inicjuj_kolumny(void)
{
    int i, j;

    for (i = 0; i < NUM_COLUMNS; i++) {
        for (j = 0; j < 25; j++)
            kolumna[i].string[j] = rand() % 255;

        kolumna[i].pozycja = rand() % 25;
        kolumna[i].v = rand() % 3 + 1;
    }
}

int main(void)
{
    unsigned char* frame_buffer;
    
    if ((frame_buffer = (unsigned char *)malloc(64000)) == NULL) {
        return 1;
    }

    srand(time(NULL));

    init_font();
    set_vga();

    clock_init();

    inicjuj_palete();
    inicjuj_kolumny();

    do {
        timer_start(10);
        wykonaj_kolumny(frame_buffer);

        copy_buffer(frame_buffer);
        clear_buffer(frame_buffer);

        timer_wait();
    } while (!is_key_pressed());

    set_text();
    clock_shutdown();

    free(frame_buffer);

    return 0;
}
