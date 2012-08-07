  $ swan s0 verify <<EOF
  > "result with spaces" = "hello, world";
  > EOF
  { "type": "string_constant", "result": "result with spaces", "contents": "hello, world" }
