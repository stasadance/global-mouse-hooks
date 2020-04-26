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

mouseEvents.on("scroll", data => {
  console.log(data);
});