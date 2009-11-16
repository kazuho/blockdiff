#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <openssl/crypto.h>

#define BLOCK_SIZE (16384)


/* read patch data from STDIN and apply to argv[0] */

int main(int argc, char **argv)
{
  int fd;
  unsigned char block[BLOCK_SIZE];
  size_t block_size;
  long long offset;
  
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <file>\n", argv[0]);
    exit(99);
  }
  
  if ((fd = open(argv[1], O_RDWR | O_CREAT, 0666)) == -1) {
    perror("failed to open file");
    exit(1);
  }
  
  while (fread(&offset, 1, sizeof(offset), stdin) == sizeof(offset)) {
    if ((block_size = fread(block, 1, sizeof(block), stdin)) <= 0) {
      perror("unexpected eof");
      exit(2);
    }
    if (lseek(fd, offset, SEEK_SET) == -1) {
      perror("seek failed");
      exit(3);
    }
    if (write(fd, block, block_size) != block_size) {
      perror("write failed");
      exit(3);
    }
  }
  
  close(fd);
  
  return 0;
}
