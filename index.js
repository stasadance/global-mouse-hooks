"use strict";
const { EventEmitter } = require("events");
const { fork } = require("child_process");
const path = require("path");
let registeredEvents = [];
let mouseProcess;

class MouseEvents extends EventEmitter {
    constructor() {
        super();

        if (require("os").platform() !== "win32")
            return;

        this.on("newListener", event => {
            if (registeredEvents.indexOf(event) !== -1)
                return;

            if ((event === "mouseup" || event === "mousedown" || event === "mousemove" || event === "mousewheel") && !mouseProcess) {
                
                mouseProcess = fork(path.join(__dirname, "./thread.js"));
                mouseProcess.on("message", e => {
                    const payload = { x: e.x, y: e.y }
                    if (e.event === "mousewheel") {
                        payload.delta = FromInt32(e.delta) / 120;
                        payload.axis = e.button
                    } else if(e.event === "mousedown" || e.event === "mouseup") {
                        payload.button = e.button;
                    }
                    this.emit(e.event, payload);
                });

            } else {
                return;
            }

            registeredEvents.push(event);
        });

        this.on("removeListener", event => {
            if (this.listenerCount(event) > 0)
                return;

            if ((event === "mouseup" || event === "mousedown" || event === "mousemove" || event === "mousewheel") && mouseProcess) {
                mouseProcess.kill();
            }

            registeredEvents = registeredEvents.filter(x => x !== event);
        });

    }
}

function FromInt32(x) {
    var uint32 = x - Math.floor(x / 4294967296) * 4294967296;
    if (uint32 >= 2147483648) {
        return (uint32 - 4294967296) / 65536
    } else {
        return uint32 / 65536;
    }
}

module.exports = new MouseEvents();