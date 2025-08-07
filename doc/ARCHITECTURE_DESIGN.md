# 🏗️ Modular Architecture Design

## 🎯 **Design Philosophy**

This physics engine is built with **modularity** and **extensibility** as core principles. Each system is designed to be:
- **Independent**: Can be modified without affecting other systems
- **Pluggable**: Easy to add new components
- **Testable**: Each module can be unit tested in isolation
- **Performant**: Optimized for real-time game development

---

## 📊 **System Architecture Overview**

```
┌─────────────────────────────────────────────────────────────┐
│                        GAME LAYER                           │
├─────────────────────────────────────────────────────────────┤
│  main.cpp  │  Input Handler  │  Scene Manager  │  UI System │
├─────────────────────────────────────────────────────────────┤
│                      PHYSICS ENGINE                         │
├─────────────────────────────────────────────────────────────┤
│   Motor    │  Constraints   │   Forces    │   Collision    │
│ (Manager)  │    System      │   System    │    System      │
├─────────────────────────────────────────────────────────────┤
│                      CORE SYSTEMS                           │
├─────────────────────────────────────────────────────────────┤
│ Particles  │ Spatial Grid  │ Memory Pool │  Math Utils    │
├─────────────────────────────────────────────────────────────┤
│                     FOUNDATION                              │
├─────────────────────────────────────────────────────────────┤
│           Raylib Graphics  │  STL Containers               │
└─────────────────────────────────────────────────────────────┘
```

---

## 🧩 **Core Component Design**

### **1. Entity-Component Pattern**

#### **Base Entity System**
```cpp
// Future expansion: Entity-Component-System
class Entity {
    uint32_t id;
    std::bitset<MAX_COMPONENTS> component_mask;
};

class Component {
    virtual ~Component() = default;
};

class System {
    virtual void update(float dt) = 0;
    virtual void render() = 0;
};
```

#### **Current Simplified Approach**
```cpp
// Current: Direct object management
class Particle {
    // Self-contained physics object
    // Easy to understand and modify
    // Direct performance optimization
};
```

### **2. Plugin Architecture for Forces**

#### **Force Interface**
```cpp
class IForce {
public:
    virtual ~IForce() = default;
    virtual Vector2 calculateForce(const Particle& particle, float dt) const = 0;
    virtual void render() const {}
    virtual bool isEnabled() const { return enabled; }
    virtual void setEnabled(bool enabled) { this->enabled = enabled; }

protected:
    bool enabled = true;
};
```

#### **Concrete Force Implementations**
```cpp
class GravityForce : public IForce {
    Vector2 direction;
    float strength;
public:
    Vector2 calculateForce(const Particle& particle, float dt) const override;
};

class WindForce : public IForce {
    Vector2 direction;
    float strength;
    float turbulence;
public:
    Vector2 calculateForce(const Particle& particle, float dt) const override;
};

class MagneticForce : public IForce {
    Vector2 center;
    float strength;
    bool attractive;
public:
    Vector2 calculateForce(const Particle& particle, float dt) const override;
};
```

#### **Force Manager**
```cpp
class ForceManager {
    std::vector<std::unique_ptr<IForce>> forces;
    
public:
    template<typename T, typename... Args>
    void addForce(Args&&... args) {
        forces.push_back(std::make_unique<T>(std::forward<Args>(args)...));
    }
    
    Vector2 calculateTotalForce(const Particle& particle, float dt) const {
        Vector2 total = {0, 0};
        for (const auto& force : forces) {
            if (force->isEnabled()) {
                total = Vector2Add(total, force->calculateForce(particle, dt));
            }
        }
        return total;
    }
    
    void render() const {
        for (const auto& force : forces) {
            force->render();
        }
    }
};
```

### **3. Constraint System Architecture**

#### **Base Constraint Interface**
```cpp
class IConstraint {
public:
    virtual ~IConstraint() = default;
    virtual void solve(std::vector<Particle>& particles) = 0;
    virtual void render(const std::vector<Particle>& particles) const = 0;
    virtual bool isValid(const std::vector<Particle>& particles) const = 0;
    
    void setEnabled(bool enabled) { this->enabled = enabled; }
    bool isEnabled() const { return enabled; }

protected:
    bool enabled = true;
};
```

