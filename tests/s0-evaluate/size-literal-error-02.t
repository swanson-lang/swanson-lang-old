  $ swan s0 evaluate <<EOF
  > size_type = kernel.size;
  > alloc = kernel.explicit(size_type);
  > size_type:~unref();
  > 
  > _lit = "not a number";
  > size = alloc.alloc();
  > size:~literal(_lit);
  > _lit:~unref();
  > size:print();
  > size:~unref();
  > 
  > alloc:~unref();
  > EOF
  Invalid size literal "not a number"
  [1]
