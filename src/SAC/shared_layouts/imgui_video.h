#ifndef SAC_SHARED_LAYOUTS_IMGUI_VIDEO_H_
#define SAC_SHARED_LAYOUTS_IMGUI_VIDEO_H_

#include <common.h>

// TODO: clean up types (remove unused)
typedef struct app_state AppState;
typedef struct document_s Document;

typedef enum {
  IMGUI_ELEMENT_E_NONE = 0,
  IMGUI_ELEMENT_E_TAB_BAR = 1 << 0,
  IMGUI_ELEMENT_E_TAB_ITEM = 1 << 1,
} IMGUI_ELEMENT_E_;

typedef enum {
  DOCUMENT_NODE_DIRECTION_E_NEXT,
  DOCUMENT_NODE_DIRECTION_E_CHILD,
} DOCUMENT_NODE_DIRECTION_E_;

// TODO: add struct for document data instead of polluting node
// or add node structure with the Document struct as its data member
typedef struct document_s {
  Sac_String title;
  Sac_String contents;
  IMGUI_ELEMENT_E_ element;
  bool is_open;
  Document *next;
  Document *child;
} Document;

typedef struct {
  Document *documents;
  uint32_t length;
  uint32_t max_length;
} DocumentArray;

typedef struct {
  int32_t selected_document_index;
  DocumentArray documents;
  Document *root_node; // for linked-list parsing instead of array
  Document *status_node;
  int allocation_failure;
} ImGuiVideo_Data;

typedef struct {
  int32_t requested_document_index;
  int32_t *selected_document_index;
} SidebarClickData;

Document *ImGuiVideo_CreateDocNode(Document *doc);
int ImGuiVideo_AddDocument(DocumentArray *doc_array, Document document);
int ImGuiVideo_AddDocNode(Document *current_node, Document *linked_node, DOCUMENT_NODE_DIRECTION_E_ direction);
ImGuiVideo_Data ImGuiVideo_Initialize();
void ImGuiVideo_UpdateData(Document *status_node, AppState *state);
void ImGuiVideo_UpdateStatusData(Document *status_node, AppState *state);
void ImGuiVideo_RenderDocuments(ImGuiVideo_Data *data);
// WARN: broken function - keeping for now for legacy's sake
void ImGuiVideo_RenderDocArray(ImGuiVideo_Data *data);
// NOTE: Consider pushing to an array vs pushing to the stack
void ImGuiVideo_RenderNodes(Document *node);
void ImGuiVideo_RenderNode(Document *node);
void ImGuiVideo_RenderTabBar(Document *node);
void ImGuiVideo_RenderTabItem(Document *node);
void ImGuiVideo_ShowDemo(AppState *state);
void ImGuiVideo_SampleWindow1(AppState *state);
void ImGuiVideo_SampleWindow2(AppState *state);

#endif
