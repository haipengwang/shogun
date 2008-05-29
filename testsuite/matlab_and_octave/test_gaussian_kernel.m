%
%
%
function y = test_gaussian_kernel(filename)

  eval(filename);

  sg('set_features', 'TRAIN', traindat);

  kname = ['set_kernel ', 'GAUSSIAN REAL ', num2str(size_),' ',  num2str(width_)];
  sg('send_command',kname);

  sg('send_command', 'init_kernel TRAIN');
  trainkm = sg('get_kernel_matrix');

  sg('set_features', 'TEST', testdat);
  sg('send_command', 'init_kernel TEST');
  testkm = sg('get_kernel_matrix');

  a = max(max(abs(km_test-testkm)))
  b = max(max(abs(km_train-trainkm)))
  if(a+b<1e-7)
    y = 0;
  else
    y = 1;
  end

