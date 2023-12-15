#ifndef CATNIP_META_H
#define CATNIP_META_H

#include <wlr/util/log.h>

// Default for dev purposes.
// For real builds this should be set by the Makefile.
#if !defined(CATNIP_INSTALL_DIR)
  #define CATNIP_INSTALL_DIR "."
#endif

// Allow setting the WLR_LOG_LEVEL from Makefile using sane flags.
#if defined(WLR_LOG_SILENT)
  #define WLR_LOG_LEVEL WLR_SILENT
#elif defined(WLR_LOG_ERROR)
  #define WLR_LOG_LEVEL WLR_ERROR
#elif defined(WLR_LOG_INFO)
  #define WLR_LOG_LEVEL WLR_INFO
#elif defined(WLR_LOG_DEBUG)
  #define WLR_LOG_LEVEL WLR_DEBUG
#else
  #define WLR_LOG_LEVEL WLR_ERROR
#endif

#endif
