# Optimized Collision: A Practical Guide (with learning steps and snippets)

This guide helps you implement an O(n)ish spatial grid for particle collisions, learn the underlying concepts, and integrate it into your current code base step by step. It includes diffs/snippets you can paste directly.

## Why a spatial grid?

- Problem: Your current all-pairs collision check is O(n²). As particle count grows, FPS drops quickly.
- Idea: Only check likely colliders by partitioning space into cells of size ~ average particle diameter. Particles can only collide with others in the same or neighboring cells.
- Result: Typical complexity approaches O(n) for uniform distributions. You also get cleaner structure for future optimizations.

## Core concepts to learn

- Uniform grids and hashing: Map (x, y) -> (cellX, cellY) using integer division by `cellSize`.
- Neighbor iteration: For each cell, only check that cell and its 8 neighbors.
- Pair de-duplication: Ensure each pair is tested once (e.g., by ordering indices and storing a small hash or by careful traversal order).
- Parameter choice: `cellSize ≈ 2 * maxRadius` is a good start. If radii vary widely, consider dynamic insertion into multiple adjacent cells (advanced).

## Minimal design for your code

We’ll add a new, optional optimized path:

- motor.h: declare `void check_collision_particles_optimized(float dt);`
- motor.cpp: implement a simple uniform grid with integer hashing and use it instead of the O(n²) loop when enabled.

You can keep both versions and toggle by a flag (e.g., a key press or compile-time macro) for learning and benchmarking.

## Step-by-step implementation

### 1) Add method declaration in `motor.h`

```diff
 class Motor
 {
 public:
   Motor(float force_x, float force_y);
   ...
   void check_collision_particles(float dt);
+  void check_collision_particles_optimized(float dt);
   void draw_debug();
 };
```

### 2) Add helpers in `motor.cpp`

Place these near the top of `motor.cpp` (above methods or in an unnamed namespace).

```cpp
namespace {
  struct CellCoord { int x, y; };

  inline CellCoord toCell(float x, float y, float cellSize) {
    return { static_cast<int>(std::floor(x / cellSize)),
             static_cast<int>(std::floor(y / cellSize)) };
  }

  inline std::uint64_t packKey(int cx, int cy) {
    // Pack two 32-bit ints into one 64-bit key (handles negatives too)
    return (static_cast<std::uint64_t>(static_cast<std::uint32_t>(cx)) << 32) |
           (static_cast<std::uint32_t>(cy));
  }
}
```

### 3) Implement the optimized collision in `motor.cpp`

- Data structure: `unordered_map<uint64_t, vector<int>> grid;`
- Build phase: For each particle, compute its cell and push its index into the grid bucket.
- Pair phase: For each particle/cell, iterate that cell and its neighbors, test pairs once.

```cpp
#include <unordered_map>

void Motor::check_collision_particles_optimized(float dt) {
  const float response_coef = 0.75f;
  const int count = static_cast<int>(particles.size());
  if (count < 2) return;

  // Choose cell size ≈ 2 * maxRadius (simple heuristic)
  float maxR = 0.0f;
  for (const auto& p : particles) maxR = std::max(maxR, p.get_radius());
  const float cellSize = std::max(1.0f, 2.0f * maxR);

  std::unordered_map<std::uint64_t, std::vector<int>> grid;
  grid.reserve(count * 2);

  // Build grid
  for (int i = 0; i < count; ++i) {
    const auto pos = particles[i].get_position();
    CellCoord c = toCell(pos.x, pos.y, cellSize);
    grid[packKey(c.x, c.y)].push_back(i);
  }

  // Neighbor offsets (the 9-cell Moore neighborhood)
  static const int OFF[9][2] = {
    {-1,-1},{0,-1},{1,-1},
    {-1, 0},{0, 0},{1, 0},
    {-1, 1},{0, 1},{1, 1}
  };

  // For simple de-duplication: only accept pairs (i,j) where i < j
  for (const auto& entry : grid) {
    // Current cell coordinate
    const std::uint64_t key = entry.first;
    const int cx = static_cast<int>(static_cast<std::int32_t>(key >> 32));
    const int cy = static_cast<int>(static_cast<std::int32_t>(key & 0xFFFFFFFF));

    for (int n = 0; n < 9; ++n) {
      const int nx = cx + OFF[n][0];
      const int ny = cy + OFF[n][1];
      const auto itA = grid.find(key);
      const auto itB = grid.find(packKey(nx, ny));
      if (itA == grid.end() || itB == grid.end()) continue;

      const auto& A = itA->second;
      const auto& B = itB->second;

      // If same bucket, nested loops with j>i to avoid duplicates
      if (nx == cx && ny == cy) {
        for (size_t ai = 0; ai < A.size(); ++ai) {
          const int i = A[ai];
          for (size_t bi = ai + 1; bi < A.size(); ++bi) {
            const int j = A[bi];
            // Narrow phase
            narrowPhaseResolve(i, j, dt, response_coef);
          }
        }
      } else {
        // Different buckets: all pairs i in A, j in B with i<j
        for (int i : A) {
          for (int j : B) {
            if (i < j) narrowPhaseResolve(i, j, dt, response_coef);
          }
        }
      }
    }
  }
}
```

