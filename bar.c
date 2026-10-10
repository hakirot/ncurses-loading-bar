
#include <ncurses.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <locale.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include "bar.h"
#include "ncurses.h"

int main(int argc, char* argv[]){

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
      get_helped();
    } else if (strcmp(argv[i], "--version") == 0) {
      version();
    }
  }

  screen m_screen;
  m_screen.rows = 0;
  m_screen.cols = 0;
  m_screen.cache = 0;
  m_screen.num_bars = 0;
  strncpy(m_screen.message, "Initializing loading bar", 256);
  quit_counter = 0;

  setlocale(LC_ALL, "");
  launch_window();
  check_size(&m_screen);
  bar_borders(white, &m_screen);

  int fd = open("load_pipe", O_RDWR);
  if (fd == -1) {
      perror("open");
      return 1;
  }

  FILE *pipe = fdopen(fd, "r");
  if (pipe == NULL) {
      perror("fdopen");
      close(fd);
      return 1;
  }

  while(1){

    check_size(&m_screen);
    process_stdin(&m_screen, pipe);
    key();

    usleep(20000);
  }

  return 0;
}

void launch_window(){
  initscr();
  start_color();
  use_default_colors();
  init_pair(black, COLOR_BLACK, -1);
  init_pair(red, COLOR_RED, -1);
  init_pair(green, COLOR_GREEN, -1);
  init_pair(yellow, COLOR_YELLOW, -1);
  init_pair(blue, COLOR_BLUE, -1);
  init_pair(magenta, COLOR_MAGENTA, -1);
  init_pair(cyan, COLOR_CYAN, -1);
  init_pair(white, COLOR_WHITE, -1);

  init_pair(white_black, COLOR_WHITE, COLOR_BLACK);
  init_pair(black_red, COLOR_BLACK, COLOR_RED);
  init_pair(black_green, COLOR_BLACK, COLOR_GREEN);
  init_pair(black_yellow, COLOR_BLACK, COLOR_YELLOW);
  init_pair(black_blue, COLOR_BLACK, COLOR_BLUE);
  init_pair(black_magenta, COLOR_BLACK, COLOR_MAGENTA);
  init_pair(black_cyan, COLOR_BLACK, COLOR_CYAN);
  init_pair(black_white, COLOR_BLACK, COLOR_WHITE);

  init_pair(white_blackd, COLOR_WHITE, COLOR_BLACK);
  init_pair(red_black, COLOR_RED, COLOR_BLACK);
  init_pair(green_black, COLOR_GREEN, COLOR_BLACK);
  init_pair(yellow_black, COLOR_YELLOW, COLOR_BLACK);
  init_pair(blue_black, COLOR_BLUE, COLOR_BLACK);
  init_pair(magenta_black, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(cyan_black, COLOR_CYAN, COLOR_BLACK);
  init_pair(black_whited, COLOR_WHITE, COLOR_BLACK);

  cbreak();
  noecho();
  nodelay(stdscr, TRUE);
  keypad(stdscr, TRUE);
  curs_set(FALSE);
  clear();
 
}

void version(){
  printf("%s %s \n", PROGRAM_NAME, PROGRAM_VERSION);
  exit(0);
}

void get_helped(){
  printf("Usage: %s [INPUT_FILE]\n\n", "bar");
  printf("echo single-line STRINGS to INPUT_FILE followed by INT to display load percentage in this program\n\n");
  printf("Visit \x1b[31mhakipaks.org/snippets\x1b[0m for detailed docs\n\n");
  printf("  --help, -h      Get helped\n");
  printf("  -v, --version   Get version\n");
  exit(0);
}

void bar_borders(int c, screen* m_screen){
  clear();
//mvprintw(0, 0, "rows: %d cols: %d", m_screen->rows, m_screen->cols);
  wchar_t wc = MenuBorder[0];
  cchar_t cchar;
  setcchar(&cchar, &wc, 0, 0, NULL);
  mvadd_wch(m_screen->rows/2 - 1, 1, &cchar);

  int dim_x = m_screen->cols - 2;

  wc = MenuBorder[1];
  setcchar(&cchar, &wc, 0, 0, NULL);
  mvadd_wch(m_screen->rows/2 - 1, m_screen->cols-2,  &cchar);

  wc = MenuBorder[2];
  setcchar(&cchar, &wc, 0, 0, NULL);
  mvadd_wch(m_screen->rows/2 + 1, 1, &cchar);

  wc = MenuBorder[3];
  setcchar(&cchar, &wc, 0, 0, NULL);
  mvadd_wch(m_screen->rows/2 + 1, m_screen->cols - 2, &cchar);

  wc = MenuBorder[4];
  setcchar(&cchar, &wc, 0, 0, NULL);
  for(int i = 1; i < dim_x - 1; i++){
    mvadd_wch(m_screen->rows/2 - 1, 1 + i, &cchar);
    mvadd_wch(m_screen->rows/2 + 1, 1 + i, &cchar);
  }

  attroff(COLOR_PAIR(c));
  wc = MenuBorder[5];
  setcchar(&cchar, &wc, 0, 0, NULL);
  for(int i = 1; i < 2; i++){
    mvadd_wch(m_screen->rows/2, 1, &cchar);
    mvadd_wch(m_screen->rows/2, m_screen->cols - 2, &cchar);
  }
  attroff(COLOR_PAIR(c));

  refresh();
  return;
}

void check_size(screen * m_screen){
  int rows = 0;
  int cols = 0;

  getmaxyx(stdscr, rows, cols);

  if(m_screen->cache != rows + cols) {
    m_screen->rows = rows;
    m_screen->cols = cols;
    m_screen->cache = rows + cols;
    reprint(m_screen);
  }
}

void process_stdin(screen* m_screen, FILE * pipe){

  char *input = (char *)malloc(256 * sizeof(char));
  memset(input, '\0', 256 * sizeof(char));

  size_t size;
  fgets(input, 256, pipe);

  if(size < 0){
    free(input);
    crit("bad data?");
    return;
  }

  char line[256] = {"\0"}; 
  strncpy(line, input, 256);
  line[strlen(line)-1] = '\0';

  int lead_char = line[0];

  if(lead_char > 47 && lead_char < 58){

    if (strlen(line) > 2){

      if(strncmp("100", line, strlen(line)) == 0){
        crit("END");
      }

      crit("err_1");

      char err[128];
      sprintf(err, "%s %s", "NUMBER ERROR", line);
      slap(err);
      free(input);
      return;
    }

    for(int i = 0; i < (int)strlen(line); i++){
      if (line[i] < 48 || line[i] > 57){
        crit("err_2");
        char err[128];
        sprintf(err, "%s %s", "NUMBER ERROR", line);
        slap(err);
        free(input);
        return;
      }
    }

    int percentage = atoi(line);

    update_progress(m_screen, percentage);
    reprint(m_screen);

  } else if(lead_char > 51) {
    load_message(m_screen, line);
    reprint(m_screen);
  }

  free(input);
  return;
}

void update_progress(screen* m_screen, int percentage){
  int num_available_bars = m_screen->cols - 4;
  float bar_percentage = (float)percentage/100;
  int num_progress_bars = (int)((float)num_available_bars*bar_percentage);
  m_screen->num_bars = num_progress_bars;
}

int key(){

}

void _quit(){
  exit(0);
}

void crit(char * err) {
  endwin();
  printf("\x1b[31m%s\n\x1b[0m", err);
  _quit();
}

void slap(char * slap_msg) {
  mvprintw(0, 0, "%s", slap_msg);
  memset(SLAP_STR, 0, 256 * sizeof(char));
  strncpy(SLAP_STR, slap_msg, 256);
  gettimeofday(&slap_time, NULL);
}

int _slap_timer() {
  struct timeval t2;
  double elapsed_time;
  gettimeofday(&t2, NULL);

  elapsed_time = (t2.tv_sec - slap_time.tv_sec) * 1000.0;

  int slap_display_time = 1750;
  if(elapsed_time > slap_display_time){
    memset(SLAP_STR, 0, 256 * sizeof(char));
    return 1;
  } else {
    return 0;
  }
}

void load_message(screen * m_screen, char* line){
  strncpy(m_screen->message, line, 256);
}

void reprint(screen* m_screen){
  clear();
  bar_borders(white, m_screen);
  mvprintw(m_screen->rows/2 - 2, 2, "%s", m_screen->message);

  cchar_t cchar;
  setcchar(&cchar, &block, 0, 0, NULL);

  for(int i = 0; i < m_screen->num_bars; i++){
    mvadd_wch(m_screen->rows/2, 2 + i,  &cchar);
  }
  refresh();

  return;
}
