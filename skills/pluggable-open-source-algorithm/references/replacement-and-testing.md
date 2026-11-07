# Contract Testing and Backend Replacement

## Contract suite

Tests should describe project semantics, not vendor API behavior. Reuse the same suite for every backend.

### Functional cases
- A representative valid case with a known expected result.
- Empty/minimal input.
- Boundary values and degenerate geometry/data.
- Invalid identifiers, dimensions, units or configuration.
- Unsupported operation or representation.
- Multiple calls and repeated updates.
- Error and exception mapping.

### Semantic cases
- Units and coordinate frames.
- Transform composition and origin conventions.
- Stable identifier mapping.
- Ordering guarantees (if any).
- Precision/tolerance and equality semantics.
- Whether results are snapshots or references.
- Determinism guarantees.
- Filtering and policy rules.

### Lifecycle and concurrency
- Construction and destruction.
- Reconfiguration and cache invalidation.
- Concurrent independent instances.
- Concurrent calls on one instance if promised.
- Shared-resource ownership.
- Shutdown while work is pending, if relevant.

### Integration and performance
- Real caller uses only the project interface.
- Representative production-sized input.
- Latency distribution, not only average latency.
- Throughput, memory and initialization cost.
- Stress, long-running and regression fixtures.
- Build and deployment without an optional backend, if that is a supported configuration.

## Test harness pattern

A backend test fixture can accept a factory function:

```cpp
using BackendFactory =
    std::function<std::unique_ptr<AlgorithmInterface>(
        const AlgorithmConfig&)>;

void RunAlgorithmContractTests(
    const BackendFactory& make_backend);
```

Register one test instantiation per backend. Keep test data and expected semantic outcomes backend-independent. If numerical algorithms differ, define acceptable tolerances based on domain requirements and explain why.

## Replacement checklist

- [ ] Custom implementation satisfies every required interface operation.
- [ ] Optional capabilities are advertised accurately.
- [ ] Unsupported behavior is explicit and tested.
- [ ] Shared contract suite passes.
- [ ] Backend-specific regression suite passes.
- [ ] Integration tests pass through actual application call paths.
- [ ] Correctness and numerical tolerances are justified.
- [ ] Performance and resource use meet project targets.
- [ ] Concurrency and lifecycle guarantees are met.
- [ ] Configuration selects the new backend without caller changes.
- [ ] Logs and diagnostics identify the selected backend and version.
- [ ] License notices and dependency manifests are updated.
- [ ] Rollback to the temporary backend remains possible until rollout is accepted.

## Detecting abstraction leakage

Search application/business code for:
- Vendor namespace references.
- Vendor headers in public include paths.
- Vendor-specific enums, handles or exceptions.
- Backend-type conditionals outside factory/composition code.
- Configuration fields that callers must understand to use the common interface.

Each occurrence should be justified or moved behind the adapter boundary.

## Migration and rollback

Treat backend replacement as a controlled rollout:
1. Keep both implementations buildable.
2. Run the same fixture set against both.
3. Compare result distributions and performance on representative workloads.
4. Use a configuration switch or deployment flag to select the backend.
5. Monitor errors, latency and resource use.
6. Retain a rollback path until acceptance criteria are met.
7. Remove the old dependency only after its rollback window and downstream usage have ended.

Avoid silently changing semantics during replacement. If the new algorithm changes accuracy, result ordering, latency or supported inputs, make the difference explicit and obtain domain-level acceptance.
