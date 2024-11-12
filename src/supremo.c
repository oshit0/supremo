/*
 * Mark Gerald Guerrero - Programmer
 * Ezekiel Villanueva - Graphics
 * Gericho Avellana Graphics
 * Earl Francis Martinez Graphics
 */
#include <stdio.h>
#include <limits.h>
#include <float.h>
#include <stdlib.h>
#include "raylib.h"
#include "raymath.h"

#define MAX_FRAME_SPEED 15
#define MIN_FRAME_SPEED 1
#define MAX_NUM_BULLETS 10
#define MAX_NUM_ENEMIES 1000
#define WAVE_1 10
#define WAVE_2 25
#define WAVE_3 50

//typedef enum { FIRST = 0, SECOND, THIRD } EnemyWave;

typedef struct Player{
  Texture2D texture;
  Rectangle hitbox_rec;
  Rectangle frame_rec;
  Vector2 speed;
  char dir;
} Player;

typedef struct Enemy{
  Texture2D texture;
  Rectangle hitbox_rec;
  Rectangle frame_rec;
  Vector2 speed;
  bool active;
  bool visible;
  int opacity;
} Enemy;

typedef struct Bullet{
  Texture2D texture;
  Rectangle hitbox_rec;
  Rectangle frame_rec;
  Vector2 speed;
  bool active;
  char dir;
} Bullet;

typedef struct Base{
  Texture2D texture;
  Rectangle hitbox_rec;
  Rectangle frame_rec;
} Base;

static const int screen_width = 800;
static const int screen_height = 450;

static bool game_over = false;
static bool pause = false;
static bool victory = false;

static Player player = { 0 };
static Enemy enemy[MAX_NUM_ENEMIES] = { 0 };
static Bullet bullet[MAX_NUM_BULLETS] = { 0 };
static Base base = { 0 };
//static EnemyWave wave = { 0 };

static int score = 0;

static int fire_rate = 0;
static float alpha = 0.0f;

static int active_enemies = 0;
static int enemies_killed = 0;
static bool smooth = false;

static int p_current_x_frame = 0;
static int p_current_y_frame = 0;
static int p_frames_counter = 0;
static int p_frames_speed = 0;

static int e_current_x_frame = 0;
static int e_current_y_frame = 0;
static int e_frames_counter = 0;
static int e_frames_speed = 0;

static Texture2D main_screen = { 0 };
static Texture2D story_controls = { 0 };
static Texture2D texture_tmp = { 0 };
static Texture2D background = { 0 };
static Texture2D bullet_tmp = { 0 };

static Rectangle start_btn_bounds = { 0 };
static bool start_btn_action = false;

static Rectangle story_btn_bounds = { 0 };
static bool story_btn_action = false;

static Vector2 mouse_point = { 0 };

static Font font_ttf = { 0 };

static FILE *high_score = { 0 };

static void init_game();
static void main_menu();
static void update_game();
static void draw_game();
static void unload_game();

int main(){
  InitWindow(screen_width, screen_height, "SUPREMO");
  init_game();
  main_menu();
  SetTargetFPS(60);
  while(!WindowShouldClose()){
    update_game();
    draw_game();
  }
  unload_game();
  CloseWindow();
  return 0;
}

