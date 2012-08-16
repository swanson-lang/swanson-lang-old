  $ swan s0 evaluate <<EOF
  > size = kernel.size;
  > _lit = "not a number";
  > value = size.~literal(_lit);
  > _lit:~unref();
  > value:print();
  > value:~unref();
  > size:~unref();
  > EOF
  Invalid size literal "not a number"
  [1]
