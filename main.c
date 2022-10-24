#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <string.h>
#include "stack/stack.h"

typedef struct {
  int x;
  int y;
} Window;

typedef struct {
  unsigned long x;
  unsigned long y;
  unsigned long len;
  unsigned char *arr;
} Map;

typedef struct {
  Texture2D *arr;
  unsigned char len;
  char path[100];
} Textures;

typedef struct {
  Window win;
  unsigned char tilesize;
  unsigned char curtile;
  unsigned char starttile;
  Textures textures;
  Map map;
  Camera2D camera;
  Stack stack;
} Data;

Texture2D *LoadTexturesIntoArr(Data d);
unsigned char *CreateMap(Data d);
void DrawMap(Data d);
void UpdateCursor(Data *d);
void DrawCursor(Data d);
void UpdateKeyboard(Data *d);
void FreeData(Data d);
void PrintData(Data d);
void DrawBorder(Data d);

int main() {
  
  // init data struct
  Data data = {0};

  // editable values
  data.map.x = 64;
  data.map.y = 16;
  data.tilesize = 16;
  data.curtile = 0;
  data.starttile = 0xff;
  strcpy(data.textures.path, "assets");
  data.textures.len = 12;

  // setting window
  data.win.x = 1920; 
  data.win.y = 1080;

  // create window before loading textures
  InitWindow(data.win.x, data.win.y, "HELLOW WORLD");

  // setting up stack inside data
  InitStack(&data.stack);

  // load textures into data
  data.textures.arr = LoadTexturesIntoArr(data);

  // init map with all indexes 
  data.map.len = data.map.x * data.map.y;
  data.map.arr = CreateMap(data);

  // create camera
  data.camera = (Camera2D) {0};
  data.camera.zoom = 0.5;
  data.camera.target = (Vector2) { ((float) data.map.x * data.tilesize) / 2 , ((float) data.map.y * data.tilesize) / 2 };
  data.camera.offset = (Vector2) { data.win.x/2.7, data.win.y/2 };

  // set fps
  SetTargetFPS(144);
  
  // game loop
  while (!WindowShouldClose()) {

    // update
    UpdateKeyboard(&data);
    UpdateCursor(&data);

    // draw
    BeginDrawing();
    ClearBackground(BLACK);

    BeginMode2D(data.camera);

    DrawBorder(data);
    DrawMap(data);
    DrawCursor(data);

    EndMode2D();

    DrawRectangle(data.win.x - data.win.x/4, 0, data.win.x/4, data.win.y, DARKGRAY);

    EndDrawing();

  }
  //PrintData(data);
  FreeData(data);
}

void FreeData(Data d) {
  free(d.textures.arr);
  free(d.map.arr);
}

void DrawBorder(Data d) {
  float boardersize = 0.25f;
  DrawRectangle( - d.tilesize * boardersize, 0, d.tilesize * boardersize, d.map.y * d.tilesize, WHITE);
  DrawRectangle(- d.tilesize * boardersize, -d.tilesize * boardersize, d.map.x * d.tilesize + ((d.tilesize * boardersize) * 2), d.tilesize * boardersize, WHITE);
  DrawRectangle(d.map.x * d.tilesize, 0, d.tilesize * boardersize, d.map.y * d.tilesize, WHITE);
  DrawRectangle(- d.tilesize * boardersize, (d.map.y * d.tilesize), (d.map.x * d.tilesize) + ((d.tilesize * boardersize) * 2), d.tilesize * boardersize, WHITE);
}    

// loads png images in path into textures array
Texture2D *LoadTexturesIntoArr(Data d) {
  Texture2D *textures = malloc(sizeof(Texture2D) * d.textures.len);
  for (int i = 0; i < d.textures.len; i++) {
    char imagepath[1000];
    sprintf(imagepath, "%s/tile%d.png", d.textures.path, i);
    textures[i] = LoadTexture(imagepath);
  }
  return textures;
}

// creates map array
unsigned char *CreateMap(Data d) {
  unsigned char *map = malloc(sizeof(unsigned char) * d.map.len);
  for (int i = 0; i < d.map.len; i++)
    map[i] = d.starttile;
  return map;
}

// draws tiles in map array to screen
void DrawMap(Data d) {
  for (int i = 0; i < d.map.len; i++) {
    int row = i / d.map.x;
    int col = i % d.map.x;
    if (d.map.arr[i] != 0xFF)
      DrawTexture(d.textures.arr[d.map.arr[i]], col * d.tilesize, row * d.tilesize, WHITE);
    else
      DrawRectangle(col * d.tilesize, row * d.tilesize, d.tilesize, d.tilesize, BLACK);
  }
}

// updates cursor
void UpdateCursor(Data *d) {
  Vector2 pos = GetScreenToWorld2D(GetMousePosition(), d->camera);
  float wheel = GetMouseWheelMove();
  if (wheel != 0) {
    // Zoom increment
    const float zoomIncrement = 0.125f;
    d->camera.zoom += (wheel * zoomIncrement);
    if (d->camera.zoom < zoomIncrement)
      d->camera.zoom = zoomIncrement;
  }

  if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
    if (pos.x < (d->map.x * d->tilesize) && pos.y < (d->map.y * d->tilesize) && pos.x >= 0 && pos.y >= 0) {
      int row = (int) pos.y / d->tilesize;
      int col = (int) pos.x / d->tilesize;
      int index = (row * d->map.x) + col;
      if (!StackEmpty(d->stack)) {
	StackNode *node = StackTop(d->stack);
	if (!(node->index == index && d->map.arr[index] == d->curtile))
	  StackPush(&d->stack, index, d->map.arr[index]);
      }
      else 
	  StackPush(&d->stack, index, d->map.arr[index]);
      d->map.arr[index] = d->curtile;
    }
  }
}

// updates keyboard
void UpdateKeyboard(Data *d) {
  if (IsKeyPressed(KEY_R)) {
    d->curtile++;   
    if (d->curtile == d->textures.len)
      d->curtile = 0;
  }
  if (IsKeyDown(KEY_W))
    d->camera.target.y -= 5;
  if (IsKeyDown(KEY_S))
    d->camera.target.y += 5;
  if (IsKeyDown(KEY_A))
    d->camera.target.x -= 5;
  if (IsKeyDown(KEY_D))
    d->camera.target.x += 5;

  if (IsKeyPressed(KEY_U) && !StackEmpty(d->stack)) {
    StackNode *node = StackTop(d->stack);
    d->map.arr[node->index] = node->value;
    StackPop(&d->stack);
  }
}

// draws under cursor
void DrawCursor(Data d) {
  Vector2 pos = GetScreenToWorld2D(GetMousePosition(), d.camera);
  Color col = GREEN;
  col.a = 150;
  DrawTexture(d.textures.arr[d.curtile], pos.x - (d.tilesize/2), pos.y - (d.tilesize/2), col);
}

void PrintData(Data d) {
  for (int i = 0; i < d.map.len; i++) {
    int row = i / d.map.y;
    int col = i % d.map.x;
    printf("%d ", d.map.arr[i]);
    if (col == d.map.x-1)
      printf("\n");
  }
}