void init_game(){
  score = 0;
  fire_rate = 0;
  pause = false;
  game_over = false;
  victory = false;
  smooth = false;
  //wave = FIRST;
  active_enemies = 10;
  enemies_killed = 0;
  alpha = 0;
  p_current_x_frame = 0;
  p_current_y_frame = 0;
  p_frames_counter = 0;
  p_frames_speed = 8;
  e_current_x_frame = 0;
  e_current_y_frame = 0;
  e_frames_counter = 0;
  e_frames_speed = 8;

  player.texture = LoadTexture("resources/bonifacio.png");
  player.hitbox_rec.x = screen_width/2;
  player.hitbox_rec.y = screen_height/2;
  player.hitbox_rec.width = 15;
  player.hitbox_rec.height = 15;
  player.frame_rec.x = 0.0f;
  player.frame_rec.y = 0.0f;
  player.frame_rec.width = (float)player.texture.width/3;
  player.frame_rec.height = (float)player.texture.height/4;
  player.speed.x = 2.0f;
  player.speed.y = 2.0f;
  player.dir = 'd';

  enemy[0].texture = LoadTexture("resources/enemy/spaniard3.png");
  for(int i = 0; i < MAX_NUM_ENEMIES; ++i){
    enemy[i].texture = LoadTexture("resources/enemy/spaniard3.png");
    enemy[i].hitbox_rec.x = GetRandomValue(screen_width, screen_width + 1000);
    enemy[i].hitbox_rec.y = GetRandomValue(0, screen_height - enemy[0].texture.height);
    enemy[i].hitbox_rec.width = 15;
    enemy[i].hitbox_rec.height = 15;
    enemy[i].frame_rec.x = 0;
    enemy[i].frame_rec.y = (float)(enemy[0].texture.height/4) * 1.0f;
    enemy[i].frame_rec.width = (float)enemy[i].texture.width/3;
    enemy[i].frame_rec.height = (float)enemy[i].texture.height/4;
    enemy[i].speed.x = 1.0f;
    enemy[i].speed.y = 1.0f;
    enemy[i].active = true;
    enemy[i].visible = true;
    enemy[i].opacity = 225;
  }

  bullet[0].texture = LoadTexture("resources/bolo/bolo_right.png");
  bullet[1].texture = LoadTexture("resources/bolo/bolo_left.png");
  bullet[2].texture = LoadTexture("resources/bolo/bolo_up.png");
  bullet[3].texture = LoadTexture("resources/bolo/bolo_down.png");
  for(int i = 0; i < MAX_NUM_BULLETS; ++i){
    bullet[i].hitbox_rec.x = player.hitbox_rec.x;
    bullet[i].hitbox_rec.y = player.hitbox_rec.y + player.hitbox_rec.height/4;
    bullet[i].hitbox_rec.width = 10;
    //bullet[i].rec.height = 5;
    bullet[i].hitbox_rec.height = 10;
    bullet[i].frame_rec.x = 0.0f;
    bullet[i].frame_rec.y = 0.0f;
    bullet[i].frame_rec.width = 0.0f;
    bullet[i].frame_rec.height = 0.0f;
    bullet[i].speed.x = 7.0f;
    bullet[i].speed.y = 7.0f;
    bullet[i].active = false;
    bullet[i].dir = ' ';
  }

  base.texture = LoadTexture("resources/kubo.png");
  base.hitbox_rec.x = 80;
  base.hitbox_rec.y = screen_height/2;
  base.hitbox_rec.width = 20;
  base.hitbox_rec.height = 20;
  base.frame_rec.x = 0;
  base.frame_rec.y = 0;
  base.frame_rec.width = 100;
  base.frame_rec.height = 100;

  main_screen = LoadTexture("resources/main_menu/main_screen.png");
  story_controls = LoadTexture("resources/main_menu/story_controls.png");
  texture_tmp = LoadTexture("resources/main_menu/main_screen.png");
  background = LoadTexture("resources/background.png");
  bullet_tmp = bullet[0].texture;

  start_btn_bounds.x = (screen_width / 2) - 78;
  start_btn_bounds.y = (screen_height / 2) + 27;
  start_btn_bounds.width = 155;
  start_btn_bounds.height = 40;
  start_btn_action = false;

  story_btn_bounds.x = (screen_width / 2) - 78;
  story_btn_bounds.y = (screen_height / 2) + 80;
  story_btn_bounds.width = 155;
  story_btn_bounds.height = 40;
  story_btn_action = false;

  mouse_point.x = 0.0f;
  mouse_point.y = 0.0f;

  font_ttf = LoadFontEx("resources/fonts/Daydream.ttf", 32, 0, 250);

  high_score = fopen("high_score.txt", "w");
}

