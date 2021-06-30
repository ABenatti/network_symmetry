#!/usr/bin/env python3
from setuptools import setup, Extension, Command
import os.path
import platform
import setuptools
import sys
print("Building on:", sys.version)

enableParallelism = True

extraOptions = []
extraLinkOptions=[]
if(platform.system()=="Darwin"):
    extraOptions = ["-D OSX"]
    if(enableParallelism):
        extraOptions += ["-DCV_USE_LIBDISPATCH=1"]
elif(platform.system()=="Windows"):
    extraOptions = ["-D WIN32 -lpthread"]
    if(enableParallelism):
        extraOptions += ["-DCV_USE_OPENMP=1","-fopenmp"]
        extraLinkOptions+=["-lgomp"]
elif(platform.system()=="Linux"):
    extraOptions = ["-D Linux","-D_GNU_SOURCE=1"]
    if(enableParallelism):
        extraOptions += ["-DCV_USE_OPENMP=1","-fopenmp"]
        extraLinkOptions+=["-lgomp"]
else:
    if(enableParallelism):
        extraOptions += ["-DCV_USE_OPENMP=1","-fopenmp"]
        extraLinkOptions+=["-lgomp"]

# WORKAROUND: https://stackoverflow.com/questions/54117786/add-numpy-get-include-argument-to-setuptools-without-preinstalled-numpy
class get_numpy_include(object):
    def __str__(self):
        import numpy
        return numpy.get_include()

with open("README.md", "r") as fh:
    long_description = fh.read()

building_on_windows = platform.system() == "Windows"

packageName = "network-symmetry"
packageDirectory = "network_symmetry"
extensionPackageName = "network_symmetry_core"

with open(os.path.join(packageDirectory, "Python", "PyCXVersion.h"), "rt") as fd:
    version = fd.readline().strip().split(" ")[-1]

print("Compiling version %s"%version)
setup(
    name=packageName,
    version=version,
    author="Filipi N. Silva, Alexandre Benatti, Henrique F. Arruda",
    author_email="filsilva@iu.edu, alexandre.benatti@usp.br, h.f.arruda@gmail.com",
    compiler = "mingw32" if building_on_windows else None,
    setup_requires=["wheel","numpy"],
    description="Library to compute accessibility and symmetry in networks",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/ABenatti/network-accessibility",
    packages=setuptools.find_packages(),
    classifiers=[
            "Programming Language :: Python :: 3",
            "License :: OSI Approved :: MIT License",
            "Operating System :: MacOS :: MacOS X",
            "Operating System :: Microsoft :: Windows",
            "Operating System :: POSIX :: Linux",
            "Development Status :: 3 - Alpha",
            "Programming Language :: C",
            "Topic :: Scientific/Engineering :: Information Analysis",
            "Intended Audience :: Science/Research"
    ],
    python_requires='>=3.6',
    ext_modules = [
        Extension(
            extensionPackageName,
            sources=[
                os.path.join(packageDirectory,"Source", "CVSimpleQueue.c"),
                os.path.join(packageDirectory,"Source", "CVSet.c"),
                os.path.join(packageDirectory,"Source", "CVNetwork.c"),
                os.path.join(packageDirectory,"Source", "CVDictionary.c"),
                os.path.join(packageDirectory,"Source", "CVDistribution.c"),
                os.path.join(packageDirectory,"Source", "fib.c"),
                os.path.join(packageDirectory,"Source", "CVNetworkSymmetry.c"),
                os.path.join(packageDirectory,"Source", "CVConcentricStructure.c"),
                os.path.join(packageDirectory,"Source", "CVNetworkCentrality.c"),
                os.path.join(packageDirectory,"Python", "PySymmetry.c"),
                os.path.join(packageDirectory,"Python", "CVSymmetryApplication.c"),
            ],
            include_dirs=[
                os.path.join(packageDirectory,"Source"),
                os.path.join(packageDirectory,"Python"),
                get_numpy_include()
            ],
            extra_compile_args=[
                # "-g",
                "-std=c11",
                # "-m64",
                "-Wall",
                "-Wno-unused-function",
                "-Wno-deprecated-declarations",
                "-Wno-sign-compare",
                "-Wno-strict-prototypes",
                "-Wno-unused-variable",
                "-O3",
                # "-fvisibility=hidden",
                "-funroll-loops",
                "-fstrict-aliasing"
            ]+extraOptions,
            extra_link_args=extraLinkOptions,
        ),
    ]
)

        
