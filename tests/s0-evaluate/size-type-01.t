  $ swan s0 evaluate <<EOF
  > size_type = kernel.size;
  > opset = size_type:opset;
  > size_type:~unref();
  > opset:~unref();
  > EOF
