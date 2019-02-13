from distutils.core import setup, Extension

module = Extension('PyExtOb',
                    sources = ['ob.cpp'],
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
