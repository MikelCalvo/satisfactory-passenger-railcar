# Changelog

## Unreleased

- Add a Linux Bash runner for the standalone C++20 policy checks, with GCC/Clang selection, precise/fast-math modes and failure/cleanup tests.
- Preserve the existing 26 policy cases with fixed IEEE-754 boundary/NaN/infinity inputs so Clang fast-math does not rewrite their construction. Runtime policy code is unchanged.
- Record successful menu startup of the published 0.1.8 binary on the affected Arch Linux / Proton 10.0-4 client. Gameplay and multiplayer remain separate unverified layers.

## 0.1.8 — Startup fix preview

- Explicitly retains the native `FactoryDedicatedClient` prerequisite through the `FGAbstractServerWidget` class reference.
- In a reproduced Windows DX11 startup, streaming errors fell from **454 to 0** and SML/the menu initialized.
- Leaves camera, HUD, effective bindings, authority/occupancy/safe-exit guards and the strict **below 30 km/h** access policy unchanged.
- First release with the project's complete own-source code, tests and build instructions under **MIT**.
- **Proton:** candidate fix; the affected client's startup block is not yet verified as resolved. Multiplayer remains unverified.

### Presentation refresh (no new binary)

The public project is now **Satisfactory Passenger Railcar**, with English/Spanish documentation and original railway-map cover artwork. The internal plugin identifier, release tags, code and binary assets are unchanged.

## 0.1.7 — Historical preview

- Deferred local HUD asset loading to avoid loading broad UI/Niagara assets from the native HUD constructor.
- Introduced the current below-30-km/h access policy.
- A startup block was reported on Proton; keep that compatibility warning when referring to this historical version.
