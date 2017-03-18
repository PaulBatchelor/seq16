#include <GLFW/glfw3.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <soundpipe.h>
#include <sporth.h>
#include <pthread.h>
#include <unistd.h>
#include "seq16.h"
#include "glyphs.h"

enum {
SELECT_ON = 0,
SELECT_OFF = 32,
NOTE_ON = 64,
NOTE_OFF = 96,
TRIANGLE = 128,
EMPTY = 160
};

static void color_rgb(uint32_t rgb)
{
    int r, g, b;
    r = (rgb & 0xff0000) >> 16;
    g = (rgb & 0x00ff00) >> 8;
    b = (rgb & 0x0000ff);
    glColor3f(r / 255.0, g / 255.0, b / 255.0);
}

static void clear_color_rgb(uint32_t rgb)
{
    int r, g, b;
    r = (rgb & 0xff0000) >> 16;
    g = (rgb & 0x00ff00) >> 8;
    b = (rgb & 0x0000ff);
    glClearColor(r / 255.0, g / 255.0, b / 255.0, 1.0);
}


static void display(seq16_d *seq)
{
    int x;
    int y = 0;
    glClear(GL_COLOR_BUFFER_BIT);

    color_rgb(0xC6E1C7);
    for(y = 0; y < MAX_ROWS; y++) {
        glRasterPos2i(4, 20 + 16 * y);
        for(x = 0; x < 16; x++) {
            glBitmap(16, 16, 0.0, 0.0, 16.0, 0.0, rasters 
                    + seq->grid[16 * y + x]);
        }
    }
   
    glFlush();
}

void init(void)
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    clear_color_rgb(0x1c2933);
}

void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, h, 0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

void idle()
{
}

void errorcb(int error, const char* desc)
{
	printf("GLFW error %d: %s\n", error, desc);
}

static void key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    seq16_d *seq = glfwGetWindowUserPointer(window);
    if(action == GLFW_PRESS) {
        switch(key) {
            case GLFW_KEY_L: 
                seq16_move_right(seq);
                seq16_tick(seq);
                break;
            case GLFW_KEY_H: 
                seq16_move_left(seq);
                seq16_tick(seq);
                break;
            case GLFW_KEY_J: 
                seq16_move_down(seq);
                seq16_tick(seq);
                break;
            case GLFW_KEY_K: 
                seq16_move_up(seq);
                seq16_tick(seq);
                break;
            case GLFW_KEY_SPACE: 
                seq16_toggle(seq);
                seq16_tick(seq);
                break;
            case GLFW_KEY_1: 
                seq16_set_skip(seq, 1);
                seq16_tick(seq);
                break;
            case GLFW_KEY_2: 
                seq16_set_skip(seq, 2);
                seq16_tick(seq);
                break;
            case GLFW_KEY_3: 
                seq16_set_skip(seq, 3);
                seq16_tick(seq);
                break;
            case GLFW_KEY_4: 
                seq16_set_skip(seq, 4);
                seq16_tick(seq);
                break;
            case GLFW_KEY_5: 
                seq16_set_skip(seq, 5);
                seq16_tick(seq);
                break;
            case GLFW_KEY_6: 
                seq16_set_skip(seq, 6);
                seq16_tick(seq);
                break;
            case GLFW_KEY_7: 
                seq16_set_skip(seq, 7);
                seq16_tick(seq);
                break;
            case GLFW_KEY_8: 
                seq16_set_skip(seq, 8);
                seq16_tick(seq);
                break;
            case GLFW_KEY_9: 
                seq16_set_skip(seq, 9);
                seq16_tick(seq);
                break;

            case GLFW_KEY_S:
                seq16_save(seq);
                break;
        }
    }
    
}

