---
name: pluggable-open-source-algorithm
description: Use this skill when a project needs to adopt a third-party or open-source algorithm as a temporary backend while preserving a clean path to replace it with a project-owned algorithm later. Covers capability research, abstraction design, adapters, factories, configuration, testing, and migration. FCL collision checking is the worked example, not a scope limitation.
---

# Pluggable Open-Source Algorithm

## Goal

Help a project use a mature third-party algorithm quickly without allowing that dependency to become embedded in business logic. The project owns the stable domain contract; each external or in-house algorithm is an interchangeable implementation behind that contract.

The desired replacement workflow is:

1. Add a new implementation of the project-owned interface.
2. Register or construct that implementation.
3. Select it through configuration or a creation-time type.
4. Run the same contract and integration tests.
5. Leave callers, business workflows, and domain-facing APIs unchanged.

Do not promise zero code changes everywhere: adding an implementation and wiring its registration/configuration is expected. The invariant is that existing consumers do not need to know which backend is selected.

## When to use

Use this skill when:
- The project needs a working algorithm before its own algorithm is ready.
- Several third-party libraries or algorithm families are candidates.
- The project expects to replace a library with an in-house implementation.
- Different deployments may select different algorithm backends.
- A library's types or lifecycle should not leak into the project's public API.

Do not use it merely to wrap every dependency. A wrapper is not justified when the library is itself the product-facing contract, when there is no realistic replacement requirement, or when an abstraction would only duplicate a stable and suitable API.

## Required workflow

### 1. Clarify the domain contract before selecting a library

Write down:
- The business capability and the exact questions callers need answered.
- Inputs, outputs, units, coordinate/frame conventions, ownership and lifetime.
- Required accuracy, determinism, latency/throughput, memory, concurrency and real-time constraints.
- Error semantics, invalid-input behavior and unsupported-operation behavior.
- Required operations versus optional capabilities.
- Data sources and representations the system must accept.
- Expected evolution and replacement boundary.

Separate domain concepts from algorithm-specific implementation details. Do not design the project API by mechanically copying a candidate library's classes or method names.

Deliverable: a concise capability matrix with operation, input, output, guarantees, and priority.

### 2. Research candidate algorithms

For each candidate, verify rather than assume:
- What problem it solves and what it explicitly does not solve.
- Supported algorithms, geometry/data types, query types and edge cases.
- Whether it supplies only a low-level kernel or also parsing, modeling, planning, scheduling, etc.
- License and obligations for redistribution, linking, modifications and notices.
- Maintenance activity, release cadence, issue health and compatibility with the project's language/toolchain/runtime.
- Dependencies, binary size, build complexity, platform support and deployment constraints.
- API stability, extension points, thread-safety and object-lifecycle expectations.
- Performance evidence relevant to the project's actual workload.
- Known limitations, numerical assumptions and failure modes.

Prefer upstream documentation, API references, source code and reproducible benchmarks. Record the version or commit examined and cite the sources in the project research note. Do not infer a feature merely because a library has a similarly named type.

Deliverable: candidate comparison and a clear adopt / trial / reject decision.

### 3. Define the project-owned abstraction

Create a domain-facing interface around the required capability, not around one vendor library.

Rules:
- Keep public headers free of third-party headers and types.
- Use project-owned domain types for inputs, outputs, identifiers, errors and configuration.
- Keep units, frames, ownership and mutability explicit.
- Keep required operations small and cohesive.
- Represent optional capabilities explicitly (capability query, separate optional interface, or well-defined unsupported result).
- Never silently report success, an empty result, or “no collision” when an operation is unsupported.
- Do not expose vendor-specific escape hatches in the stable API unless deliberately accepted as a permanent coupling.
- Keep domain/model data separate from backend runtime objects.

Do not create a giant interface containing unrelated operations just because one library offers them. Split interfaces by cohesive capability when implementations may support different subsets.

### 4. Isolate the third-party dependency in an adapter

Implement an adapter that satisfies the project-owned interface and owns all vendor-specific work:
- Translate project domain inputs into library-specific objects.
- Translate results, status codes and exceptions back into project-owned types.
- Manage vendor object creation, update, caching and destruction.
- Handle coordinate/unit conversions in one documented place.
- Apply project policy such as filtering, allowed-pair rules or result normalization outside vendor-specific code where possible.
- Keep third-party includes in adapter implementation files or private headers.
- Pin and document the dependency version.

Avoid leaking vendor objects through return values, callbacks, public structs, configuration schemas or business-layer logs.

### 5. Separate models, adapters and business logic

Use a dependency direction similar to:

```text
Business / Planner / Application
              |
              v
      Project-owned Interface
              |
       Factory / Registry
          /         \
         v           v
 Third-party       In-house
   Adapter         Adapter
      |                |
      v                v
 External library   Own algorithm
```

The business layer depends on the project interface, never on concrete backend classes. The adapter depends on both the project interface and the external library.

Keep these concerns distinct:
- Domain model: what the project means and stores.
- Algorithm interface: what callers can ask for.
- Backend adapter/model: how one implementation represents and executes it.
- Factory/registry: how an implementation is selected and constructed.
- Business logic: how the capability participates in the application workflow.

