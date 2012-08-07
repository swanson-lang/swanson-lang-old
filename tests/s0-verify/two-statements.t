  $ swan s0 verify <<EOF
  > x = "hello";
  > target.operation;
  > EOF
  { "type": "string_constant", "result": "x", "contents": "hello" }
  { "type": "operation_call", "target": "target", "operation": "operation", "params": [] }
