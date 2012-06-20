.. _memory-management:

Memory management
=================

.. default-domain:: py

Everything that you can do to a value is represented by some operation.  Memory
management is no different.  The operations described in this section can be
used to allocate and deallocate values, and to track *aliases* to those values.
(In Swanson, an alias is when there are two values referring to the same
content.)


.. class:: MemoryManagement

   .. method:: alloc(dest)

      ::

          dest: → α

      Allocates a new instance of a value, storing a reference to it in *dest*.

   .. method:: alias(self, dest)

      ::

          self: α → α
          dest: → α

      Creates a new alias to *self* in *dest*.

   .. method:: unref(self)

      ::

          self: α → ()

      Removes a reference to *self*.  If *self* is the last reference to the
      underlying object, it will be deallocated and freed.  *self* will no
      longer be a valid object after this call.


Not all allocation strategies will implement all of these methods.  For
instance, static data can't be allocated, since it exists when the program
starts or the library is loaded.  Thus, the operation set for static data will
only include the :meth:`~MemoryManagement.alias` and
:meth:`~MemoryManagement.unref` methods.  Static data is never actually freed,
either, so both of these methods are no-ops.

For heap-allocated, reference-counted data, the :meth:`~MemoryManagement.alias`
method increments the reference count of the data, while
:meth:`~MemoryManagement.unref` decrements it.

For heap-allocated, garbage-collected data, something else happens.

For heap-allocated, explicitly managed data, you get a _unique pointer*.  The
:meth:`~MemoryManagement.unref` method will immediately free the data.  There
won't be a :meth:`~MemoryManagement.alias` method, since only one pointer to the
data is allowed.  (You can still *use* the data for the duration of its
lifetime, since you can pass the value into an arbitrary number of operation
calls; however, those operations will not have any way to retain a reference to
the data after their calls return.)

For stack-allocated data, there also won't be any
:meth:`~MemoryManagement.alias` method.  This prevents you from creating any
dangling references to the data.  (Like with explicitly managed heap data, you
can still *use* the data as much as you want for the duration of its lifetime,
but you can't create additional aliases to the data.)  The
:meth:`~MemoryManagement.unref` will be a no-op, since the memory will be
automatically freed when the stack frame moves out of scope.
