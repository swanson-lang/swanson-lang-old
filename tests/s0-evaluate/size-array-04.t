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
  > _4:~unref();
  > 
  > alloc = kernel.assignable(size_type);
  > sum = alloc:alloc();
  > alloc:~unref();
  > size_type:~unref();
  > 
  > block1 = {
  >     (element, size)
  >     element:"="(size);
  > };
  > 
  > block2 = {
  >     (element, size, sum)
  >     v1 = sum:"*"();
  >     v2 = element:"*"();
  >     v3 = v1:"+"(v2);
  >     sum:"="(v3);
  >     v1:~unref();
  >     v2:~unref();
  >     v3:~unref();
  > };
  > 
  > array:each(block1);
  > array:each(block2, sum);
  > result = sum:"*"();
  > result:print();
  > 
  > array:~unref();
  > sum:~unref();
  > result:~unref();
  > block1:~unref();
  > block2:~unref();
  > 
  > EOF
  6
