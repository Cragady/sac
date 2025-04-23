#ifndef SAC_SHARED_LAYOUTS_IMGUI_VIDEO_H_
#define SAC_SHARED_LAYOUTS_IMGUI_VIDEO_H_

#include <common.h>

typedef struct app_state AppState;

typedef struct {
  Sac_String title;
  Sac_String contents;
} Document;

typedef struct {
  Document *documents;
  uint32_t length;
  uint32_t max_length;
} DocumentArray;

typedef struct {
  int32_t selected_document_index;
  float y_offset;
  DocumentArray documents;
  int allocation_failure;
} ImGuiVideo_Data;

typedef struct {
  int32_t requested_document_index;
  int32_t *selected_document_index;
} SidebarClickData;


#endif
