#include <stdio.h>
#include <unistd.h>	//alarm
#include <signal.h> //signal函数
 
void handler(int sig) {
    pid_t pid = getpid();
    printf("%d awake!\n", pid);
}

int set_alarm(int time) {
    pid_t pid = getpid();
    printf("%d set alarm!\n", pid);
    signal(SIGALRM, handler);//注册信号，当收到SIGALRM时触发handler
    alarm(time);  // 设置time秒后触发SIGALRM信号
    sleep(time-1);
    printf("%d 1\n", pid);
    return 0;
}