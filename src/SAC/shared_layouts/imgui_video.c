#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <common.h>

#include "SAC/sac.h"
#include "SAC/shared_layouts/imgui_video.h"

#define SAC_SHARED_LAYOUTS_IMGUI_VIDEO_C_FONT_ID_BODY_16 0
#define SAC_SHARED_LAYOUTS_IMGUI_VIDEO_C_COLOR_WHITE { 255, 255, 255, 255}
#define SAC_SHARED_LAYOUTS_IMGUI_VIDEO_C_DOCS_LEN 4
#define SAC_SHARED_LAYOUTS_IMGUI_VIDEO_C_STATUS_STR_BUFF 256
#define SAC_SHARED_LAYOUTS_IMGUI_VIDEO_C_STATUS_STR_ERR "FAILED STR ALLOC IN IMGUI_STRING"
#define SAC_SHARED_LAYOUTS_IMGUI_VIDEO_C_STATUS_STR_ERR_BUFF 31

ImGuiVideo_Data ImGuiVideo_Initialize() {
  const size_t DOC_LEN = SAC_SHARED_LAYOUTS_IMGUI_VIDEO_C_DOCS_LEN;
  ImGuiVideo_Data data = {
    .root_node = malloc(sizeof(Document)),
    .status_node = malloc(sizeof(Document)),
    .allocation_failure = false,
  };

  if (data.root_node == NULL || data.status_node == NULL) {
    if (data.root_node) free(data.root_node);
    if (data.status_node) free(data.status_node);
    return data;
  }

  Sac_String status_str = {
    .is_statically_allocated = false,
    .length = SAC_SHARED_LAYOUTS_IMGUI_VIDEO_C_STATUS_STR_BUFF,
  };
  status_str.chars = malloc(sizeof(char) * status_str.length);
  if (status_str.chars == NULL) {
    status_str = SAC_STRING(SAC_SHARED_LAYOUTS_IMGUI_VIDEO_C_STATUS_STR_ERR);
  } else {
    memset((char *)status_str.chars, '\0', status_str.length);
  }

  Document *root_node = ImGuiVideo_CreateDocNode(&(Document){ .is_heap = false, .title = SAC_STRING("ControlTabBar"), .contents = SAC_STRING(""), .element = IMGUI_ELEMENT_E_TAB_BAR, .is_open = true, });

  // Setup the status/info node and its child
  Document *info_node = ImGuiVideo_CreateDocNode(&(Document){ .is_heap = false, .title = SAC_STRING("Status"), .contents = SAC_STRING(""), .element = IMGUI_ELEMENT_E_TAB_ITEM, .is_open = true, });
  ImGuiVideo_AddDocTextNode(info_node, status_str);
  Document *status_node = info_node->child;

  // Setup the settings node
  Document *settings_node = ImGuiVideo_CreateDocNode(&(Document){ .is_heap = false, .title = SAC_STRING("Settings - WIP"), .contents = SAC_STRING(""), .element = IMGUI_ELEMENT_E_TAB_ITEM, .is_open = true, });
  ImGuiVideo_AddDocTextNode(settings_node, SAC_STRING("WIP"));

  // Setup the script node
  Document *script_node = ImGuiVideo_CreateDocNode(&(Document){ .is_heap = false, .title = SAC_STRING("Script - WIP"), .contents = SAC_STRING(""), .element = IMGUI_ELEMENT_E_TAB_ITEM, .is_open = true, });
  ImGuiVideo_AddDocTextNode(script_node, SAC_STRING("WIP"));

  // NOTE: add last siblings first (reverse order), then add that node as a child to parent
  // NOTE: allocation failures can be handled at this level; we don't need to worry
  // too much about `ImGuiVideo_AddDocTextNode` failures.
  bool success = true;
  bool delete_info_node = false;
  success = ImGuiVideo_AddDocNode(settings_node, script_node, DOCUMENT_NODE_DIRECTION_E_NEXT);
  if (!success) {
    printf("FAILURE: add doc node, script_node to settings_node\n");
    ImGuiVideo_DeleteDocNode(script_node);
    settings_node->next = NULL;
  }

  success = ImGuiVideo_AddDocNode(info_node, settings_node, DOCUMENT_NODE_DIRECTION_E_NEXT);
  if (!success){
    printf("FAILURE: add doc node, settings_node to info_node\n");
    ImGuiVideo_DeleteDocNode(settings_node);
    info_node->next = NULL;
  }

  success = ImGuiVideo_AddDocNode(root_node, info_node, DOCUMENT_NODE_DIRECTION_E_CHILD);
  if (!success) {
    printf("FAILURE: add doc node, info_node to root_node\n");
    delete_info_node = true;
    root_node->child = NULL;
  }

  *data.root_node = *root_node;
  data.root_node->is_heap = true;

  if (delete_info_node) {
    printf("FAILURE STATUS: cleaning up status_node\n");
    // NOTE: since status_node is part of the node tree, we let the tree clean
    // itself up.
    ImGuiVideo_DeleteDocNode(info_node);
    data.status_node = NULL;
    data.root_node->child = NULL;
  } else {
    *data.status_node = *status_node;
  }

  printf("SETUP: ImGuiVideo_Initialize() Finished\n");
  return data;
}

