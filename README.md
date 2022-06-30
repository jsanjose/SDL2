# SDL2 Tests

## Para compilar:

En propiedades del proyecto comprobar que:

**Directorios de VC++**

- **Directorios de archivos de inclusión**: añadir las carpetas *`"include"`* de `SDL2` y `SDL2_ttf`.

- **Directorios de archivos de bibliotecas**: añadir las carpetas *`"lib\x64"`* de `SDL2` y `SDL2_ttf`.

- **Vinculador-Entrada-Dependencias adicionales**: añadir `SDL2.lib`, `SDL2main.lib` y `SDL2_ttf.lib`.

## Para ejecutar:

En el PATH deben estar las rutas a las DLLs:

```
...\SDL2lib\SDL2-2.0.22\lib\x64

...\SDL2lib\SDL2_ttf-2.0.18\lib\x64
```
