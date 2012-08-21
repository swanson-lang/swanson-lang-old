  $ swan s0 verify <<EOF
  > result = {
  >     (param)
  >     x = param.method(param2);
  > };
  > EOF
  { "type": "start_block", "target": "result", "params": ["param"] }
  { "type": "operation_call", "target": "param", "operation": "method", "params": ["x", "param2"] }
  { "type": "end_block" }
