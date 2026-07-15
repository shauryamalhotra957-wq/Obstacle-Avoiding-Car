# Robot Operator Experience

The car communicates what it senses, what it decides, and what it is doing. This separation makes bench testing and fault diagnosis much faster.

## Primary journey

1. Lift the drive wheels and power the controller.
2. Confirm `READY` and the stopped-motor state.
3. Verify centered, left, and right distance readings.
4. Place the car on a clear test path and observe `ACTION` transitions.
5. Test critical-distance and dead-end recovery at low speed.

## Console vocabulary

- `SENSOR`: a measured distance.
- `ACTION`: an intentional motor command.
- `VERIFY`: the post-turn clearance check.
- `SAFETY`: a critical-distance or dead-end recovery.
- `READY`: initialization finished with motors stopped.

## Interaction and safety rules

- Never present a distance without units.
- Never report `turn complete` until the forward verification succeeds.
- Keep safety messages distinct in text so they remain understandable without color.
- A future display should prioritize forward clearance, current motion, and stop state.
- Manual controls must be hold-to-run, keyboard accessible, and at least 44px on touch screens.

## Recovery model

`clear` -> `obstacle` -> `stopped` -> `scanning` -> `turning` -> `verifying` -> `clear`

If verification exhausts its attempts, a future revision should enter a latched stopped state and require operator acknowledgment rather than continuing indefinitely.
