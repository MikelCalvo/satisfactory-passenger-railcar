# Tsabs Passenger Wagon

Vagón de pasajeros experimental para **Satisfactory Steam**. Código propio bajo **MIT**.

## Estado: v0.1.8 preliminar

- Una plaza técnica invisible por vagón; cámara exterior orbital, sin conducir el tren y sin inventario.
- Subir/bajar exige velocidad absoluta **estrictamente inferior a30km/h**, también en reversa, además de las comprobaciones de autoridad, ocupación y salida nativa libre. No limita la velocidad del tren mientras se viaja.
- Coste/desbloqueo de pruebas; no es una versión final de equilibrio o capacidad.
- 018 añade el prerrequisito nativo FactoryDedicatedClient. En una prueba real Windows DX11 los errores de importación pasaron de454 a0 y se alcanzó el menú con SML. Las fuentes de cámara/HUD/controles/guardas no cambian respecto a017.
- **Linux/Steam/Proton: corrección candidata, aún sin validación remota de018.**017 tenía un bloqueo de arranque reportado en Proton. No anunciarlo como resuelto hasta comprobar el equipo afectado. No cambiar simultáneamente Proton ni overrides.
- Partida del amigo, multijugador, viaje con conductor humano/automático y múltiples plazas siguen pendientes de verificación.

## Instalar y probar

Descarga el asset **TsabsPassengerWagon-0.1.8-Steam-Proton-Publico.zip** de la release v0.1.8, no el archivo automático Source code si solo quieres jugar. Requiere juego Steam1.2.4.1/CL513885 y SML3.12.0. Las instrucciones escalonadas están en LEEME.txt del ZIP. SML no se incluye.

Con el juego cerrado, conserva fuera de FactoryGame/Mods una copia del vagón anterior y reemplaza su carpeta por VAGON/TsabsPassengerWagon del ZIP. No mezcles DLLs/versiones. Mantén SML en su carpeta independiente. Si falla el nuevo arranque, retira solo el vagón y conserva el log de ese intento antes de volver a abrir el juego.

Primera comprobación: **solo llegar al menú**, sin abrir partidas personales. En el cliente afectado no iniciar aún la prueba multijugador. Las pruebas de juego se hacen exclusivamente en tsabsrailtest, con copia de seguridad.

## Código abierto y compilación

Este repositorio contiene el código propio completo de esta versión, descriptor, tests y documentación. Ver [BUILDING.md](BUILDING.md), [LICENSE](LICENSE), [NOTICES.md](NOTICES.md) y [CHANGELOG.md](CHANGELOG.md). El tag v0.1.8 corresponde al binario de esta release. El manifiesto SOURCE-MANIFEST.json permite contrastar los archivos propios con el paquete compilado.

El SDK, Unreal Engine, Wwise, SML y los datos/recursos de Satisfactory se obtienen por separado de sus distribuidores; **no se redistribuyen ni relicencian aquí**. No se incluyen guardados, configuración personal, diagnósticos privados ni símbolos PDB.
