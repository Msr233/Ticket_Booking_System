#include<stdio.h>
//fork
#include <unistd.h>
#include <sys/types.h>
//wait
#include <sys/wait.h>
//exit
#include <stdlib.h>
//IPC&sem
#include <sys/ipc.h>
#include <sys/sem.h>
//share memery
#include <sys/shm.h>

#include "./include/dictionary_shm.c"

key_t get_key(const char *path){
    key_t key;
    for (int i = 1; i <= 128; ++i){
        key = ftok(path, i);
        if (key != -1){
            break;
        }
    }
    return key;
}

pid_t wait_for_all_children() {
    int status;
    pid_t child_pid;
    int child_count = 0;

    while ((child_pid = wait(&status)) > 0) {
        child_count++;
        if (WIFEXITED(status)) {
            printf("Child process %d terminated with exit status: %d\n", child_pid, WEXITSTATUS(status));
        }
        // 可以添加其他状态检查，如WIFSIGNALED(status)等
    }
    printf("Total child processes terminated: %d\n", child_count);
    return child_count;
}


int main(){
    key_t key_dic = get_key("./include/dictionary.h");
    int shm_dic = shmget(key_dic, sizeof(Dictionary), IPC_CREAT | 0666); //初始化
    printf("%d", shm_dic);
    Dictionary* dic = (Dictionary*) shmat(shm_dic, NULL, 0);  
    initDictionary(dic);
    dic_insert(dic, "AA", "get 1 d");
    dic_insert(dic, "AA", "get 2 d");
    pid_t pid;
    pid = fork();
    if (pid == 0){
        Dictionary *dic = (Dictionary*) shmat(shm_dic, NULL, 0);  
        dic_insert(dic, "AA", "get 3 d");
        dic_insert(dic, "AA", "get 4 d");
        dic_insert(dic, "BB", "123434");
        dic_insert(dic, "BB", "1234");
    }
    else{
        wait_for_all_children();

        dic_show(dic);
        if (shmctl(shm_dic, IPC_RMID, NULL) == -1) {
        perror("shmctl IPC_RMID");
        exit(1);
    }
    }
    
}