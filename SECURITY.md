# Security and safety policy

This is Arduino robotics firmware. It can move motors unexpectedly and must be treated as physical-device code.

- Keep motor power isolated during firmware upload and bench tests.
- Do not connect unknown sensors, serial peripherals, or network bridges without reviewing their data and command paths.
- Never commit Wi-Fi credentials, camera data, private wiring photos, or device logs containing personal information.
- Validate sensor timeouts and motor-stop behavior before operating near people, pets, stairs, or traffic.
- Treat third-party Arduino libraries and copied sketches as untrusted until reviewed.

Report suspected unsafe command handling, credential exposure, or a reproducible fail-safe bypass privately to the repository owner. Include the board, wiring, firmware revision, and sanitized reproduction steps; do not publish an active physical-safety issue first.
