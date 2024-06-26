
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<sys/shm.h>
#include<string.h>
#include<unistd.h>
#include<signal.h>
#include<wait.h>

#include"time_alarm.h"

int main(){
    int n = 4;
    pid_t pid;
    for (int i = 1; i < n; ++i){
        pid = fork();
        if(pid==0){//child
            break;
        }
        sleep(1);
    }
    if (pid == 0){ //child
        set_alarm(2);
        
    }
    else{
        while(true);
        return 0;
    }
}