void main_menu(){
  while(!WindowShouldClose()){
    mouse_point = GetMousePosition();
    start_btn_action = false;
    story_btn_action = false;
    if(CheckCollisionPointRec(mouse_point, start_btn_bounds)){
      if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) start_btn_action = true;
    }
    if(CheckCollisionPointRec(mouse_point, story_btn_bounds)){
      if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) story_btn_action = true;
    }

    if(start_btn_action){
      break;
    }
    else if(story_btn_action){
      texture_tmp = story_controls;
    }
    BeginDrawing();
      ClearBackground(RAYWHITE);
      DrawTexture(texture_tmp, screen_width/2 - background.width/2, screen_height/2 - background.height/2, WHITE);
    EndDrawing();
  }
}

void update_game(){
  if(!game_over && !victory){
    if(IsKeyPressed('P')) pause = !pause;
    if(!pause){
      /*
      switch(wave){
        case FIRST:
          if(!smooth){
            alpha += 0.02f;
            if(alpha >= 1.0f) smooth = true;
          }
          if(enemies_killed == active_enemies){
            enemies_killed = 0;
            for(int i = 0; i < active_enemies; ++i){
              if(!enemy[i].active){
                enemy[i].active = true;
                enemy[i].visible = true;
              }
            }
            active_enemies = WAVE_2;
            wave = SECOND;
            smooth = false;
            alpha = 0.0f;
          }
          break;
        case SECOND:
          if(!smooth){
            alpha += 0.02f;
            if(alpha >= 1.0f) smooth = true;
          }
          if(smooth) alpha -= 0.02f;
          if(enemies_killed == active_enemies){
            enemies_killed = 0;
            for(int i = 0; i < active_enemies; ++i){
              if(!enemy[i].active){
                enemy[i].active = true;
                enemy[i].visible = true;
              }
            }
            active_enemies = WAVE_3;
            wave = THIRD;
            smooth = false;
            alpha = 0.0f;
          }
          break;
        case THIRD:
          if(!smooth){
            alpha += 0.02f;
            if(alpha >= 1.0f) smooth = true;
          }
          if(smooth) alpha -= 0.0f;
          if(enemies_killed == active_enemies) victory = true;
          break;
        default:
          break;
      }
      */
      if(enemies_killed == active_enemies){
        enemies_killed = 0;
        active_enemies += 10;
        for(int i = 0; i < active_enemies; ++i){
          enemy[i].hitbox_rec.x = GetRandomValue(screen_width, screen_width + 1000);
          enemy[i].hitbox_rec.y = GetRandomValue(0, screen_height - enemy[0].texture.height);
          enemy[i].active = true;
          enemy[i].visible = true;
          enemy[i].opacity = 225;
        }

      };

      //Fix this
      if(IsKeyDown(KEY_D)){
        player.hitbox_rec.x += player.speed.x;
        p_current_y_frame = 2;
        p_frames_counter += 1;
        player.dir = 'd';
      }
      if(IsKeyDown(KEY_A)){
        player.hitbox_rec.x -= player.speed.x;
        p_current_y_frame = 1;
        p_frames_counter += 1;
        player.dir = 'a';
      }
      if(IsKeyDown(KEY_W)){
        player.hitbox_rec.y -= player.speed.y;
        p_current_y_frame = 3;
        p_frames_counter += 1;
        player.dir = 'w';
      }
      if(IsKeyDown(KEY_S)){
        player.hitbox_rec.y += player.speed.y;
        p_current_y_frame = 0;
        p_frames_counter += 1;
        player.dir = 's';
      }
      if(p_frames_counter >= 60/p_frames_speed){
        p_frames_counter = 0;
        p_current_x_frame += 1;
        if(p_current_x_frame > 3){
          p_current_x_frame = 0;
        }
      }
      player.frame_rec.x = (float)p_current_x_frame*player.texture.width/3;
      player.frame_rec.y = (float)p_current_y_frame*player.texture.height/4;

      for(int i = 0; i < active_enemies; ++i){
        if(CheckCollisionRecs(player.hitbox_rec, enemy[i].hitbox_rec)) game_over = true;
      }

      for(int i = 0; i < active_enemies; ++i){
        if(CheckCollisionRecs(base.hitbox_rec, enemy[i].hitbox_rec)) game_over = true;
      }

      // <-Enemy Random Movement Here->
      for(int i = 0; i < active_enemies; ++i){
        if(enemy[i].active){
          if(enemy[i].hitbox_rec.x < 0){
            enemy[i].hitbox_rec.x = GetRandomValue(screen_width, screen_width + 1000);
            enemy[i].hitbox_rec.y = GetRandomValue(0, screen_height - enemy[i].hitbox_rec.height);
          }
          if(enemy[i].hitbox_rec.x > 40){
            enemy[i].hitbox_rec.x -= enemy[i].speed.x;
          }
          if(enemy[i].hitbox_rec.x <= base.hitbox_rec.x + 200){
            if(enemy[i].hitbox_rec.y > base.hitbox_rec.y){
              enemy[i].hitbox_rec.y -= enemy[i].speed.y;
            }
            else if(enemy[i].hitbox_rec.y < base.hitbox_rec.y){
              enemy[i].hitbox_rec.y += enemy[i].speed.y;
            }
          }
        }
      }

      e_frames_counter++;
      if(e_frames_counter >= 60/e_frames_speed){
        e_frames_counter = 0;
        e_current_x_frame += 1;
        if(e_current_x_frame > 3){
          e_current_x_frame = 0;
        }
      }
      for(int i = 0; i < active_enemies; ++i){
        enemy[i].frame_rec.x = (float)e_current_x_frame*enemy[0].texture.width/3;
      }

      if(player.hitbox_rec.x <= 0) player.hitbox_rec.x = 2;
      if(player.hitbox_rec.x + player.frame_rec.width >= screen_width) player.hitbox_rec.x = screen_width - player.frame_rec.width;
      if(player.hitbox_rec.y <= 0) player.hitbox_rec.y = 2;
      if(player.hitbox_rec.y + player.frame_rec.height >= screen_height) player.hitbox_rec.y = screen_height - player.frame_rec.height;

      if (IsKeyDown(KEY_J)){
        fire_rate += 5;
        for (int i = 0; i < MAX_NUM_BULLETS; i++){
          if(!bullet[i].active && fire_rate % 20 == 0){
            bullet[i].hitbox_rec.x = player.hitbox_rec.x;
            bullet[i].hitbox_rec.y = player.hitbox_rec.y + player.hitbox_rec.height/2;
            bullet[i].dir = player.dir;
            bullet[i].active = true;
            break;
          }
        }
      }

      for(int i = 0; i < MAX_NUM_BULLETS; ++i){
        if(bullet[i].active){
          switch(bullet[i].dir){
            case 'd':
              bullet[i].hitbox_rec.x += bullet[i].speed.x;
              bullet[i].frame_rec.width = bullet[0].texture.width;
              bullet[i].frame_rec.height = bullet[0].texture.height;
              break;
            case 'a':
              bullet[i].hitbox_rec.x -= bullet[i].speed.x;
              bullet[i].frame_rec.width = bullet[1].texture.width;
              bullet[i].frame_rec.height = bullet[1].texture.height;
              break;
            case 'w':
              bullet[i].hitbox_rec.y -= bullet[i].speed.y;
              bullet[i].frame_rec.width = bullet[2].texture.width;
              bullet[i].frame_rec.height = bullet[2].texture.height;
              break;
            case 's':
              bullet[i].hitbox_rec.y += bullet[i].speed.y;
              bullet[i].frame_rec.width = bullet[3].texture.width;
              bullet[i].frame_rec.height = bullet[3].texture.height;
              break;
          }
          for(int j = 0; j < active_enemies; ++j){
            if(enemy[j].active){
              if(CheckCollisionRecs(bullet[i].hitbox_rec, enemy[j].hitbox_rec)){
                score += 10;
                bullet[i].active = false;
                bullet[i].dir = ' ';
                enemy[j].active = false;
                fire_rate = 0;
                enemies_killed++;
              }
              if(bullet[i].hitbox_rec.x + bullet[i].frame_rec.width >= screen_width + bullet[i].frame_rec.width||
                  bullet[i].hitbox_rec.x + bullet[i].frame_rec.width <= bullet[i].frame_rec.height ||
                  bullet[i].hitbox_rec.y + bullet[i].frame_rec.height >= screen_height + bullet[i].frame_rec.height ||
                  bullet[i].hitbox_rec.y + bullet[i].frame_rec.height <= bullet[i].frame_rec.height){
                bullet[i].active = false;
                bullet[i].dir = ' ';
                fire_rate = 0;
              }
            }
          }
        }
        for(int i = 0; i < active_enemies; ++i){
          if(!enemy[i].active && enemy[i].visible){
            if(enemy[i].opacity > 0){
              enemy[i].opacity -= 1;
            }
            else{
              enemy[i].hitbox_rec.x = -100;
              enemy[i].hitbox_rec.y = -100;
              enemy[i].visible = false;
              enemy[i].opacity = 225;
            }
          }
        }
      }
    }
  }
  else if(victory || game_over){
    fprintf(high_score, "%d", score);
    if(IsKeyPressed(KEY_ENTER)){
      victory = false;
      game_over = false;
      unload_game();
      init_game();
    }
  }
}

