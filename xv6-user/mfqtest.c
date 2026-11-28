#include "kernel/include/types.h"
#include "user.h"

static int
test_wakeup(void)
{
  int ready[2], data[2], status;
  char byte = 'x';

  if(pipe(ready) < 0 || pipe(data) < 0)
    return 1;
  int pid = fork();
  if(pid < 0)
    return 1;
  if(pid == 0){
    close(ready[0]);
    close(data[1]);
    int start = uptime();
    while(uptime() - start < 12)
      ;
    if(write(ready[1], &byte, 1) != 1)
      exit(1);
    close(ready[1]);
    exit(read(data[0], &byte, 1) != 1);
  }

  close(ready[1]);
  close(data[0]);
  printf("mfqtest: wake worker pid=%d\n", pid);
  int failed = read(ready[0], &byte, 1) != 1;
  close(ready[0]);
  if(!failed){
    sleep(4);
    failed = write(data[1], &byte, 1) != 1;
  }
  close(data[1]);
  if(wait(&status) != pid || status != 0)
    failed = 1;
  return failed;
}

int
main(int argc, char **argv)
{
  if(argc == 2 && strcmp(argv[1], "wake") == 0){
    int failed = test_wakeup();
    printf("mfqtest: wake %s\n", failed ? "FAILED" : "worker finished");
    exit(failed);
  }

  int started = 0;
  int failed = 0;
  int workers = argc == 2 && strcmp(argv[1], "aging") == 0 ? 10 : 2;
  int duration = workers == 2 ? 96 : 256;

  for(int i = 0; i < workers; i++){
    int pid = fork();
    if(pid < 0){
      printf("mfqtest: fork failed\n");
      failed = 1;
      break;
    }
    if(pid == 0){
      int start = uptime();
      while(uptime() - start < duration)
        ;
      exit(0);
    }
    started++;
    printf("mfqtest: worker pid=%d\n", pid);
  }

  for(int i = 0; i < started; i++){
    int status;
    if(wait(&status) < 0 || status != 0)
      failed = 1;
  }

  printf("mfqtest: %s\n", failed ? "FAILED" : "workers finished");
  exit(failed);
}
