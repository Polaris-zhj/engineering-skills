# Architecture Patterns for Replaceable Algorithms

## Dependency rule

The application owns the abstraction. The external library is an implementation detail.

```text
Application / Planner / Service
             |
             v
      Domain Interface
             |
      Factory / DI wiring
        /          \
       v            v
  Vendor Adapter  Custom Adapter
       |            |
       v            v
 Third-party lib  Own algorithm
```

The adapter depends inward on the project contract. The application must not depend outward on the vendor API.

## Recommended boundaries

### Domain types

Represent stable business concepts: input state, geometry, constraints, result, status, identifiers and error codes. Document:
- Units and coordinate/frame conventions.
- Numeric precision and tolerances.
- Ownership and lifetime.
- Whether values are immutable snapshots or mutable handles.
- Validity rules and default values.

Do not use a vendor class as a domain type simply to avoid writing a conversion.

### Interface

Keep the interface focused on what the application needs. Prefer a few cohesive operations over a mirror of the external library's entire API.

For optional features, choose one explicit design:
- A capability descriptor queried before use.
- A separate optional interface.
- A result status such as `kUnsupported`.

Do not return a valid-looking empty result for unsupported work.

### Adapter

Own:
- Vendor object construction and destruction.
- Data conversion and validation.
- Transform/unit conversion.
- Error/exception translation.
- Vendor-specific caching and update rules.
- Mapping vendor results to stable project results.

Keep vendor includes out of public project headers. Prefer private implementation files (PIMPL can help when ABI isolation is important).

### Factory / registry

A factory maps a stable backend key to a constructor. It should:
- Validate unknown backend keys.
- Validate backend-specific configuration.
- Make registration deterministic.
- Return clear construction errors.
- Have documented ownership and lifecycle.
- Be testable without relying on hidden global initialization.

A simple explicit registry is often easier to reason about than a complex plugin loader. Use dynamic plugins only when runtime extension without recompilation is a real requirement.

## Lifecycle choices

| Pattern | Use when | Watch out for |
|---|---|---|
| Create per owner | Backend has mutable state tied to a caller | Startup cost and duplicated resources |
| Get-or-create cache | Expensive immutable/shared resources can safely be reused | Cache key, synchronization, invalidation, shared mutable state |
| Dependency injection | Application/container already owns lifecycle | More wiring, but explicit ownership |
| Process-wide singleton | Truly process-wide immutable service | Usually unsuitable for stateful scenes/configurations |

Do not assume one instance per backend type is correct. If configuration changes behavior, backend type alone is usually an insufficient cache key.

## Configuration separation

Keep common/domain settings distinct from backend-specific settings. Validate only the selected backend's settings, but reject unknown keys where practical. Preserve defaults in one documented place.

Example:

```yaml
algorithm:
  backend: fcl
  common:
    result_detail: contacts
  backends:
    fcl:
      enable_distance: true
    custom:
      resolution: 0.01
```

## Avoid these failure patterns

- **Vendor leakage:** public headers expose vendor geometry, handles, exceptions or enums.
- **Business branching:** planner/service contains `if (backend == ...)` logic.
- **False compatibility:** adapter silently approximates or ignores an operation.
- **Over-abstraction:** generic framework with many hooks before a second implementation exists.
- **Hidden global state:** static registration order changes behavior.
- **Ambiguous ownership:** factory caches mutable instances with unclear sharing.
- **Config coupling:** common config is polluted with parameters meaningful to only one backend.
- **Premature plugin system:** dynamic loading is added when a simple factory is sufficient.

## A useful replacement test

Write a small consumer using only the project interface. Compile and run it once with the temporary adapter and once with a fake or custom adapter. If consumer code must include vendor headers, branch on backend type, or understand vendor-specific result types, the boundary is incomplete.
