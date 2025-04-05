#ifndef SAC_SHARED_LAYOUTS_CLAY_VIDEO_H
#define SAC_SHARED_LAYOUTS_CLAY_VIDEO_H

#include <clay.h>

typedef struct app_state AppState;

typedef struct {
  Clay_String title;
  Clay_String contents;
} Document;

typedef struct {
  Document *documents;
  uint32_t length;
  uint32_t max_length;
} DocumentArray;

typedef struct {
  intptr_t offset;
  intptr_t memory;
} ClayVideo_Arena;

typedef struct {
  int32_t selectedDocumentIndex;
  float yOffset;
  ClayVideo_Arena frameArena;
  DocumentArray documents;
  int allocationFailure;
} ClayVideo_Data;

typedef struct {
  int32_t requestedDocumentIndex;
  int32_t *selectedDocumentIndex;
} SidebarClickData;


void RenderHeaderButton(Clay_String text);
void RenderDropdownMenuItem(Clay_String text);
void HandleSidebarInteraction(
  Clay_ElementId elementId,
  Clay_PointerData pointerData,
  intptr_t userData
);
int ClayVideo_AddDocument(DocumentArray *doc_array, Document document);
ClayVideo_Data ClayVideo_Initialize();
void ClayVideo_UpdateData(DocumentArray *doc_array, AppState *state);
void ClayVideo_UpdateStatusData(DocumentArray *doc_array, AppState *state);
Clay_RenderCommandArray ClayVideo_CreateLayout(ClayVideo_Data *data);

#endif
