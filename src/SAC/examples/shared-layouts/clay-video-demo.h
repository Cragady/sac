#ifndef SAC_EXAMPLES_SHARED_LAYOUTS_CLAY_VIDEO_DEMO_H
#define SAC_EXAMPLES_SHARED_LAYOUTS_CLAY_VIDEO_DEMO_H

#include <clay.h>

typedef struct {
  Clay_String title;
  Clay_String contents;
} Document;

typedef struct {
  Document *documents;
  uint32_t length;
} DocumentArray;

typedef struct {
  intptr_t offset;
  intptr_t memory;
} ClayVideoDemo_Arena;

typedef struct {
  int32_t selectedDocumentIndex;
  float yOffset;
  ClayVideoDemo_Arena frameArena;
} ClayVideoDemo_Data;

typedef struct {
  int32_t requestedDocumentIndex;
  int32_t *selectedDocumentIndex;
} SidebarClickData;

#endif
