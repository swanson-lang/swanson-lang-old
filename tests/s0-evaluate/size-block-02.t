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
  > v2 = double:"()"(v1);
  > v4 = double:"()"(v2);
  > v4:print();
  > double:~unref();
  > v4:~unref();
  > EOF
  4
