  $ swan s0 evaluate <<EOF
  > false_branch = {
  >     (size, value)
  >     _lit = "0";
  >     value = size.~literal(_lit);
  >     _lit:~unref();
  > };
  > 
  > true_branch = {
  >     (size, value)
  >     _lit = "1";
  >     value = size.~literal(_lit);
  >     _lit:~unref();
  > };
  > 
  > size = kernel.size;
  > boolean = kernel.boolean;
  > cond = boolean.false;
  > control = kernel.control;
  > control.if(cond, true_branch, false_branch, size, value);
  > value:print();
  > value:~unref();
  > cond:~unref();
  > true_branch:~unref();
  > false_branch:~unref();
  > boolean:~unref();
  > control:~unref();
  > size:~unref();
  > EOF
  0
