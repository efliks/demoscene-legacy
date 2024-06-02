; sys
global do_startup:PROC
global do_shutdown:PROC

global copy_buffer:PROC
global clear_buffer:PROC

global wait_for_vsync:PROC

global set_palette:PROC
global get_palette:PROC

global alloc_seg:PROC
global free_seg:PROC

global init_font:PROC
global put_string:PROC

global buffer_seg:WORD
global font_data:BYTE

global timer_wait:PROC
global timer_setup:PROC
global timer_shutdown:PROC
