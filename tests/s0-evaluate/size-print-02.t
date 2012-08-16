  $ swan s0 evaluate <<EOF
  > size = kernel.size;
  > type = size.type;
  > alloc = kernel.assignable(type);
  > int8_copy = alloc:alloc();
  > alloc:~unref();
  > size:~unref();
  > type:~unref();
  > 
  > sizeof = kernel.sizeof;
  > int8 = sizeof.int8;
  > int8_copy:"="(int8);
  > value = int8_copy:"*";
  > value:print();
  > int8:~unref();
  > int8_copy:~unref();
  > value:~unref();
  > sizeof:~unref();
  > 
  > EOF
  1