void draw_game(){
  BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawTexture(background, screen_width/2 - background.width/2, screen_height/2 - background.height/2, WHITE);
    DrawTextureRec(player.texture, player.frame_rec, (Vector2){player.hitbox_rec.x - 6, player.hitbox_rec.y - 6}, WHITE);
    for(int i = 0; i < active_enemies; ++i){
      DrawTextureRec(enemy[0].texture, enemy[i].frame_rec, (Vector2){enemy[i].hitbox_rec.x - 6, enemy[i].hitbox_rec.y - 6}, (Color){255, 255, 225, enemy[i].opacity});
    }
    DrawTextureRec(base.texture, base.frame_rec, (Vector2){base.hitbox_rec.x - 40.5f, base.hitbox_rec.y - 50.5f}, WHITE);
    for(int i = 0; i < MAX_NUM_BULLETS; ++i){
      if(bullet[i].active){
        switch(bullet[i].dir){
          case 'd':
            bullet_tmp = bullet[0].texture;
            break;
          case 'a':
            bullet_tmp = bullet[1].texture;
            break;
          case 'w':
            bullet_tmp = bullet[2].texture;
            break;
          case 's':
            bullet_tmp = bullet[3].texture;
            break;
        }
        DrawTextureRec(bullet_tmp, bullet[i].frame_rec, (Vector2){bullet[i].hitbox_rec.x , bullet[i].hitbox_rec.y}, WHITE);
      }
    }
    DrawText(TextFormat("Score:%08i", score), 20, 5, 30, BLACK);
    /*
    switch(wave){
      case FIRST:
        DrawText("WAVE 1", 20, 10, 30, GREEN);
        break;
      case SECOND:
        DrawText("WAVE 2", 20, 10, 30, YELLOW);
        break;
      case THIRD:
        DrawText("WAVE 3", 20, 10, 30, RED);
        break;
    }
    if(victory){
      DrawText("YOU WIN", screen_width / 2 - MeasureText("YOU WIN", 40) / 2, screen_height / 2 - 40, 40, GREEN);
      DrawText("PRESS [ENTER] TO PLAY AGAIN", screen_width / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2, screen_height / 2 + 30, 20, GREEN);
    }
    */
    if(pause) DrawText("GAME PAUSED", screen_width / 2 - MeasureText("GAME PAUSED", 40) / 2, screen_height / 2 - 40, 40, BLACK);
    if(game_over){
      DrawText("GAME OVER!", screen_width / 2 - MeasureText("GAME OVER!", 50) / 2, screen_height / 2 - 50, 50, RED);
      DrawText("PRESS [ENTER] TO PLAY AGAIN", screen_width / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2, screen_height / 2 + 30, 20, RED);
    }
  EndDrawing();
}

void unload_game(){
  // Don't forgor about freeing those sweet vrams
  UnloadTexture(player.texture);
  UnloadTexture(base.texture);
  UnloadTexture(enemy[0].texture);
  for(int i = 0; i < 4; ++i){
    UnloadTexture(bullet[i].texture);
  }
  UnloadTexture(main_screen);
  UnloadTexture(story_controls);
  UnloadTexture(background);
  UnloadFont(font_ttf);
  fclose(high_score);
}
