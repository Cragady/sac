#ifndef SAC_SHARED_LAYOUTS_CLAY_VIDEO_C
#define SAC_SHARED_LAYOUTS_CLAY_VIDEO_C

#include <clay.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "SAC/sac.h"
#include "SAC/shared-layouts/clay-video.h"

#define SAC_SHARED_LAYOUTS_CLAY_VIDEO_C_FONT_ID_BODY_16 0
#define SAC_SHARED_LAYOUTS_CLAY_VIDEO_C_COLOR_WHITE { 255, 255, 255, 255}
#define SAC_SHARED_LAYOUTS_CLAY_VIDEO_C_DOCS_LEN 4
#define SAC_SHARED_LAYOUTS_CLAY_VIDEO_C_STATUS_DOC_POS 0
#define SAC_SHARED_LAYOUTS_CLAY_VIDEO_C_STATUS_STR_BUFF 256

void RenderHeaderButton(Clay_String text) {
  CLAY({
    .layout = { .padding = { 16, 16, 8, 8 }},
    .backgroundColor = { 140, 140, 140, 255 },
    .cornerRadius = CLAY_CORNER_RADIUS(5)
  }) {
    CLAY_TEXT(text, CLAY_TEXT_CONFIG({
      .fontId = SAC_SHARED_LAYOUTS_CLAY_VIDEO_C_FONT_ID_BODY_16,
      .fontSize = 16,
      .textColor = { 255, 255, 255, 255 }
    }));
  }
}

void RenderDropdownMenuItem(Clay_String text) {
  CLAY({.layout = { .padding = CLAY_PADDING_ALL(16)}}) {
    CLAY_TEXT(text, CLAY_TEXT_CONFIG({
      .fontId = SAC_SHARED_LAYOUTS_CLAY_VIDEO_C_FONT_ID_BODY_16,
      .fontSize = 16,
      .textColor = { 255, 255, 255, 255 }
    }));
  }
}

void HandleSidebarInteraction(
  Clay_ElementId elementId,
  Clay_PointerData pointerData,
  intptr_t userData
) {
  SidebarClickData *clickData = (SidebarClickData*)userData;
  // If this button was clicked
  if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
    if (clickData->requestedDocumentIndex >= 0 && clickData->requestedDocumentIndex < SAC_SHARED_LAYOUTS_CLAY_VIDEO_C_DOCS_LEN) {
      // Select the corresponding document
      *clickData->selectedDocumentIndex = clickData->requestedDocumentIndex;
    }
  }
}

int ClayVideo_AddDocument(DocumentArray *doc_array, Document document) {
  if (doc_array->length == doc_array->max_length) return 1;
  doc_array->documents[doc_array->length] = document;
  doc_array->length++;
  return 0;
}

ClayVideo_Data ClayVideo_Initialize() {
  ClayVideo_Data data = {
    .frameArena = { .memory = (intptr_t)malloc(1024) },
    .documents = {
      .max_length = SAC_SHARED_LAYOUTS_CLAY_VIDEO_C_DOCS_LEN,
      .length = 0,
      .documents = malloc(sizeof(Document) * SAC_SHARED_LAYOUTS_CLAY_VIDEO_C_DOCS_LEN)
    },
    .allocationFailure = false,
  };

  DocumentArray *documents = &data.documents;

  if (documents->documents == NULL || (void *)data.frameArena.memory == NULL) {
    data.allocationFailure = true;
    return data;
  }


  Clay_String status_str = {
    .isStaticallyAllocated = false,
    .length = SAC_SHARED_LAYOUTS_CLAY_VIDEO_C_STATUS_STR_BUFF,
  };
  status_str.chars = malloc(sizeof(char) * status_str.length);
  if (status_str.chars == NULL) {
    status_str = CLAY_STRING("FAILED STR ALLOC IN CLAY_STRING");
  } else {
    memset((char *)status_str.chars, '\0', status_str.length);
  }

  ClayVideo_AddDocument(documents, (Document){ .title = CLAY_STRING("Status"), .contents = status_str, });
  ClayVideo_AddDocument(documents, (Document){ .title = CLAY_STRING("WIP"), .contents = CLAY_STRING(""), });
typedef struct {
  int is_used;
  char *str;
} NudeString;

  return data;
}

void ClayVideo_UpdateData(DocumentArray *doc_array, AppState *state) {
  ClayVideo_UpdateStatusData(doc_array, state);
}

