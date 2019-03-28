#ifndef FILE_INCLUDED_
#define FILE_INCLUDED_

#include <stdio.h>
#include <setjmp.h>
#include <stdint.h>

#define FILE_FORMAT_VERSION 1

typedef enum {
	FE_SUCCESS = 0,
	FE_SYSTEM,
	FE_UNEXPECTED_EOF,
	FE_INVALID_FORMAT,
	FE_INVALID_GENOME_ID,
	FE_INVALID_DIRECTION,
	FE_INVALID_DIMENSIONS
} file_error_t;

void read_file_header(FILE *from, jmp_buf jb);

void write_file_header(FILE *to, jmp_buf jb);

uint32_t read_32(FILE *from, jmp_buf jb);

void write_32(uint32_t val, FILE *to, jmp_buf jb);

void file_error_print(file_error_t err);

#endif /* FILE_INCLUDED_ */
