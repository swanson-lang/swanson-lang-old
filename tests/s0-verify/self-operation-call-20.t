  $ swan s0 verify <<EOF
  > result1, result2 = target:operation;
  > EOF
  { "type": "operation_call", "target": "target", "operation": "operation", "params": ["target", "result1", "result2"] }
