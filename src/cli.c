#include "cli.h"
#include "config.h"
#include "default_config.h"
#include "extensions/string.h"
#include "log.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wlr/util/log.h>

static const char* catnip_version = "0.1.0";

static const char* catnip_short_options = "hvc:ld";

static const struct option catnip_long_options[] = {
  {"help", no_argument, NULL, 'h'},
  {"version", no_argument, NULL, 'v'},
  {"config", required_argument, NULL, 'c'},
  {"log", required_argument, NULL, 'l'},
  {"default", no_argument, NULL, 'd'},
  {0, 0, 0, 0}
};

static const char* catnip_help =
  "usage: catnip [OPTION]\n"
  "  -c, --config FILE  specify lua config path\n"
  "  -l, --log LEVEL    specify log level, must be one of:\n"
  "                     silent, error, warning, info, debug \n"
  "  -d, --default      print default user config and exit\n"
  "  -v, --version      print help and exit\n"
  "  -h, --help         print version and exit\n";

static char*
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

static int
catnip_cli_getopt(int argc, char* argv[])
{
  return getopt_long(
    argc,
    argv,
    catnip_short_options,
    catnip_long_options,
    NULL
  );
}

void
catnip_cli_init(int argc, char* argv[])
{
  int opt;

  while ((opt = catnip_cli_getopt(argc, argv)) != -1) {
    switch (opt) {
      case 'h':
        printf("%s", catnip_help);
        exit(0);
      case 'v':
        printf("%s\n", catnip_version);
        exit(0);
      case 'c':
        // Make sure to get the absolute path here, since we change the
        // working directory when loading the user config.
        catnip_config_user_path = abspath(optarg);
        break;
      case 'l': {
        if (streq(optarg, "silent")) {
          catnip_log_level = CATNIP_LOG_LEVEL_SILENT;
          wlr_log_init(WLR_SILENT, NULL);
        } else if (streq(optarg, "error")) {
          catnip_log_level = CATNIP_LOG_LEVEL_ERROR;
          wlr_log_init(WLR_ERROR, NULL);
        } else if (streq(optarg, "warning")) {
          catnip_log_level = CATNIP_LOG_LEVEL_WARNING;
          wlr_log_init(WLR_ERROR, NULL);
        } else if (streq(optarg, "info")) {
          catnip_log_level = CATNIP_LOG_LEVEL_INFO;
          wlr_log_init(WLR_INFO, NULL);
        } else if (streq(optarg, "debug")) {
          catnip_log_level = CATNIP_LOG_LEVEL_DEBUG;
          wlr_log_init(WLR_DEBUG, NULL);
        } else {
          catnip_log_warning("unknown log level: %s", optarg);
        }
      }; break;
      case 'd':
        printf("%s", catnip_default_config);
        exit(0);
      default:
        printf("%s", catnip_help);
        exit(1);
    }
  }
}
