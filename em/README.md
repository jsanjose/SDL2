# EMSCRIPTEN SDL2 EJEMPLOS

## Instalación de emscripten

Seguir los pasos descritos en la guía:

> https://emscripten.org/docs/getting_started/index.html

## Compilar ejemplos

Abrir el fichero "cpp" correspondiente y lanzar la tarea de compilación con: **CTRL + SHIFT + B**

Para ejecutar usar un servidor local (p.ej. la extensión *Live Server*) y abrir el HTML generado

## Depuración

Se puede depurar el código generado usando Edge o Chrome.

Se debe habilitar la opción de *WebAssembly Debugging: Enable DWARF support* desde la configuración *Experimental* de las herramientas de desarrollador.

Se recomienda instalar la extensión *C/C++ DevTools Support (DWARF)* que se puede obtener en el siguiente enlace: https://goo.gle/wasm-debugging-extension

Más detalles en:

> https://developer.chrome.com/blog/wasm-debugging-2020/
