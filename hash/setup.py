from distutils.core import setup, Extension

module = Extension('PyExtHash',
                    sources = ['blockwise.c'],  ['chash.c',    'hmac.c',   'pbkdf2.c',  'sha1.c' , 'DtaHashPwd.cpp'],
                    #include_dirs = ['/usr/local/include'],
                    #libraries = ['tcl83'],
                    #library_dirs = ['/usr/local/lib'],
                    #language = ["g++"],
                    #linker = ["g++"],
                    extra_compile_args=["-Wno-narrowing",  "-std=c++0x"],
                    extra_link_args=["-fno-exceptions"]
                    )

setup (name = 'PackageName',
       version = '1.0',
       description = 'This is a demo package',
       author='Jerry Hwang',
       author_email='jerry@fidelityheight.com',
       url='www.fidelityheight.com',
       ext_modules = [module])