static void * run_loop(void *ud)
{
    seq16_d *seq = (seq16_d *)ud;

    int w, h;

	if (!glfwInit()) {
		printf("Failed to init GLFW.");
		pthread_exit(NULL);
	}

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    seq->window = glfwCreateWindow(264, 88, "seq16", NULL, NULL);

	if (!seq->window) {
		glfwTerminate();
		pthread_exit(NULL);
	}

	glfwSetKeyCallback(seq->window, key);
	glfwSetErrorCallback(errorcb);
	glfwMakeContextCurrent(seq->window);

	glfwSwapInterval(0);
	glfwSetTime(0);
    init();
    glfwSetWindowUserPointer(seq->window, seq);
    while(seq->run) {
		glfwGetFramebufferSize(seq->window, &w, &h);

        glfwPollEvents();
        glViewport(0, 0, (GLsizei) w, (GLsizei) h);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, w, h, 0, -1.0, 1.0);
        glMatrixMode(GL_MODELVIEW);

        if(seq->please_tick) {
            display(seq);
            seq->please_tick = 0;
        }
        glfwSwapBuffers(seq->window);
        usleep(8000);
    }


	glfwTerminate();
    pthread_exit(0);
    return NULL;
}

void seq16_start(seq16_d *seq, const char *filename)
{
    int i;
    seq->run = 1;
    seq->please_tick = 1;
    seq->playhead= -1;
    seq->pos = 16;
    seq->skip = 1;
    seq->filename = filename;

    for(i = 0; i < MAX_ROWS * 16; i++) {
        if(i < 16) {
            seq->grid[i] = EMPTY;
        } else {
            seq->grid[i] = NOTE_OFF;
        }
    }


    seq16_load(seq);
    seq16_set_pos(seq, seq->pos);
    pthread_create(&seq->thread, NULL, run_loop, seq);
}

void seq16_stop(seq16_d *seq)
{
    seq->run = 0;
    pthread_join(seq->thread, NULL);
}

void seq16_tick(seq16_d *seq)
{
    seq->please_tick = 1;
}

void seq16_set_playhead(seq16_d *seq, int pos)
{
    if(seq->playhead >= 0) {
        seq->grid[seq->playhead] = EMPTY;
    }
    seq->grid[pos] = TRIANGLE;
    seq->playhead= pos;
}

void seq16_set_skip(seq16_d *seq, int skip)
{
    seq->skip = skip;
}

void seq16_set_pos(seq16_d *seq, int pos)
{
    int old;
    int new;

    switch(seq->grid[seq->pos]) {
        case SELECT_ON:
            old = NOTE_ON;
            break;
        case SELECT_OFF:
            old = NOTE_OFF;
            break;
    }
    
    switch(seq->grid[pos]) {
        case SELECT_ON:
        case NOTE_ON:
            new = SELECT_ON;
            break;
        case SELECT_OFF:
        case NOTE_OFF:
            new = SELECT_OFF;
            break;
    }

    seq->grid[seq->pos] = old;
    seq->grid[pos] = new;
    seq->pos = pos;
}

void seq16_move_right(seq16_d *seq)
{
    int new_pos;
    new_pos = (seq->pos + seq->skip);

    if(new_pos > MAX_ROWS * 16 - 1) new_pos = 16;
    seq16_set_pos(seq, new_pos);
}

void seq16_move_left(seq16_d *seq)
{
    int new_pos;
    new_pos = (seq->pos - seq->skip);

    if(new_pos < 16) {
        new_pos = (MAX_ROWS * 16) - 1;
    }
    seq16_set_pos(seq, new_pos);
}

void seq16_move_up(seq16_d *seq)
{
    int new_pos;
    new_pos = seq->pos - 16;

    if(new_pos < 16) {
        new_pos = seq->pos + (MAX_ROWS - 2) * 16;
    }
    seq16_set_pos(seq, new_pos);
}

void seq16_move_down(seq16_d *seq)
{
    int new_pos;
    new_pos = seq->pos + 16;

    if(new_pos > MAX_ROWS * 16 - 1) {
        new_pos = (seq->pos % 16) + 16;
    }
    seq16_set_pos(seq, new_pos);
}

