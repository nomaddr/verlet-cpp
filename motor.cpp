#include "motor.h"
#include "particle.h"
#include "raylib.h"
#include "stick.h"
#include <cmath>
#include <iostream>
#include <string>
#include <algorithm>

Motor::Motor(float force_x, float force_y) {
  this->force_x = force_x;
  this->force_y = force_y;
}

void Motor::update(float dt) {
  // 1) Apply forces (a = F/m)
  apply_force(force_x, force_y);

  // 2) Integrate positions (Verlet)
  update_positions(dt);

  // 3) Iteratively solve constraints and collisions for stability
  const int iterations = 3; // tweak 2..6 depending on stability desired
  for (int i = 0; i < iterations; ++i) {
    update_sticks();
    check_collision_particles(dt);
    // Re-apply world bounds each iteration for robust corner handling
    for (Particle &p : particles) {
      p.constrain_rectangle(GetScreenWidth(), GetScreenHeight());
    }
  }

  // applyConstraint(); // TODO: Circle/world constraints if needed
}

void Motor::draw() {

  for (Particle &p : particles) {
    p.draw();
  }
  for (Stick &s : sticks) {
    s.draw(particles);
  }
  draw_debug();
}

void Motor::update_positions(float dt) {

  for (Particle &p : particles) {
    p.update(dt);
    p.constrain_rectangle(GetScreenWidth(), GetScreenHeight());
  }
}

void Motor::update_sticks() {

  for (Stick &s : sticks) {
    s.update(particles);
  }
}

void Motor::apply_force(float force_x, float force_y) {
  for (Particle &p : particles) {
    p.accelerate(force_x, force_y);
  }
}

void Motor::apply_random_force() {
  for (Particle &p : particles) {
    // Apply random force in a more reasonable range
    float random_force_x = GetRandomValue(-500, 500);
    float random_force_y = GetRandomValue(-500, 500);
    p.accelerate(random_force_x, random_force_y);
  }
}

void Motor::spawn_particle(float x, float y, float radius, float mass) {
  // Add bounds checking and reasonable limits
  if (radius <= 0.0f || radius > 100.0f) {
    radius = 5.0f; // Default radius
  }
  if (mass <= 0.0f || mass > 1000.0f) {
    mass = 1.0f; // Default mass
  }

  // Limit total number of particles to prevent performance issues
  if (particles.size() >= 500) {
    return; // Don't add more particles
  }

  Particle particle(x, y, radius, mass);
  particles.push_back(particle);
}

void Motor::spawn_stick(int particle_a_index, int particle_b_index,
                        float length) {
  // Bounds checking to ensure valid indices
  if (particle_a_index < 0 || particle_a_index >= particles.size() ||
      particle_b_index < 0 || particle_b_index >= particles.size() ||
      particle_a_index == particle_b_index) {
    return; // Invalid indices or same particle, don't create stick
  }

  Stick stick(particle_a_index, particle_b_index, length);
  sticks.push_back(stick);
}

