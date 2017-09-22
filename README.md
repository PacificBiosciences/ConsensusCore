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

DISCLAIMER
----------
THIS WEBSITE AND CONTENT AND ALL SITE-RELATED SERVICES, INCLUDING ANY DATA, ARE PROVIDED "AS IS," WITH ALL FAULTS, WITH NO REPRESENTATIONS OR WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, ANY WARRANTIES OF MERCHANTABILITY, SATISFACTORY QUALITY, NON-INFRINGEMENT OR FITNESS FOR A PARTICULAR PURPOSE. YOU ASSUME TOTAL RESPONSIBILITY AND RISK FOR YOUR USE OF THIS SITE, ALL SITE-RELATED SERVICES, AND ANY THIRD PARTY WEBSITES OR APPLICATIONS. NO ORAL OR WRITTEN INFORMATION OR ADVICE SHALL CREATE A WARRANTY OF ANY KIND. ANY REFERENCES TO SPECIFIC PRODUCTS OR SERVICES ON THE WEBSITES DO NOT CONSTITUTE OR IMPLY A RECOMMENDATION OR ENDORSEMENT BY PACIFIC BIOSCIENCES.
