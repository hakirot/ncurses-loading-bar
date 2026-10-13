#define PROGRAM_NAME "ncurses-loading-bar"
#define PROGRAM_VERSION "0.1.0"

#ifndef BAR_H
#define BAR_H

#include "wchar.h"
#include <stdlib.h>

#define black           1
#define red             2
#define green           3
#define yellow          4
#define blue            5
#define magenta         6
#define cyan            7
#define white           8
#define white_black     9
#define black_red       10
#define black_green     11
#define black_yellow    12
#define black_blue      13
#define black_magenta   14
#define black_cyan      15
#define black_white     16
#define white_blackd    17
#define red_black       18
#define green_black     19
#define yellow_black    20
#define blue_black      21
#define magenta_black   22
#define cyan_black      23
#define black_whited    24

typedef struct {
  int  rows;
  int  cols;
  int  cache;
  int  num_bars;
  char message[256];
  char header[256];
} screen;

typedef struct {
  char  executable[256];
  char  input_log[256];
  char  pipe_file[256];
//FILE* input_log_fd;
  int   remove_input_log;
} args_config;

//const wchar_t MenuBorder[] =   L"┌┐└┘─│";
const wchar_t MenuBorder[] =     L"╭╮╰╯─│";
wchar_t block = L'\u2588';
int quit_counter;

char SLAP_STR[256];
int PRINT_FLAG;
struct timeval slap_time;

void  get_helped();
void  launch_window();
void  version();
void  check_size(      screen*    m_screen                         );
void  bar_borders(     int               c, screen*     m_screen   );
void  log_borders(     int               c, screen*     m_screen   );
void  log_print(       screen*    m_screen                              );
void  process_stdin(   screen*    m_screen, FILE  * control_pipe   );
void  load_message(    screen*    m_screen, char  *         line   );
void  update_progress( screen*    m_screen, int       percentage   );
void  handle_args(     int            argc, char  *         argv[] );
void  _quit(           int       exit_code                         );
void  crit(            char  *         err                         );
void  launch(          char  *  executable                         );
void  animate_log_open(screen*  m_screen);
off_t find_last_lines(int fd, size_t n);
void  clear_log_box(int dim_y, int dim_x);
int   key();
void  slap(char * slap_msg);
void  reprint(screen* m_screen);
void  play_end(screen* m_screen);
void  load_header(screen* m_screen, char * line);

#endif
