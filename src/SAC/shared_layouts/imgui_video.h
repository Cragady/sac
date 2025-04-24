#ifndef SAC_SHARED_LAYOUTS_IMGUI_VIDEO_H_
#define SAC_SHARED_LAYOUTS_IMGUI_VIDEO_H_

#include <common.h>

// TODO: clean up types (remove unused)
typedef struct app_state AppState;
typedef struct document_s Document;

// TODO: rename enum and enum members to include the concept of components
typedef enum {
  IMGUI_ELEMENT_E_NONE = 0,
  IMGUI_ELEMENT_E_TAB_BAR,
  IMGUI_ELEMENT_E_TAB_ITEM,
  IMGUI_ELEMENT_E_TEXT,
  // Component Deferral
  IMGUI_ELEMENT_E_COMPONENT,
} IMGUI_ELEMENT_E_;

typedef enum {
  IMGUI_COMPONENT_E_NONE = 0,
  IMGUI_COMPONENT_E_SETTINGS_TAB_CONTENT,
} IMGUI_COMPONENT_E_;

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
  IMGUI_COMPONENT_E_ component;
  bool is_open;
  bool is_heap;
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
  Document *root_node; // for linked-list parsing instead of array
  Document *status_node;
  int allocation_failure;
} ImGuiVideo_Data;

typedef struct {
  int32_t requested_document_index;
  int32_t *selected_document_index;
} SidebarClickData;

/* Init */
ImGuiVideo_Data ImGuiVideo_Initialize();

/* Doc & Node Creation */
Document *ImGuiVideo_CreateDocNode(Document *doc);
// Returns true on success
bool ImGuiVideo_AddDocument(DocumentArray *doc_array, Document document);
// Returns true on success
bool ImGuiVideo_AddDocNode(Document *current_node, Document *linked_node, DOCUMENT_NODE_DIRECTION_E_ direction);
// Returns true on success
bool ImGuiVideo_AddDocTextNode(Document *current_node, Sac_String text);

/* Doc & Node Deletion */
void ImGuiVideo_DeleteDocNode(Document *doc);
void ImGuiVideo_DeleteDocNodeTree(Document *doc);

/* Lifetime */
void ImGuiVideo_UpdateData(Document *status_node, AppState *state);
void ImGuiVideo_UpdateStatusData(Document *status_node, AppState *state);

/* Doc Rendering */
void ImGuiVideo_RenderDocuments(ImGuiVideo_Data *data, AppState *state);

/* Node Rendering */
// NOTE: Consider pushing to an array vs pushing to the stack
void ImGuiVideo_RenderNodes(Document *node, AppState *state);
void ImGuiVideo_RenderNode(Document *node, AppState *state);
void ImGuiVideo_RenderNodeComponent(Document *node, AppState *state);
void ImGuiVideo_RenderNodeChildren(Document *node, AppState *state);

/* Elements Rendering */
void ImGuiVideo_RenderTabBar(Document *node, AppState *state);
void ImGuiVideo_RenderTabItem(Document *node, AppState *state);
void ImGuiVideo_RenderText(Document *node, AppState *state);
// NOTE: below may not be possible with current data structures
// void ImGuiVideo_RenderCheckbox(Document *node, bool *flag);

/* Component Rendering */
void ImGuiVideo_SettingsTabContent(Document *node, AppState *state);
// global_imgui_video_show_sample_window

/* Samples/Examples */
void ImGuiVideo_ShowDemo(AppState *state);
void ImGuiVideo_SampleWindow1(AppState *state);
void ImGuiVideo_SampleWindow2(AppState *state);

/* Util */
// NOTE: This was lifted from imgui
static void ImGuiVideo_HelpMarker(const char* desc);

#endif
