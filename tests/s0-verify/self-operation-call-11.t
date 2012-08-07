  $ swan s0 verify <<EOF
  > result1 = target:operation(param1);
  > EOF
  { "type": "operation_call", "target": "target", "operation": "operation", "params": ["result1", "target", "param1"] }
