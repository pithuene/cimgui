checktag
========

This is a utility to help ensure that there are no unstable stack-allocated pointers used as tags by accident.
Tags must be stable across frames to uniquely identify a widget, this is used in event handling.
If a widget is stateful, using a pointer to part of its state is the easiest way to obtain such a stable identifier.
However, if a pointer to a variable which is stack-allocated in the draw function is used by accident, this will cause loss of events and unintended behaviour because the address will sometimes change between draw calls.

The checktag utility ensures that a given pointer is not unstable.
Simply call `check_tag(void *)` with the pointer / tag, if it is unstable, the program will exit and print an error message telling you what went wrong and where the unstable tag was checked.

In release builds you should define `NDEBUG` to disable this functionality.

How it works
------------

The checktag function works by comparing the given pointer to the `draw_stack_start` pointer, which is the address of a variable in the stackframe calling the draw function, and the `check_ptr`, which is the address of a variable inside the checktag function itself.

```
                                high memory address 

                                +---------------+        
                                |               |        
         good_heap_tag -------->|               |        
                                |     heap      |        
                                |               |        
                                |               |        
                                +---------------+        
                                       |                 
                                       | heap growing    
                                       v                 
                                                         
                                                         
                                       ^                 
                                       | stack growing   
                                       |                 
                                +---------------+        
             check_ptr -------->| checktag      |       -+
                                | stackframe    |        |
                                +---------------+        |
                                | layout        |        |
               bad_tag -------->| function      |        |
                                | stackframe    |        |
                                +---------------+        |
                                |               |        | unstable
                                |      ...      |        | range
                                |               |        |
                                +---------------+        |
                                | draw function |        |
                                | stackframe    |        |
                                +---------------+        |
                                | draw function |        |
                                | caller        |        |
      draw_stack_start -------->| stackframe    |       -+
                                +---------------+        
                                |               |        
                                |      ...      |        
                                |               |        
                                +---------------+        
                                |               |        
                                |   previous    |        
        good_stack_tag -------->|   stack       |        
                                |               |        
                                +---------------+        

                                low memory address 
```
(checktag checks whether the stack grows upwards or downwards on your machine)

If the tag is in between these two (`bad_tag`), it was stack allocated during the draw function and is therefore unstable.
If it is outside the range between these two (`good_stack_tag` and ` good_heap_tag`), it was anywhere else in memory and is therefore considered stable.
Note that this doesn't necessarily mean the value is actually stable, only that it is not a stack-allocated pointer.
