---
name: open-source-algorithm-explorer
description: Systematically understand an open-source algorithm or repository from its problem definition and mathematical foundations through architecture, source-code execution, experiments, limitations, and reuse opportunities. Use when a user provides a GitHub repository, paper, algorithm name, or codebase and wants to learn how it works, trace its implementation, or produce a technically grounded study report.
---

# Open Source Algorithm Explorer

## Purpose

Help the user quickly and accurately understand an open-source algorithm. Move from **why the algorithm exists** to **how its mathematics works**, then to **how the repository implements it**, and finally to **how to run, evaluate, and reuse it**.

This skill is for learning and technical investigation. Do not silently turn the task into a refactoring or pluggable-component implementation task. If the user also wants to integrate or encapsulate the algorithm, treat that as a separate follow-up phase.

## Operating principles

1. **Evidence before conclusions.** Inspect the actual repository, source files, tests, build files, examples, and documentation. Do not infer implementation details from a README, paper, class name, or directory name alone.
2. **Separate theory from implementation.** Clearly label what the paper/specification says, what the source code actually does, and what is an interpretation or recommendation.
3. **Trace real execution paths.** Identify concrete entry points and follow calls/data through the code. Cite file paths and symbol names; include line numbers when the available tools expose them.
4. **Explain, don't dump.** Explain the role and reasoning behind important code. Avoid listing every file or reproducing large source blocks.
5. **Respect repository reality.** Record the commit/branch or version inspected when available. Note missing dependencies, generated code, submodules, unavailable assets, and unverified claims.
6. **Use diagrams when they clarify.** Prefer Mermaid for architecture, data flow, sequence, and call-chain diagrams. Ensure diagrams use valid syntax and match the inspected code.
7. **Be explicit about uncertainty.** Mark findings as Verified, Strongly inferred, or Unverified where useful.
8. **Adapt depth to the user's goal.** A quick overview can be concise; a deep-dive should cover all six phases below. Ask one focused question only if the target or desired depth is genuinely unclear.

## Required workflow

### Phase 0 — Scope and repository reconnaissance

Establish:
- Target algorithm and repository URL/name.
- Repository purpose and problem domain.
- Version/commit/branch inspected, if discoverable.
- Primary language, build system, runtime, major dependencies.
- Whether the repository is an algorithm library, application, framework, or research prototype.
- User's goal: conceptual learning, source walkthrough, reproduction, comparison, or preparation for reuse.

Inspect, as relevant:
- README and documentation.
- Top-level directory tree.
- Build and dependency manifests (`CMakeLists.txt`, `package.xml`, `pyproject.toml`, `Cargo.toml`, etc.).
- Executables, examples, demos, tests, benchmarks, CI configuration.
- Papers or design documents linked by the project.

Deliver a short repository identity statement and a scope boundary. Do not start by summarizing every directory.

### Phase 1 — Algorithm background

Explain:
- The concrete problem being solved.
- Inputs, outputs, assumptions, and operating conditions.
- Why the problem matters and what makes it difficult.
- The algorithm's central idea in plain language.
- Typical use cases and non-use cases.
- Related approaches and the key differences, when comparison is useful.

Include a concise input → processing → output view. Distinguish the algorithm's general concept from repository-specific features.

### Phase 2 — Mathematical and theoretical foundations

Explain only the mathematics needed to understand the implementation:
- State variables, parameters, coordinate systems, constraints, and objective functions.
- Core equations with every symbol defined.
- Derivation or intuition for important equations; do not merely paste formulas.
- Algorithm steps and update rules.
- Convergence, completeness, optimality, stability, or approximation guarantees, if applicable.
- Time and space complexity, including assumptions behind the stated complexity.
- Known theoretical limitations and practical trade-offs.

For algorithms without substantial mathematics, explain the logical model, data structures, or protocol instead. Never invent guarantees. If a paper is relevant, distinguish paper claims from implementation behavior.

### Phase 3 — Repository architecture

