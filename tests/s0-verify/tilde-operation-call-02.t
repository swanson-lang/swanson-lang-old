  $ swan s0 verify <<EOF
  > target.~operation(param1, param2);
  > EOF
  { "type": "operation_call", "target": "target", "operation": "~operation", "params": ["param1", "param2"] }
