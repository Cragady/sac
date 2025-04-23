#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "SAC/sac.h"
#include "cimgui.h"
#include "SAC/shared_layouts/imgui_video.h"

#define SAC_SHARED_LAYOUTS_IMGUI_VIDEO_C_FONT_ID_BODY_16 0
#define SAC_SHARED_LAYOUTS_IMGUI_VIDEO_C_COLOR_WHITE { 255, 255, 255, 255}
#define SAC_SHARED_LAYOUTS_IMGUI_VIDEO_C_DOCS_LEN 4
#define SAC_SHARED_LAYOUTS_IMGUI_VIDEO_C_STATUS_DOC_POS 0
#define SAC_SHARED_LAYOUTS_IMGUI_VIDEO_C_STATUS_STR_BUFF 256
#define SAC_SHARED_LAYOUTS_IMGUI_VIDEO_C_STATUS_STR_ERR "FAILED STR ALLOC IN IMGUI_STRING"
#define SAC_SHARED_LAYOUTS_IMGUI_VIDEO_C_STATUS_STR_ERR_BUFF 31

int ImGuiVideo_AddDocument(DocumentArray *doc_array, Document document) {
  if (doc_array->length == doc_array->max_length) return 1;
  doc_array->documents[doc_array->length] = document;
  doc_array->length++;
  return 0;
}


ImGuiVideo_Data ImGuiVideo_Initialize() {
  const size_t DOC_LEN = SAC_SHARED_LAYOUTS_IMGUI_VIDEO_C_DOCS_LEN;
  ImGuiVideo_Data data = {
    .documents = {
      .max_length = DOC_LEN,
      .length = 0,
      .documents = malloc(sizeof(Document) * DOC_LEN),
    },
    .allocation_failure = false,
  };

  DocumentArray *documents = &data.documents;

  if (documents->documents == NULL) {
    data.allocation_failure = true;
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

  ImGuiVideo_AddDocument(documents, (Document){ .title = SAC_STRING("Status"), .contents = status_str, .element = IMGUI_ELEMENT_E_TAB, .is_open = true, });
  ImGuiVideo_AddDocument(documents, (Document){ .title = SAC_STRING("WIP"), .contents = SAC_STRING("WIP"), .element = IMGUI_ELEMENT_E_TAB, .is_open = true, });

  return data;
}

void ImGuiVideo_UpdateData(DocumentArray *doc_array, AppState *state) {
  ImGuiVideo_UpdateStatusData(doc_array, state);
}

// TODO: review logic in this function
void ImGuiVideo_UpdateStatusData(DocumentArray *doc_array, AppState *state) {
  const int STATUS_INDEX = SAC_SHARED_LAYOUTS_IMGUI_VIDEO_C_STATUS_DOC_POS;
  const int STR_BUFF = SAC_SHARED_LAYOUTS_IMGUI_VIDEO_C_STATUS_STR_BUFF;
  const char *ERR_MSG = SAC_SHARED_LAYOUTS_IMGUI_VIDEO_C_STATUS_STR_ERR;
  const int ERR_MSG_BUFF = SAC_SHARED_LAYOUTS_IMGUI_VIDEO_C_STATUS_STR_ERR_BUFF;
  const char *format_str = "Mouse Pos: (%i, %i)\n"
                           "Is Clicking: %s\n";

  const Document *WANTED_DOC = &doc_array->documents[STATUS_INDEX];
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
  bool tabs_active = false;
  size_t next_tab_item = -1;

  for (size_t i = 0; i < data->documents.length; i++) {
    Document *current_item = &data->documents.documents[i];
    Document *next_item = i + 1 < data->documents.length
      ? &data->documents.documents[i]
      : NULL;

    switch (current_item->element) {
      case IMGUI_ELEMENT_E_NONE:
        break;
      case IMGUI_ELEMENT_E_TAB:
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
        if (next_item->element != IMGUI_ELEMENT_E_TAB) {
          igEndTabBar();
        }
        break;
    }
  }

  igEnd();

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
