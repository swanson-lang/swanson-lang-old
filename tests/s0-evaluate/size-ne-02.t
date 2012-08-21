  $ swan s0 evaluate <<EOF
  > size = kernel.size;
  > 
  > _lit = "0";
  > value1 = size.~literal(_lit);
  > _lit:~unref();
  > 
  > _lit = "1";
  > value2 = size.~literal(_lit);
  > _lit:~unref();
  > 
  > result = value1:"!="(value2);
  > result:print();
  > 
  > value1:~unref();
  > value2:~unref();
  > result:~unref();
  > size:~unref();
  > EOF
  true
