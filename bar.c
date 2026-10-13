
#include <ncurses.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <locale.h>
#include <getopt.h>
#include <time.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "bar.h"
#include "ncurses.h"

args_config config;

int main(int argc, char* argv[]){

  handle_args(argc, argv);

  screen m_screen;
  m_screen.rows = 0;
  m_screen.cols = 0;
  m_screen.cache = 0;
  m_screen.num_bars = 0;
  config.remove_input_log = 1;
  char message[256];
  sprintf(message, "%s %s", "Waiting for data from", config.pipe_file);
  strncpy(m_screen.message, message, 256);
  strncpy(m_screen.header, "ncurses-loading-bar 0.1.0", 256);
  quit_counter = 0;

  mkfifo(config.pipe_file, 0666);

  // TODO test whether this block is needed
  int control_fd = open(config.pipe_file, O_RDWR);
  if (control_fd == -1) {
      perror("open");
      return 1;
  }

  FILE *pipe = fdopen(control_fd, "r");
  if (pipe == NULL) {
      perror("fdopen");
      close(control_fd);
      return 1;
  }

  setlocale(LC_ALL, "");
  launch_window();
  check_size(&m_screen);
  animate_log_open(&m_screen);
  bar_borders(white, &m_screen);
  // possible sleep
  if(config.executable[0] != 0) launch(config.executable);
  reprint(&m_screen);
  while(1){

    check_size(&m_screen);
    process_stdin(&m_screen, pipe);
    key();

    usleep(20000);
  }

  _quit(0);
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
  printf("%s \n", "Visit hakipaks.org/snippets#loading-bar for more details");
  exit(0);
}

void get_helped(){
  printf("Usage: %s [INPUT_FILE]\n\n", "bar");
  printf("echo single-line STRINGS to INPUT_FILE followed by INT to display load percentage in this program\n\n");
  printf("Visit \x1b[31mhakipaks.org/snippets\x1b[0m for detailed docs\n\n");
  printf("  --help, -h       Get helped\n");
  printf("  -v, --version    Get version\n");
  printf("  -n [pipe_name]   Get control data from /tmp/[pipe_name]\n");
  printf("  -E [executable]  execute [executable].\n");
  printf("  -l [log_file]    Provide [log_file] from which to display logs, not compatible with -E\n");
  printf("  -d               Do not remove [log_file]\n");
  exit(0);
}

void bar_borders(int c, screen* m_screen){
  clear();
//mvprintw(0, 0, "rows: %d cols: %d", m_screen->rows, m_screen->cols);
  wchar_t wc = MenuBorder[0];
  cchar_t cchar;
  setcchar(&cchar, &wc, 0, 0, NULL);
  mvadd_wch(4, 1, &cchar);

  int dim_x = m_screen->cols - 2;

  wc = MenuBorder[1];
  setcchar(&cchar, &wc, 0, 0, NULL);
  mvadd_wch(4, m_screen->cols-2,  &cchar);

  wc = MenuBorder[2];
  setcchar(&cchar, &wc, 0, 0, NULL);
  mvadd_wch(6, 1, &cchar);

  wc = MenuBorder[3];
  setcchar(&cchar, &wc, 0, 0, NULL);
  mvadd_wch(6, m_screen->cols - 2, &cchar);

  wc = MenuBorder[4];
  setcchar(&cchar, &wc, 0, 0, NULL);
  for(int i = 1; i < dim_x - 1; i++){
    mvadd_wch(4, 1 + i, &cchar);
    mvadd_wch(6, 1 + i, &cchar);
  }

  attroff(COLOR_PAIR(c));
  wc = MenuBorder[5];
  setcchar(&cchar, &wc, 0, 0, NULL);
  for(int i = 1; i < 2; i++){
    mvadd_wch(5, 1, &cchar);
    mvadd_wch(5, m_screen->cols - 2, &cchar);
  }
  attroff(COLOR_PAIR(c));

  refresh();
  return;
}

void log_borders(int c, screen* m_screen){
  clear();
//mvprintw(0, 0, "rows: %d cols: %d", m_screen->rows, m_screen->cols);
  wchar_t wc = MenuBorder[0];
  cchar_t cchar;
  setcchar(&cchar, &wc, 0, 0, NULL);
  mvadd_wch(7, 1, &cchar);

  int dim_x = m_screen->cols - 2;

  wc = MenuBorder[1];
  setcchar(&cchar, &wc, 0, 0, NULL);
  mvadd_wch(7, m_screen->cols-2,  &cchar);

  wc = MenuBorder[2];
  setcchar(&cchar, &wc, 0, 0, NULL);
  mvadd_wch(m_screen->rows, 1, &cchar);

  wc = MenuBorder[3];
  setcchar(&cchar, &wc, 0, 0, NULL);
  mvadd_wch(m_screen->rows, m_screen->cols - 2, &cchar);

  wc = MenuBorder[4];
  setcchar(&cchar, &wc, 0, 0, NULL);
  for(int i = 1; i < dim_x - 1; i++){
    mvadd_wch(7, 1 + i, &cchar);
    mvadd_wch(m_screen->rows, 1 + i, &cchar);
  }

  attroff(COLOR_PAIR(c));
  wc = MenuBorder[5];
  setcchar(&cchar, &wc, 0, 0, NULL);
  for(int i = 0; i < m_screen->rows - 9; i++){
    mvadd_wch(8 + i, 1, &cchar);
    mvadd_wch(8 + i, dim_x + 1, &cchar);
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
    // TODO relevent log reprints
    reprint(m_screen);
  }
}

