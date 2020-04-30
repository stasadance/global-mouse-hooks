const mouseEvents = require("./index");

mouseEvents.on("mouseup", data => {
  console.log(data);
});

mouseEvents.on("mousemove", data => {
  console.log(data);
});

mouseEvents.on("mousedown", data => {
  console.log(data);
});

mouseEvents.on("mousewheel", data => {
  console.log(data);
});

setInterval(() => {
  console.log("Still listening...");
}, 5000);