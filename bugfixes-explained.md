# Bugfixes and Improvements: What Changed and Why

This document explains each change I made, shows the actual code diffs, and clarifies why each fix is better. These are small, safe improvements oriented toward learning and long‑term stability.

---

## 1) Particle constructors: fixed member shadowing with initializer lists

Problem: Parameters had the same names as members and were assigned to themselves (e.g., `mass = mass;`). Members never received values.

Better: Use constructor member initializer lists (or `this->member = param`) to initialize members correctly and efficiently.

Diff (particle.cpp):

```diff
-Particle::Particle(float x, float y, float radius, float mass) {
-  position.x = x;
-  position.y = y;
-  position_prev.x = x;
-  position_prev.y = y;
-  acceleration.x = 0;
-  acceleration.y = 0;
-  mass = mass;
-  radius = radius;
-  bounce = .9f;
-}
+Particle::Particle(float x, float y, float radius, float mass)
+    : position{ x, y }, position_prev{ x, y }, acceleration{ 0, 0 },
+      radius(radius), mass(mass), bounce(0.9f) {}
```

```diff
-Particle::Particle(Vector2 position, float radius, float mass) {
-  position = position;
-  position_prev = position;
-  acceleration.x = 0;
-  acceleration.y = 0;
-  mass = mass;
-  radius = radius;
-  bounce = .9f;
-}
+Particle::Particle(Vector2 pos, float radius, float mass)
+    : position{ pos }, position_prev{ pos }, acceleration{ 0, 0 },
+      radius(radius), mass(mass), bounce(0.9f) {}
```

```diff
-Particle::Particle(float x, float y, float radius, float mass, float bounce) {
-  position.x = x;
-  position.y = y;
-  position_prev.x = x;
-  position_prev.y = y;
-  acceleration.x = 0;
-  acceleration.y = 0;
-  mass = mass;
-  radius = radius;
-  bounce = bounce;
-}
+Particle::Particle(float x, float y, float radius, float mass, float bounce)
+    : position{ x, y }, position_prev{ x, y }, acceleration{ 0, 0 },
+      radius(radius), mass(mass), bounce(bounce) {}
```

Why it’s better:
- Correctness: Members actually get initialized to the intended values.
- Performance: Avoids default-construction then assignment.
- Style: Clear, idiomatic C++.

---

## 2) Particle setters: fixed self-assignment

Problem: Setters assigned the parameter to itself (no effect).

Diff (particle.cpp):

```diff
-void Particle::set_radius(float radius) { radius = radius; }
+void Particle::set_radius(float r) { this->radius = r; }

-float Particle::get_mass() { return mass; }
-void Particle::set_mass(float mass) { mass = mass; }
+float Particle::get_mass() { return mass; }
+void Particle::set_mass(float m) { this->mass = m; }

-float Particle::get_bounce() { return bounce; }
-void Particle::set_bounce(float bounce) { bounce = bounce; }
+float Particle::get_bounce() { return bounce; }
+void Particle::set_bounce(float b) { this->bounce = b; }
```

Why it’s better:
- Correctly updates member state; avoids a no-op.
- `this->` makes member access explicit and avoids shadowing.

---

## 3) Direction/velocity functions: dt safety and clearer math

Problem: Division by very small `dt` could cause huge spikes; `get_direction` used operator division on `Vector2` instead of explicit math.

Diff (particle.cpp):

```diff
 float Particle::get_velocity_x(float dt) {
-  return (position.x - position_prev.x) / dt;
+  if (dt <= 1e-6f) return 0.0f;
+  return (position.x - position_prev.x) / dt;
 }
 
 float Particle::get_velocity_y(float dt) {
-  return (position.y - position_prev.y) / dt;
+  if (dt <= 1e-6f) return 0.0f;
+  return (position.y - position_prev.y) / dt;
 }
 
 Vector2 Particle::get_direction(float dt) {
-  return (Vector2Subtract(position, position_prev)) / dt;
+  if (dt <= 1e-6f) return Vector2{0.0f, 0.0f};
+  return Vector2Scale(Vector2Subtract(position, position_prev), 1.0f / dt);
 }
```

Why it’s better:
- Stability: Guards against divide-by-near-zero artifacts.
- Readability: Uses Raylib’s `raymath` helpers explicitly.

---

## 4) Boundary constraints: resolve both axes independently

Problem: `else if` chain corrected only one axis per frame. At corners, particles could slide or stick.

Diff (particle.cpp):

