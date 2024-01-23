#include "cli.h"
#include "config.h"
#include "utils/log.h"
#include <getopt.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wlr/util/log.h>

static const char* catnip_version = "0.1.0";

static const char* catnip_short_options = "hvc:l:";

static const struct option catnip_long_options[] = {
  {"help", no_argument, NULL, 'h'},
  {"version", no_argument, NULL, 'v'},
  {"config", required_argument, NULL, 'c'},
  {"loglevel", required_argument, NULL, 'l'},
  {0, 0, 0, 0}
};

static const char* catnip_help =
  "usage: catnip [OPTION]\n"
  "  -c, --config FILE     specify lua config path\n"
  "  -l, --loglevel LEVEL  specify log level, must be one of:\n"
  "                        silent, error, info, debug \n"
  "  -v, --version         print help and exit\n"
  "  -h, --help            print version and exit\n";

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
        catnip_config_path = strdup(optarg);
        break;
      case 'l': {
        if (g_str_equal(optarg, "silent")) {
          wlr_log_init(WLR_SILENT, NULL);
        } else if (g_str_equal(optarg, "error")) {
          wlr_log_init(WLR_ERROR, NULL);
        } else if (g_str_equal(optarg, "info")) {
          wlr_log_init(WLR_INFO, NULL);
        } else if (g_str_equal(optarg, "debug")) {
          wlr_log_init(WLR_DEBUG, NULL);
        } else {
          log_warning("invalid option '--loglevel %s'", optarg);
        }
      }; break;
      default:
        printf("%s", catnip_help);
        exit(1);
    }
  }
}
