# ConsensusCore

ConsensusCore is a library of C++ algorithms for PacBio multiple
sequence consensus that powers Quiver (Python) and ConsensusTools
(.NET).

To install Quiver, you will need to install ConsensusCore and
[GenomicConsensus](https://github.com/PacificBiosciences/GenomicConsensus).


# Building/installing

For Python:

```sh
% python setup.py install [configure flags, see ./configure --help]
```

For .NET (command line):

```sh
./configure  [see ./configure --help for details]
make csharp
```

For .NET (MonoDevelop):

run ``./configure`` as above, but then open the ConsensusCore.csproj
in MonoDevelop and build.


# Notes

.NET doesn't handle native libraries very gracefully.  In order for
client tools to use the ``libConsensusCore.so``, the library needs to
be copied to the bin/{Debug,Release} directory for every assembly that
transitively depends on ConsensusCore and creates an executable.  You
can do this using a pre-build rule.

On OS X, the build uses ``clang++``, but we resort back to libstdc++
to avoid some compatibility issues.  This means you will have to make
sure to compile your gmock installation with ``-stdlib=libstdc++`` as
well, if you are trying to run the tests.