```diff
-  if (position.x - radius <= 0) {
-    position.x = radius;
-    position_prev.x = position.x + velocity_x * bounce;
-  } else if (position.y - radius <= 0.f) {
-    position.y = radius;
-    position_prev.y = position.y + velocity_y * bounce;
-  } else if (position.x + radius >= width) {
-    position.x = width - radius;
-    position_prev.x = position.x + velocity_x * bounce;
-  }
-  else if (position.y + radius >= height) {
-    position.y = height - radius;
-    position_prev.y = position.y + velocity_y * bounce;
-  }
+  // Handle X axis independently
+  if (position.x - radius <= 0.0f) {
+    position.x = radius;
+    position_prev.x = position.x + velocity_x * bounce;
+  } else if (position.x + radius >= width) {
+    position.x = width - radius;
+    position_prev.x = position.x + velocity_x * bounce;
+  }
+
+  // Handle Y axis independently
+  if (position.y - radius <= 0.0f) {
+    position.y = radius;
+    position_prev.y = position.y + velocity_y * bounce;
+  } else if (position.y + radius >= height) {
+    position.y = height - radius;
+    position_prev.y = position.y + velocity_y * bounce;
+  }
```

Why it’s better:
- Correctness: Resolves corner collisions properly.
- More natural bounce behavior.

---

## 5) Collision circle check: parameter shadowing fixed

Problem: The function used the parameter `radius` for both circles, shadowing the member radius.

Diff (particle.cpp):

```diff
-bool Particle::check_collision_circle(float x, float y, float radius) {
-  return CheckCollisionCircles(Vector2{position.x, position.y}, radius,
-                               Vector2{x, y}, radius);
-}
+bool Particle::check_collision_circle(float x, float y, float r) {
+  return CheckCollisionCircles(Vector2{position.x, position.y}, this->radius,
+                               Vector2{x, y}, r);
+}
```

Why it’s better:
- Correctness: Uses the particle’s own radius for the first circle and the provided `r` for the other.

---

## 6) Randomness: seed once, don’t reseed every frame

Changes:
- Seeded RNG once in `main.cpp` with `SetRandomSeed((unsigned int)time(nullptr));`
- Removed reseeding from `Motor::apply_random_force()`.

Diffs:

```diff
 // main.cpp
 InitWindow(screenWidth, screenHeight, "verlz");
+// Seed RNG once for the app
+SetRandomSeed((unsigned int)time(nullptr));
```

```diff
 // motor.cpp
-void Motor::apply_random_force() {
-  SetRandomSeed(GetTime());
+void Motor::apply_random_force() {
   for (Particle &p : particles) {
     float random_force_x = GetRandomValue(-500, 500);
     float random_force_y = GetRandomValue(-500, 500);
     p.accelerate(random_force_x, random_force_y);
   }
 }
```

Why it’s better:
- Predictability and statistical correctness: Reseeding per frame wrecks randomness and can cause patterns.
- Good practice: Seed once (optionally make the seed user-configurable for determinism).

---

## 7) Random spawn axis fix

Problem: X and Y were swapped when generating random coordinates.

Diff (main.cpp):

```diff
-  float random_x = GetRandomValue(0, GetScreenHeight());
-  float random_y = GetRandomValue(0, GetScreenWidth());
+  float random_x = GetRandomValue(0, GetScreenWidth());
+  float random_y = GetRandomValue(0, GetScreenHeight());
```

Why it’s better:
- Correctness: X should be within width; Y within height.

---

## 8) Minor type and tooling improvements

- DrawText int types:

```diff
-DrawText("PARTICLE A", a.get_x(), a.get_y(), 4.f, RAYWHITE);
+DrawText("PARTICLE A", (int)a.get_x(), (int)a.get_y(), 4, RAYWHITE);
```

- Added stronger warnings in Makefile:

```diff
-CXXFLAGS = -std=c++14 -Wall -Wextra -O2
+CXXFLAGS = -std=c++14 -Wall -Wextra -Wshadow -O2
```

Why it’s better:
- Catches shadowing bugs automatically in the future.
- Keeps draw code explicit and consistent.

---

## 9) Not changed: early-return inside collision loop

I looked for a `return`-vs-`continue` bug inside `Motor::check_collision_particles`, but that exact code isn’t present in your current file, so no change was made there. If you see that pattern later (e.g., `if (vel_along_normal > 0) return;` inside a nested loop), switch it to `continue;` so it only skips that pair.

---

## Why these changes make the code better

- Fewer hidden bugs: Shadowing and self-assignment are classic “looks fine but does nothing” issues. Fixing them stabilizes everything downstream.
- More stable numerics: dt guards and independent axis constraints reduce unpredictable behavior.
- Predictable randomness: Seeding once avoids subtle artifacts and enables deterministic playback with a fixed seed.
- Safer, clearer code: Stronger warnings and explicit math functions make future refactors easier.

Overall, these fixes strengthen correctness, maintainability, and learning value without adding complexity.
