# Patch 2: Robust Collision, Epsilon Handling, and Damping (Guide)

This patch addresses particles “melting” into each other and odd jumpy behavior by fixing collision ordering, adding an epsilon fallback, removing duplicate bounds constraints, and outlining parameter tuning.

## What changed (code-level)

1) Sample velocities before separation
- In `Motor::check_collision_particles`, we now capture per-second velocities via `get_velocity(dt)` before changing positions. After separation, we apply a restitution impulse along the collision normal using those pre-correction velocities and write back via `set_velocity`.

2) Epsilon and fallback normal
- When two particles are extremely close (`dist < 1e-6`), we use a fallback normal `{1,0}` and `dist_safe = 1e-6` rather than skipping resolution. This avoids persistent overlaps.

3) Remove duplicate bounds constraints
- We removed `constrain_rectangle` calls from `update_positions`. Bounds are now applied only inside the solver iteration loop in `Motor::update`, reducing double-corrections and energy injection.

4) Keep iterative solver and full separation
- The solver still runs 3 iterations by default and uses `response_coef = 1.0f`.

### Key diffs

- In `motor.cpp` (collision loop):
```diff
+        // Sample velocities BEFORE changing them with new prev positions
+        Vector2 v1_ps = p1.get_velocity(dt);
+        Vector2 v2_ps = p2.get_velocity(dt);
...
-        // Avoid division by zero
-        if (dist < 0.001f) {
-          continue;
-        }
-
-        const float n_x = vel_x / dist;
-        const float n_y = vel_y / dist;
+        // Avoid division by zero and provide fallback normal
+        float n_x, n_y;
+        float dist_safe = dist;
+        if (dist < 1e-6f) {
+          n_x = 1.0f; n_y = 0.0f; // arbitrary
+          dist_safe = 1e-6f;
+        } else {
+          n_x = vel_x / dist;
+          n_y = vel_y / dist;
+        }
...
-        const float overlap = min_dist - dist;
+        const float overlap = min_dist - dist_safe;
...
-        // Verlet-friendly bounce: adjust previous-position-encoded velocities
-        if (dt > 1e-6f) {
-          // Current per-second velocities inferred from position and prev
-          Vector2 v1_ps = p1.get_velocity(dt);
-          Vector2 v2_ps = p2.get_velocity(dt);
+        // Verlet-friendly bounce: adjust previous-position-encoded velocities
+        if (dt > 1e-6f) {
+          // Use pre-correction velocities captured above
           Vector2 v1_step = { v1_ps.x * dt, v1_ps.y * dt };
           Vector2 v2_step = { v2_ps.x * dt, v2_ps.y * dt };
           float relN = (v1_step.x - v2_step.x) * n_x + (v1_step.y - v2_step.y) * n_y;
           if (relN < 0.0f) {
             float e = std::min(p1.get_bounce(), p2.get_bounce());
             ...
             p1.set_velocity(v1_ps_new.x, v1_ps_new.y, dt);
             p2.set_velocity(v2_ps_new.x, v2_ps_new.y, dt);
           }
         }
```

- In `Motor::update_positions`:
```diff
-  for (Particle &p : particles) {
-    p.update(dt);
-    p.constrain_rectangle(GetScreenWidth(), GetScreenHeight());
-  }
+  for (Particle &p : particles) {
+    p.update(dt);
+  }
```

## Why these fixes help
- Pre-separation velocities: Prevents computing bounce from already-corrected positions, avoiding energy artifacts and “snap-back.”
- Epsilon fallback: Ensures even coincident particles get a meaningful separation direction and magnitude.
- Single bounds pass per iteration: Avoids double corrections that cause jitter/teleporting.
- Iterations + full separation: Multiple passes converge constraints and clear residual overlap.

## Parameter tuning and what they do
- bounce (Particle::bounce)
  - Controls restitution (bounciness). Used for particle-particle and world collisions.
  - Start low (0.2–0.5). High values (0.9) can be jittery without damping.

- mass (Particle::mass)
  - Affects separation and impulse: heavier particles move less and change velocity less.
  - Keep within moderate ranges initially (0.5–5.0).

- gravity/force (Motor::force_y)
  - Acceleration in px/s^2. With dt≈1/60, displacement from gravity is ~a*dt^2 per step.
  - Start ~800–1200. If overlap persists, reduce gravity or add more iterations.

- iterations (in Motor::update)
  - 2–6 typical; more iterations = firmer constraints and fewer overlaps.

- damping (optional next step)
  - To reduce jitter, you can apply slight damping to velocities each iteration:

```cpp
for (Particle &p : particles) {
  Vector2 v = p.get_velocity(dt);
  v.x *= 0.98f; v.y *= 0.98f;
  p.set_velocity(v.x, v.y, dt);
}
```

## Next steps
- If you still see melting or jitter under high gravity, consider:
  - Fixed timestep accumulator (1/60) or clamp dt.
  - Spatial grid broad-phase to reduce deep overlaps from large clusters.
  - Add keybinds to tweak bounce, gravity, iterations live and observe behavior in real time.
