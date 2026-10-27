
# Contributing to the ECE357 Rover

## 1. Purpose

This guide defines how Group 19 contributes code, documentation, and hardware test results to the Autonomous Adaptive Self-Calibrating 4WD Rover project.

## 2. Before Making Changes

1. Read `README.md` and the relevant documentation in `docs/`.
2. Check the existing implementation before creating duplicate files.
3. Confirm hardware specifications before adding hardware-specific behavior.
4. Keep unverified hardware assumptions clearly marked as provisional.

## 3. Repository Organization

- `config/`: rover configuration files.
- `src/rover/`: application and rover modules.
- `tests/`: automated software tests.
- `scripts/`: diagnostic and development scripts.
- `docs/`: setup instructions, wiring records, inventory, and test evidence.

## 4. Code Guidelines

- Use descriptive names for functions, variables, and files.
- Add docstrings to public functions and modules.
- Handle expected errors explicitly.
- Keep hardware access separate from navigation, perception, and control logic.
- Avoid hard-coded credentials, unexplained pin assignments, and unverified electrical assumptions.
- Do not claim a component works until a corresponding test has been performed.

## 5. Testing

Before proposing a code change:

1. Run the existing test suite when a suitable Python environment is available.
2. Add or update tests for changed behavior.
3. Record any failures and their causes.
4. Clearly distinguish software test results from physical hardware test results.

The GitHub Actions workflow may run tests automatically after commits or pull requests.

## 6. Hardware Changes

Before modifying wiring or powered hardware:

1. Confirm the exact component model and its specifications.
2. Update `docs/hardware-inventory.md` if the inventory changes.
3. Record confirmed pin mappings in `docs/wiring-record.md`.
4. Update `docs/test-plan.md` with the intended test.
5. Follow the power and GPIO safety requirements.

Never connect an unverified signal directly to a Raspberry Pi GPIO pin.

## 7. Documentation and Evidence

Record meaningful changes in `docs/build-log.md`.

Include:
- What changed.
- Why it changed.
- Hardware models involved, if applicable.
- Test procedure and observed results.
- Remaining issues and unresolved assumptions.

Do not record API keys, passwords, or other secrets.

## 8. Collaboration

- Communicate changes that affect shared interfaces or hardware connections.
- Review potentially unsafe hardware changes before powering the system.
- Avoid overwriting another team member's work.
- Use pull requests when practical, especially for changes affecting control, power, or hardware interfaces.

## 9. Definition of Done

A contribution is ready when:
- The change has a clear purpose.
- Relevant documentation is updated.
- Applicable tests have been run, or their pending status is stated.
- No credentials or unnecessary generated files are included.
- Hardware safety implications have been reviewed.
