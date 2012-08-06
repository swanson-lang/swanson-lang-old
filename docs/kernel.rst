.. _kernel:

Kernel
======

.. default-domain:: py

Most of the :doc:`Swanson prelude <prelude>` is implemented in Swanson itself.
However, a small core, known as the *kernel*, must be implemented in a
*bootstrap language*.  This kernel must provide enough functionality to be able
to implement the prelude.  However, it doesn't need to be too sophisticated,
since most of the "meat" of the prelude will be implemented on top of this core
kernel in Swanson itself.


.. function:: kernel.explicit

   ::

       dest: → Allocator
       type: type (aliasable)

   Create an :class:`Allocator` for *type* that allocates new instances from the
   heap.  The new instances will *not* be aliasable.


.. class:: kernel.sizeof

   .. attribute:: int8
                  int16
                  int32
                  int64

      The size of these types, in bytes, on the current machine.  (These values
      are exceptionally interesting, since they're the same on all platforms.)

   .. attribute:: char
                  short
                  int
                  long

      The size of these C types, in bytes, on the current machine.

   .. attribute:: pointer

      The size of a pointer, in bytes, on the current machine.

   .. attribute:: size

      The size of an integer that is large enough to index into memory on the
      current machine.  (This is equivalent to the C ``size_t`` type.)
