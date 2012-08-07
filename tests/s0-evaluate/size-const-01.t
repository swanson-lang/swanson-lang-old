  $ swan s0 evaluate <<EOF
  > sizeof = kernel.sizeof;
  > int8 = sizeof.int8;
  > int16 = sizeof.int16;
  > int16:"="(int8);
  > EOF
  Value doesn't contain operation named =
  [1]
