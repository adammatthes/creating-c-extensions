from setuptools import setup, Extension

fibheap_module = Extension(
    'fibheap',  # Name of the module as it will be imported in Python (e.g., import fibheap)
    sources=[
        'fibheap_wrapper.c',
        'fibonacci_heap.c'
    ],
    # include_dirs=[], # Add any include directories if necessary (e.g., if fibonacci_heap.h was in a subfolder)
    # library_dirs=[],   # Add library directories if necessary
    # libraries=[]       # Add external libraries if necessary
)

setup(
    name='FibHeapPackage', # Name of the package
    version='0.1.0',
    description='A Python C extension for a Fibonacci Heap.',
    ext_modules=[fibheap_module],
    author='AI Assistant', # Replace with actual author if desired
    # author_email='',
    # long_description='''...''' # Optional long description
)
