from setuptools import setup
from torch.utils.cpp_extension import BuildExtension, CppExtension

setup(
    name='pointnet_custom', # This will be the name of the Python library
    ext_modules=[
        CppExtension(
            name='pointnet_custom',
            sources=['src/ball_query.cpp', 'src/bindings.cpp'],
        ),
    ],
    cmdclass={
        'build_ext': BuildExtension
    }
)