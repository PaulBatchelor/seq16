#ifndef SEQ16_H
#define SEQ16_H
#define MAX_ROWS 5
typedef struct seq16_d {
    GLFWwindow *window;
    pthread_t thread;
    int run;
    int please_tick;
    int grid[16 * MAX_ROWS];
    int playhead;
    int pos;
    int skip;
    sp_ftbl *ft;
    const char *filename;
    plumber_data *pd;
} seq16_d;

void seq16_start(seq16_d *seq, const char *filename);
void seq16_stop(seq16_d *seq);
void seq16_tick(seq16_d *seq);
void seq16_set_playhead(seq16_d *seq, int pos);
void seq16_set_pos(seq16_d *seq, int pos);
void seq16_move_right(seq16_d *seq);
void seq16_move_left(seq16_d *seq);
void seq16_move_up(seq16_d *seq);
void seq16_move_down(seq16_d *seq);
void seq16_move_playhead(seq16_d *seq);
void seq16_toggle(seq16_d *seq);
void seq16_set_skip(seq16_d *seq, int skip);
void seq16_update_ftbl(seq16_d *seq);
void seq16_save(seq16_d *seq);
void seq16_load(seq16_d *seq);
#endif
