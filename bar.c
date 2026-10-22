
#include <ncurses.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <locale.h>
#include <getopt.h>
#include <time.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "bar.h"
#include "ncurses.h"

int DEBUG = 0;

args_config config;

int main(int argc, char* argv[]){

  screen m_screen;
  m_screen.rows = 0;
  m_screen.cols = 0;
  m_screen.cache = 0;
  m_screen.num_bars = 0;
  config.remove_input_log = true;
//char message[256];
//strncpy(m_screen.message, message, 256);
  strncpy(m_screen.header, "ncurses-loading-bar 0.1.0", 256);
  quit_counter = 0;

  handle_args(argc, argv, &m_screen);
  sprintf(m_screen.message, "%s %s", "Waiting for data from", config.pipe_file);

  mkfifo(config.pipe_file, 0666);

  // TODO test whether this block is needed
  int control_fd = open(config.pipe_file, O_RDWR);
  if (control_fd == -1) {
      perror("open");
      return 1;
  }

  config.pipe_fd = fdopen(control_fd, "r");
  if (config.pipe_fd == NULL) {
      perror("fdopen");
      close(control_fd);
      return 1;
  }

  setlocale(LC_ALL, "");
  launch_window(&m_screen);
  if(config.executable[0] != 0) launch(config.executable); // possible sleep
  if(config.input_log[0] != 0 && config.skip_log_startup == false) {
    //bar_borders(white, &m_screen);
    print_header(&m_screen);
    print_message(&m_screen);
    animate_log_open(&m_screen);
  }
  reprint(&m_screen);

  int poll_pipe_fd = fileno(config.pipe_fd);
  struct pollfd pfd = {
    .fd = poll_pipe_fd,
    .events = POLLIN
  };

  int pipe_updated;
  int pipe_updated_ret = 0;

  int log_exists = 0;
  if(config.input_log[0] != 0){
    log_exists = 1;
  }

  if(log_exists){
    config.log_fd = fopen(config.input_log, "r");
    if (config.log_fd == NULL) {
      crit("fdopen");
      close(control_fd);
    }
  }

  int poll_log_fd = 0;
  struct pollfd lfd;

  if(log_exists){
    poll_log_fd = fileno(config.log_fd);
    lfd.fd = poll_log_fd;
    lfd.events = POLLIN;
  }

  int log_updated;
  int log_updated_ret = 0;

  while(1){

    pipe_updated = 0;
    pipe_updated_ret = poll(&pfd, 1, 0);
    if (pipe_updated_ret > 0 && (pfd.revents & POLLIN)) {
      pipe_updated = 1;
    }

    if(log_exists){
      log_updated = 0;
      log_updated_ret = poll(&lfd, 1, 0);
      if (log_updated_ret > 0 && (lfd.revents & POLLIN)) {
        log_updated = 1;
      }
    }

    check_size(&m_screen);
    process_stdin(&m_screen, config.pipe_fd, pipe_updated, log_updated);
    key();

    usleep(20000);
  }

  _quit(0);
}

void launch_window(screen* m_screen){
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

  int rows, cols = 0;
  getmaxyx(stdscr, rows, cols);
  m_screen->rows  = rows;
  m_screen->cols  = cols;
  m_screen->cache = rows + cols;
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
  printf("  -E [executable]  execute [executable]. Executable inherits CONTROL_PIPE\n");
  printf("                   env var, while loading bar captures output log\n");
  printf("  -l [log_file]    Provide [log_file] from which to display logs, not compatible with -E\n");
  printf("  -d               Preserve [log_file]\n");
  printf("  -H               Overwrite default startup header string\n");
  printf("  -S               Skip log startup animation\n");
  printf("  -C               Specify closing message\n");
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

  wc = MenuBorder[5];
  setcchar(&cchar, &wc, 0, 0, NULL);
  for(int i = 1; i < 2; i++){
    mvadd_wch(5, 1, &cchar);
    mvadd_wch(5, m_screen->cols - 2, &cchar);
  }

  refresh();
  return;
}

