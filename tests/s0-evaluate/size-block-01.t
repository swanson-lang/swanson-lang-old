  $ swan s0 evaluate <<EOF
  > size = kernel.size;
  > 
  > _lit = "1";
  > v1 = size.~literal(_lit);
  > _lit:~unref();
  > size:~unref();
  > 
  > double = {
  >     (out, in)
  >     out = in:"+"(in);
  >     in:~unref();
  > };
  > 
  > double:"()"(v2, v1);
  > v2:print();
  > double:~unref();
  > v2:~unref();
  > EOF
  2
