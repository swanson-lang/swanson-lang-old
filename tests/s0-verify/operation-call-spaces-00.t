  $ swan s0 verify <<EOF
  > "target with spaces"."operation with spaces";
  > EOF
  { "type": "operation_call", "target": "target with spaces", "operation": "operation with spaces", "params": [] }
