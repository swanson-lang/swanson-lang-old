  $ swan s0 evaluate <<EOF
  > sizeof = kernel.sizeof;
  > int8 = sizeof.int8;
  > int8:print();
  > int8:~unref();
  > sizeof:~unref();
  > EOF
  1