void ClayVideo_UpdateStatusData(DocumentArray *doc_array, AppState *state) {
  const int STATUS_INDEX = SAC_SHARED_LAYOUTS_CLAY_VIDEO_C_STATUS_DOC_POS;
  const int STR_BUFF = SAC_SHARED_LAYOUTS_CLAY_VIDEO_C_STATUS_STR_BUFF;
  const char *format_str = "Mouse Pos: (%i, %i)\n"
                           "Is Clicking: %s\n";

  const Document *WANTED_DOC = &doc_array->documents[STATUS_INDEX];

  const int MOUSE_POS_X = state->mouse_info.x;
  const int MOUSE_POS_Y = state->mouse_info.y;

  const char *clicking_t_f = state->should_click
    ? "True"
    : "False";

  // NOTE: I don't know if the following memset will fix the garbage issue I was
  // having; shit was cropping up in the strings.
  // returns num chars if n was sufficiently large enough
  int chars_changed = snprintf((char *)WANTED_DOC->contents.chars, STR_BUFF, format_str, MOUSE_POS_X, MOUSE_POS_Y, clicking_t_f);
  int offset = chars_changed < STR_BUFF - 1
    ? chars_changed
    : 0;

  memset((char *)WANTED_DOC->contents.chars + offset, '\0', STR_BUFF - offset);
}

