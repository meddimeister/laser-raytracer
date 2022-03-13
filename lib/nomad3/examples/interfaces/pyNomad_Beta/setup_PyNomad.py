from distutils.core import setup, Extension
from Cython.Build import cythonize

import numpy as np
import os
import sys
import glob

if sys.platform.startswith('win'):
    if ( str(os.environ.get('NOMAD_HOME_PERSONAL')) == '' or str(os.environ.get('NOMAD_HOME_PERSONAL')) == 'None'):
        print ("A NOMAD_HOME_PERSONAL environment variable is needed for building Nomad for Python (PyNomad) \n")
        exit() 
    os_include_dirs1 = str(os.environ.get('NOMAD_HOME_PERSONAL'))+'/src'
    os_include_dirs2 = str(os.environ.get('NOMAD_HOME_PERSONAL'))+'/ext/sgtelib/src'
    os_include_dirs3 = np.get_include()
	# Libraries located in bin directory for windows (other os --> in lib directory)
    os_library_dirs = str(os.environ.get('NOMAD_HOME_PERSONAL')) + '/bin' 
	
else:
    if ( str(os.environ.get('NOMAD_HOME')) == '' or str(os.environ.get('NOMAD_HOME')) == 'None'):
        print ("A NOMAD_HOME environment variable is needed for building Nomad for Python (PyNomad) \n")
        exit()
    os_include_dirs1 = str(os.environ.get('NOMAD_HOME'))+'/src'
    os_include_dirs2 = str(os.environ.get('NOMAD_HOME'))+'/ext/sgtelib/src'
    os_include_dirs3 = np.get_include()
    os_library_dirs = str(os.environ.get('NOMAD_HOME')) + '/lib'

compile_args = ['-w']
link_args = []


# Look for librairies in Nomad distribution
if sys.platform.startswith('linux'):
    link_args.append('-Wl,-rpath,'+str(os.environ.get('NOMAD_HOME'))+'/lib')

# Prevent error message when changing the location of libnomad.so (OSX)
if sys.platform == 'darwin':
     link_args.append('-headerpad_max_install_names')
     os.environ['CC'] = "gcc"
     os.environ['CXX'] = "g++"    

setup(
	ext_modules = cythonize(Extension(
           "PyNomad",                                # the extension name
           sources=["PyNomad.pyx", "nomadCySimpleInterface.cpp"], # the Cython source and
	       include_dirs=[ os_include_dirs1 , os_include_dirs2, os_include_dirs3 ],		   #[str(os.environ.get('NOMAD_HOME_PERSONAL'))+'/src',str(os.environ.get('NOMAD_HOME_PERSONAL'))+'/ext/sgtelib/src', np.get_include()],
           extra_compile_args=compile_args,
           extra_link_args=link_args,
           language = 'c++',
           libraries = ['nomad'],
           library_dirs= [ os_library_dirs ] ))
)



if sys.platform == 'darwin':
    for PyNomadLib in glob.glob("PyNomad*.so"):
        os.system('install_name_tool -change libnomad.so '+os.environ.get('NOMAD_HOME')+'/lib/libnomad.so '+PyNomadLib) 
