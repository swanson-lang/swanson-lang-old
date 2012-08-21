  $ swan s0 evaluate <<EOF
  > size = kernel.size;
  > size_type = size.type;
  > 
  > _lit = "4";
  > _4 = size.~literal(_lit);
  > _lit:~unref();
  > size:~unref();
  > 
  > array_type = kernel.array(size_type, _4);
  > alloc = kernel.assignable(array_type);
  > array = alloc:alloc();
  > alloc:~unref();
  > array_type:~unref();
  > size_type:~unref();
  > _4:~unref();
  > 
  > block = {
  >     (element, size)
  >     value = element:"*"();
  >     value:print();
  >     value:~unref();
  > };
  > 
  > array:each(block);
  > array:~unref();
  > block:~unref();
  > 
  > EOF
  0
  0
  0
  0
