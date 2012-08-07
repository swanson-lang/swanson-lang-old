  $ swan s0 verify <<EOF
  > "result1 with spaces", "result2 with spaces" = "target with spaces":"operation with spaces"("param1 with spaces");
  > EOF
  { "type": "operation_call", "target": "target with spaces", "operation": "operation with spaces", "params": ["result1 with spaces", "result2 with spaces", "target with spaces", "param1 with spaces"] }