### 6. Add explicit backend selection and lifecycle

Provide a stable backend identifier, such as an enum or validated string, and a factory/registry that creates an implementation from typed configuration.

Choose lifecycle semantics deliberately:
- `Create(type, config)`: create an independent instance per request/owner.
- `GetOrCreate(type, config)`: cache and reuse an instance when shared state/resource reuse is intentional.
- Dependency injection: construct outside the factory when lifecycle is managed by an application container.

Do not default to a singleton or global cache. If caching is used, define the cache key (backend type plus relevant configuration), ownership, thread safety, invalidation and shutdown behavior. Avoid returning the same mutable instance to unrelated consumers unless that is part of the contract.

Unknown backend types and invalid configurations must fail clearly. Factory registration should be deterministic and testable. Prefer explicit registration or compile-time wiring over hidden static initialization with order-dependent side effects.

### 7. Design configuration for replacement

Separate:
- Domain/application configuration (what capability is required).
- Backend selection (which implementation).
- Backend-specific parameters (only meaningful to that implementation).

Example:

```yaml
algorithm:
  backend: fcl
  common:
    # Project-owned settings shared by all implementations.
    result_detail: contacts
  backends:
    fcl:
      enable_distance: true
      broadphase: dynamic_aabb
    custom:
      # Custom implementation settings.
      resolution: 0.01
```

The schema above is illustrative. Validate the selected backend's configuration at construction time. Do not silently ignore misspelled or unsupported settings. Do not force every backend to pretend it has identical tuning parameters.

### 8. Verify behavioral substitutability

Create a backend-independent contract test suite and run it against every implementation.

At minimum test:
- Valid nominal inputs and outputs.
- Boundary and degenerate inputs.
- Invalid input and error mapping.
- Unsupported capabilities.
- Units, frames, precision and conversion correctness.
- Determinism/repeatability where required.
- Concurrency and lifecycle behavior.
- Integration with the real caller.
- Regression fixtures from known cases.
- Performance and memory under representative workloads.

Where algorithms intentionally differ in precision or behavior, define tolerances and semantic guarantees rather than asserting bitwise equality. Document unavoidable capability differences. A backend must not pass tests by returning conservative placeholders.

### 9. Plan the replacement path

When the in-house algorithm becomes ready:
1. Implement the same project-owned interface.
2. Reuse domain types and contract tests.
3. Add backend-specific tests and benchmarks.
4. Register the new backend and validate its configuration.
5. Run integration and regression suites with both backends.
6. Compare correctness, performance, robustness and resource usage.
7. Change the selected backend in configuration or construction code.
8. Remove the temporary dependency only after no deployment or workflow still needs it.
9. Update dependency/license notices, documentation and operational configuration.

Existing callers should not need changes. If callers do need backend-specific branches, the abstraction boundary has leaked and should be reviewed.

## Required outputs for a project task

Produce:
1. Requirements and capability matrix.
2. Candidate-library research and adoption rationale.
3. Architecture/dependency diagram.
4. Project-owned interface and domain types.
5. Adapter mapping and data-conversion rules.
6. Factory/registry and lifecycle decision.
7. Configuration example and validation rules.
8. Contract, integration, regression and performance test plan.
9. Replacement checklist and known limitations.

Use the templates in `templates/` as starting points. Adapt them to the target language and project conventions rather than copying blindly.

## Worked example: FCL collision checking

FCL (Flexible Collision Library) is a geometric collision and distance-query library. It is not a robot kinematics engine, URDF semantic model, motion planner or perception system.

A robotics project may define a project-owned `CollisionChecker` interface. An `FclCollisionChecker` adapter converts project collision geometry and transforms into FCL objects and maps FCL results back to project result types. A future `CustomCollisionChecker` implements the same interface.

Keep `KinematicsModel` and `RobotCollisionModel` independent:
- Kinematics computes link poses from a joint configuration.
- RobotCollisionModel describes collision geometry attached to links and each geometry's fixed local pose.
- The FCL adapter owns FCL-specific collision objects and updates their world transforms using FK results.

For a collision geometry attached to a link:

```text
T_world_collision = T_world_link * T_link_collision
```

The project owns the transform convention and documents multiplication order. FCL receives the resulting transform; it does not compute FK or interpret link/joint semantics.

The FCL-specific reference in `references/fcl-collision-example.md` gives a more detailed mapping.

## Quality gates

Before declaring the integration complete, verify:
- [ ] The business layer includes no third-party headers.
- [ ] Public APIs expose only project-owned types.
- [ ] The adapter is the only layer translating to/from the external library.
- [ ] Backend selection is explicit and invalid selections fail clearly.
- [ ] Lifecycle and thread-safety semantics are documented.
- [ ] Unsupported operations cannot masquerade as successful empty results.
- [ ] The same contract tests run against each backend.
- [ ] Switching backend requires no edits to existing business consumers.
- [ ] Dependency version, license and notices are recorded.
