  $ swan s0 verify <<EOF
  > result = {
  >     (param)
  >     inner = {
  >         (param2)
  >     };
  > };
  > EOF
  { "type": "start_block", "target": "result", "params": ["param"] }
  { "type": "start_block", "target": "inner", "params": ["param2"] }
  { "type": "end_block" }
  { "type": "end_block" }
