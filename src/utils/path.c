#include "path.h"
#include "utils/string.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char*
abspath(const char* path)
{
  if (path == NULL || strlen(path) == 0) {
    return getcwd(NULL, 0);
  } else if (path[0] == '/') {
    return strdup(path);
  }

  char* cwd = getcwd(NULL, 0);
  char* abspath = strfmt("%s/%s", cwd, path);
  free(cwd);

  return abspath;
}
