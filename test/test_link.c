#include <stdio.h>
#include "opts.c"
// #include "dictionary.c"

int main(){
    Dictionary *dic;
    char a[5] = "AA";
    do_show(a, dic);
    dic_show(dic);
    return 0;
}