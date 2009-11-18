#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <openssl/crypto.h>

/* read patch data from STDIN and apply to argv[0] */

int main(int argc, char **argv)
{
  int fd;
  unsigned char* block;
  int blocksize;
  size_t rlen;
  long long offset;
  
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <file>\n", argv[0]);
    exit(99);
  }
  
  if ((fd = open(argv[1], O_RDWR | O_CREAT, 0666)) == -1) {
    perror("failed to open file");
    exit(1);
  }
  
  if (fread(&blocksize, sizeof(blocksize), 1, stdin) != 1) {
    perror("unexpected eof");
    exit(2);
  }
  
  if ((block = malloc(blocksize)) == NULL) {
    perror("no memory");
    exit(2);
  }
  
  while (fread(&offset, 1, sizeof(offset), stdin) == sizeof(offset)) {
    if ((rlen = fread(block, 1, blocksize, stdin)) <= 0) {
      perror("unexpected eof");
      exit(2);
    }
    if (lseek(fd, offset, SEEK_SET) == -1) {
      perror("seek failed");
      exit(3);
    }
    if (write(fd, block, rlen) != rlen) {
      perror("write failed");
      exit(3);
    }
  }
  
  close(fd);
  
  return 0;
}
