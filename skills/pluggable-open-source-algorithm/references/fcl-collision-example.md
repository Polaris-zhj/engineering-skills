# Worked Example: FCL as a Replaceable Collision Backend

This is a concrete example of the general pattern. It is not a requirement that every project use FCL, ROS 2, URDF or C++.

## What FCL is

FCL (Flexible Collision Library) is a library for geometric collision and distance queries. Typical capabilities include collision queries, distance queries, and continuous collision detection for supported geometry representations.

FCL is not:
- A robot kinematics engine.
- A URDF semantic/model loader.
- A perception or point-cloud understanding system.
- A motion planner or trajectory optimizer.
- A project-specific allowed-collision policy manager.

Treat FCL as a geometric query backend. The application must supply geometry and transforms in the representation FCL expects.

## Conceptual mapping

| Project concept | FCL-side concept | Adapter responsibility |
|---|---|---|
| Primitive collision shape | FCL primitive geometry | Construct geometry and dimensions |
| Triangle mesh | BVH model | Convert vertices/triangles and build model |
| Occupancy map | FCL OcTree integration | Convert supported occupancy representation |
| Collision instance | CollisionObject | Pair geometry with current transform |
| Pair collision query | Collision request/result | Execute query and map contacts/status |
| Distance query | Distance request/result | Execute query and map minimum distance |

Common FCL geometry classes include box, sphere, cylinder and capsule types; triangle meshes can be represented using a BVH model. Exact template aliases and APIs depend on the FCL version in use, so verify against the selected release's headers and documentation.

FCL's OcTree support is based on an occupancy representation such as an OctoMap tree. A raw point cloud is not automatically an OcTree: the application must decide how to interpret points, voxelize them, and construct a valid occupancy structure. Unknown/free/occupied semantics must be defined by the perception-to-map pipeline; do not invent free space merely because no point was observed.

## Keep robot kinematics and collision modeling independent

Use separate project-owned structures:

- `KinematicsModel`: joint/link topology, joint limits and FK.
- `RobotCollisionModel`: collision geometries attached to links and each geometry's fixed pose relative to its link.
- `FclCollisionModel`: FCL-specific geometry instances and collision objects created from the project collision model.

The relationship is by stable link identity (for example, `link_name`), not by making one model own the other.

For each collision geometry attached to a link:

```text
T_world_collision = T_world_link * T_link_collision
```

Where:
- `T_world_link` comes from the FK result.
- `T_link_collision` is the fixed local pose of that collision geometry.
- `T_world_collision` is the transform supplied to the FCL collision object.

The multiplication order assumes column-vector transform composition; document and consistently apply the project's convention. Do not mix Euler-angle assumptions or frame conventions across layers.

The FCL adapter updates collision-object transforms after receiving FK results. FCL itself does not calculate the robot's link poses.

## Environment obstacle path

Define a project-owned obstacle representation or scene interface. It may accept primitive geometry, triangle mesh, point cloud, occupancy data, or a normalized scene representation depending on project requirements.

The adapter converts supported inputs into FCL geometries/objects. Keep the perception layer separate:
- Perception estimates object/point data.
- A scene-construction layer interprets and maintains obstacle geometry.
- The collision backend performs geometric queries.

If using an occupancy map, define resolution, coordinate frame, occupancy threshold, unknown-cell policy and update strategy. A point cloud alone does not provide complete free/unknown semantics.

## Example project-owned interface

The following is illustrative. Adapt result details and error handling to the actual project contract.

```cpp
enum class CollisionStatus {
  kValid,
  kInvalidInput,
  kUnsupported,
  kBackendError,
};

struct CollisionResult {
  CollisionStatus status;
  bool in_collision;
  double minimum_distance;
  // Optional project-owned contact records.
};

class CollisionChecker {
 public:
  virtual ~CollisionChecker() = default;

  virtual CollisionResult CheckConfiguration(
      const JointConfiguration& q,
      const SceneSnapshot& scene) const = 0;

  virtual CollisionResult CheckSelfCollision(
      const JointConfiguration& q) const = 0;

  virtual CollisionResult CheckEnvironmentCollision(
      const JointConfiguration& q,
      const SceneSnapshot& scene) const = 0;

  virtual PathCollisionResult CheckPath(
      const JointPath& path,
      const SceneSnapshot& scene) const = 0;
};
```

Avoid using this sketch as a fixed API without deciding whether path checking is discrete or continuous, whether distance is always computed, how self-collision exclusions work, and how detailed contacts need to be.

## Example implementation split

```text
collision/
  collision_types.hpp          # project-owned data and result types
  collision_checker.hpp        # stable interface
  collision_checker_factory.hpp
  robot_collision_model.hpp    # project-owned geometry + link attachment
  fcl/
    fcl_collision_checker.hpp  # private/concrete adapter declaration
    fcl_collision_checker.cpp  # FCL API and conversion
    fcl_collision_model.hpp    # FCL objects and update logic
  custom/
    custom_collision_checker.hpp
    custom_collision_checker.cpp
```

Only the `fcl/` implementation should need FCL headers. A future custom backend should be able to build without FCL installed if the build is configured to exclude the FCL adapter.

## Factory selection

For example, a validated configuration may select `fcl` or `custom`. The planner receives a `std::shared_ptr<CollisionChecker>` (or another project-chosen ownership type) and calls only the interface.

A `GetOrCreate(type)` factory can be useful when one shared backend instance is intentionally reused. If instances carry scene state, mutable caches or per-planner configuration, use a per-owner `Create(type, config)` or dependency injection instead. Thread safety and cache-key semantics must be explicit.

## Collision contract examples

For a robot arm project, useful operations may include:
- Collision at a joint configuration (self + environment).
- Self-collision at a joint configuration.
- Environment collision at a joint configuration.
- Discrete path collision checking.
- Path checking against a selected obstacle.
- Distance to an obstacle at a joint configuration.

Name operations precisely. For example, `CheckPathCollisionWithObstacle` is clearer than an overloaded `CheckPathCollision` whose obstacle scope is implicit.

Define allowed-collision pairs and filtering policy at the project level unless a backend-independent scene/model component owns that policy. Do not let FCL-specific object IDs become the public robot-link identity.

## Replacement acceptance criteria

A custom collision backend is ready to replace FCL when:
- It implements the same project-owned collision contract.
- It passes the shared configuration/self/path contract tests.
- It has defined behavior for unsupported geometry and query modes.
- Its transform and unit handling match project conventions.
- Its accuracy/performance tradeoffs are measured against representative scenes.
- Planner and other callers need only a backend selection change.
