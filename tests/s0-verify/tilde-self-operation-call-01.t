  $ swan s0 verify <<EOF
  > target:~operation(param1);
  > EOF
  { "type": "operation_call", "target": "target", "operation": "~operation", "params": ["target", "param1"] }
