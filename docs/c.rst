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

.. type:: swan_value_array

   A :ref:`libcork array <libcork:array>` that contains :c:func:`swan_value`
   instances (not pointers, instances).

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

   .. member:: struct swan_operation \*(\*get_operation)(struct swan_opset \*opset, const char \*op_name)

      Return the operation with the given name.  If the operation set doesn't
      contain an operation with that name, you should set a
      :c:func:`swan_undefined` error and return ``NULL``.

   .. member:: struct swan_opset \*(\*alias)(struct swan_opset \*opset)

      Create a new reference to this operation set.  This is analogous to the
      :py:meth:`~MemoryManagement.alias` method in our :doc:`memory management
      interface <memory-management>`.

   .. member:: void (\*unref)(struct swan_opset \*opset)

      Remove a reference to this operation set.  This is analogous to the
      :py:meth:`~MemoryManagement.unref` method in our :doc:`memory management
      interface <memory-management>`.

.. function:: struct swan_operation \*swan_opset_get_operation(struct swan_opset \*opset, const char \*op_name)

   Return the operation in *opset* with the given name.  If *opset* doesn't
   contain an operation with that name, we'll raise a :c:func:`swan_undefined`
   error and return ``NULL``.

.. function:: struct swan_opset \*swan_opset_alias(struct swan_opset \*opset)

   Create a new reference to *opset*.

.. function:: void swan_opset_unref(struct swan_opset \*opset)

   Unreference *opset*.


.. type:: struct swan_operation

   .. member:: int (\*evaluate)(struct swan_operation \*operation, size_t param_count, struct swan_value \*params)

      Evaluate *operation*.  The actual parameters are given by *param_count*
      and *params*.  The :c:type:`swan_value` instances passed in as actual
      parameters might be updated as part of the operation's behavior.  You are
      free to overwrite the operation sets of any of the actual parameters; if
      you do, you are responsible for :c:member:`unref <swan_opset.unref>`-ing
      the old operation sets first.

.. function:: int swan_operation_evaluate(struct swan_operation \*operation, size_t param_count, struct swan_value \*params)

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

.. function:: int swan_value_evaluate(struct swan_value \*value, const char \*op_name, size_t param_count, struct swan_value \*params)

   Look for an operation with the named *op_name* in *value*'s operation set.
   If the operation exists, evaluate it with the given actual parameters.

   .. note::

      *value* itself is not automatically added to the list of actual
      parameters, since not all operations take in a ``self`` parameter.


.. function:: int swan_value_unref(struct swan_value \*value)

   Unreference a value by calling its :py:meth:`~MemoryManagement.unref` method.


.. macro:: struct swan_value  SWAN_VALUE_EMPTY

   A static initializer that can be used to declare an empty value on the stack.

.. function:: bool swan_value_is_empty(struct swan_value \*value)

   Return whether the given *value* is empty.



Naming conventions
------------------

This section describes the naming convention that we follow in the C
implementations of the Swanson kernel.  There are several helper macros that let
help us adhere to these conventions.

.. note::

   These macros are **not** defined in the public ``swanson.h`` header file.
   They are **only** available in the C files that implement the Swanson kernel.
   This section is intended for core Swanson developers; third-party C libraries
   are integrated into Swanson using an entirely different mechanism.


Operations
~~~~~~~~~~

For an operation named ``name`` in a family of operation sets named ``opset``,
you must define the following C instances:

* an *operation object* (an instance of :c:type:`swan_operation`), named
  ``opset__name__op``, and

* an *evaluation function* (the operation object's
  :c:member:`~swan_operation.evaluate` field), named ``opset__name__evaluate``.

Quite often, you'll want yourr operation object to have ``static`` C linkage, so
that it's not visible outside of the current compilation unit.  (Presumably this
is because you'll only refer to the operation object in an operation set defined
later in the file.) In this case, you can use the :c:macro:`_static_op_` macro
to define your operation:

.. macro:: _evaluate_(SYMBOL opset, SYMBOL name)

   Declare a new evaluation function with the given *name*, which will belong to
   a family of operation sets named *opset*.  The names of the operation's
   constituent C objects will be named according the convention described above.

   This macro should be immediately followed by the body of the evaluation
   function::

       _evaluate_(my_opset, my_operation)
       {
           printf("%p %zu %p %p\n",
                  operation, param_count, params[0].opset, params[0].content);
           return 0;
       }

   Note that the parameters of the evaluation function are declared using the
   same names as in the :c:member:`~swan_operation.evaluate` documentation.


.. macro:: _static_op_(SYMBOL opset, SYMBOL name)

   Declare a new operation instance with the given *name*, which will belong to
   a family of operation sets named *opset*.  The names of the operation's
   constituent C objects will be named according the convention described above.
   The operation instance will be declared with ``static`` C linkage.

   This macro should be immediately followed by the body of the operation's
   evaluation function::

       _static_op_(my_opset, my_operation)
       {
           printf("%p %zu %p %p\n",
                  operation, param_count, params[0].opset, params[0].content);
           return 0;
       }

   Note that the parameters of the evaluation function are declared using the
   same names as in the :c:member:`~swan_operation.evaluate` documentation.


