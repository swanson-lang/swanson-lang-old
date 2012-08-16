  $ swan s0 evaluate <<EOF
  > size = kernel.size;
  > size_type = size.type;
  > size:~unref();
  > 
  > sizeof = kernel.sizeof;
  > _2 = sizeof.int16;
  > _4 = sizeof.int32;
  > sizeof:~unref();
  > 
  > array_type = kernel.array(size_type, _4);
  > alloc = kernel.assignable(array_type);
  > array = alloc:alloc();
  > alloc:~unref();
  > array_type:~unref();
  > size_type:~unref();
  > _4:~unref();
  > 
  > element = array:"[]"(_2);
  > value = element:"*"();
  > value:print();
  > value:~unref();
  > element:~unref();
  > _2:~unref();
  > 
  > EOF
  0