Clay_RenderCommandArray ClayVideo_CreateLayout(ClayVideo_Data *data) {
  data->frameArena.offset = 0;
  DocumentArray *documents = &data->documents;

  Clay_BeginLayout();

  Clay_Sizing layoutExpand = {
    .width = CLAY_SIZING_GROW(0),
    .height = CLAY_SIZING_GROW(0)
  };

  Clay_Color contentBackgroundColor = { 90, 90, 90, 255 };

  // Build UI here
  CLAY({ .id = CLAY_ID("OuterContainer"),
    .backgroundColor = {43, 41, 51, 255 },
    .layout = {
      .layoutDirection = CLAY_TOP_TO_BOTTOM,
      .sizing = layoutExpand,
      .padding = CLAY_PADDING_ALL(16),
      .childGap = 16
    }
  }) {
    // Child elements go inside braces
    CLAY({ .id = CLAY_ID("HeaderBar"),
      .layout = {
        .sizing = {
          .height = CLAY_SIZING_FIXED(60),
          .width = CLAY_SIZING_GROW(0)
        },
        .padding = { 16, 16, 0, 0 },
        .childGap = 16,
        .childAlignment = {
          .y = CLAY_ALIGN_Y_CENTER
        }
      },
      .backgroundColor = contentBackgroundColor,
      .cornerRadius = CLAY_CORNER_RADIUS(8)
    }) {
      // Header buttons go here
      CLAY({ .id = CLAY_ID("FileButton"),
        .layout = { .padding = { 16, 16, 8, 8 }},
        .backgroundColor = {140, 140, 140, 255 },
        .cornerRadius = CLAY_CORNER_RADIUS(5)
      }) {
        CLAY_TEXT(CLAY_STRING("File"), CLAY_TEXT_CONFIG({
          .fontId = SAC_SHARED_LAYOUTS_CLAY_VIDEO_C_FONT_ID_BODY_16,
          .fontSize = 16,
          .textColor = { 255, 255, 255, 255 }
        }));

        bool fileMenuVisible =
          Clay_PointerOver(Clay_GetElementId(CLAY_STRING("FileButton")))
          ||
          Clay_PointerOver(Clay_GetElementId(CLAY_STRING("FileMenu")));

        if (fileMenuVisible) { // Below has been changed slightly to fix the small bug where the menu would dismiss when mousing over the top gap
          CLAY({ .id = CLAY_ID("FileMenu"),
            .floating = {
              .attachTo = CLAY_ATTACH_TO_PARENT,
              .attachPoints = {
                .parent = CLAY_ATTACH_POINT_LEFT_BOTTOM
              },
            },
            .layout = {
              .padding = {0, 0, 8, 8 }
            }
          }) {
            CLAY({
              .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = {
                  .width = CLAY_SIZING_FIXED(200)
                },
              },
              .backgroundColor = {40, 40, 40, 255 },
              .cornerRadius = CLAY_CORNER_RADIUS(8)
            }) {
              // Render dropdown items here
              RenderDropdownMenuItem(CLAY_STRING("New"));
              RenderDropdownMenuItem(CLAY_STRING("Open"));
              RenderDropdownMenuItem(CLAY_STRING("Close"));
            }
          }
        }
      }
      // NOTE: add more menu buttons in this level of nesting if wanted
    }

    CLAY({
      .id = CLAY_ID("LowerContent"),
      .layout = { .sizing = layoutExpand, .childGap = 16 }
    }) {
      CLAY({
        .id = CLAY_ID("Sidebar"),
        .backgroundColor = contentBackgroundColor,
        .layout = {
          .layoutDirection = CLAY_TOP_TO_BOTTOM,
          .padding = CLAY_PADDING_ALL(16),
          .childGap = 8,
          .sizing = {
            .width = CLAY_SIZING_FIXED(250),
            .height = CLAY_SIZING_GROW(0)
          }
        }
      }) {
        for (int i = 0; i < documents->length; i++) {
          Document document = documents->documents[i];
          Clay_LayoutConfig sidebarButtonLayout = {
            .sizing = { .width = CLAY_SIZING_GROW(0) },
            .padding = CLAY_PADDING_ALL(16)
          };

          if (i == data->selectedDocumentIndex) {
            CLAY({
              .layout = sidebarButtonLayout,
              .backgroundColor = {120, 120, 120, 255 },
              .cornerRadius = CLAY_CORNER_RADIUS(8)
            }) {
              CLAY_TEXT(document.title, CLAY_TEXT_CONFIG({
                .fontId = SAC_SHARED_LAYOUTS_CLAY_VIDEO_C_FONT_ID_BODY_16,
                .fontSize = 20,
                .textColor = { 255, 255, 255, 255 }
              }));
            }
          } else {
            SidebarClickData *clickData = (SidebarClickData *)(data->frameArena.memory + data->frameArena.offset);
            *clickData = (SidebarClickData) { .requestedDocumentIndex = i, .selectedDocumentIndex = &data->selectedDocumentIndex };
            data->frameArena.offset += sizeof(SidebarClickData);
            CLAY({ .layout = sidebarButtonLayout, .backgroundColor = (Clay_Color) { 120, 120, 120, Clay_Hovered() ? 120 : 0 }, .cornerRadius = CLAY_CORNER_RADIUS(8) }) {
              Clay_OnHover(HandleSidebarInteraction, (intptr_t)clickData);
              CLAY_TEXT(document.title, CLAY_TEXT_CONFIG({
                .fontId = SAC_SHARED_LAYOUTS_CLAY_VIDEO_C_FONT_ID_BODY_16,
                .fontSize = 20,
                .textColor = { 255, 255, 255, 255 }
              }));
            }
          }
        }
      }

      CLAY({ .id = CLAY_ID("MainContent"),
        .backgroundColor = contentBackgroundColor,
        .scroll = { .vertical = true },
        .layout = {
          .layoutDirection = CLAY_TOP_TO_BOTTOM,
          .childGap = 16,
          .padding = CLAY_PADDING_ALL(16),
          .sizing = layoutExpand
        }
      }) {
        Document selectedDocument = documents->documents[data->selectedDocumentIndex];
        CLAY_TEXT(selectedDocument.title, CLAY_TEXT_CONFIG({
          .fontId = SAC_SHARED_LAYOUTS_CLAY_VIDEO_C_FONT_ID_BODY_16,
          .fontSize = 24,
          .textColor = SAC_SHARED_LAYOUTS_CLAY_VIDEO_C_COLOR_WHITE
        }));
        CLAY_TEXT(selectedDocument.contents, CLAY_TEXT_CONFIG({
          .fontId = SAC_SHARED_LAYOUTS_CLAY_VIDEO_C_FONT_ID_BODY_16,
          .fontSize = 24,
          .textColor = SAC_SHARED_LAYOUTS_CLAY_VIDEO_C_COLOR_WHITE
        }));
      }
    }
  }

  Clay_RenderCommandArray renderCommands = Clay_EndLayout();
  for (int32_t i = 0; i < renderCommands.length; i++) {
    Clay_RenderCommandArray_Get(&renderCommands, i)->boundingBox.y += data->yOffset;
  }
  return renderCommands;
}

#undef SAC_SHARED_LAYOUTS_CLAY_VIDEO_C_FONT_ID_BODY_16
#undef SAC_SHARED_LAYOUTS_CLAY_VIDEO_C_COLOR_WHITE
#undef SAC_SHARED_LAYOUTS_CLAY_VIDEO_C_DOCS_LEN
#undef SAC_SHARED_LAYOUTS_CLAY_VIDEO_C_STATUS_DOC_POS
#undef SAC_SHARED_LAYOUTS_CLAY_VIDEO_C_STATUS_STR_BUFF

#endif
