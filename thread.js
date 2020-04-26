const addon = require("bindings")("global_mouse_events");

addon.createMouseHook((event, x, y, button, delta) => {
  process.send({ event, x, y, button, delta });
});
