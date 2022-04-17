widgets
=======

Widgets encapsulate both the functions as well as the data required to draw a UI element.
To acomplish this, a widget is a struct beginning with a `widget_t` called `widget`, this contains the draw function aswell as a size function to determine the size of the widget without actually drawing it yet.

The rest of the struct can contain any data specific to the widget.
