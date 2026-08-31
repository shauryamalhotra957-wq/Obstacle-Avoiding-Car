# Contributing

## Before changing firmware

- Keep the wheels lifted and motor power isolated while testing.
- Preserve the HC-SR04 timeout fail-safe: a missing echo must never be treated as a clear path.
- Update the pin map or threshold documentation when wiring or constants change.

## Verification

Compile for an Arduino Uno in the Arduino IDE, then run the bench checks in [docs/VALIDATION_CHECKLIST.md](docs/VALIDATION_CHECKLIST.md). Include the board, sensor wiring, and observed Serial Monitor states in the pull request.

Do not commit credentials, personal photos, or unreviewed third-party sketches.
