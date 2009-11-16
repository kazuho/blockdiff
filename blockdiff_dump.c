#include <stdio.h>
#include <string.h>
#include <openssl/crypto.h>
#include <openssl/md5.h>

#define BLOCK_SIZE (16384)

/*
 * file descriptors:
 *   0 -> data file
 *   1 <- patch data
 *   5 -> old digest (or none)
 *   6 <- new digest
 */

int main(int argc, char **argv)
{
  FILE *orig_digest_fp, *new_digest_fp;
  unsigned char block[BLOCK_SIZE], digest[MD5_DIGEST_LENGTH],
    orig_digest[MD5_DIGEST_LENGTH];
  size_t block_size;
  long long offset;
  
  orig_digest_fp = fdopen(5, "rb");
  new_digest_fp = fdopen(6, "wb");
  
  for (offset = 0;
       (block_size = fread(block, 1, sizeof(block), stdin)) != 0;
       offset += block_size) {
    MD5(block, block_size, digest);
    if (orig_digest_fp != NULL
	&& fread(orig_digest, 1, sizeof(orig_digest), orig_digest_fp)
	== sizeof(orig_digest)
	&& memcmp(digest, orig_digest, sizeof(digest)) == 0) {
      /* no need to dump data */
    } else {
      /* dump data */
      if (fwrite(&offset, 1, sizeof(offset), stdout) != sizeof(offset)
	  || fwrite(block, 1, block_size, stdout) != block_size) {
	perror("failed to write patch data");
	exit(2);
      }
    }
    /* write digest */
    if (new_digest_fp != NULL
	&& fwrite(digest, 1, sizeof(digest), new_digest_fp) != sizeof(digest)) {
      perror("failed to write digest data");
      exit(2);
    }
  }
  
  if (orig_digest_fp != NULL)
    fclose(orig_digest_fp);
  if (new_digest_fp != NULL)
    fclose(new_digest_fp);
  
  return 0;
}
