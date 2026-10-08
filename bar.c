
#include <ncurses.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <locale.h>
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

  if(argc != 2){
    get_helped();
  }

  char input_file[256] = {'\0'};
  FILE *file = fopen(input_file, "r");

  screen m_screen;
  m_screen.rows = 0;
  m_screen.cols = 0;
  m_screen.cache = 0;
  quit_counter = 0;

  setlocale(LC_ALL, "");
  launch_window();
  check_size(&m_screen);
  bar_borders(white, &m_screen);

//char err[128];
//sprintf(err, "%d", m_screen.rows);
//crit(err);

  while(1){
    load_bar();
    check_size(&m_screen);
    key();
    usleep(40000);
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
  mvprintw(0, 0, "rows: %d cols: %d", m_screen->rows, m_screen->cols);
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

int check_size(screen * m_screen){
  int rows = 0;
  int cols = 0;

  getmaxyx(stdscr, rows, cols);

  m_screen->rows = rows;
  m_screen->cols = cols;
  return m_screen->rows + m_screen->cols;
}

void load_bar(){
  return;
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