void Motor::check_collision_particles(float dt) {
  const float response_coef = 1.0f;
  const int particle_count = particles.size();

  // Early exit if not enough particles for collision
  if (particle_count < 2) {
    return;
  }

  // Iterate Particles
  for (int i = 0; i < particle_count; i++) {
    Particle &p1 = particles.at(i);
    // Iterate on object involved in new collision pairs
    for (int k = i + 1; k < particle_count; k++) {
      Particle &p2 = particles.at(k);
      const float vel_x = p1.get_x() - p2.get_x();
      const float vel_y = p1.get_y() - p2.get_y();

      const float dist2 = vel_x * vel_x + vel_y * vel_y;
      const float min_dist = p1.get_radius() + p2.get_radius();
      // Check overlapping
      if (dist2 < min_dist * min_dist) {
        const float dist = sqrt(dist2);

        // Avoid division by zero
        if (dist < 0.001f) {
          continue;
        }

        const float n_x = vel_x / dist;
        const float n_y = vel_y / dist;

        // Use actual mass for proper physics response
        const float total_mass = p1.get_mass() + p2.get_mass();
        const float mass_ratio_1 =
            p2.get_mass() / total_mass; // p1 moves based on p2's mass
        const float mass_ratio_2 =
            p1.get_mass() / total_mass; // p2 moves based on p1's mass

        const float overlap = min_dist - dist;
        const float separation = overlap * response_coef;

        // Update positions based on mass ratios
        Vector2 pos1 = p1.get_position();
        Vector2 pos2 = p2.get_position();

        // Move particles apart proportional to their mass ratios
        pos1.x = pos1.x - n_x * (mass_ratio_1 * separation);
        pos1.y = pos1.y - n_y * (mass_ratio_1 * separation);
        pos2.x = pos2.x + n_x * (mass_ratio_2 * separation);
        pos2.y = pos2.y + n_y * (mass_ratio_2 * separation);

        p1.set_position(pos1.x, pos1.y);
        p2.set_position(pos2.x, pos2.y);

        // Verlet-friendly bounce: adjust previous-position-encoded velocities
        if (dt > 1e-6f) {
          // Current per-second velocities inferred from position and prev
          Vector2 v1_ps = p1.get_velocity(dt);
          Vector2 v2_ps = p2.get_velocity(dt);
          // Convert to per-step velocities (displacement per step)
          Vector2 v1_step = { v1_ps.x * dt, v1_ps.y * dt };
          Vector2 v2_step = { v2_ps.x * dt, v2_ps.y * dt };

          // Relative velocity along collision normal (per-step)
          float relN = (v1_step.x - v2_step.x) * n_x + (v1_step.y - v2_step.y) * n_y;
          if (relN < 0.0f) { // approaching along normal
            float e = std::min(p1.get_bounce(), p2.get_bounce());
            float m1 = p1.get_mass();
            float m2 = p2.get_mass();
            float invMassSum = (1.0f / m1) + (1.0f / m2);
            if (invMassSum > 0.0f) {
              float j = -(1.0f + e) * relN / invMassSum; // impulse scalar (per-step)
              float jnx = j * n_x;
              float jny = j * n_y;
              v1_step.x += jnx / m1;
              v1_step.y += jny / m1;
              v2_step.x -= jnx / m2;
              v2_step.y -= jny / m2;

              // Back to per-second velocities and write to prev via set_velocity
              Vector2 v1_ps_new = { v1_step.x / dt, v1_step.y / dt };
              Vector2 v2_ps_new = { v2_step.x / dt, v2_step.y / dt };
              p1.set_velocity(v1_ps_new.x, v1_ps_new.y, dt);
              p2.set_velocity(v2_ps_new.x, v2_ps_new.y, dt);
            }
          }
        }
      }
    }
  }
}

// void Motor::applyConstraint()
// {
//     const Vector2 position = {800.0f, 450.0f};
//     const float radius = 400.0f;
//     for (Particle* p : particles)
//     {
//         const Vector2 toVec = Vector2Subtract(p->cur, position);
//         const float dist = Vector2Length(toVec);

//         if (dist > radius - 50)
//         {
//             const Vector2 n = Vector2Scale(toVec, 1.f / dist);
//             p->cur = Vector2Add(position, Vector2Scale(n, (dist - 50.f)));
//         }
//     }
// }

void Motor::draw_debug() {
  Color dbg_col = ORANGE;
  DrawRectangleLines(10, 10, 200, 80, dbg_col);
  DrawText("Particles:", 15, 15, 18, dbg_col);
  DrawText(std::to_string(particles.size()).c_str(), 100, 15, 18, dbg_col);
  DrawText("FPS:", 15, 35, 18, dbg_col);
  DrawText(std::to_string(GetFPS()).c_str(), 100, 35, 18, dbg_col);
  DrawText("Sticks:", 15, 55, 18, dbg_col);
  DrawText(std::to_string(sticks.size()).c_str(), 100, 55, 18, dbg_col);

  // Draw Particle IDs (only for reasonable number of particles to avoid
  // performance issues)
  if (particles.size() <= 50) {
    for (int i = 0; i < particles.size(); i++) {
      // Use static buffer to avoid string allocation every frame
      char id_text[8];
      snprintf(id_text, sizeof(id_text), "%d", i);
      DrawText(id_text, particles[i].get_x() - 5, particles[i].get_y() - 5, 12,
               BLACK);
    }
  }
}
