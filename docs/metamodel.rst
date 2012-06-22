.. _metamodel:

Metamodel
=========

This section describes the Swanson object metamodel.  It's heavily influenced by
Smalltalk's object model and the Capability Calculus *[add citation]*.


All Swanson programs and libraries consist of a sequence of *operations* that
operate on *values*.  A value consists of a *capability set* and some *content*.
A capability set is the set of named operations that relate to that particular
value, and which are valid in the current context.

An operation consists of a list of *formal parameters* and a *behavior*.  Each
formal parameter consists of an *input capability set* and an *output capability
set*.  You *call* an operation by providing a list of *actual parameters*.  The
number of actual parameters must match the number of formal parameters.  Each
actual parameter is a value, and the value's capability set must match the input
capability set of the corresponding formal parameter.  The operation then
performs its behavior, which defines how the operation will affect the content
of the actual parameter values.  (In most cases, the behavior of an operation
will be given by a chunk of Swanson code, but this is not a requirement.) Inside
of the behavior, any references to the formal parameters apply to the actual
parameter values that have been passed in.  When the behavior is finished, the
actual parameter values will have their capability sets replaced by the output
capability set of the respective formal parameter.

.. note::

   Capability sets and operations seem similar to the classes and methods of an
   object-oriented metamodel.  However, they are **not** the same.  In
   particular, the content of a Swanson value is not a set of fields, as it
   would be in an object-oriented metamodel.  And that content is not inherently
   tied to one paritcular capability set: the capabilities for a partcular value
   can change during the execution of the program.

   Operations also seem similar functions in a traditional imperative
   programming language, but again, they are **not** the same.  Operations do
   not have distinct "return values".  To return a new value, an operation will
   have a formal parameter with an empty input capability set, and an output
   capability set appropriate to the returned value.  When calling that
   function, you pass in an "empty" value, which will then be filled in with the
   function result.
