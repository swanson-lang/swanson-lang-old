# S₀ take 2

Kinds of thing:

- atom
- binary literal
- block
- environment (maps names to things)
- type


Types:

- void
- specific kind of thing
- atom with a specific identity
- environment with specific contents (maps names to types; not exclusive)


Statements:

- create thing in current environment
- invoke block (mapping of names in current environment to names in block spec)

  semantics:
    - create a new environment with the names in block spec pointing to the
      corresponding values from the calling environment (must match spec's input
      types)
    - execute each statement in the block
    - copy contents of any remaining names in block environment (which must
      match the spec's output types) back into the calling environment

every statement associated with a location (source name, line/column range)


Blocks:

  spec + list of statements
  spec maps names to input/output types

  associated with an environment when invoked; block has an implicit type for
  this environment before and after every statement, which can be derived
  automatically