#### **Constraint Implementations**
```cpp
class DistanceConstraint : public IConstraint {
    int particle_a, particle_b;
    float target_distance;
    float stiffness;
    
public:
    void solve(std::vector<Particle>& particles) override;
    void render(const std::vector<Particle>& particles) const override;
    bool isValid(const std::vector<Particle>& particles) const override;
};

class AngleConstraint : public IConstraint {
    int particle_a, particle_b, particle_c;
    float target_angle;
    float stiffness;
    
public:
    void solve(std::vector<Particle>& particles) override;
    // ... other methods
};

class SpringConstraint : public IConstraint {
    int particle_a, particle_b;
    float rest_length;
    float spring_constant;
    float damping;
    
public:
    void solve(std::vector<Particle>& particles) override;
    // ... other methods
};
```

---

## 🔧 **Extensibility Patterns**

### **1. Factory Pattern for Particle Types**

```cpp
enum class ParticleType {
    BASIC,
    HEAVY,
    LIGHT,
    BOUNCY,
    STICKY,
    LIQUID
};

class ParticleFactory {
public:
    static Particle createParticle(ParticleType type, float x, float y) {
        switch (type) {
            case ParticleType::BASIC:
                return Particle(x, y, 5.0f, 1.0f);
            case ParticleType::HEAVY:
                return Particle(x, y, 8.0f, 5.0f);
            case ParticleType::LIGHT:
                return Particle(x, y, 3.0f, 0.2f);
            case ParticleType::BOUNCY:
                return Particle(x, y, 5.0f, 1.0f, 1.5f); // High bounce
            case ParticleType::STICKY:
                return Particle(x, y, 5.0f, 1.0f, 0.1f); // Low bounce
            case ParticleType::LIQUID:
                return Particle(x, y, 2.0f, 0.5f, 0.8f);
        }
        return Particle(x, y, 5.0f, 1.0f); // Default
    }
};
```

### **2. Observer Pattern for Events**

```cpp
enum class PhysicsEvent {
    PARTICLE_COLLISION,
    CONSTRAINT_BROKEN,
    PARTICLE_SPAWNED,
    PARTICLE_DESTROYED
};

class IPhysicsEventListener {
public:
    virtual ~IPhysicsEventListener() = default;
    virtual void onPhysicsEvent(PhysicsEvent event, const void* data) = 0;
};

class PhysicsEventManager {
    std::vector<IPhysicsEventListener*> listeners;
    
public:
    void addListener(IPhysicsEventListener* listener) {
        listeners.push_back(listener);
    }
    
    void removeListener(IPhysicsEventListener* listener) {
        listeners.erase(
            std::remove(listeners.begin(), listeners.end(), listener),
            listeners.end()
        );
    }
    
    void fireEvent(PhysicsEvent event, const void* data = nullptr) {
        for (auto* listener : listeners) {
            listener->onPhysicsEvent(event, data);
        }
    }
};
```

### **3. Strategy Pattern for Collision Detection**

```cpp
class ICollisionStrategy {
public:
    virtual ~ICollisionStrategy() = default;
    virtual void detectCollisions(
        std::vector<Particle>& particles, 
        float dt
    ) = 0;
};

class BruteForceCollision : public ICollisionStrategy {
public:
    void detectCollisions(std::vector<Particle>& particles, float dt) override {
        // O(n²) collision detection
    }
};

class SpatialGridCollision : public ICollisionStrategy {
    std::unique_ptr<SpatialGrid> grid;
    
public:
    void detectCollisions(std::vector<Particle>& particles, float dt) override {
        // O(n) collision detection using spatial grid
    }
};

class QuadTreeCollision : public ICollisionStrategy {
    std::unique_ptr<QuadTree> quadtree;
    
public:
    void detectCollisions(std::vector<Particle>& particles, float dt) override {
        // O(n log n) collision detection using quadtree
    }
};
```

---

## 🎮 **Game Development Framework**

### **1. Scene Management System**

```cpp
class Scene {
public:
    virtual ~Scene() = default;
    virtual void initialize() = 0;
    virtual void update(float dt) = 0;
    virtual void render() = 0;
    virtual void cleanup() = 0;
    
    virtual void onEnter() {}
    virtual void onExit() {}
};

class PhysicsScene : public Scene {
    Motor physics_engine;
    ForceManager force_manager;
    
public:
    void initialize() override;
    void update(float dt) override;
    void render() override;
    void cleanup() override;
};

class SceneManager {
    std::stack<std::unique_ptr<Scene>> scene_stack;
    
public:
    void pushScene(std::unique_ptr<Scene> scene);
    void popScene();
    void replaceScene(std::unique_ptr<Scene> scene);
    
    void update(float dt);
    void render();
};
```