Operation sets
~~~~~~~~~~~~~~

You will often declare a family of operation sets together.  For instance,
you'll define one operation set that contains the operations that you can
perform all instances of a particular type.  You'll then define "extensions" of
that base operation set that implement the :ref:`memory management operations
<memory-management>` in different ways: for instance, one extension for
statically allocated instances, and one for explicit heap allocated instances,
and one for garbage-collected instances.

In this case, the family of operation sets will have a name (``family``), and
each extension will have a name (``extension``).  For the built-in ``size``
type, for instance, the family name will be ``swan_size`` and the extensions
will be ``static``, ``explicit``, and ``gc``.

For each of the operation sets in this family, you must define the following C
instances:

* an *opset object* (an instance of :c:type:`swan_opset` or one of its
  subclasses), named ``_family__extension__opset`` (note the leading
  underscore),

* an *operation lookup function* (the opset object's
  :c:member:`~swan_opset.get_operation` field), named
  ``family__extension__get_operation``,

* a *free function* (the opset object's :c:member:`~swan_opset.unref` field),
  named ``family__extension__unref``, and

* an *opset function*, named ``family__extension__opset``, which returns the
  :c:type:`swan_opset` pointer for the opset object.


Operation lookup functions
^^^^^^^^^^^^^^^^^^^^^^^^^^

There are several helper macros that make it easier to write operation lookup
functions.  The first one lets you define a bare operation lookup function
(without also declaring an operation set that uses that lookup function).

.. macro:: _get_operation_(SYMBOL name)

   Declare a new operation lookup function for an operation set with the given
   name.  The operation set *name* should be of the form ``family_extension``.
   The names of the operation's constituent C objects will be named according
   the convention described above.  This macro should be immediately followed by
   the body of the lookup function::

       _get_operation_(family, extension)
       {
           printf("%p %s\n", opset, op_name);
           return 0;
       }

   Note that the parameters of the evaluation function are declared using the
   same names as in the :c:member:`~swan_opset.get_operation` documentation.

Many of the other macros defined in this section also let you provide an
operation lookup function, which presumably will be immediately attached to an
operation set that you're also defining.  Regardless of which of these macros
creates the operation lookup function for you, you can use the following macros
to list the operations defined in the set:

.. macro:: get_static_op(const char \*operation_name, SYMBOL opset, SYMBOL name)

   Declare that the current operation set contains an operation named
   *operation_name*, which is defined in a :c:type:`swan_operation` instance
   that was declared using the :c:macro:`_static_op_` macro.  Note that the
   *operation_name* is given as a C string, since Swanson operation names are
   not limited to the usual C-family identifier characters.

.. macro:: try_get_operation(SYMBOL other)

   Try to retrieve an operation from another operation set named *other*.  You
   must have declared an operation set named *other* using
   :c:macro:`_static_opset_` or :c:macro:`_extern_opset_`, or an operation
   lookup function named *other* using :c:macro:`_get_operation_`.  (This macro
   is only valid within an operation lookup function; the name of the operation
   to look for is given by the lookup function's *op_name* parameter.) If the
   other operation set has an operation with the requested name, it is returned.
   Otherwise, the macro does nothing.


Simple operation sets
^^^^^^^^^^^^^^^^^^^^^

A simple operation set is one that can be declared as a C variable (i.e., it
doesn't require any run-time state to construct it), and whose set of operations
is defined at compile time.  A large majority of the operation sets in the
Swanson kernel are simple operation sets.

.. macro:: _simple_opset_(SYMBOL name)

   Declare a new "simple" operation set with the given *name*.  The *name*
   should be of the form ``family_extension``.  The names of the operation's
   constituent C objects will be named according the convention described above.
   The opset object is declared with ``static`` C linkage.

   The operations in this set must be defined at compile time.  You define the
   operations by writing an operation lookup function.  This macro should be
   immediately followed by the body of the lookup function::

       _get_operation_(family, extension)
       {
           printf("%p %s\n", opset, op_name);
           return 0;
       }

   Note that the parameters of the lookup function are declared using the same
   names as in the :c:member:`~swan_opset.get_operation` documentation.

.. macro:: struct swan_opset \*simple_opset(SYMBOL name)

   Return a :c:type:`swan_opset` object that was declared via
   :c:macro:`_simple_opset_` or :c:macro:`_public_simple_opset_`.  This macro
   only works within the compilation unit that the operation set was defined in.



.. _s0-parsing:

S₀ parsing
----------

We provide a parser for the S₀ language that is implemented in C.  The parser is
based on callbacks; you provide a set of callback functions that are called
whenever different syntactic elements are encountered.

.. function:: int swan_s0_parse(const char \*buf, size_t length, struct swan_s0_callback \*callback)

   Parse a block of S₀ code from the buffer *buf*, which contains *length*
   bytes.  The *callback* instance is used to define what behavior should be
   executed when each syntactic element is encountered.


.. type:: struct swan_s0_callback

   .. member:: int (\*operation_call)(struct swan_s0_callback \*callback, const char \*target, const char \*operation_name, size_t param_count, const char \*\*params)

      Called when an S₀ operation call is encountered.  *target* is the name of
      the value whose operation set contains the operation to be called.
      *operation_name* is the name of the operation.  *param_count* and *params*
      provide the list of values that are passed in as actual parameters to the
      operation.

   .. member:: int (\*string_constnat)(struct swan_s0_callback \*callback, const char \*result, const char \*contents, size_t content_length)

      Called when an S₀ string constant is encountered.  *contents* and
      *content_length* give the content of the string constant.  *result* is the
      name of a value to store the string constant into.

   .. member:: int (\*finish)(struct swan_s0_callback \*callback)

      Called when the end of the block of S₀ code is encountered.  This callback
      can be used for any checks that can't be performed until all of the code
      in the block has been processed.



.. _s0-evaluation:

S₀ evaluation
-------------

The functions in this section implement the basic, "primordial" evaluation
strategy for S₀ code.  The semantics of this evaluation match up with what's
defined in the :ref:`S₀ language reference <s0>`.

Scopes
~~~~~~

The *scope* object is the main bookkeeping object that we need when evaluating
S₀ code.  Each S₀ statement consumes and produces a set of Swanson values.
Within the S₀ code, the values are given names.  S₀ does not have any complex
scoping rules; each block of S₀ code has a flat namespace of objects that is
works with.  In addition to the objects that are created by the code within the
block, you can also provide a set of *predefined values* that are added to the
block's scope when the block is created.  Higher level code generation logic can
simulate closures, dynamic scoping, and lexical scoping by constructing S₀
blocks and setting predefined values in different ways.

.. type:: struct swan_scope

   Contains the named set of values that a block of S₀ code works with.

   .. member:: const char \*name

      The name of this scope.

.. function:: struct swan_scope \*swan_scope_new(const char \*name)

   Create a new scope object with the given *name*.

.. function:: void swan_scope_free(struct swan_scope \*scope)

   Free the given scope object.  Any non-empty values that are still in the
   scope will be unreferenced.

.. function:: int swan_scope_check_values(struct swan_scope \*scope)

   Verify that any values added to the scope using :c:func:`swan_scope_add` have
   been unreferenced.  Values that were added using
   :c:func:`swan_scope_add_predefined` do not need to be unreferenced.  (This is
   how we distinguish between the S₀ code's predefined values, and the values
   that it creates with operation calls.  The S₀ language requires the second
   group of values to be unreferenced by the code that creates them.)

.. function:: void swan_scope_add(struct swan_scope \*scope, const char \*name struct swan_value \*value)
              void swan_scope_add_predefined(struct swan_scope \*scope, const char \*name struct swan_value \*value)

   Add a *value* with the given *name* to the *scope* object.  If there is
   already a value with this name in the scope, it will be overwritten.  In this
   case, it is the responsibility of the code that created the new value to
   unreference the old value before calling this method.  (This is the same
   delegation of responsibilities for the values passed in to an operation
   call.)

   We do not create a new reference to the value, so you should consider *value*
   to be invalid after these functions return.

   The ``_add_predefined`` variant should be used to add the predefined values
   to the scope object; since the S₀ code associated with the scope did not
   create these values, the code is not responsible for unreferencing the
   values.  The ``_add`` variant should be used for values created by the S₀
   code; these values must be freed by the S₀ code before the block terminates.

.. function:: void swan_scope_get(struct swan_scope \*scope, const char \*name struct swan_value \*dest)

   Retrieve the value with the given *name* in the *scope* object, storing the
   value into *dest*.  We do not create a new reference to the value.  If there
   is not value with the given name, *dest* will be filled in with an empty
   value.

Evaluation
~~~~~~~~~~

.. type:: struct swan_s0_evaluator

   An object that lets you evaluate a block of S₀ code in terms of a particular
   scope object.

   .. member:: struct swan_s0_callback  callback

      An :ref:`S₀ parsing callback object <s0-parsing>`.  You can use this
      callback object with the :c:func:`swan_s0_parse` function to evaluate some
      S₀ code.

   .. member:: struct swan_scope  \*scope

      The scope object that will be used to evaluate the S₀ code.  You can add
      predefined values to this scope using :c:func:`swan_scope_add_predefined`
      before evaluating S₀ code.

.. function:: struct swan_s0_evaluator \*swan_s0_evaluator_new_empty(const char \*scope_name)

   Create a new evaluator object with an initially empty scope.  You must add
   any predefined values to the :c:member:`~swan_s0_evaluator.scope` object
   yourself before evaluating any S₀ code with this evaluator.

.. function:: struct swan_s0_evaluator \*swan_s0_evaluator_new_kernel(void)

   Create a new evaluator object that makes the :doc:`Swanson kernel <kernel>`
   available as a predefined value named ``kernel``.  This evaluator is useful
   for evaluating the definition of the :doc:`Swanson prelude <prelude>`.

.. function:: void swan_s0_evaluator_free(struct swan_s0_evaluator \*eval)

   Free an evaluator object.



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