void log_borders(int c, screen* m_screen){

  if(m_screen->rows < 14) return;

  wchar_t wc = MenuBorder[0];
  cchar_t cchar;
  setcchar(&cchar, &wc, 0, 0, NULL);
  mvadd_wch(9, 1, &cchar);

  int dim_x = m_screen->cols - 2;

  wc = MenuBorder[1];
  setcchar(&cchar, &wc, 0, 0, NULL);
  mvadd_wch(9, m_screen->cols-2,  &cchar);

  wc = MenuBorder[2];
  setcchar(&cchar, &wc, 0, 0, NULL);
  mvadd_wch(m_screen->rows - 3, 1, &cchar);

  wc = MenuBorder[3];
  setcchar(&cchar, &wc, 0, 0, NULL);
  mvadd_wch(m_screen->rows - 3, m_screen->cols - 2, &cchar);

  wc = MenuBorder[4];
  setcchar(&cchar, &wc, 0, 0, NULL);
  for(int i = 1; i < dim_x - 1; i++){
    mvadd_wch(9, 1 + i, &cchar);
    mvadd_wch(m_screen->rows - 3, 1 + i, &cchar);
  }

  wc = MenuBorder[5];
  setcchar(&cchar, &wc, 0, 0, NULL);
  for(int i = 0; i < m_screen->rows - 13; i++){
    mvadd_wch(10 + i, 1, &cchar);
    mvadd_wch(10 + i, dim_x, &cchar);
  }

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

void process_stdin(screen* m_screen, FILE * control_pipe, int pipe_updated, int log_updated){

  char *input = (char *)malloc(256 * sizeof(char));
  memset(input, '\0', 256 * sizeof(char));

  if(pipe_updated){
    fgets(input, 256, control_pipe);

    char line[256] = {"\0"}; 
    strncpy(line, input, 256);
    line[strlen(line)-1] = '\0';

    int lead_char = line[0];

    if(lead_char > 47 && lead_char < 58){

      if (strlen(line) > 2 && strncmp("100", line, strlen(line)) != 0){
        char err[128];
        sprintf(err, "%s %s", "NUMBER INPUT ERROR: possibly not in range:", line);
        crit(err);
        return;
      }

      int percentage = atoi(line);

      update_progress(m_screen, percentage);
      reprint(m_screen);

    } else if(lead_char == '+'){
      load_header(m_screen, line);
      reprint(m_screen);
    } else if(lead_char == '-' && strstr(line, "-CRIT")){
      crit(line);
    } else if (strncmp(line, "PIPE_END", 7) == 0){
      play_end(m_screen);
    } else if(lead_char > 51) {
      load_message(m_screen, line);
      reprint(m_screen);
    }
  } 

  if (log_updated) {
    log_print(m_screen);
  }

  free(input);
  return;
}

void update_progress(screen* m_screen, int percentage){
  int num_available_bars = m_screen->cols - 4;
  float bar_percentage = (float)percentage/100;
  int num_progress_bars = (int)((float)num_available_bars*bar_percentage);
  m_screen->num_bars = num_progress_bars;
//if(percentage == 100){
//  char err[128];
//  sprintf(err, "%s%d %s%d", "num available: ", num_available_bars, "num bars: ", num_progress_bars);
//  crit(err);
//}
}

int key(){

}

void _quit(int exit_code){
  endwin();
  fclose(config.pipe_fd);
  remove(config.pipe_file);
  if(config.remove_input_log) remove(config.input_log);
  if(config.input_log[0] != 0) fclose(config.log_fd);
  exit(exit_code);
}

void crit(char * err) {
  endwin();
  printf("\x1b[31m%s\n\x1b[0m", err);
  remove(config.pipe_file);
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

  if(config.input_log[0] != 0) {
    log_borders(white, m_screen);
    log_print(m_screen);
  }

  print_message(m_screen);
  print_header(m_screen);

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

  if(config.closing_message[0] == 0){
    strncpy(config.closing_message, "- = COMPLETE = -", 256);
  }

  int flag = 0;
  int num_frames = 15;
  int sleep_time = 50000;

  animate_close(m_screen);

  while (num_frames > 0) {

    usleep(sleep_time);
    if(flag == 0){
      mvprintw(m_screen->rows/2 - 1, m_screen->cols/2 - strlen(config.closing_message)/2, "%s", config.closing_message);
      flag++;

//    L"╭╮╰╯─│";
//    wchar_t wc = MenuBorder[0];
      cchar_t cchar;

      setcchar(&cchar, &MenuBorder[0], 0, 0, NULL);
      mvadd_wch(m_screen->rows/2 - 2, m_screen->cols/2 - strlen(config.closing_message)/2 - 2, &cchar);

      setcchar(&cchar, &MenuBorder[1], 0, 0, NULL);
      mvadd_wch(m_screen->rows/2 - 2, m_screen->cols/2 + strlen(config.closing_message)/2 + 2, &cchar);

      setcchar(&cchar, &MenuBorder[2], 0, 0, NULL);
      mvadd_wch(m_screen->rows/2, m_screen->cols/2 - strlen(config.closing_message)/2 - 2, &cchar);

      setcchar(&cchar, &MenuBorder[3], 0, 0, NULL);
      mvadd_wch(m_screen->rows/2, m_screen->cols/2 + strlen(config.closing_message)/2 + 2, &cchar);

      setcchar(&cchar, &MenuBorder[5], 0, 0, NULL);
      mvadd_wch(m_screen->rows/2 - 1, m_screen->cols/2 - strlen(config.closing_message)/2 - 2, &cchar);
      mvadd_wch(m_screen->rows/2 - 1, m_screen->cols/2 + strlen(config.closing_message)/2 + 2, &cchar);

      setcchar(&cchar, &MenuBorder[4], 0, 0, NULL);
      for(int i = 0; i < strlen(config.closing_message) + 2; i++){

        mvadd_wch(m_screen->rows/2 - 2, m_screen->cols/2 - strlen(config.closing_message)/2 - 1 + i, &cchar);
        mvadd_wch(m_screen->rows/2, m_screen->cols/2 - strlen(config.closing_message)/2 - 1 + i, &cchar);

      }

    } else {
      clear();
      flag--;
    }
    refresh();
    num_frames--;
  }
  mvprintw(m_screen->rows/2 - 1, m_screen->cols/2 - strlen(config.closing_message)/2, "%s", config.closing_message);
  refresh();
  usleep(500000);
  _quit(0);
}

void load_header(screen* m_screen, char * line){
  memmove(line, line+1, strlen(line));
  strncpy(m_screen->header, line, 256);
}

void handle_args(int argc, char * argv[], screen* m_screen){

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--help") == 0 
    || strcmp(argv[i], "-h")      == 0)
    {
      get_helped();
    } else if (strcmp(argv[i], "--version") == 0) {
      version();
    }
  }

  memset(config.executable,       '\0', 256);
  memset(config.input_log,        '\0', 256);
  memset(config.pipe_file,        '\0', 256);
  memset(config.closing_message,  '\0', 256);
  config.skip_log_startup = false;

  int opt;

  while ((opt = getopt(argc, argv, "vl:E:n:dH:SC:")) != -1) {
      switch (opt) {
      case 'l':
          // TODO test these
//        if(config.executable[0] != 0) crit("-E not compatible with -l");
          if(optarg[0] != '/') crit("error: -l must be absolute path to log file");
          strncpy(config.input_log, optarg, 256);
          break;
      case 'v':
          version();
      case 'E':
          // TODO test these
//        if(config.input_log[0] != 0) crit("-E not compatible with -l");

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
      case 'H':
          strncpy(m_screen->header, optarg, 256);
          break;
      case 'd':
          config.remove_input_log = false;
          break;
      case 'S':
          config.skip_log_startup = true;
          break;
      case 'C':
          strncpy(config.closing_message, optarg, 256);
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
    crit("setenv error");
  }

  char input_log_path[256];
  if(config.input_log[0] == 0){
    char input_log_path[256];
    sprintf(input_log_path, "%s%d", "/tmp/loading-bar-", bar_pid);
    strncpy(config.input_log, input_log_path, 256);
  } else {
    sprintf(input_log_path, "%s", config.input_log);
  }

  pid_t pid = fork();
  if (pid < 0) {
    crit("forked it up");
  } else if (pid == 0) {

    // child process (executable) writes all output to input_log_path
    // to be displayed by parent proc (loading bar)
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

  if(m_screen->rows < 14) return;

  int frame_sleep = 5000;
  int log_low     = 8;
  int log_high    = m_screen->rows - 2;
  int dim_y       = log_high - log_low;
  int mid         = (log_high - log_low)/2 + 8;
  int dim_x       = m_screen->cols - 2;

  wchar_t wc = MenuBorder[4];
  cchar_t cchar;
  setcchar(&cchar, &wc, 0, 0, NULL);

  for(int i = 0; i < dim_x/2; i+=2){
    for(int j = 0; j < i; j++){
      mvadd_wch(mid, m_screen->cols/2 + j, &cchar);
      mvadd_wch(mid, m_screen->cols/2 - j, &cchar);
    }
    refresh();
    usleep(frame_sleep);
    for(int j = 0; j < i; j++){
      mvaddch(mid, m_screen->cols/2 + j, ' ');
      mvaddch(mid, m_screen->cols/2 - j, ' ');
    }
    refresh();
    usleep(frame_sleep);
  }

  frame_sleep = 10000;
  for(int i = 0; i < dim_y/2; i++){
    for(int j = 0; j < dim_x; j++){
      mvadd_wch(mid + i, 1 + j, &cchar);
      mvadd_wch(mid - i, 1 + j, &cchar);
      mvaddch(mid + i - 1, 1 + j, ' ');
      mvaddch(mid - i + 1, 1 + j, ' ');
    }
    refresh();
    usleep(frame_sleep);
  }
  refresh();

  log_borders(white, m_screen);
}

void log_print(screen* m_screen){

  if(m_screen->rows < 14) return;
  int dim_y = m_screen->rows - 11;

  int rows = dim_y - 2;
  int cols = m_screen->cols - 4;

  if (rows <= 0 || cols <= 0) {
    return;
  }

  int fd = open(config.input_log, O_RDONLY);
  if (fd == -1) return;

  off_t start = find_last_lines(fd, (size_t)rows);

  if (start < 0 || lseek(fd, start, SEEK_SET) == -1) {
    close(fd);
    return;
  }

  FILE *fp = fdopen(fd, "r");
  if (fp == NULL) {
    close(fd);
    return;
  }

  clear_log_box(rows, cols);

  char *line = NULL;
  size_t capacity = 0;
  int row = 0;

  wchar_t wc = MenuBorder[5];
  cchar_t cchar;
  setcchar(&cchar, &wc, 0, 0, NULL);

  while (row <= rows && getline(&line, &capacity, fp) != -1) {
     // Avoid scrolling the window. Clip to its width.
    int len = strlen(line);
    int print_row = row + 10;
    for(int i = 0; i < len - 1; i++){
      if(i > m_screen->cols - 5) break;
      if(print_row > m_screen->rows - 4) break;// sometimes prompted output tries to escape
      mvaddch(print_row, 2 + i, line[i]);
    }
    row++;
  }

  free(line);
  fclose(fp);

  refresh();
}

off_t find_last_lines(int fd, size_t n) {

    int CHUNK_SIZE = 4096;
    struct stat st;

    if (n == 0 || fstat(fd, &st) == -1)
        return -1;

    off_t pos = st.st_size;

    if (pos == 0)
        return 0;

    char buf[CHUNK_SIZE];
    size_t newlines = 0;
    int skip_final_newline = 1;

    while (pos > 0) {
        size_t count = pos < CHUNK_SIZE
                     ? (size_t)pos : CHUNK_SIZE;

        pos -= count;

        ssize_t bytes = pread(fd, buf, count, pos);
        if (bytes <= 0)
            return -1;

        for (ssize_t i = bytes - 1; i >= 0; i--) {
            if (buf[i] != '\n')
                continue;

            // Don't count the final newline as
            // an additional empty line.
            if (skip_final_newline) {
                skip_final_newline = 0;
                continue;
            }

            newlines++;

            if (newlines == n)
                return pos + i + 1;
        }

        skip_final_newline = 0;
    }

    // Fewer than n lines: start at the beginning.
    return 0;
}

void clear_log_box(int dim_y, int dim_x){

  if(dim_y < 3){
    return;
  }

  wchar_t wc = L' ';
  cchar_t cchar;
  setcchar(&cchar, &wc, 0, 0, NULL);
  for(int i = 0; i < dim_y; i++){
    for(int j = 0; j < dim_x; j++){
      mvadd_wch(10 + i, 2 + j, &cchar);
    }
  }
}

void print_message(screen* m_screen){
  mvprintw(3, 2, "%s", m_screen->message);
  refresh();
}

void print_header(screen* m_screen){
  mvprintw(1, 2, "%s", m_screen->header);
  refresh();
}

void animate_close(screen*  m_screen){

  int rows, cols = 0;
  getmaxyx(stdscr, rows, cols);
  int sleep_time = 7500;

  int dim_y = rows;
  int dim_x = cols;

  int delta = 0;

  int r = 0;
  int l = 0;
  int u = 0;
  int d = 0;

  cchar_t cchar;
  wchar_t wc;
  while(rows/2 > delta){
    clear();

    dim_y = rows - (delta*2);
    dim_x = cols - (delta*2);

    wc = MenuBorder[4];
    setcchar(&cchar, &wc, 0, 0, NULL);
    for(int i = 1; i < dim_x - 1; i++){
      mvadd_wch(0 + delta, 0 + i + delta, &cchar);
      mvadd_wch(rows - 1 - delta, 0 + i + delta, &cchar);
    }

    refresh();
    usleep(sleep_time);
    delta++;
  }

  clear();
  wc = MenuBorder[4];
  setcchar(&cchar, &wc, 0, 0, NULL);

  sleep_time = 4500;
  for(int i = delta; i < cols/2; i+=3){
    refresh();
    usleep(sleep_time);

    clear();
    refresh();
    usleep(sleep_time);

    for(int j = 0; j < cols - i*2; j++){
      mvadd_wch(rows/2, i + j, &cchar);
    }
  }
}
