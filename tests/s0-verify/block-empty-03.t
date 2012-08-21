  $ swan s0 verify <<EOF
  > result = { (param1, "param 2") };
  > EOF
  { "type": "start_block", "target": "result", "params": ["param1", "param 2"] }
  { "type": "end_block" }
