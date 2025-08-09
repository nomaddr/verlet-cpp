# Updates 1: Iterative Solver and Collision Bounce (Learning Guide)

This update improves collision resolution quality by:
- Iterating the constraint/collision solver several times per frame, and
- Applying a restitution-aware bounce that is compatible with Verlet integration by adjusting previous positions (via `set_velocity`).

## What changed

1) Iterative solver loop in `Motor::update`
- Before: One pass of `update_sticks()` and `check_collision_particles()` per frame.
- After: We run a small loop (default 3 iterations) each frame:

```cpp
const int iterations = 3; // tweak 2..6
for (int i = 0; i < iterations; ++i) {
  update_sticks();
  check_collision_particles(dt);
  for (Particle &p : particles) {
    p.constrain_rectangle(GetScreenWidth(), GetScreenHeight());
  }
}
```

Why: Multiple passes allow corrections to propagate across chains/clusters, preventing residual overlap and improving stability.

2) Stronger separation in collisions
- We increased `response_coef` from 0.75f to 1.0f in `check_collision_particles`. With iterations, full separation per overlap is fine and avoids lingering penetration.

3) Verlet-friendly bounce (restitution)
- After positional separation, we infer each particle's current per-second velocity with `get_velocity(dt)`, convert to per-step displacement, apply an impulse along the collision normal using the lower of the two bounces, then convert back to per-second and call `set_velocity(vx, vy, dt)`.

Key snippet in `Motor::check_collision_particles` after separation:

```cpp
// After p1.set_position, p2.set_position
if (dt > 1e-6f) {
  Vector2 v1_ps = p1.get_velocity(dt);
  Vector2 v2_ps = p2.get_velocity(dt);
  Vector2 v1_step = { v1_ps.x * dt, v1_ps.y * dt };
  Vector2 v2_step = { v2_ps.x * dt, v2_ps.y * dt };

  float relN = (v1_step.x - v2_step.x) * n_x + (v1_step.y - v2_step.y) * n_y;
  if (relN < 0.0f) {
    float e = std::min(p1.get_bounce(), p2.get_bounce());
    float m1 = p1.get_mass(), m2 = p2.get_mass();
    float invMassSum = (1.0f / m1) + (1.0f / m2);
    if (invMassSum > 0.0f) {
      float j = -(1.0f + e) * relN / invMassSum;
      float jnx = j * n_x, jny = j * n_y;
      v1_step.x += jnx / m1; v1_step.y += jny / m1;
      v2_step.x -= jnx / m2; v2_step.y -= jny / m2;
      Vector2 v1_ps_new = { v1_step.x / dt, v1_step.y / dt };
      Vector2 v2_ps_new = { v2_step.x / dt, v2_step.y / dt };
      p1.set_velocity(v1_ps_new.x, v1_ps_new.y, dt);
      p2.set_velocity(v2_ps_new.x, v2_ps_new.y, dt);
    }
  }
}
```

Why this works with Verlet: In Verlet, `prev` encodes velocity (v ≈ cur - prev). Adjusting velocity means recomputing `prev` using `set_velocity`. This keeps the next integration step consistent with your collision response rather than letting it "undo" the separation.

## Concepts you learn here
- Iterative constraint solving: Why multiple passes improve convergence and reduce penetration.
- Restitution along the collision normal: Only adjust the component of relative velocity that points into the collision; tangential motion is preserved.
- Mass-weighted impulses: Heavier bodies move less.
- Position vs. velocity correction in Verlet: How to translate impulses into `prev` updates.

## Tuning tips
- iterations: 2..6 depending on scene complexity. More iterations = firmer structures and fewer overlaps.
- e (bounce): Use 0.0 for no bounce, up to 1.0 for perfectly elastic. We used `min(bounce1, bounce2)` for realism.
- dt: Consider a fixed timestep and/or clamp spikes to keep penetration shallow per step.

## Next steps (optional but recommended)
- Fixed timestep accumulator with a timeScale to explore slow motion. This keeps solver behavior consistent when you vary render frame rate.
- Spatial grid broad-phase from `optimized-collision.md` to scale to hundreds/thousands of particles.
- Expose `iterations` and `timeScale` to keybinds and to your debug overlay so you can feel the effects live.
