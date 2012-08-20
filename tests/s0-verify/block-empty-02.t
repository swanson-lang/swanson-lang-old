  $ swan s0 verify <<EOF
  > result = { (param) };
  > EOF
  { "type": "start_block", "target": "result", "params": ["param"] }
  { "type": "end_block" }