Identify the actual architecture:
- Main modules and their responsibilities.
- Public API versus internal implementation.
- Core classes, interfaces, data structures, and ownership/lifetime relationships.
- Dependency direction and important external libraries.
- Configuration and extension points.
- Build targets, libraries, executables, plugins, and test targets.

Produce:
- A compact module responsibility table.
- An architecture diagram grounded in the source.
- A list of the 5–15 most important files/symbols, with reasons.

Avoid treating folder names as proof of module boundaries. Verify dependencies through includes, build declarations, registration code, and call sites.

### Phase 4 — Core source-code walkthrough

Trace at least one representative end-to-end execution path:
1. Entry point or public API invocation.
2. Input validation and preprocessing.
3. Initialization and state construction.
4. Core algorithm loop or main computation.
5. Termination / success / failure handling.
6. Output construction and publication/return.
7. Cleanup, resource ownership, and error handling where relevant.

For each key step, identify:
- File path.
- Class/function/method.
- Inputs and outputs.
- Important state/data structures.
- Who calls it and what it calls next.
- How it corresponds to the theory in Phase 2.

Include a call-chain or sequence diagram. If multiple execution modes exist, choose the most representative one and briefly identify the others. Do not claim a function is executed unless a call site or registration path supports that claim.

### Phase 5 — Build, run, and experiments

Determine whether the repository can be built and run in the available environment. Do not claim execution unless it was actually performed.

Document:
- Supported platform/toolchain and prerequisites.
- Dependency installation and build commands, based on repository instructions.
- Smallest runnable example or test.
- Input data/configuration required.
- Expected output and how to inspect it.
- Tests, benchmarks, and evaluation metrics already provided.
- Parameters that materially affect behavior.
- A minimal experiment plan: baseline, parameter sweep, edge cases, and reproducibility notes.

If execution is not possible, state exactly why and provide commands the user can run. Do not fabricate outputs, performance numbers, screenshots, or test results.

### Phase 6 — Evaluation and learning summary

Assess:
- Strengths and weaknesses.
- Computational cost and scaling behavior.
- Robustness and edge cases.
- Assumptions and environmental constraints.
- Difference between paper/design intent and code behavior.
- Maintenance signals: release activity, tests, CI, documentation, dependency health (only if inspected).
- Suitable and unsuitable application scenarios.

Conclude with:
- A one-paragraph mental model.
- The 5–10 concepts the user should remember.
- A recommended reading order through the source.
- Open questions / unverified areas.
- Suggested next learning or experiment steps.

### Optional Phase 7 — Reuse and engineering handoff

Only perform this phase when the user asks about reuse, integration, or encapsulation. Identify:
- Reusable algorithm core versus framework-specific glue.
- Public API and hidden assumptions.
- Data types and conversion boundaries.
- External dependencies and licensing considerations (verify the license file).
- Candidate adapter/facade boundaries.
- What should remain independent from the host application.
- Integration risks and a small proof-of-concept plan.

Do not redesign the user's architecture or generate a full integration implementation unless requested.

## Output format

Use the report structure in `references/analysis-report-template.md`. For a quick scan, provide the executive summary first and mark deeper sections as omitted rather than pretending they were completed.

Every source-backed claim should include a path and symbol reference, and a line range when available. Web or paper-derived claims should carry source citations. Keep a clear distinction between:
- **Verified:** directly confirmed in inspected source or executed test.
- **Inferred:** strongly supported by evidence but not directly confirmed.
- **Unverified:** not inspected or could not be established.

## Interaction rules

- If the user gives a repository URL, begin by inspecting it; do not ask them to restate the repository.
- If only an algorithm name is given, identify the canonical or likely repository and ask for confirmation only when multiple materially different projects exist.
- If a paper and implementation disagree, report both rather than forcing consistency.
- If the repository is too large, first map the architecture, then narrow to the smallest representative execution path.
- If the user asks “quickly understand”, prioritize Phases 0, 1, 3, and a concise Phase 4; offer the mathematical deep dive and experiments as next steps.
- If the user asks for a complete understanding, cover all required phases.
- Do not modify repository files unless explicitly asked.
- Do not create or update external documentation workspaces unless explicitly asked.
