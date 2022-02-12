element
=======

Elements encapsulate a widget along with other data like

- its size inside the container (relative or absolute)
- its alignment inside the container
- ...

For example rows and columns layout a list of elements, each of which can individually specify its alignment.

Sizing
------

The size given to an element is not a hard limit,
it is used to calculate the constraints with which the widget is drawn.
The widget can however ignore the constraints and the container should therefore handle widgets based on the size they report themselves.
