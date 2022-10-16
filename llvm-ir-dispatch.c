#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define DO_CACHE (1)

// Returns a file descriptor to the in-memory LLVM IR of the specified file
int get_ir_fd_of(char *fpath){
  int pipe_fd[2];
  if (pipe(pipe_fd) == -1){
    perror("pipe"); exit(-1);
  }

  int pid = fork();
  switch (pid) {
  case -1: { perror("fork"); exit(-1); break; }
  case 0: {
    if ( close(pipe_fd[0]) == -1 ){ perror("close"); exit(-1); }
    if ( dup2(pipe_fd[1], STDOUT_FILENO ) == -1 ){
      perror("dup2");
      exit(-1);
    }

    echo_ir_clang(fpath, pipe_fd[1]);
  }
  default: {
    if (close(pipe_fd[1]) == -1){ perror("close"); exit(-1); }
    return pipe_fd[0];
  }
  }
}

void echo_ir_clang(char *fpath, int write_fd){
  if (system(NULL) == 0){
    fprintf(stderr,
	    "Shell not available on the current system. "
	    "See 'man 3 system' for details.");
    close(write_fd);
    exit(-1);
  }

  int shell_pid;
  int shell_command_len = 256;
  char shell_command[shell_command_len];
  snprintf(shell_command, shell_command_len, "clang %s -emit-llvm -c", fpath);
  
  if ( (shell_pid = system(shell_command)) != 0 ) {
    perror("system(3)");

    int wait_status;
    if ( waitpid(shell_pid, &wait_status, 0) != shell_pid) { perror("waitpid"); }
  }

  if ( close(pipe_fd[0]) == -1 ){ perror("close"); exit(-1); }
}

