const path = require("path");

// Ruta al módulo nativo compilado
const cursorLock = require("./build/Release/cursor_lock.node");

cursorLock.lockCursor(0, 0, 1080, 600);
console.log("🧲 Cursor bloqueado en 1080x600");

setTimeout(() => {
  cursorLock.unlockCursor();
  console.log("✅ Cursor desbloqueado");
}, 5000);
