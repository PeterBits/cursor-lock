## Guía paso a paso: Addon nativo C++ para bloquear el cursor en Windows con Electron y Node.js

### 🎯 Objetivo

Crear una extensión (addon) nativo en C++ que permita bloquear el cursor dentro de una región de la pantalla en Windows, integrada en una aplicación de Electron con Node.js.

---

## 1. Requisitos previos

1. **Node.js** (recomendado v14+).
2. **Python 3.x**: necesario para compilar con `node-gyp`.
3. **Visual Studio Build Tools** (compilador de C++ y Windows SDK).

Instalación recomendada:

````bash
node-gyp configure   # Genera archivos de proyecto:
                    # - build/config.gypi        (configuración interna)
                    # - build/Release/           (directorio destino de compilación)
                    # - build/*.sln              (solución de Visual Studio)
                    # - build/*.vcxproj          (proyecto C++ para MSBuild)
                    ```

Al final debe aparecer en consola `build finished successfully` y:
````

build/Release/cursor\_lock.node

```

## 6. Archivos generados por `node-gyp configure`
Cuando ejecutas `node-gyp configure`, se crean varios archivos y carpetas en el directorio `build/`:

- **`build/config.gypi`**: Archivo de configuración interno que contiene variables y rutas usadas por `node-gyp`.
- **`build/*.sln`**: Archivo de solución de Visual Studio. Equivalente a un proyecto multi-archivo; puedes abrirlo en VS.
- **`build/*.vcxproj`**: Proyecto individual de C++ para MSBuild, que define cómo compilar tu addon.
- **`build/Release/`**: Carpeta donde se colocarán los artefactos compilados (incluyendo `cursor_lock.node`).

Estos archivos permiten que Visual Studio o MSBuild gestione la compilación de tu código C++ en el entorno de Windows.

---
build/Release/cursor_lock.node
```

---

## 7. Probar el módulo (`test.js`)

```js
// test.js
const cursorLock = require('./build/Release/cursor_lock.node');

// Bloquear cursor en 0,0 → 800x600
cursorLock.lockCursor(0, 0, 800, 600);
console.log('🧲 Cursor bloqueado en 800x600');

setTimeout(() => {
  cursorLock.unlockCursor();
  console.log('✅ Cursor desbloqueado');
}, 5000);
```

Ejecución:

```bash
node test.js
```

Deberías ver el cursor confinado en la región indicada durante 5 segundos.

---

## 8. Integración en Electron

En tu `main.js` o `main.ts` de Electron:

```ts
const { app, BrowserWindow } = require('electron');
const cursorLock = require('./native-cursor-addon/build/Release/cursor_lock.node');

app.whenReady().then(() => {
  const win = new BrowserWindow({ width: 800, height: 600 });
  win.loadFile('index.html');

  win.once('ready-to-show', () => {
    const b = win.getBounds();
    cursorLock.lockCursor(b.x, b.y, b.width, b.height);

    // Opcional: desbloquear tras un tiempo
    setTimeout(() => cursorLock.unlockCursor(), 10000);
  });
});
```

---

## 9. Compilar desde un subdirectorio

Si copiaste la carpeta del addon dentro de tu proyecto y `binding.gyp` ya no está en la raíz, debes indicar correctamente dónde ejecutar cada comando.

### 9.1. Entra al directorio del addon

```bash
cd native-cursor-addon
```

Este paso es **obligatorio** antes de usar `npm install` o `node-gyp`, porque estas herramientas buscan `package.json` o `binding.gyp` en el directorio actual.

### 9.2. Instala las dependencias y compila (método recomendado)

Una vez dentro de `native-cursor-addon/`, ejecuta:

```bash
npm install node-addon-api
npm install    # Si tienes un package.json en el addon con node-addon-api
node-gyp configure
node-gyp build
```

- **No mezcles** `npm install` y `node-gyp` en la misma línea, ya que `npm install` no acepta la opción `--directory` para el addon.
- Después de `configure` y `build`, encontrarás el binario en:

native-cursor-addon/build/Release/cursor\_lock.node

````

### 9.3. Uso de `--directory` para node-gyp (opcional)

Si prefieres ejecutar los comandos desde la raíz del proyecto, **solo** `node-gyp` admite el flag `--directory`. Por ejemplo, desde `TPV_1/`:

```bash
# Instala node-addon-api dentro del addon
cd TPV_1/native-cursor-addon
npm install node-addon-api
cd ..

# Ejecuta node-gyp desde la raíz apuntando al addon
node-gyp configure --directory native-cursor-addon
node-gyp build     --directory native-cursor-addon
````

> **Nota:** `npm install` **no** soporta `--directory` para instalar paquetes en subdirectorios. Siempre debes instalar dependencias desde su propio directorio.

---

## 10. Llevar el addon a otros proyectos Llevar el addon a otros proyectos

### Opción A: Copia manual

1. Copia carpeta `native-cursor-addon/` a tu proyecto.
2. Ajusta rutas en `require`.
3. Ejecuta `npm install node-addon-api && node-gyp configure && node-gyp build`.

### Opción B: Paquete local

```json
"dependencies": {
  "cursor-lock": "file:./native/cursor-lock"
}
```

### Opción C: Módulo en GitHub

git install + compilación automática.

---

## 11. Publicar como librería npm (paquete reutilizable)

Para convertir tu addon en una **librería npm** que puedas instalar desde cualquier proyecto, sigue estos pasos:

1. \*\*Añade o ajusta tu \*\*\`\` en la carpeta de tu addon:

   ```json
   {
     "name": "@tu-namespace/cursor-lock",
     "version": "1.0.0",
     "description": "Node.js addon para bloquear el cursor en Windows",
     "main": "build/Release/cursor_lock.node",
     "scripts": {
       "install": "node-gyp rebuild"
     },
     "dependencies": {
       "node-addon-api": "^<versión>"
     },
     "gypfile": true,
     "files": [
       "build/Release/cursor_lock.node"
     ],
     "author": "Tu Nombre <tu.email@ejemplo.com>",
     "license": "MIT"
   }
   ```

   - \`\`: usa un scope si trabajas en una organización. Ej: `@anjana-investments/cursor-lock`.
   - \`\`: apunta al módulo compilado.
   - \`\`: hará que `npm install` ejecute `node-gyp rebuild` para compilar el addon automáticamente.
   - \`\`: permite que npm reconozca que usas `binding.gyp`.
   - \`\`: incluye solo los artefactos necesarios.

2. **Publica en npm**

   - Inicia sesión: `npm login`
   - Publica: `npm publish --access public`

   Si usas un scope privado en una organización, puedes indicar `--access restricted`.

3. **Instala desde otro proyecto**

   ```bash
   npm install @tu-namespace/cursor-lock
   ```

   npm ejecutará el script `install` que definiste y compilará el addon en cada máquina.

4. **Uso en código**

   ```js
   const cursorLock = require('@tu-namespace/cursor-lock');
   cursorLock.lockCursor(0, 0, 800, 600);
   cursorLock.unlockCursor();
   ```

---



