from setuptools import Extension, setup

module = Extension("symnmfapi", sources=['symnmf.c', 'symnmfmodule.c'])
setup(name='symnmfapi',
     version='1.0',
     description='Python wrapper for custom C extension',
     ext_modules=[module])