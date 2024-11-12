#include "raylib.h"

void animate_char(Texture2D bonifacio, Rectangle *frame_rec, int frame, int *frames_counter, int *current_frame, int *frames_speed);

int main(){
  const int screen_width = 800;
  const int screen_height = 450;
  InitWindow(screen_width, screen_height, "Hump Day");
  Texture2D bonifacio = LoadTexture("resources/bonifacio.png");
  Image jim_img = LoadImage("resources/jim_kalbo.png");
  Texture2D jim_graphics = LoadTextureFromImage(jim_img);
  UnloadImage(jim_img);
  Vector2 position = {800 / 2, 450 / 2};
  Rectangle frame_rec = {0.0f, ((float)(bonifacio.height/4))*3.0f, (float)bonifacio.width/3, (float)bonifacio.height/4};
  int current_frame = 1;
  int frames_counter = 0;
  int frames_speed = 8;
  SetTargetFPS(60);
  while (!WindowShouldClose()){
    if(IsKeyDown(KEY_W)){
      animate_char(bonifacio, &frame_rec, 3, &frames_counter, &current_frame, &frames_speed);
      position.y -= 1;
    }
    if(IsKeyDown(KEY_A)){
      animate_char(bonifacio, &frame_rec, 1, &frames_counter, &current_frame, &frames_speed);
      position.x -= 1;
    }
    if(IsKeyDown(KEY_S)){
      animate_char(bonifacio, &frame_rec, 0, &frames_counter, &current_frame, &frames_speed);
      position.y += 1;
    }
    if(IsKeyDown(KEY_D)){
      animate_char(bonifacio, &frame_rec, 2, &frames_counter, &current_frame, &frames_speed);
      position.x += 1;
    }
    BeginDrawing();
      ClearBackground(RAYWHITE);
      DrawTexture(jim_graphics, screen_width/2 - jim_graphics.width/2, screen_height/2 - jim_graphics.height/2, WHITE);
      DrawTextureRec(bonifacio, frame_rec, position, WHITE);
    EndDrawing();
  }
  UnloadTexture(jim_graphics);
  UnloadTexture(bonifacio);
  CloseWindow();
  return 0;
}

void animate_char(Texture2D bonifacio, Rectangle *frame_rec, int frame, int *frames_counter, int *current_frame, int *frames_speed){
  *frames_counter += 1;
  if(*frames_counter >= (60/(*frames_speed))){
    *frames_counter = 0;
    *current_frame += 1;
    if(*current_frame > 3){
      *current_frame = 0;
    }
    frame_rec->x = (float)(*current_frame)*(float)bonifacio.width/3;
    frame_rec->y = (float)frame*(float)bonifacio.height/4;
  }
}
