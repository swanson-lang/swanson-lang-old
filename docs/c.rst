.. _c:

C implementation
================

.. highlight:: c

This section describes the C implementation of the Swanson metamodel and of the
S₀ language.


Metamodel
---------

*Values*, which consist of an *operation set* and some *content*, are
represented by an instance of :c:type:`swan_value`.

.. type:: struct swan_value

   .. member:: struct swan_opset \*opset
               void \*content

.. note::

   You will almost never allocate a :c:type:`swan_value` instance from the heap.
   Instead, it will be allocated on your local stack.  The *content* of the
   value might allocated from the heap, but again, not always; our :doc:`memory
   management interface <memory-management>` supports pretty much every memory
   allocation strategy.

There can be many :c:type:`swan_value` instances that point at the same
:c:member:`~swan_value.content`.  Some of these can be considered *aliases*,
where two independent sections of your code have a reference to some content.
Some of these are not aliases, and represent different levels of the same
section of code referring to some content.  (For instance, a local variable
that's passed in a parameter to an operation gives you two :c:type:`swan_value`
instances that point at the same content, but they aren't aliases, since only
one of those two values can be "used" at any given time.)

As mentioned when describing the :doc:`metamodel <metamodel>`, operation sets
are not an inherent property of a value's content.  Different aliases to the
same content might access that content using different operation sets.  And the
operation set of a particular :c:type:`swan_value` instance can change during
the execution of an operation.

An operation set is simply a collection of named *operations*.  Each operation
set is represented by an instance of :c:type:`swan_opset`, while each operation
is represented by an instance of :c:type:`swan_operation`.

.. type:: struct swan_opset

   .. member:: struct swan_operation \*(\*get_operation)(struct swan_opset \*opset, const char \*name)

      Return the operation with the given name.  If the operation set doesn't
      contain an operation with that name, you should set a
      :c:func:`swan_undefined` error and return ``NULL``.

   .. member:: void (\*unref)(struct swan_opset \*opset)

      Remove a reference to this operation set.  This is analogous to the
      :py:meth:`~MemoryManagement.unref` method in our :doc:`memory management
      interface <memory-management>`.

.. function:: struct swan_operation \*swan_opset_get_operation(struct swan_opset \*opset, const char \*name)

   Return the operation in *opset* with the given name.  If *opset* doesn't
   contain an operation with that name, we'll raise a :c:func:`swan_undefined`
   error and return ``NULL``.

.. function:: void swan_opset_unref(struct swan_opset \*opset)

   Unreference *opset*.


.. type:: struct swan_operation

   .. member:: int (\*evaluate)(struct swan_operation \*operation, size_t param_count, struct swan_value \*\*params)

      Evaluate *operation*.  The actual parameters are given by *param_count*
      and *params*.  The :c:type:`swan_value` instances passed in as actual
      parameters might be updated as part of the operation's behavior.  You are
      free to overwrite the operation sets of any of the actual parameters; if
      you do, you are responsible for :c:member:`unref <swan_opset.unref>`-ing
      the old operation sets first.

.. function:: int swan_operation_evaluate(struct swan_operation \*operation, size_t param_count, struct swan_value \*\*params)

   Evaluate *operation* with the given parameters.  The :c:type:`swan_value`
   instances passed in as actual parameters might be updated as part of the
   operation's behavior.  The operation is allowed to overwrite the operation
   sets of any of its parameters; if it does, the operation will unreference the
   old operation sets for you.

.. note::

   Unlike operation sets, there's no ``unref`` method for operations.  Instead,
   each operation set is responsible for freeing or unreferencing all of its
   methods when the operation set is freed.

We also have a helper method for calling an operation from a
:c:type:`swan_value` instance:

.. function:: int swan_value_evaluate(struct swan_value \*value, const char \*name, size_t param_count, struct swan_value \*\*params)

   Look for an operation with the given *name* in *value*'s operation set.  If
   the operation exists, evaluate it with the given actual parameters.

   .. note::

      *value* itself is not automatically added to the list of actual
      parameters, since not all operations take in a ``self`` parameter.



Errors
------

We use libcork's :ref:`error reporting API <libcork:errors>` for reporting and
handling error conditions within the Swanson C implementation.  The following
builtin error conditions are available:

.. macro:: SWAN_ERROR
           SWAN_UNDEFINED

   The error class and codes used for the error conditions in this section.

.. function:: void swan_undefined(const char \*fmt, ...)

   Raise a new error signifying that some named entity doesn't exist.  (For
   instance, this error is used when you ask an operation set for an operation
   that doesn't exist.)
