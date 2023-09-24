#ifndef CATNIP_CONFIG_H
#define CATNIP_CONFIG_H

#include <wlr/util/log.h>

#ifdef DEV_MODE
  #define ROOT_DIR "."
  #define WLR_LOG_LEVEL WLR_DEBUG
#else
  #define ROOT_DIR "/usr/local"
  #define WLR_LOG_LEVEL WLR_ERROR
#endif

#endif