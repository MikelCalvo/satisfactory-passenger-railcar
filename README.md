<!-- compatibility-proton-startup -->
## Aviso de compatibilidad: Linux / Proton

Se ha reportado un bloqueo antes del menu con la version0.1.7 en una prueba Steam/Proton. SML sin el vagon llega al menu, pero al anadir el vagon el arranque no termina. **No considerar esta version compatible con Proton por ahora; investigacion pendiente de registros completos.** No abrir partidas personales ni cambiar Proton u opciones de arranque a ciegas. La verificacion del anfitrion Windows no valida el cliente Linux.

# Tsabs Passenger Wagon — pruebas

Paquete experimental de un vagón de pasajeros para Satisfactory Steam.

## Versión 0.1.7
- Vagón independiente, sin conducción ni inventario para el pasajero.
- Cámara exterior orbital y avisos de interacción con la tecla configurada.
- Subir y bajar solo con velocidad absoluta inferior a **30 km/h**, en ambos sentidos y manteniendo la comprobación de salida libre.
- Una plaza de prueba por vagón. No se ha validado todavía capacidad múltiple.

## Descarga e instalación
Usa el ZIP de **Releases**, no los botones automáticos «Source code».
El paquete incluye `LEEME.txt`, un perfil generado para SMM3 que solicita **solo SML 3.12.0** y la carpeta del vagón privado.
**No se importa el ZIP completo en Mod Manager.** Descomprime y sigue las fases del LEEME.

Entorno de prueba: Satisfactory Steam **1.2.4.1 / CL513885**, **SML 3.12.0**.
No usar partidas personales. Primero verificar el cargador en el menú, después el vagón y solo entonces la conexión a la partida de pruebas.

## Estado de verificación
- Compilación, paquete y arranque del anfitrión Windows comprobados.
- Las DLL y recursos de esta descarga son los mismos que los instalados y verificados en el anfitrión; se excluyen los símbolos de depuración.
- **Perfil SMM:** importacion real confirmada por un informe de prueba en SMM3.1.0; SML3.12.0 sin el vagon llega al menu. **Linux/Proton con el vagon0.1.7:** bloqueo de arranque reportado, en investigacion. **Cliente multijugador:** aun no probado.
- Este repositorio distribuye el paquete de pruebas; no incluye el SDK, el proyecto de desarrollo, guardados ni configuraciones personales.
