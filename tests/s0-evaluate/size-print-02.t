  $ swan s0 evaluate <<EOF
  > size_type = kernel.size;
  > alloc = kernel.explicit(size_type);
  > size_type:~unref();
  > 
  > sizeof = kernel.sizeof;
  > int8_copy = alloc.alloc();
  > int8 = sizeof.int8;
  > int8_copy:"="(int8);
  > int8_copy:print();
  > int8:~unref();
  > int8_copy:~unref();
  > sizeof:~unref();
  > 
  > alloc:~unref();
  > EOF
  1