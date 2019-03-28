#include "file.h"
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>

void read_file_header(FILE *from, jmp_buf jb)
{
	if (read_32(from, jb) != FILE_FORMAT_VERSION)
		longjmp(jb, FE_INVALID_FORMAT);
}

void write_file_header(FILE *to, jmp_buf jb)
{
	write_32(FILE_FORMAT_VERSION, to, jb);
}

uint32_t read_32(FILE *from, jmp_buf jb)
{
	uint32_t bytes;
	if (fread(&bytes, 4, 1, from) != 1)
		longjmp(jb, feof(from) ? FE_UNEXPECTED_EOF : FE_SYSTEM);
	return ntohl(bytes);
}

void write_32(uint32_t val, FILE *to, jmp_buf jb)
{
	uint32_t bytes = htonl(val);
	if (fwrite(&bytes, 4, 1, to) != 1) longjmp(jb, FE_SYSTEM);
}

void file_error_print(file_error_t err)
{
	switch (err) {
	case FE_SUCCESS:
		fprintf(stderr, "Success");
		return;
	case FE_SYSTEM:
		fprintf(stderr, "System error: %s", strerror(errno));
		return;
	case FE_UNEXPECTED_EOF:
		fprintf(stderr, "Unexpected end-of-file");
		return;
	case FE_INVALID_FORMAT:
		fprintf(stderr, "Invalid file format marker");
		return;
	case FE_INVALID_GENOME_ID:
		fprintf(stderr, "Invalid genome ID");
		return;
	case FE_INVALID_DIRECTION:
		fprintf(stderr, "Invalid animal direction");
		return;
	default:
		fprintf(stderr, "Unknown error: %d", err);
		return;
	}
}
