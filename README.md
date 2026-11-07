# Engineering Skills

Reusable engineering skills, playbooks, references, and templates for building maintainable software systems.

The repository focuses on repeatable engineering workflows: understanding a problem, evaluating implementation options, defining clean boundaries, and producing maintainable project artifacts.

## Skills

### [Pluggable Open-Source Algorithm](skills/pluggable-open-source-algorithm/SKILL.md)

Adopt a mature third-party algorithm as a temporary backend while keeping the project free to replace it with an in-house implementation later.

The skill covers:
- Requirements and capability-contract definition.
- Open-source library research, licensing and suitability evaluation.
- Project-owned interfaces and domain types.
- Adapter boundaries and third-party data conversion.
- Factory/registry design, configuration and instance lifecycle.
- Contract testing, integration validation and backend replacement.
- FCL collision detection as a detailed worked example.

## Repository layout

```text
skills/
└── pluggable-open-source-algorithm/
    ├── SKILL.md
    ├── references/
    │   ├── algorithm-research.md
    │   ├── architecture-patterns.md
    │   ├── fcl-collision-example.md
    │   └── replacement-and-testing.md
    └── templates/
        ├── algorithm-interface.hpp
        ├── algorithm-factory.hpp
        └── algorithm-config.yaml
```

## Design principles

1. The project owns the stable domain contract.
2. Third-party libraries are isolated behind adapters.
3. Business logic depends on abstractions, not concrete backends.
4. Backend selection and backend-specific configuration are explicit.
5. Lifecycle, unsupported capabilities and failure behavior are documented.
6. Every backend is validated against shared contract tests.
7. Replacing an implementation should not require changes to existing consumers.

## Using a skill

Open the relevant `SKILL.md` and provide the target project's requirements, constraints and existing architecture. Treat the references as supporting guidance and the templates as starting points; adapt names, types and conventions to the target project rather than copying them blindly.
