#ifndef _STRING_H_
#define _STRING_H_


void memory_copy(void *src, int size, void *dst);

void memory_set(void *dist, char ch, int size);

void print_string(char *str);

void print_hex(int num);

char *itoa(char *str, int num);


#endif