Document *ImGuiVideo_CreateDocNode(Document *doc) {
  doc->next = NULL;
  doc->child = NULL;
  return doc;
}

bool ImGuiVideo_AddDocument(DocumentArray *doc_array, Document document) {
  if (doc_array->length == doc_array->max_length) return false;
  doc_array->documents[doc_array->length] = document;
  doc_array->length++;
  return true;
}

bool ImGuiVideo_AddDocNode(Document *current_node, Document *linked_node, DOCUMENT_NODE_DIRECTION_E_ direction) {

  Document *new_node = malloc(sizeof(Document));
  if (new_node == NULL) {
    printf("Error in allocating next!");
    return false;
  }

  *new_node = *linked_node;
  new_node->is_heap = true;

  switch (direction) {
    case DOCUMENT_NODE_DIRECTION_E_NEXT:
      current_node->next = new_node;
      break;
    case DOCUMENT_NODE_DIRECTION_E_CHILD:
      current_node->child = new_node;
      break;
  }

  return true;
}

bool ImGuiVideo_AddDocTextNode(Document *current_node, Sac_String text) {
  Document text_doc = {
    .title = SAC_STRING(""),
    .contents = text,
    .element = IMGUI_ELEMENT_E_TEXT,
    .is_open = true,
    .is_heap = false,
    .next = NULL,
    .child = NULL,
  };

  return ImGuiVideo_AddDocNode(current_node, &text_doc, DOCUMENT_NODE_DIRECTION_E_CHILD);
}

void ImGuiVideo_DeleteDocNode(Document *doc) {
  while (doc) {
    if (doc->child) {
      ImGuiVideo_DeleteDocNode(doc->child);
    }
    free_sac_string(&doc->title);
    free_sac_string(&doc->contents);
    Document *next = doc->next;
    if (doc->is_heap) {
      free(doc);
    } else {
      doc = NULL;
    }
    doc = next;
  }
}

void ImGuiVideo_UpdateData(Document *status_node, AppState *state) {
  if (status_node) ImGuiVideo_UpdateStatusData(status_node, state);
}

// TODO: review logic in this function
void ImGuiVideo_UpdateStatusData(Document *status_node, AppState *state) {
  const int STR_BUFF = SAC_SHARED_LAYOUTS_IMGUI_VIDEO_C_STATUS_STR_BUFF;
  const char *ERR_MSG = SAC_SHARED_LAYOUTS_IMGUI_VIDEO_C_STATUS_STR_ERR;
  const int ERR_MSG_BUFF = SAC_SHARED_LAYOUTS_IMGUI_VIDEO_C_STATUS_STR_ERR_BUFF;
  const char *format_str = "Mouse Pos: (%i, %i)\n"
                           "Is Clicking: %s\n";

  const Document *WANTED_DOC = status_node;
  const char *WANTED_STR = WANTED_DOC->contents.chars;

  if (!memcmp(WANTED_STR, ERR_MSG, ERR_MSG_BUFF))
    return;

  const int MOUSE_POS_X = state->mouse_info.x;
  const int MOUSE_POS_Y = state->mouse_info.y;

  const char *clicking_t_f = state->should_click
    ? "True"
    : "False";

  // NOTE: I don't know if the following memset will fix the garbage issue I was
  // having; shit was cropping up in the strings.
  // returns num chars if n was sufficiently large enough
  int chars_changed = snprintf((char *)WANTED_STR, STR_BUFF, format_str, MOUSE_POS_X, MOUSE_POS_Y, clicking_t_f);
  int offset = chars_changed <= STR_BUFF
    ? chars_changed
    : STR_BUFF;

  memset((char *)WANTED_STR + offset, '\0', STR_BUFF - offset);
}

void ImGuiVideo_RenderDocuments(ImGuiVideo_Data *data) {
  igBegin("Hello, ImGuiVideo!", NULL, 0);

  Document *current_node = data->root_node;
  ImGuiVideo_RenderNodes(current_node);


  igEnd();

}

