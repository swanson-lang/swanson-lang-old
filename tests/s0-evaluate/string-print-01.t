  $ swan s0 evaluate <<EOF
  > x = "hello, world";
  > x.print(x);
  > x.~unref(x);
  > EOF
  hello, world
