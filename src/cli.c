#include "cli.h"
#include "config.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

static const char* catnip_version = "0.1.0";

static const char* catnip_short_options = "hvc:";

static const struct option catnip_long_options[] = {
  {"help", no_argument, NULL, 'h'},
  {"version", no_argument, NULL, 'v'},
  {"config", required_argument, NULL, 'c'},
  {0, 0, 0, 0}
};

static const char* catnip_help =
  "usage: catnip [OPTION]\n"
  "  -c, --config FILE  specify lua config path\n"
  "  -v, --version      print help and exit\n"
  "  -h, --help         print version and exit\n";

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
        catnip_config_path = optarg;
        break;
      default:
        printf("%s", catnip_help);
        exit(1);
    }
  }
}
