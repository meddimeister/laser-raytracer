Procedure for Unix/Linux/mac OSX

1- Set the NOMAD_HOME environment variable
2- Compile with the command make
3- Go to one of the problem directories
4- Compile the black box (g++ -o bb.exe bb.cpp)
5- Start the optimization: mpirun -np p ../../psdmads.exe param.txt bbe ns
   Ex: mpirun -np 4 ../../psdmads.exe param.txt 20 3
   p: Number of processors to use
   bbe: Maximal number of evaluations performed by each slave
   ns: Number of variables considered by the slaves.


Procedure for Windows

1- Open the psdmads Microsoft VisualStudio solution located in %NOMAD_EXAMPLES%\VisualStudio
2- Build the solution
3- Open a VisualStudio command shell
4- Change the directory to one of the problem directories in %NOMAD_EXAMPLES%\tools\PSD-MADS\problems
5- Build the blackbox executable with the command: cl bb.cpp
6- Start the optimization: mpirun -np p ../../psdmads.exe param.txt bbe ns
   p: Number of processors to use
   bbe: Maximal number of evaluations performed by each slave
   ns: Number of variables considered by the slaves.
   Ex: mpiexec -n 4 ..\..\psdmads.exe param.txt 20 3
