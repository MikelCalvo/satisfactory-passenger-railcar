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
call "%UE_ROOT%\Engine\Build\BatchFiles\RunUAT.bat" -ScriptsForProject="%CD%\FactoryGame.uproject" PackagePlugin -project="%CD%\FactoryGame.uproject" -DLCName=TsabsPassengerWagon -Target=FactoryGameSteam -platform=Win64 -clientconfig=Shipping -build -installed -nocompileeditor -utf8output -verbose -timestamps -nop4 -UbtArgs="-MaxParallelActions=4 -NoUBA -NoXGE -NoFASTBuild"
```

Es el mismo objetivo/flujo UAT usado para el paquete publicado, con rutas parametrizadas. Alpakit también puede empaquetar el plugin para Windows/Steam. No actives despliegue automático al juego; revisa el ZIP generado en `Saved/ArchivedPlugins/TsabsPassengerWagon` antes de instalar. No se promete igualdad bit a bit entre compilaciones realizadas con toolchains/rutas diferentes.

## Pruebas sin Engine

Desde una consola x64 Native Tools de MSVC, en la raíz del plugin:

```bat
call Tests\run-policy-tests.cmd
python Tests\test_native_prerequisite.py
```

El primer script compila y ejecuta26 casos en `/fp:precise` y en `/fp:fast`, incluidos frontera30, reversa, NaN e infinitos. El segundo verifica la dependencia explícita y la referencia a la clase nativa que conserva el import de la DLL. No requieren SDK para esas dos comprobaciones.

En Linux, con Bash y un compilador GCC/Clang con C++20:

```sh
CXX=clang++ bash Tests/run-policy-tests.sh
CXX=g++ bash Tests/run-policy-tests.sh
python3 -B -m unittest discover -s Tests -p 'test_*.py' -v
```

El ejecutor Bash usa archivos temporales y ejecuta los mismos 26 casos en
`-fno-fast-math` y `-ffast-math`. `CXX` selecciona un ejecutable de compilador
(por defecto, `c++`); conserva ese mismo `CXX` en las pruebas Python si quieres
probar el ejecutor con ese compilador. Las entradas especiales usan bits
IEEE-754 fijos para que Clang no transforme su generación con fast-math.
Son pruebas de políticas y del ejecutor en el host, no una compilación del
plugin Unreal.

## Integración continua

[Standalone checks](.github/workflows/standalone-checks.yml) se ejecuta en cada
push y pull request, y permite ejecución manual con `workflow_dispatch`. Solo
usa runners alojados por GitHub; no necesita secretos del repositorio ni juego/SDK:

- Ubuntu 24.04: GCC y Clang ejecutan cada uno los 26 casos de política en modo
  preciso y fast-math, las cinco pruebas Python de contratos/ejecutor y ShellCheck.
- Windows Server 2022: la herramienta x64 de Visual Studio 2022 instalada ejecuta
  `Tests/run-policy-tests.cmd` con `/fp:precise` y `/fp:fast`, seguida de los dos
  contratos Python del prerrequisito nativo. Las pruebas del ejecutor Bash son
  exclusivas de Linux y no se seleccionan en Windows.

La CI usa Python 3.12. Las versiones de compilador aparecen en los logs; el
MSVC alojado no se fija a la versión usada para compilar la publicación anterior.
Cada job tiene un límite de diez minutos. Una ejecución nueva cancela la anterior
para el mismo evento y PR/ref. El workflow tiene acceso de solo lectura, no
conserva credenciales Git y fija sus acciones a hashes de commit.

Un resultado verde solo acredita estas pruebas sin Engine. No compila ni
empaqueta el plugin Unreal, inspecciona ZIP/DLL, arranca Satisfactory/Proton ni
valida jugabilidad, entrega RPC o multijugador. La publicación y las pruebas en
juego siguen siendo independientes; este workflow no publica ni despliega nada.

## Pruebas en editor y límites

Las pruebas Unreal están en `Source/TsabsPassengerWagon/Private/Tests`. Desde la raíz del Starter Project:

```bat
"%UE_ROOT%\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" "%CD%\FactoryGame.uproject" -unattended -NullRHI -NoSound -nosplash -ExecCmds="Automation RunTests Tsabs.Passenger" -TestExit="Automation Test Queue Empty" -ReportExportPath="%CD%\Saved\PassengerTestReport"
```

Revisa el informe exportado y sus contadores, no solo el código de salida del editor. La automatización usada para018 dio27 casos correctos (24 sin avisos +3 con avisos SDK/UI), cero fallos/no ejecutados. Algunas implementaciones nativas del Starter Project son stubs: estas pruebas no demuestran transiciones nativas ni replicación en el juego real.

También se verificaron el cook/ZIP y la presencia del import real de la DLL FactoryDedicatedClient en Shipping. Se probó un arranque real Windows DX11 hasta el menú, con cero errores Streaming. El binario publicado 0.1.8 también llegó al menú en el [cliente Proton afectado](docs/PROTON-STARTUP.md); posteriormente ambos probadores dieron el visto bueno multijugador. El nuevo aviso de 0.1.9 todavía requiere una comprobación específica de cliente remoto. Los logs privados no forman parte de esta distribución.

## Verificación local 0.1.9

27 casos del editor correctos(24 sin avisos+3 SDK/UI),7 checks de paquete, política MSVC en2modos y arranque real Windows DX11 al menú. La fixture comprueba crucero sin aviso, evento de rechazo, duración3s/reinicio y limpieza al reducir velocidad. Las flags RPC no demuestran entrega de red real. Los resultados GCC/Clang/Proton de PR#1 son del autor; no se configuró CI para esa PR.
