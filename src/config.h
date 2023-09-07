#ifndef CONFIG_H
#define CONFIG_H

#include <wlr/util/log.h>

#ifdef DEV_MODE
#define ROOT_DIR "."
#define WLR_LOG_LEVEL WLR_DEBUG
#else
#define ROOT_DIR "/usr/local"
#define WLR_LOG_LEVEL WLR_ERROR
#endif

#endif