### **2. Input System Architecture**

```cpp
enum class InputAction {
    SPAWN_PARTICLE,
    DELETE_PARTICLE,
    CREATE_CONSTRAINT,
    TOGGLE_GRAVITY,
    PAUSE_SIMULATION,
    RESET_SCENE
};

class InputBinding {
public:
    int key;
    int mouse_button;
    InputAction action;
    
    bool isTriggered() const;
};

class InputManager {
    std::vector<InputBinding> bindings;
    std::map<InputAction, std::function<void()>> action_handlers;
    
public:
    void addBinding(int key, InputAction action);
    void addBinding(int mouse_button, InputAction action);
    void setActionHandler(InputAction action, std::function<void()> handler);
    
    void update();
};
```

### **3. Configuration System**

```cpp
class Config {
    std::map<std::string, float> float_values;
    std::map<std::string, int> int_values;
    std::map<std::string, bool> bool_values;
    std::map<std::string, std::string> string_values;
    
public:
    // Type-safe getters and setters
    template<typename T>
    T get(const std::string& key) const;
    
    template<typename T>
    void set(const std::string& key, const T& value);
    
    void loadFromFile(const std::string& filename);
    void saveToFile(const std::string& filename) const;
    
    // Physics-specific convenience methods
    float getGravityStrength() const { return get<float>("physics.gravity.strength"); }
    void setGravityStrength(float strength) { set("physics.gravity.strength", strength); }
    
    int getMaxParticles() const { return get<int>("physics.max_particles"); }
    void setMaxParticles(int count) { set("physics.max_particles", count); }
};
```

---

## 🔄 **Data Flow Architecture**

### **1. Update Pipeline**

```
Input → State Changes → Physics Update → Constraint Solving → Collision → Render
  ↓           ↓              ↓               ↓              ↓         ↓
Events → Modifications → Integration → Stabilization → Response → Display
```

### **2. Memory Management Strategy**

```cpp
class MemoryManager {
    // Object pools for different types
    ObjectPool<Particle> particle_pool;
    ObjectPool<DistanceConstraint> constraint_pool;
    
    // Temporary storage for calculations
    std::vector<Vector2> temp_forces;
    std::vector<CollisionPair> temp_collisions;
    
public:
    template<typename T, typename... Args>
    T* acquire(Args&&... args);
    
    template<typename T>
    void release(T* object);
    
    void reserveTemporaryStorage(size_t particle_count);
    void clearTemporaryStorage();
};
```

### **3. Performance Monitoring**

```cpp
class PerformanceProfiler {
    struct ProfileData {
        std::string name;
        float total_time;
        float average_time;
        int call_count;
    };
    
    std::map<std::string, ProfileData> profiles;
    std::chrono::high_resolution_clock::time_point start_time;
    
public:
    void beginProfile(const std::string& name);
    void endProfile(const std::string& name);
    
    void displayResults() const;
    void resetCounters();
    
    // RAII helper for automatic profiling
    class ScopedProfiler {
        PerformanceProfiler& profiler;
        std::string name;
    public:
        ScopedProfiler(PerformanceProfiler& p, const std::string& n) 
            : profiler(p), name(n) { profiler.beginProfile(name); }
        ~ScopedProfiler() { profiler.endProfile(name); }
    };
};

#define PROFILE_SCOPE(profiler, name) \
    PerformanceProfiler::ScopedProfiler _prof(profiler, name)
```

---

## 🎯 **Extension Guidelines**

### **1. Adding New Particle Types**
1. Define new `ParticleType` enum value
2. Add factory method in `ParticleFactory`
3. Optionally override behavior in derived class
4. Add visual differentiation in rendering

### **2. Adding New Forces**
1. Inherit from `IForce` interface
2. Implement `calculateForce()` method
3. Add to `ForceManager` via template method
4. Optionally add debug visualization

### **3. Adding New Constraints**
1. Inherit from `IConstraint` interface
2. Implement constraint solving logic
3. Add validation and rendering methods
4. Register with constraint manager

### **4. Adding New Collision Strategies**
1. Inherit from `ICollisionStrategy`
2. Implement detection algorithm
3. Integrate with existing collision response
4. Add performance benchmarking

This architecture provides a solid foundation that can grow from a simple physics demo to a full game engine while maintaining clean, modular code that's easy to understand and extend.