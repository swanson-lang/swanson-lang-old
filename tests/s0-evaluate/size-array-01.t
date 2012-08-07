  $ swan s0 evaluate <<EOF
  > size_type = kernel.size;
  > 
  > sizeof = kernel.sizeof;
  > _2 = sizeof.int16;
  > _4 = sizeof.int32;
  > sizeof:~unref();
  > 
  > array_type = kernel.array(size_type, _4);
  > size_type:~unref();
  > _4:~unref();
  > alloc = kernel.explicit(array_type);
  > array_type:~unref();
  > 
  > array = alloc.alloc();
  > element = array:claim(_2);
  > element:print();
  > array = element:return();
  > array:~unref();
  > _2:~unref();
  > 
  > alloc:~unref();
  > EOF
  0