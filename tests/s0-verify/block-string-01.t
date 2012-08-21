  $ swan s0 verify <<EOF
  > result = {
  >     (param)
  >     x = "string constant";
  > };
  > EOF
  { "type": "start_block", "target": "result", "params": ["param"] }
  { "type": "string_constant", "result": "x", "contents": "string constant" }
  { "type": "end_block" }
