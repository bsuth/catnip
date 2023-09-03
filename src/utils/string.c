#include "string.h"
#include <stdarg.h>
#include <stdlib.h>

// TODO use glib?

char*
string_concat(const char* s1, const char* s2)
{
  int s1_len = strlen(s1);
  int s2_len = strlen(s2);

  int concatenated_len = s1_len + s2_len + 1;
  char* concatenated = malloc(concatenated_len * sizeof(char));

  strcpy(concatenated, s1);
  strcpy(&concatenated[s1_len], s2);
  concatenated[concatenated_len] = '\0';

  return concatenated;
}
