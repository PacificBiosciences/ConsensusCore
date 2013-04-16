
from __future__ import print_function

from distutils.command.build import build as _build
from distutils.errors import CompileError
from distutils import sysconfig
from setuptools import setup

import os, re, sys
from glob import glob
from os.path import join, dirname as up

BOOST_MINIMUM_VERSION = (1,47,0)
SWIG_MINIMUM_VERSION  = (2,0,7)

def die(msg):
    print(msg)
    sys.exit(1)

def str_version(tpl):
    return ".".join(map(str, tpl))

def boost_version(boostDir):
    versionFile = join(boostDir, "boost/version.hpp")
    try:
        m = re.search("#define BOOST_VERSION (.*)", open(versionFile).read())
        if m:
            versionInt = int(m.group(1))
            versionTuple = (versionInt // 100000,
                            (versionInt// 100) % 1000,
                            versionInt % 100)
            return versionTuple
    except:
        pass
    return None

def find_boost():
    """
    Look for boost in some standard filesystem places.
    Return the one with the highest version, or None if can't find any.
    """
    boost_candidates = \
        glob("/usr/include/boost")         + \
        glob("/usr/include/boost*/")       + \
        glob("/usr/local/boost/")          + \
        glob("/usr/local/boost*/")         + \
        glob("/opt/local/include/boost/")  + \
        glob("/opt/local/include/boost*/") + \
        glob("/home/UNIXHOME/dalexander/Packages/boost_1_47_0/boost/version.hpp")

    boosts_found = [ (boost, boost_version(boost))
                     for boost in boost_candidates ]
    if boosts_found:
        best_boost = max(boosts_found, key=lambda t: t[1])[0]
        return best_boost
    else:
        return None

def swig_version(swigExecutablePath):
    """
    Return the version of the swig provided, or None if it is not a
    valid SWIG.
    """
    swig_version_pipe = os.popen(swigExecutablePath + " -version")
    swig_version_output = swig_version_pipe.read()
    swig_version_status = swig_version_pipe.close()
    if swig_version_status == None:
        m = re.search("SWIG Version (.*)\n", swig_version_output)
        if m:
            swig_version_string = m.group(1)
            return tuple(map(int, swig_version_string.split(".")))
    return None

def command_line_arguments():
    """
    Pull out from the command line:
      --boost=...
      --swig=...
      --debug/-g
    Return as a dict
    """
    arguments = { "debug" : "",
                  "boost" : None,
                  "swig"  : None  }

    for arg in sys.argv[:]:
        # Debug build?
        if (arg=="-g") or (arg=="--debug"):
            arguments["debug"] = "1"
            sys.argv.remove(arg)

        # --boost=...
        elif arg.find("--boost=") == 0:
            boost_inc = arg.split("=")[1]
            version = boost_version(boost_inc)
            if not version:
                die("Invalid boost directory specified.")
            sys.argv.remove(arg)
            arguments["boost"] = boost_inc

        # --swig==...
        elif arg.find("--swig=") == 0:
            swig_executable = arg.split("=")[1]
            version = swig_version(swig_executable)
            if not version:
                die("Invalid SWIG binary specified")
            sys.argv.remove(arg)
            arguments["swig"] = swig_executable

    return arguments

def configure(arguments):
    """
    Look up the dependencies
     - Python.h
     - numpy headers
     - boost headers (from configuration flag)
    """
    # Python.h
    python_inc = sysconfig.get_python_inc()

    # Libpython
    python_libdir = join(sysconfig.get_python_lib(standard_lib=True), "config")
    python_lib = "python" + sysconfig.get_python_version()

    # numpy headers
    try:
        import numpy
        numpy_inc = numpy.get_include()
    except ImportError:
        die("Requires numpy >= 1.6.0")

    boost_inc = arguments["boost"] or find_boost()
    if not boost_inc:
        die("Path to boost must be specified using --boost=<path>")

    # verify that the boost directory is valid and has a good boost version
    if boost_version(boost_inc) < BOOST_MINIMUM_VERSION:
        print("Boost version at least %s required!" \
            % str_version(BOOST_MINIMUM_VERSION))
        print("Use --boost=<path> to specify boost location.")
        sys.exit(1)
    print("boost=%s" % boost_inc)

    # Verify that the swig version is adequate
    swig_executable = arguments["swig"] or "swig"
    if swig_version(swig_executable) < SWIG_MINIMUM_VERSION:
        print("SWIG (version >= %s) must be in your $PATH," \
            % str_version(SWIG_MINIMUM_VERSION))
        print("or specified using --swig=<path/to/swig>.")
        sys.exit(1)
    print("swig=%s" % swig_executable)


    debug = arguments["debug"]

    # handshake with the makefile is through these
    env_vars = "PYTHON_SYS_INCLUDE_PATH=%s " % python_inc      + \
               "NUMPY_INCLUDE_PATH=%s "      % numpy_inc       + \
               "BOOST_INCLUDE_PATH=%s "      % boost_inc       + \
               "SWIG=%s "                    % swig_executable + \
               "DEBUG=%s "                   % debug

    return env_vars

# This always has to be run, to remove extra arguments that will
# confuse setuptools otherwise
arguments = command_line_arguments()

class build(_build):
    """
    Build the native code using the Makefile, sidestepping
    most of the horrors of Python packaging.
    """
    def run(self):
        configuration = configure(arguments)
        error = os.system(configuration + "make python")
        if error:
            raise CompileError("Failed to compile or link ConsensusCore C++ code")

    @staticmethod
    def pythonBuildDirectory():
        """
        Returns the directory where the build stashed the generated
        Python module.
        """
        return os.popen("make --no-print-directory echo-python-build-directory").read().strip()

# HACK: "setup.py install" will fail if build hasn't been called yet,
# because setuptools attempts to scan the package directory before
# invoking the build---which will fail because the directory is
# created by the build.  I consider this a bug in setuptools, and this
# is the least involved workaround I have found: inject "build" into
# the command line.
if "install" in sys.argv and not "build" in sys.argv:
    installPos = sys.argv.index("install")
    sys.argv.insert(installPos, "build")

setup(name="ConsensusCore",
      version="0.6.0",
      author="Pacific Biosciences",
      author_email="devnet@pacificbiosciences.com",
      url="http://www.github.com/PacificBiosciences/ConsensusCore",
      description= \
          """A library for genomic consensus and variant calling""",
      license=open("LICENSES").read(),
      py_modules=["ConsensusCore"],
      packages = [""],
      package_dir={"": build.pythonBuildDirectory() },

      # Smuggle the native library in as a data file
      package_data={"": ["_ConsensusCore.so"]},

      cmdclass={"build": build})
