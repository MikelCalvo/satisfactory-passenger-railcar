# Compilar y verificar

## Entorno probado

- Satisfactory Steam1.2.4.1, CL513885; SML3.12.0.
- Starter Project de satisfactorymodding/SatisfactoryModLoader en el commit8eebc9e659f0b2ff4392d2f8cf6e2cff60c8ea49.
- Unreal Engine5.6.1-css-83 de Coffee Stain.
- Visual Studio2022, MSVC14.38 (v14.38-17.8), Windows SDK10.0.26100.0.
- Wwise2025.1.6.9117, integración2025.1.6.9117.4248.

Obtén/prepara ese entorno mediante la documentación oficial de Satisfactory Modding (https://docs.ficsit.app/). Aquí solo se publica el plugin propio, no esas herramientas ni el SDK. No se ha probado este proceso de compilación en un host Linux.

## Colocar el plugin

En un Starter Project separado y preparado, coloca este repositorio en `Mods/TsabsPassengerWagon`, de modo que exista `Mods/TsabsPassengerWagon/TsabsPassengerWagon.uplugin`. No anides otra carpeta del mismo nombre. No copies Binaries/Intermediate de otra versión.

Para los comandos siguientes abre una consola Windows CMD en la raíz del Starter Project y define `UE_ROOT` con la ubicación real de tu Engine CSS. No cambies el perfil/configuración personal del juego para compilar.

```bat
set "UE_ROOT=C:\Ruta\Al\UnrealEngine"
call "%UE_ROOT%\Engine\Build\BatchFiles\Build.bat" FactoryEditor Win64 Development "%CD%\FactoryGame.uproject" -WaitMutex -NoHotReloadFromIDE
call "%UE_ROOT%\Engine\Build\BatchFiles\RunUAT.bat" -ScriptsForProject="%CD%\FactoryGame.uproject" PackagePlugin -Project="%CD%\FactoryGame.uproject" -DLCName=TsabsPassengerWagon -ClientConfig=Shipping -Build -Cook -SkipSaveAssetRegistry -AdditionalCookerOptions=-NoVerifyAssetRegistry -pak -stage -NoCompileEditor -Zip -TargetPlatforms=Windows -GameClientTarget=FactoryGameSteam -NoInstalledEngine -UTF8Output
```

Es el mismo objetivo/flujo UAT usado para el paquete publicado, con rutas parametrizadas. Alpakit también puede empaquetar el plugin para Windows/Steam. No actives despliegue automático al juego; revisa el ZIP generado en `Saved/ArchivedPlugins/TsabsPassengerWagon` antes de instalar. No se promete igualdad bit a bit entre compilaciones realizadas con toolchains/rutas diferentes.

## Pruebas sin Engine

Desde una consola x64 Native Tools de MSVC, en la raíz del plugin:

```bat
call Tests\run-policy-tests.cmd
python Tests\test_native_prerequisite.py
```

El primer script compila y ejecuta26 casos en `/fp:precise` y en `/fp:fast`, incluidos frontera30, reversa, NaN e infinitos. El segundo verifica la dependencia explícita y la referencia a la clase nativa que conserva el import de la DLL. No requieren SDK para esas dos comprobaciones.

## Pruebas en editor y límites

Las pruebas Unreal están en `Source/TsabsPassengerWagon/Private/Tests`. Desde la raíz del Starter Project:

```bat
"%UE_ROOT%\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" "%CD%\FactoryGame.uproject" -unattended -NullRHI -NoSound -nosplash -ExecCmds="Automation RunTests Tsabs.Passenger" -TestExit="Automation Test Queue Empty" -ReportExportPath="%CD%\Saved\PassengerTestReport"
```

Revisa el informe exportado y sus contadores, no solo el código de salida del editor. La automatización usada para018 dio27 casos correctos (24 sin avisos +3 con avisos SDK/UI), cero fallos/no ejecutados. Algunas implementaciones nativas del Starter Project son stubs: estas pruebas no demuestran transiciones nativas ni replicación en el juego real.

También se verificaron el cook/ZIP y la presencia del import real de la DLL FactoryDedicatedClient en Shipping. Se probó un arranque real Windows DX11 hasta el menú, con cero errores Streaming. Aún faltan prueba real del cliente Proton, singleplayer del amigo y multijugador. Los logs privados no forman parte de esta distribución.
