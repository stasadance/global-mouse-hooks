# global-mouse-hooks

Global mouse events listener for Node.js (Windows only) with added prebuilds.
Based off [xanderfrangos/global-mouse-events](https://github.com/xanderfrangos/global-mouse-events).

## Installation

```cmd
npm i global-mouse-hooks
```

or

```cmd
yarn add global-mouse-hooks
```

## Usage

Import the module and register for the mouse events you'd like to listen to.

### Available event listeners

**`mouseup`** / **`mousedown`** — _Fires when a mouse button is pressed / released._\
Returns:

-   **x:** The X position of the mouse, relative to the top left of the primary display.
-   **y:** The Y position of the mouse, relative to the top left of the primary display.
-   **button:** Which button was pressed. 1 is left-click. 2 is right-click. 3 is middle-click.

**`mousemove`** — _Fires when the mouse cursor is moved._\
Returns:

-   **x:** The X position of the mouse, relative to the top left of the primary display.
-   **y:** The Y position of the mouse, relative to the top left of the primary display.

**`mousewheel`** — _Fires when the mouse wheel is scrolled. Some trackpads may not fire this event unless "Scroll inactive windows when I hover over them" is disabled in the Windows settings._\
Returns:

-   **x:** The X position of the mouse, relative to the top left of the primary display.
-   **y:** The Y position of the mouse, relative to the top left of the primary display.
-   **delta:** How much the mouse wheel was scrolled. Positive numbers are considered "up" and negative numbers are "down".
-   **axis:** Whether the scroll was vertical or horizontal. 0 is vertical. 1 is horizontal.

### Example

```js
const mouseEvents = require("global-mouse-hooks");

mouseEvents.on("mouseup", (event) => {
    console.log(event); // { x: 2962, y: 483, button: 1 }
});

mouseEvents.on("mousedown", (event) => {
    console.log(event); // { x: 2962, y: 483, button: 1 }
});

mouseEvents.on("mousemove", (event) => {
    console.log(event); // { x: 2962, y: 482 }
});

mouseEvents.on("mousewheel", (event) => {
    console.log(event); // { x: 2962, y: 483, delta: -1, axis: 0 }
});
```
