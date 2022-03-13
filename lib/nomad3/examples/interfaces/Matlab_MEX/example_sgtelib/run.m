%% MINLP 1 [fval = -5]
%clc

x0 = [0;0];
opts = nomadset('display_degree',3,'max_eval',100,'bb_output_type','OBJ','param_file','param.txt');

[xr,fval,ef,iter] = nomad(@bb,x0,[-10;-10],[10;10],opts)