You’ll need a small helper to keep the narrow-phase logic readable. Extract your current body from `check_collision_particles` into a helper function that operates on indices:

```cpp
void Motor::narrowPhaseResolve(int i, int j, float dt, float response_coef) {
  Particle& p1 = particles[i];
  Particle& p2 = particles[j];

  const float dx = p1.get_x() - p2.get_x();
  const float dy = p1.get_y() - p2.get_y();
  const float rsum = p1.get_radius() + p2.get_radius();
  const float dist2 = dx*dx + dy*dy;
  if (dist2 >= rsum*rsum) return; // no overlap

  const float dist = std::sqrt(dist2);
  if (dist < 1e-4f) return; // avoid singularities

  const float nx = dx / dist;
  const float ny = dy / dist;

  // Mass-based separation (your current approach)
  const float total_mass = p1.get_mass() + p2.get_mass();
  const float m1 = p2.get_mass() / total_mass;
  const float m2 = p1.get_mass() / total_mass;

  const float overlap = rsum - dist;
  const float separation = overlap * response_coef;

  auto pos1 = p1.get_position();
  auto pos2 = p2.get_position();

  pos1.x -= nx * (m1 * separation);
  pos1.y -= ny * (m1 * separation);
  pos2.x += nx * (m2 * separation);
  pos2.y += ny * (m2 * separation);

  p1.set_position(pos1.x, pos1.y);
  p2.set_position(pos2.x, pos2.y);
}
```

Then declare it in `motor.h` (private) or keep it `static`/in an unnamed namespace if you move `particles` access accordingly. Minimal change:

```diff
 class Motor {
 public:
   ...
   void check_collision_particles_optimized(float dt);
   void draw_debug();
 private:
+  void narrowPhaseResolve(int i, int j, float dt, float response_coef);
 };
```

Finally, switch which function you call in `Motor::update`:

```diff
-  check_collision_particles(dt);
+  // Toggle between naive and optimized
+  // check_collision_particles(dt);
+  check_collision_particles_optimized(dt);
```

### 4) Stretch goal: handle large particles spanning multiple cells

If some particles have radius > cellSize/2, a particle can overlap neighboring cells. The simple solution is to insert it into all cells that its AABB overlaps:

```cpp
// Compute min/max cell indices using (x-r, y-r) to (x+r, y+r)
int minCx = static_cast<int>(std::floor((pos.x - r) / cellSize));
int maxCx = static_cast<int>(std::floor((pos.x + r) / cellSize));
int minCy = static_cast<int>(std::floor((pos.y - r) / cellSize));
int maxCy = static_cast<int>(std::floor((pos.y + r) / cellSize));
for (int cx = minCx; cx <= maxCx; ++cx)
  for (int cy = minCy; cy <= maxCy; ++cy)
    grid[packKey(cx, cy)].push_back(i);
```

This retains correctness at the cost of a bit more insertion work.

## Learning path and exercises

1) Baseline and measure
- Log particle counts and frame times with the current O(n²) method.
- Create a debug toggle key to switch between naive and optimized methods.
- Observe scaling (e.g., 100, 200, 500, 1000 particles).

2) Implement the minimal grid
- Use `cellSize = 2 * maxRadius`.
- Insert each particle by its center cell only.
- Iterate 9 neighbors.
- Verify correctness with simple scenes (clusters, lines, corners).

3) Robustness improvements
- Insert into multiple cells for large radii.
- Add assertions/sanitizers in Debug builds.
- Add unit tests for `toCell`, `packKey`, and `narrowPhaseResolve` using doctest/Catch2 (optional but great practice).

4) Performance tuning
- Reserve bucket sizes (e.g., `grid.reserve(n*2)`).
- Profile allocation hotspots (enable `-O2 -g` and use a profiler).
- Benchmark different `cellSize` values.

5) Extensions
- Add broad-phase AABBs and only do circle-circle on overlap.
- Move to SoA for hot data (positions/radii) if you want more perf.
- Parallelize per-cell updates when comfortable with threading.

## Common pitfalls

- Duplicated pairs: Always enforce `i < j` or store a visited set.
- Edge cells: Don’t forget to check neighbor cells even across negative indices.
- Large dt spikes: Consider a fixed timestep accumulator for stability.
- Determinism: If you randomize, seed once and expose seed in debug UI.

## Optional: Minimal diffs to wire it in

- motor.h: add two lines as shown (declaration and helper).
- motor.cpp: add namespace helpers, add the method, extract `narrowPhaseResolve`, call optimized version from `update`.
- main.cpp: add a keyboard toggle to switch algorithms at runtime for learning (e.g., press 'C' to toggle).

That’s it. Start small, measure, then iterate. This path leads you to data structures (hashing, grids), algorithmic complexity, and practical engine design—all with simple, approachable steps.