void process_stdin(screen* m_screen, FILE * control_pipe){

  char *input = (char *)malloc(256 * sizeof(char));
  memset(input, '\0', 256 * sizeof(char));

  size_t size;
  fgets(input, 256, control_pipe);

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
        play_end(m_screen);
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

  } else if(lead_char == '+'){
    load_header(m_screen, line);
  } else if(lead_char > 51) {
    load_message(m_screen, line);
    reprint(m_screen);
//} else if(lead_char == '+'){
//  crit("+");
//  load_header(m_screen, line);
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

void _quit(int exit_code){
  remove(config.pipe_file);
  remove(config.input_log);
  if(config.remove_input_log) remove(config.input_log);
  endwin();
  exit(exit_code);
}

void crit(char * err) {
  endwin();
  printf("\x1b[31m%s\n\x1b[0m", err);
  remove(config.pipe_file);
  remove(config.input_log);
  if(config.remove_input_log) remove(config.input_log);
  exit(1);
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
  mvprintw(3, 2, "%s", m_screen->message);
  mvprintw(1, 2, "%s", m_screen->header);

  cchar_t cchar;
  setcchar(&cchar, &block, 0, 0, NULL);

  for(int i = 0; i < m_screen->num_bars; i++){
    mvadd_wch(5, 2 + i,  &cchar);
  }
  refresh();

  return;
}

void play_end(screen* m_screen){
  clear();
  refresh();
  char * complete = "- = COMPLETE = -";
  int flag = 0;
  int num_frames = 15;
  int sleep_time = 50000;

  while (num_frames > 0) {

    usleep(sleep_time);
    if(flag == 0){
      mvprintw(m_screen->rows/2 - 1, m_screen->cols/2 - strlen(complete)/2, "%s", complete);
      flag++;
    } else {
      clear();
      flag--;
    }
    refresh();
    num_frames--;
  }
  mvprintw(m_screen->rows/2 - 1, m_screen->cols/2 - strlen(complete)/2, "%s", complete);
  refresh();
  usleep(500000);
  _quit(0);
}

void load_header(screen* m_screen, char * line){
  memmove(line, line+1, strlen(line));
  strncpy(m_screen->header, line, 256);
}

void handle_args(int argc, char * argv[]){

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--help") == 0 
    || strcmp(argv[i], "-h")      == 0)
    {
      get_helped();
    } else if (strcmp(argv[i], "--version") == 0) {
      version();
    }
  }

  memset(config.executable, '\0', 256);
  memset(config.input_log,  '\0', 256);
  memset(config.pipe_file,  '\0', 256);

  int opt;

  while ((opt = getopt(argc, argv, "l:E:n:d")) != -1) {
      switch (opt) {
      case 'l':
          // TODO test these
          if(config.executable[0] != 0) crit("-E not compatible with -l");
          strncpy(config.input_log, optarg, 256);
          break;
      case 'E':
          // TODO test these
          if(config.input_log[0] != 0) crit("-E not compatible with -l");

          char executable[256];
          if(optarg[0] != '/'){
            char * env = getenv("PWD");
            sprintf(executable, "%s/%s", env, optarg);
            if (access(executable, F_OK) != 0){
              crit("inaccessible executable");
            } else {
              // TODO check executable perms
              strncpy(config.executable, executable, 256);
            }
          } else{
            strncpy(config.executable, optarg, 256);
          }
          break;
      case 'n':
          strncpy(config.pipe_file, optarg, 256);
          break;
      case 'd':
          config.remove_input_log = 0;
          break;
      case '?':
          _quit(1);

      default:
          return;
      }
  }

  if(strlen(config.pipe_file) == 0) {
    sprintf(config.pipe_file, "%s", "/tmp/load_pipe");
  } else {
    char temp[256];
    memset(temp, '\0', 256 * sizeof(char));
    strncpy(temp, config.pipe_file, 256);
    char * tmp = "/tmp/";
    char path[256];
    sprintf(path, "%s%s", tmp, temp);
    strncpy(config.pipe_file, path, 256);
  }
}

void exit_if_slash_in_arg(char * arg){
  for(int i = 0; i < (int)strlen(arg); i++){
    if (arg[i] == '/'){
      crit("paths not allowed");
    }
  }
}

void launch(char * executable){

  int bar_pid = getpid();

  if (setenv("CONTROL_PIPE", config.pipe_file, 1) != 0) {
    slap("setenv error");
  }

  char input_log_path[256];
  sprintf(input_log_path, "%s%d", "/tmp/loading-bar-", bar_pid);
  strncpy(config.input_log, input_log_path, 256);

  pid_t pid = fork();
  if (pid < 0) {
    crit("forked it up");
  } else if (pid == 0) {

    // child process (executable) writes all output to input_log_path
    // to be displayed by parent (loading bar)
    int fd = open(input_log_path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    dup2(fd, 1);
    dup2(fd, 2);

    close(fd);

    execl(executable, executable);
    perror("execl");
    crit("execl critical error :|");
  }
}

void animate_log_open(screen* m_screen){

  if(config.input_log[0] == '0'){
    return;
  }
   
  int frame_sleep = 500000;
  int log_low     = 7;
  int log_high    = m_screen->rows;
  int mid         = (log_high - log_low)/2 + 7;
  int dim_x       = m_screen->cols - 2;

  wchar_t wc = MenuBorder[4];
  cchar_t cchar;
  setcchar(&cchar, &wc, 0, 0, NULL);

  for(int i = 0; i < dim_x; i++){
    mvadd_wch(mid, 1 + i, &cchar);
  }
  refresh();

  getchar();

  char err[128];
  sprintf(err, "%d", mid);
  crit(err);
}
