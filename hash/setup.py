from distutils.core import setup, Extension

module = Extension('PyExtHash',
                    sources = ['DtaHashPwd.cpp'],
                    include_dirs = ['../Common','../Common/pbkdf2', '../linux' , '../License' ],
                    extra_compile_args=["-Wno-narrowing", "-std=c++11",'-fpermissive','-fPIC', '-w', 
                                        "-F/opt/local/library/Frameworks",
                                        "-framework","Python", "-framework","sedutil", "-framework","cifra", "-framework","log" ],
                    extra_link_args=["-fno-exceptions",
                                     "-F/opt/local/library/Frameworks",
                                        "-framework","Python", "-framework","sedutil" , "-framework","cifra", "-framework","log" ]
                  )

setup (name = 'PyExtHash',
       version = '1.0',
       description = 'This is the package for PyExtHash',
       author='Jerry Hwang',
       author_email='jerry@fidelityheight.com',
       url='www.fidelityheight.com',
       ext_modules = [module]
      )
