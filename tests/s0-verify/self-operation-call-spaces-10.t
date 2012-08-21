  $ swan s0 verify <<EOF
  > "result1 with spaces" = "target with spaces":"operation with spaces";
  > EOF
  { "type": "operation_call", "target": "target with spaces", "operation": "operation with spaces", "params": ["target with spaces", "result1 with spaces"] }
