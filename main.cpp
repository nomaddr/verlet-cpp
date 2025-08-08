#include "motor.h"
#include "particle.h"
#include "stick.h"
#include <ctime>
#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <vector>

class EnvItem {
public:
  Vector2 pos;
  float width;
  float height;
  bool collision;
};

Particle spawn_particle_rand(float radius, float mass) {
  float random_x = GetRandomValue(0, GetScreenWidth());
  float random_y = GetRandomValue(0, GetScreenHeight());
  return Particle{Vector2{random_x, random_y}, radius, mass};
}

int main() {
  Color darkGreen = Color{20, 160, 133, 255};

  const int screenWidth = 1200;
  const int screenHeight = 800;

  InitWindow(screenWidth, screenHeight, "verlz");

  // Seed RNG once for the app
  SetRandomSeed((unsigned int)time(nullptr));

  // INIT VARIABLES

  float dt;
  float default_mass = 1.0f;
  float default_radius = 5.0f;
  // Vector2 origin = {screenWidth / 2, screenHeight / 2};
  Particle player({screenWidth / 2, screenHeight / 2}, 5.f, 2.f);
  Motor motor(0.f, 500.f);

  motor.particles.push_back(player);

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    // EVENT HANDLING -----------------------------
    dt = GetFrameTime();

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      // set prev to opposite of mouse coord
      const float vel_x = (player.get_x() - GetMouseX()) * -1;
      const float vel_y = (player.get_y() - GetMouseY()) * -1;

      // player.set_velocity(GetMouseX(), GetMouseY(), dt); // normalize maybe;
      // player.add_velocity(5000.f, dt);
      // player.accelerate();
    }
    if (IsKeyDown(KEY_G)) {
      motor.spawn_particle(GetMouseX(), GetMouseY(), default_radius,
                           default_mass);
    }

    if (IsKeyDown(KEY_SPACE)) {
    }

    // --------------------
    //        UPDATE
    // --------------------
    // stick1.update();

    // player.update(dt);
    motor.update(dt);
    // --------------------
    //        DRAW
    // --------------------

    BeginDrawing();
    ClearBackground(darkGreen);
    // stick1.draw();

    motor.draw();
    // player.draw();

    EndDrawing();
  }

  CloseWindow();

  return 0;
}