// NOTE: Consider pushing to an array vs pushing to the stack
void ImGuiVideo_RenderNodes(Document *node) {

  while (node) {
    // Let element handle rendering children by calling `ImGuiVideo_RenderNodes()`
    ImGuiVideo_RenderNode(node);
    node = node->next;
  }
}

void ImGuiVideo_RenderNode(Document *node) {
  switch(node->element) {
    case IMGUI_ELEMENT_E_NONE:
      break;
    case IMGUI_ELEMENT_E_TAB_BAR:
      ImGuiVideo_RenderTabBar(node);
      break;
    case IMGUI_ELEMENT_E_TAB_ITEM:
      ImGuiVideo_RenderTabItem(node);
      break;
    case IMGUI_ELEMENT_E_TEXT:
      ImGuiVideo_RenderText(node);
      break;
  }
}

void ImGuiVideo_RenderNodeChildren(Document *node) {
  if (node->child) {
    ImGuiVideo_RenderNodes(node->child);
  }
}

void ImGuiVideo_RenderTabBar(Document *node) {
  ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
  if(igBeginTabBar(node->title.chars, tab_bar_flags)) {
    ImGuiVideo_RenderNodeChildren(node);
    igEndTabBar();
  };
}

void ImGuiVideo_RenderTabItem(Document *node) {
  ImGuiTabItemFlags tab_item_flags = ImGuiTabItemFlags_None;
  if (igBeginTabItem(node->title.chars, NULL, tab_item_flags)) {
    ImGuiVideo_RenderNodeChildren(node);
    igEndTabItem();
  }
}

void ImGuiVideo_RenderText(Document *node) {
  igText(node->contents.chars);
}

void ImGuiVideo_ShowDemo(AppState *state) {
  // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
  if (state->show_demo)
    igShowDemoWindow(&state->show_demo);
}

void ImGuiVideo_SampleWindow1(AppState *state) {
  // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
  static float f = 0.0f;
  static int counter = 0;

  igBegin("Hello, world!", NULL, 0);
  igText("This is some useful text");
  igCheckbox("Demo window", &state->show_demo);
  igCheckbox("Another window", &state->show_another_window);

  igSliderFloat("Float", &f, 0.0f, 1.0f, "%.3f", 0);
  igColorEdit3("clear color", (float*)&state->clear_color, 0);

  ImVec2 buttonSize;
  buttonSize.x = 0;
  buttonSize.y = 0;
  if (igButton("Button", buttonSize))
    counter++;
  igSameLine(0.0f, -1.0f);
  igText("counter = %d", counter);

  igText("Application average %.3f ms/frame (%.1f FPS)",
         1000.0f / igGetIO()->Framerate, igGetIO()->Framerate);
  igEnd();
}

void ImGuiVideo_SampleWindow2(AppState *state) {
  // 3. Show another simple window.
  if (state->show_another_window) {
    igBegin("imgui Another Window", &state->show_another_window, 0);
    igText("Hello from imgui");
    ImVec2 buttonSize;
    buttonSize.x = 0; buttonSize.y = 0;
    if (igButton("Close me", buttonSize)) {
      state->show_another_window = false;
    }
    igEnd();
  }
}

// WARN: broken function - keeping for now for legacy's sake
void ImGuiVideo_RenderDocArray(ImGuiVideo_OldData *data) {
  bool tabs_active = false;
  size_t next_tab_item = -1;

  for (size_t i = 0; i < data->documents.length; i++) {
    Document *current_item = &data->documents.documents[i];
    Document *next_item = i + 1 < data->documents.length
      ? &data->documents.documents[i]
      : NULL;

    // was a switch
    if(IMGUI_ELEMENT_E_TAB_ITEM) {
      if (!tabs_active || i != next_tab_item) {
        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
        tabs_active = igBeginTabBar(current_item->title.chars, tab_bar_flags);
      }
      next_tab_item = i + 1;
      if (tabs_active) {
        ImGuiTabItemFlags tab_item_flags = ImGuiTabItemFlags_None;
        // NOTE: second arg is used to add a "tab close" button
        if (igBeginTabItem(current_item->title.chars, NULL, tab_item_flags)) {
          igText(current_item->contents.chars);
          igEndTabItem();
        }
      }

      if (next_item == NULL) {
        igEndTabBar();
        break;
      }
      if (next_item->element != IMGUI_ELEMENT_E_TAB_ITEM) {
        igEndTabBar();
      }
    }


  }
}
