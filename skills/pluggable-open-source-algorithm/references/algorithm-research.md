# Third-Party Algorithm Research Guide

Use this checklist before adopting an external algorithm as a temporary implementation.

## 1. Define the problem precisely

Describe the real workload, not just the algorithm category:
- What input data arrives, in what format, at what rate and scale?
- What output must be produced and what does it mean?
- What are the correctness and safety consequences of false positives/negatives or approximation?
- What are the latency, throughput, memory and startup constraints?
- Which platforms, compilers, language standards and runtime environments must work?
- Which operations are mandatory on day one and which are future requirements?

Create representative fixtures. A library that passes a toy example may still fail on the actual workload.

## 2. Inspect the library at multiple levels

| Dimension | Questions |
|---|---|
| Scope | What does it solve? What is outside its scope? |
| Algorithm | Which methods/algorithms are implemented? Exact, approximate, heuristic, probabilistic? |
| Data | Which primitives, formats, dimensions and numeric types are supported? |
| Queries | Which operations and result details are available? |
| Correctness | What assumptions, tolerances and degeneracies exist? |
| Performance | Which benchmark workload, hardware and build flags produced the claims? |
| Integration | API, build system, dependencies, ABI and language compatibility? |
| Lifecycle | Who owns objects? Can objects be updated? Is it thread-safe? |
| Maintenance | Recent releases, active maintainers, issue/PR response and roadmap? |
| License | Is use, modification, redistribution and commercial deployment compatible? |
| Operations | Packaging, deployment, observability, failure modes and security? |

Read the official docs and relevant source code. Verify critical claims with a small prototype. Record exact release/tag/commit and date reviewed.

## 3. Distinguish capability layers

A common research error is assuming a low-level library provides an entire application capability.

For example, a geometric collision library may provide geometry intersection and distance queries but not:
- Robot description parsing.
- Forward kinematics.
- Scene/perception fusion.
- Allowed-collision policy.
- Motion planning.
- Trajectory sampling or time parameterization.

List the surrounding project-owned components that remain necessary.

## 4. Compare candidates with explicit weights

Score candidates against the project workload, not popularity alone. Example dimensions:
- Required capability coverage.
- Correctness and numerical robustness.
- Runtime performance.
- Integration/build cost.
- Maintenance and compatibility.
- License fit.
- Ease of replacement and adapter complexity.

Use qualitative ratings or weighted scores, but retain evidence and unknowns. A missing critical capability should not be hidden by a high overall score.

## 5. Prototype before committing

Build a narrow proof of integration:
1. Build the library using the intended toolchain.
2. Exercise the hardest required input/query.
3. Measure representative latency and memory.
4. Test invalid, boundary and degenerate inputs.
5. Verify threading and update semantics.
6. Confirm packaging and deployment.
7. Confirm license and attribution obligations.

Do not write a large adapter before proving that the library can meet the essential contract.

## 6. Record the decision

A decision note should include:
- Problem statement and non-goals.
- Candidates considered and why they were rejected/selected.
- Source URLs and exact versions.
- Capability matrix and known gaps.
- License conclusion and required notices.
- Prototype results and benchmark conditions.
- Integration risks and mitigation.
- Exit criteria for replacing the temporary backend.

## Stop conditions

Reject or defer adoption when:
- License obligations conflict with distribution or business needs.
- A required capability is absent and cannot be safely composed.
- The library's assumptions conflict with the project's data semantics.
- Build/runtime compatibility is unacceptable.
- The API is too unstable for the planned maintenance window.
- The adapter would expose vendor-specific types throughout the project.
- There is no credible test strategy for correctness.
