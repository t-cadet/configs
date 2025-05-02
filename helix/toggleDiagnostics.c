#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  char* HOME = getenv("HOME");
  if (!HOME) {
    fprintf(stderr, "getenv");
    exit(1);
  }

  char fPath[1024] = {0};
  if (snprintf(fPath, 1024, "%s%s", HOME, "/.config/helix/languages.toml") < 0) {
    perror("snprintf");
    exit(1);
  }

  FILE* f = fopen(fPath, "r");
  if (!f) {
    perror("fopen");
    exit(1);
  }

  if (fseek(f, 0, SEEK_END) == -1) {
    perror("fseek");
    exit(1);
  }

  long fSize = ftell(f);
  if (fSize == -1) {
    perror("ftell");
    exit(1);
  }

  rewind(f);

  char* fContent = calloc(fSize + 1, 1);
  if (fread(fContent, 1, fSize, f) != fSize) {
    perror("fread");
    exit(1);
  }
  if (fclose(f) != 0) {
    perror("fclose");
  }

  char* out = calloc(fSize * 2 + 1, 1);

  char* needle = "except-features";
  size_t needleSize = strlen(needle);

  size_t outIndex = 0;
  size_t needleIndex = 0;
  size_t lineStartIndex = 0;
  bool shouldToggleLineComment = false;

  for (size_t i = 0; i < fSize; ++i) {
    char c = fContent[i];
    if (c == needle[needleIndex]) {
      ++needleIndex;
      if (needleIndex == needleSize) {
        shouldToggleLineComment = true;
        needleIndex = 0;
      }
    } else if (c == '\n') {
      if (shouldToggleLineComment) {
        assert(lineStartIndex < fSize);
        if (fContent[lineStartIndex] == '#') {
          ++lineStartIndex;
          assert(lineStartIndex < fSize);
          if (fContent[lineStartIndex] == ' ') {
            ++lineStartIndex;
          }
        } else {
          out[outIndex++] = '#';
          out[outIndex++] = ' ';
        }
      }

      size_t lineSize = i + 1 - lineStartIndex;
      memcpy(out + outIndex, fContent + lineStartIndex, lineSize);

      needleIndex = 0;
      shouldToggleLineComment = false;
      lineStartIndex = i + 1;
      outIndex += lineSize;
    }
  }

  assert(outIndex <= fSize * 2);

  // printf("%s", out);

  f = fopen(fPath, "w");
  if (!f) {
    perror("fopen");
    exit(1);
  }

  if (fwrite(out, 1, outIndex, f) != outIndex) {
    perror("fwrite");
    exit(1);
  }

  if (fclose(f) != 0) {
    perror("fclose");
  }

  free(fContent);
  free(out);
}
