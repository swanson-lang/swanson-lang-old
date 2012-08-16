  $ swan s0 evaluate <<EOF
  > size = kernel.size;
  > _lit = "0";
  > value = size.~literal(_lit);
  > _lit:~unref();
  > value:print();
  > value:~unref();
  > size:~unref();
  > EOF
  0