void seq16_move_playhead(seq16_d *seq)
{
    seq16_set_playhead(seq, (seq->playhead + 1) % 16);
    seq16_tick(seq);
}

void seq16_toggle(seq16_d *seq)
{
    int *grid;

    grid = &seq->grid[seq->pos];
    switch(*grid) {
        case SELECT_ON:
            *grid = SELECT_OFF;
            break;
        case SELECT_OFF:
            *grid = SELECT_ON;
            break;
    }
}

void seq16_update_ftbl(seq16_d *seq)
{
    int i;
    for(i = 0; i < 4; i++) {
        switch(seq->grid[16 + i * 16 + seq->playhead]) {
            case SELECT_ON:
            case NOTE_ON:
                seq->ft->tbl[i] = 1.0;
                break;
            case SELECT_OFF:
            case NOTE_OFF:
                seq->ft->tbl[i] = 0.0;
                break;
        }
    }
}

void seq16_save(seq16_d *seq)
{
    int i;
    sp_audio spa;
    SPFLOAT buf[4 * 16];
    spa_open(seq->pd->sp, &spa, seq->filename, SPA_WRITE);
    for(i = 0; i < 4 * 16; i++) {
        switch(seq->grid[16 + i]) {
            case SELECT_ON:
            case NOTE_ON:
                buf[i] = 1.0;
                break;
            case SELECT_OFF:
            case NOTE_OFF:
                buf[i] = 0.0;
                break;
        }
    }
    spa_write_buf(seq->pd->sp, &spa, buf, 4 * 16);
    spa_close(&spa);

    plumber_print(seq->pd, "seq16: wrote file to %s\n", seq->filename);

}

void seq16_load(seq16_d *seq)
{
    int i;
    int rc;
    sp_audio spa;
    SPFLOAT buf[4 * 16];

    rc = spa_open(seq->pd->sp, &spa, seq->filename, SPA_READ);
    if(rc == SP_NOT_OK) {
        return;
    }
    plumber_print(seq->pd, "seq16: loading file %s\n", seq->filename);
    spa_read_buf(seq->pd->sp, &spa, buf, 4 * 16);
    for(i = 0; i < 4 * 16; i++) {
        if(buf[i] != 0.0) {
            seq->grid[16 + i] = NOTE_ON;
        } else {
            seq->grid[16 + i] = NOTE_OFF;
        }
    }
    spa_close(&spa);


}

static int sporth_seq16(plumber_data *pd, sporth_stack *stack, void **ud)
{
    seq16_d *seq;
    SPFLOAT in;
    char *str;
    char *filename;
    switch(pd->mode) {
        case PLUMBER_CREATE:
            seq = malloc(sizeof(seq16_d));
            *ud = seq;
            str = sporth_stack_pop_string(stack);
            filename = sporth_stack_pop_string(stack);
            sporth_stack_pop_float(stack);
            seq->pd = pd; 
            sp_ftbl_create(pd->sp, &seq->ft, 4);
            plumber_ftmap_add(pd, str, seq->ft);
            
            break;

        case PLUMBER_INIT:
            seq = *ud;
            sporth_stack_pop_string(stack);
            filename = sporth_stack_pop_string(stack);
            sporth_stack_pop_float(stack);
            seq16_start(seq, filename);
            break;

        case PLUMBER_COMPUTE:
            seq = *ud;
            in = sporth_stack_pop_float(stack);
            if(in != 0) {
                seq16_move_playhead(seq);
                seq16_update_ftbl(seq);
            }
            break;
        case PLUMBER_DESTROY:
            seq = *ud;
            seq16_stop(seq);
            free(seq);
            break;
    }
    return PLUMBER_OK;
}

plumber_dyn_func sporth_return_ugen()
{
    return sporth_seq16;
}
