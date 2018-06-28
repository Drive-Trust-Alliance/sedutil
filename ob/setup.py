from distutils.core import setup, Extension

module1 = Extension('ob',
                    sources = ['ob.cpp'],
                    language = "g++",
                    extra_compile_args=["-Wno-narrowing",  "-std=c++0x"])

setup (name = 'PackageName',
       version = '1.0',
       description = 'This is a demo package',
       ext_modules = [module1])
