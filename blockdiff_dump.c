#include <stdio.h>
#include <string.h>
#include <openssl/crypto.h>
#include <openssl/md5.h>

#define BLOCKSIZE_ENV "BLOCKSIZE"
#define DEFAULT_BLOCKSIZE (16384)

/*
 * file descriptors:
 *   0 -> data file
 *   1 <- patch data
 *   5 -> old digest (or none)
 *   6 <- new digest
 */

int main(int argc, char** argv)
{
  FILE* orig_digest_fp, * new_digest_fp;
  unsigned char* block;
  unsigned char digest[MD5_DIGEST_LENGTH], orig_digest[MD5_DIGEST_LENGTH];
  char* blocksize_s;
  int blocksize = DEFAULT_BLOCKSIZE;
  size_t rlen;
  long long offset;
  
  if ((blocksize_s = getenv(BLOCKSIZE_ENV)) != NULL) {
    if ((blocksize = strtol(blocksize_s, NULL, 10)) <= 0) {
      perror("environment variable " BLOCKSIZE_ENV " is invalid");
      exit(1);
    }
  }
  
  /* open files (and check block size) */
  if ((new_digest_fp = fdopen(6, "wb")) == NULL) {
    perror("could not open digest data output (fd:6)");
    exit(1);
  }
  orig_digest_fp = fdopen(5, "rb");
  if (orig_digest_fp != NULL) {
    if (fread(&blocksize, sizeof(blocksize), 1, orig_digest_fp) != 1) {
      perror("failed to read digest data (fd:5)");
      exit(2);
    }
  }
  
  /* write block size */
  if (fwrite(&blocksize, sizeof(blocksize), 1, stdout) != 1) {
    perror("failed to write patch data (fd:1)");
    exit(2);
  }
  if (fwrite(&blocksize, sizeof(blocksize), 1, new_digest_fp) != 1) {
    perror("failed to write digest data (fd:6)");
    exit(2);
  }
  
  if ((block = malloc(blocksize)) == NULL) {
    perror("no memory");
    exit(2);
  }

  for (offset = 0;
       (rlen = fread(block, 1, blocksize, stdin)) != 0;
       offset += rlen) {
    MD5(block, rlen, digest);
    if (orig_digest_fp != NULL
	&& fread(orig_digest, 1, sizeof(orig_digest), orig_digest_fp)
	== sizeof(orig_digest)
	&& memcmp(digest, orig_digest, sizeof(digest)) == 0) {
      /* no need to dump data */
    } else {
      /* dump data */
      if (fwrite(&offset, 1, sizeof(offset), stdout) != sizeof(offset)
	  || fwrite(block, 1, rlen, stdout) != rlen) {
	perror("failed to write patch data (fd:6)");
	exit(2);
      }
    }
    /* write digest */
    if (fwrite(digest, 1, sizeof(digest), new_digest_fp) != sizeof(digest)) {
      perror("failed to write digest data (fd:1)");
      exit(2);
    }
  }
  
  if (orig_digest_fp != NULL)
    fclose(orig_digest_fp);
  fclose(new_digest_fp);
  
  return 0;
}
