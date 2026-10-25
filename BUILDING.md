# Build Satisfactory Passenger Railcar

[Español](BUILDING.es.md) · [Project overview](README.md)

This repository is the **plugin**, not a complete Satisfactory SDK checkout. Its internal name stays `TsabsPassengerWagon`.

## Verified toolchain for 0.1.8

| Component | Pinned environment |
| :--- | :--- |
| Game | Satisfactory Steam 1.2.4.1, CL 513885 |
| Loader | SML 3.12.0 |
| Starter Project | satisfactorymodding/SatisfactoryModLoader, commit `8eebc9e659f0b2ff4392d2f8cf6e2cff60c8ea49` |
| Engine | Coffee Stain Unreal Engine 5.6.1-css-83 |
| Compiler | Visual Studio 2022, MSVC 14.38 |
| Windows SDK | 10.0.26100.0 |
| Wwise | 2025.1.6.9117; integration 2025.1.6.9117.4248 |

Obtain and configure dependencies separately using the [Satisfactory Modding documentation](https://docs.ficsit.app/). They are not redistributed or relicensed by this project. Building on a Linux host has not been validated.

## Layout and package

Place this repository in `Mods/TsabsPassengerWagon` inside a separately prepared Starter Project. The descriptor must be `Mods/TsabsPassengerWagon/TsabsPassengerWagon.uplugin`, not nested in an additional directory. Do not reuse another version's Binaries/Intermediate.

Open Windows CMD in the Starter Project root and set `UE_ROOT` to your installed CSS Engine:

```bat
set "UE_ROOT=C:\Path\To\UnrealEngine"
call "%UE_ROOT%\Engine\Build\BatchFiles\Build.bat" FactoryEditor Win64 Development "%CD%\FactoryGame.uproject" -WaitMutex -NoHotReloadFromIDE
call "%UE_ROOT%\Engine\Build\BatchFiles\RunUAT.bat" -ScriptsForProject="%CD%\FactoryGame.uproject" PackagePlugin -Project="%CD%\FactoryGame.uproject" -DLCName=TsabsPassengerWagon -ClientConfig=Shipping -Build -Cook -SkipSaveAssetRegistry -AdditionalCookerOptions=-NoVerifyAssetRegistry -pak -stage -NoCompileEditor -Zip -TargetPlatforms=Windows -GameClientTarget=FactoryGameSteam -NoInstalledEngine -UTF8Output
```

These targets and packaging arguments were exercised for the published build, with local paths substituted. Inspect `Saved/ArchivedPlugins/TsabsPassengerWagon` before installation. Leave automatic deployment disabled. Different paths/toolchains may not produce byte-identical binaries.

## Standalone checks

In an x64 MSVC Native Tools CMD, from the plugin root:

```bat
call Tests\run-policy-tests.cmd
python Tests\test_native_prerequisite.py
```

The policy script compiles/runs 26 checks with `/fp:precise` and again with `/fp:fast`, covering the strict 30 km/h boundary, reverse travel, NaN and infinity. The Python contract checks the native module dependency and retained class reference. Both were also executed from the exported source tree.

## Unreal automation

From the Starter Project root:

```bat
"%UE_ROOT%\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" "%CD%\FactoryGame.uproject" -unattended -NullRHI -NoSound -nosplash -ExecCmds="Automation RunTests Tsabs.Passenger" -TestExit="Automation Test Queue Empty" -ReportExportPath="%CD%\Saved\PassengerTestReport"
```

Inspect the exported report, not only the process exit code. The 0.1.8 code passed 27 cases: 24 without warnings plus 3 with SDK/UI warnings, no failures or skipped cases. Starter Project native stubs do not prove actual in-game behavior or replication.

The package also passed checks for the retained FactoryDedicatedClient DLL import. A real Windows DX11 startup reached the menu with zero streaming errors. **Proton and multiplayer still need separate runtime validation.** No private logs are published.
