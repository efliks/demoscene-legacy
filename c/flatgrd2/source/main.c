/*
 * FlatGrd2 06/02/02
 * Mikolaj Felix a.k.a. Majuma
 * mfelix@polbox.com
 */

#include <mem.h>
#include <time.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "3dobj.h"
#include "graph.h"
#include "font.h"
#include "envmap.h"
#include "bumpmap.h"
#include "phongpal.h"
#include "dump_scr.h"

#define DELAY 0
#define NO_DELAY 1

#define TEXT_COLOR MAX_PHONG_COLORS + 1


void syntax_err(void)
{
    printf("%s", "Syntax: flatgrd input_file [/m] [/r] [/s]\n\n");

    printf("%s", "\t/m display messages\n");
    printf("%s", "\t/r no delay, real computer's speed\n");
    printf("%s", "\t/b enables frame softing\n");
    printf("%s", "\t/s save each frame as PCX file\n\n");
}

int main(int argc, char** argv)
{
    OBJECT3D* obj;
    VECTOR3D light_source;

    char drawing_mode;
    char display_mode;
    char messages_on;
    char pal_loaded;
    char txt_buffer[64];
    char txt_buffer2[32];
    char framerate_txt[40];
    char object_info[32];
    char key;
    
    time_t before;
    time_t after;
    time_t indep_before;
    time_t clock_hrs;
    time_t clock_min;
    time_t clock_sec;
    
    long all_frames;
    long frames;
    long saved_frames;
    
    int dump_mode;
    int blur_mode;
    int i;
    
    FILE* log_file;

    char frame_file[16];
    long bytes_written;

    /* ---------- check program arguments ---------- */

    clrscr();
    printf("%s", "Flat Shading/Gouraud Shading/Environment Mapping\n\n");
    printf("%s", "Code: Mikolaj Felix a.k.a. Majuma (mfelix@polbox.com)\n");
    printf("%s%s%s%s%s", "Build date/time: ", __DATE__, " ", __TIME__, "\n\n");

    if (argc < 2) {
        syntax_err();
        return 1;
    }

    display_mode = DELAY;
    messages_on = 0;
    dump_mode = 0;
    blur_mode = 0;

    for (i = 2; i < argc; i++) {
        if (strncmp(argv[i], "/r", 2) == 0)
            display_mode = NO_DELAY;
        else if (strncmp(argv[i], "/m", 2) == 0)
            messages_on = 1;
        else if (strncmp(argv[i], "/s", 2) == 0) {
            dump_mode = 1;
            display_mode = DELAY;
            system("mkdir dump");
        } else if (strncmp(argv[i], "/b", 2) == 0)
            blur_mode = 1;
        else if (strncpy(argv[i], "", 1) == 0)
            ;
        else {
            syntax_err();
            return 2;
        }
    }

    /* ---------- checked ---------- */

    if ((frame_buffer = (unsigned char*)malloc(BUFFER_SIZE)) == NULL) {
        return 3;
    }

    if (!enable_envmap()) {
        free(frame_buffer);
        return 3;
    }

    //if(!enable_bumpmap()) { disable_envmap(); free(frame_buffer); return 3; }

    clear_buffer();
    init_font();
    init_sincos();

    if ((obj = load_object3d(argv[1])) == NULL) {
        printf("%s", "Unable to load .3d file.\n\n");
        free(frame_buffer);
        return 4;
    }

    /* set light */
    light_source.x = 0;
    light_source.y = 0;
    light_source.z = -100;
    normalize_vector(&light_source);

    if (!load_palette("pal_13h.bin")) {
        random_phong_palette();
        pal_loaded = 0;
    } else {
        pal_loaded = 1;
    }

    make_font_palette();

    set_vga();
    set_palette();

    drawing_mode = ENVIRONMENT_MAPPING;
    bytes_written = 0;

    strcpy(object_info, "Obj: ");
    itoa(obj->n_points, txt_buffer, 10);
    strcat(object_info, txt_buffer);
    strcat(object_info, " vertices, ");
    itoa(obj->n_faces, txt_buffer, 10);
    strcat(object_info, txt_buffer);
    strcat(object_info, " faces");

    before = indep_before = time(NULL);
    all_frames = frames = saved_frames = 0;

    clock_hrs = 0;
    clock_min = 0;
    clock_sec = 0;

    strcpy(framerate_txt, "Framerate (in last sec): ");

    /* ---------- main loop ---------- */

    while (1) {
        /* ---------- change drawing mode ---------- */

        if (kbhit()) {
            if ((key = getch()) == 27)
                break;
            else if (key == 'p' || key == 'P') {
                random_phong_palette();
                pal_loaded = 0;
                set_palette();
            }

            else {
                drawing_mode++;
                /* bump mapping is disabled ! */
                if (drawing_mode == 3)
                    drawing_mode = 0;
            }
        }

        rotate_translate_object3d(obj, 1, 3, -1);
        draw_object3d(obj, &light_source, drawing_mode);

        if (blur_mode) {
            soft_screen();
        }

        /* ---------- print text messages ---------- */

        all_frames++;

        if (messages_on) {
            frames++;
            after = time(NULL);

            //put_string("Code: Mikolaj Felix (mfelix@polbox.com)",0,1,TEXT_COLOR,frame_buffer);
            put_string(object_info, 0, 1, TEXT_COLOR, frame_buffer);

            put_string("Shade model: ", 0, 13, TEXT_COLOR, frame_buffer);
            switch (drawing_mode) {
            case FLAT_SHADING:
                put_string("flat", 104, 13, TEXT_COLOR, frame_buffer);
                break;
            case GOURAUD_SHADING:
                put_string("gouraud", 104, 13, TEXT_COLOR, frame_buffer);
                break;
            case ENVIRONMENT_MAPPING:
                put_string("environment mapping", 104, 13, TEXT_COLOR, frame_buffer);
                break;
            case ENV_BUMP:
                put_string("bump mapping", 104, 13,
                    TEXT_COLOR, frame_buffer);
            }

            if (after != before) {
                before = time(NULL);
                itoa(frames, txt_buffer, 10);
                strcpy(framerate_txt, "Framerate (in last sec): ");
                strcat(framerate_txt, txt_buffer);
                frames = 0;

                clock_sec++;
                if (clock_sec > 59) {
                    clock_sec = 0;
                    clock_min++;
                    if (clock_min > 59) {
                        clock_min = 0;
                        clock_hrs++;
                    }
                }
            }
            put_string(framerate_txt, 0, 25, TEXT_COLOR, frame_buffer);

            strcpy(txt_buffer, "Average framerate: ");
            if ((after - indep_before)) {
                gcvt(((float)all_frames / (float)(after - indep_before)), 5, txt_buffer2);
                strcat(txt_buffer, txt_buffer2);
            }

            put_string(txt_buffer, 0, 37, TEXT_COLOR, frame_buffer);

            strcpy(txt_buffer, "Visible faces: ");
            itoa(obj->visible_faces, txt_buffer2, 10);
            strcat(txt_buffer, txt_buffer2);

            put_string(txt_buffer, 0, 167, TEXT_COLOR, frame_buffer);

            strcpy(txt_buffer, "Total rendered frames: ");
            itoa(all_frames, txt_buffer2, 10);
            strcat(txt_buffer, txt_buffer2);

            put_string(txt_buffer, 0, 179, TEXT_COLOR, frame_buffer);

            /* ---------- print elapsed time ---------- */

            strcpy(txt_buffer, "Elapsed time: ");

            itoa(clock_hrs, txt_buffer2, 10);
            strcat(txt_buffer, txt_buffer2);
            strcat(txt_buffer, ":");
            itoa(clock_min, txt_buffer2, 10);
            strcat(txt_buffer, txt_buffer2);
            strcat(txt_buffer, ":");
            itoa(clock_sec, txt_buffer2, 10);
            strcat(txt_buffer, txt_buffer2);

            put_string(txt_buffer, 0, 191, TEXT_COLOR, frame_buffer);
        }

        /* ---------- end of messages ---------- */

        if (dump_mode) {
            if (save_frame()) {
                strcpy(txt_buffer, "Saving file: ");
                strcat(txt_buffer, frame_file);
                put_string(txt_buffer, 160 - (strlen(txt_buffer) << 2), 90, TEXT_COLOR, frame_buffer);

                saved_frames++;
            } else {
                put_string("Unable to save frame!", 76, 90, TEXT_COLOR, frame_buffer);
            }

            strcpy(txt_buffer, "Saved: ");
            strcat(txt_buffer, itoa(bytes_written >> 20, txt_buffer2, 10));
            strcat(txt_buffer, " MB");

            put_string(txt_buffer, 160 - (strlen(txt_buffer) << 2), 102, TEXT_COLOR, frame_buffer);
        }

        if (display_mode == DELAY) {
            retrace();
        }

        copy_buffer();
        clear_buffer();
    }

    set_text();

    /* ---------- save log file ---------- */

    /*
	log_file=fopen("log.txt","a");
	if(log_file)
	{
		time(&after);
		fprintf(log_file,"%s%s","Log update: ",
			asctime(localtime(&after)));
		
		fprintf(log_file,"%s%s%s","Object: ",argv[1],"\n");

		itoa(obj->n_points,txt_buffer,10);
		fprintf(log_file,"%s%s%s","Vertices: ",txt_buffer,"\n");
		itoa(obj->n_faces,txt_buffer,10);
		fprintf(log_file,"%s%s%s","Faces: ",txt_buffer,"\n");

		fprintf(log_file,"%s","Program arguments: ");
		for(i=2;i<argc;i++)
			fprintf(log_file,"%s%s",argv[i]," ");
		fputc('\n',log_file);

		itoa(all_frames,txt_buffer,10);
		fprintf(log_file,"%s%s%s","Rendered frames: ",txt_buffer,"\n");

		after-=indep_before;
		itoa(after/3600,txt_buffer,10);
		fprintf(log_file,"%s%s%s","Elapsed time: ",txt_buffer,":");
		itoa((after%3600)/60,txt_buffer,10);
		fprintf(log_file,"%s%s",txt_buffer,":");
		itoa(after%60,txt_buffer,10);
		fprintf(log_file,"%s%s",txt_buffer,"\n");

		gcvt((float)all_frames/(float)after,5,txt_buffer);
		fprintf(log_file,"%s%s%s","Average framerate: ",txt_buffer,"\n");

		fprintf(log_file,"%s","Palette: ");
		if(pal_loaded)
			fprintf(log_file,"%s","loaded\n");
		else
			fprintf(log_file,"%s","generated\n");

		if(dump_mode)
		{
			itoa(bytes_written,txt_buffer,10);
			fprintf(log_file,"%s%s%s","Dump mode: ",
				txt_buffer," bytes saved (");
			itoa(saved_frames,txt_buffer,10);
			fprintf(log_file,"%s%s",txt_buffer," frames)\n\n");
		}
		else
			fprintf(log_file,"%s","Dump mode: off\n\n");

		fclose(log_file);
	}
	*/

    //save_tex_coords(obj,"coords.txt");

    unload_object3d(obj);
    free(frame_buffer);

    disable_envmap();
    //disable_bumpmap();

    save_palette("pal_13h.bin");

    return 0;
}
