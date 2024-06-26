#ifndef OPTS_H
#define OPTS_H

#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "sem.h"
#include "seat.h"
#include "dictionary_shm.h"



int Atoi(const char *str);

void parse_seat(char *seat, int n, int m, int *ss, int *ee, int *type);

int sold_check(Seat* Hall, int start, int end, char *name);

int sold_find(Seat* Hall, int row, int num, char *name, int len, int *to_do);

int do_reserve(char *seat, char *name, int n, int m, Seat* Hall, enum State st);

int do_show(char *name, Dictionary *dic);

#endif