  $ swan s0 evaluate <<EOF
  > x = "hello, world";
  > x:print();
  > x:~unref();
  > EOF
  hello, world
