from distutils.core import setup, Extension

module = Extension('PyExtHash',
                    sources = ['DtaHashPwd.cpp'], #, 'blockwise.c', 'chash.c',    'hmac.c',   'pbkdf2.c',  'sha1.c' , ],
                    include_dirs = ['../Common','../Common/pbkdf2', '../linux' , '../License' ],
                    #libraries = ['tcl83'],
                    #library_dirs = ['/usr/local/lib'],
                    #language = ["g++"],
                    #linker = ["g++"],
                    extra_compile_args=["-Wno-narrowing", "-std=c++11",'-fpermissive','-fPIC', '-w'], #'-Wno-sign-compare]', '-Wno-format]'], #"-w",'-std=c++11'], #"-std=c++0x", '-fpermissive',],
                    extra_link_args=["-fno-exceptions" ,  'chash.o', 'pbkdf2.o', 'blockwise.o', 'sha1.o', 'hmac.o']
                    )

setup (name = 'PackageName',
       version = '1.0',
       description = 'This is a demo package',
       author='Jerry Hwang',
       author_email='jerry@fidelityheight.com',
       url='www.fidelityheight.com',
       ext_modules = [module])


'''
	./build/Debug_i686/GNU-Linux/_ext/cdbdd37b/chash.o
        ${OBJECTDIR}/_ext/cdbdd37b/chash.o \
        ${OBJECTDIR}/_ext/cdbdd37b/hmac.o \
        ${OBJECTDIR}/_ext/cdbdd37b/pbkdf2.o \
        ${OBJECTDIR}/_ext/cdbdd37b/sha1.o \
'''
