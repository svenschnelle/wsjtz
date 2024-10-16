#pragma once

#ifndef VERSION_INFO_message_aggregator_H__
#define VERSION_INFO_message_aggregator_H__

#include "scs_version.h"

#ifndef PRODUCT_VERSION_MAJOR
#define PRODUCT_VERSION_MAJOR 2
#endif

#ifndef PRODUCT_VERSION_MINOR
#define PRODUCT_VERSION_MINOR 7
#endif

#ifndef PRODUCT_VERSION_PATCH
#define PRODUCT_VERSION_PATCH 0
#endif

#ifndef PRODUCT_VERSION_TWEAK
#define PRODUCT_VERSION_TWEAK 0
#endif

#ifndef PRODUCT_VERSION_REVISION
#define PRODUCT_VERSION_REVISION -rc2
#endif

#ifndef FILE_VERSION_MAJOR
#define FILE_VERSION_MAJOR 2
#endif

#ifndef FILE_VERSION_MINOR
#define FILE_VERSION_MINOR 7
#endif

#ifndef FILE_VERSION_PATCH
#define FILE_VERSION_PATCH 0
#endif

#ifndef FILE_VERSION_TWEAK
#define FILE_VERSION_TWEAK 0
#endif

#ifndef TO_STRING__
#define TO_STRING_IMPL__(x) #x
#define TO_STRING__(x) TO_STRING_IMPL__(x)
#endif

#define PRODUCT_VERSION_MAJOR_MINOR_STR        TO_STRING__ (PRODUCT_VERSION_MAJOR) "." TO_STRING__(PRODUCT_VERSION_MINOR)
#define PRODUCT_VERSION_MAJOR_MINOR_PATCH_STR  PRODUCT_VERSION_MAJOR_MINOR_STR "." TO_STRING__ (PRODUCT_VERSION_PATCH)
#define PRODUCT_VERSION_FULL_STR               PRODUCT_VERSION_MAJOR_MINOR_PATCH_STR TO_STRING__ (PRODUCT_VERSION_REVISION)
#define PRODUCT_VERSION_RESOURCE               PRODUCT_VERSION_MAJOR,PRODUCT_VERSION_MINOR,PRODUCT_VERSION_PATCH,PRODUCT_VERSION_TWEAK
#define PRODUCT_VERSION_RESOURCE_STR           PRODUCT_VERSION_FULL_STR " " SCS_VERSION_STR "\0"

#define FILE_VERSION_MAJOR_MINOR_STR        TO_STRING__ (FILE_VERSION_MAJOR) "." TO_STRING__ (FILE_VERSION_MINOR)
#define FILE_VERSION_MAJOR_MINOR_PATCH_STR  FILE_VERSION_MAJOR_MINOR_STR "." TO_STRING__ (FILE_VERSION_PATCH)
#define FILE_VERSION_FULL_STR               FILE_VERSION_MAJOR_MINOR_PATCH_STR TO_STRING__ (PRODUCT_VERSION_REVISION)
#define FILE_VERSION_RESOURCE               FILE_VERSION_MAJOR,FILE_VERSION_MINOR,FILE_VERSION_PATCH,FILE_VERSION_TWEAK
#define FILE_VERSION_RESOURCE_STR           FILE_VERSION_FULL_STR "\0"

#ifndef PRODUCT_ICON
#define PRODUCT_ICON "icons/windows-icons/wsjtx.ico"
#endif

#ifndef PRODUCT_COMMENTS
#define PRODUCT_COMMENTS           "WSJT-X: Digital Modes for Weak Signal Communications in Amateur Radio\0"
#endif

#ifndef PRODUCT_VENDOR_NAME
#define PRODUCT_VENDOR_NAME        "Joe Taylor, K1JT\0"
#endif

#ifndef PRODUCT_LEGAL_COPYRIGHT
#define PRODUCT_LEGAL_COPYRIGHT    "Copyright (C) 2001-2023 by Joe Taylor, K1JT\0"
#endif

#ifndef PRODUCT_FILE_DESCRIPTION
#define PRODUCT_FILE_DESCRIPTION   "Example WSJT-X UDP Message Protocol application\0"
#endif

#ifndef PRODUCT_INTERNAL_NAME
#define PRODUCT_INTERNAL_NAME      "message_aggregator\0"
#endif

#ifndef PRODUCT_ORIGINAL_FILENAME
#define PRODUCT_ORIGINAL_FILENAME  "message_aggregator\0"
#endif

#ifndef PRODUCT_BUNDLE
#define PRODUCT_BUNDLE             "WSJT-X\0"
#endif

#endif

