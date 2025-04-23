#ifndef SAC_SHARED_LAYOUTS_IMGUI_VIDEO_H_
#define SAC_SHARED_LAYOUTS_IMGUI_VIDEO_H_

#include <common.h>

typedef struct app_state AppState;

typedef enum {
  IMGUI_ELEMENT_E_NONE = 0,
  IMGUI_ELEMENT_E_TAB,
} IMGUI_ELEMENT_E_;

typedef struct {
  Sac_String title;
  Sac_String contents;
  IMGUI_ELEMENT_E_ element;
  bool is_open;
} Document;

typedef struct {
  Document *documents;
  uint32_t length;
  uint32_t max_length;
} DocumentArray;

typedef struct {
  int32_t selected_document_index;
  DocumentArray documents;
  int allocation_failure;
} ImGuiVideo_Data;

typedef struct {
  int32_t requested_document_index;
  int32_t *selected_document_index;
} SidebarClickData;

int ImGuiVideo_AddDocument(DocumentArray *doc_array, Document document);
ImGuiVideo_Data ImGuiVideo_Initialize();
void ImGuiVideo_UpdateData(DocumentArray *doc_array, AppState *state);
void ImGuiVideo_UpdateStatusData(DocumentArray *doc_array, AppState *state);
void ImGuiVideo_RenderDocuments(ImGuiVideo_Data *data);
void ImGuiVideo_ShowDemo(AppState *state);
void ImGuiVideo_SampleWindow1(AppState *state);
void ImGuiVideo_SampleWindow2(AppState *state);

#endif
