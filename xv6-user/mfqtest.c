#include "kernel/include/types.h"
#include "user.h"

int
main(void)
{
  int started = 0;
  int failed = 0;
  int pids[2];

  for(int i = 0; i < 2; i++){
    int pid = fork();
    if(pid < 0){
      printf("mfqtest: fork failed\n");
      failed = 1;
      break;
    }
    if(pid == 0){
      int start = uptime();
      while(uptime() - start < 96)
        ;
      exit(0);
    }
    pids[started++] = pid;
  }
  if(started == 2)
    printf("mfqtest: worker pids=%d,%d\n", pids[0], pids[1]);

  for(int i = 0; i < started; i++){
    int status;
    if(wait(&status) < 0 || status != 0)
      failed = 1;
  }

  printf("mfqtest: %s\n", failed ? "FAILED" : "workers finished");
  exit(failed);
}
