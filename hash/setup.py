from os.path import realpath
frameworkArgs = [ "-F" + realpath("/Applications/Xcode.app/Contents/Developer/Library/Frameworks"), # Xcode.app may be symbolic link
                  "-framework","Python3",
                  "-F/Library/Frameworks", # assume SED tools have been installed: sedutil, cifra, log
                  "-framework","sedutil", "-framework","cifra", "-framework","log" ]

from distutils.core import Extension, setup
module = Extension('PyExtHash',
                    sources = ['PyDtaHashPassword.cpp', 'DtaHashPassword.cpp'],
                    include_dirs = ['../Common','../Common/pbkdf2', '../linux' , '../License', '../../sedutil/macos/sedutil project' ],
                    extra_compile_args = ["-Wno-narrowing", "-std=c++11",'-fpermissive','-fPIC', '-w'] + frameworkArgs,
                    extra_link_args = ["-fno-exceptions"] + frameworkArgs
                  )
setup (name = 'PyExtHash',
       version = '1.0',
       description = 'This is the package for PyExtHash',
       author='Jerry Hwang',
       author_email='jerry@fidelityheight.com',
       url='www.fidelityheight.com',
       ext_modules = [module]
      )
