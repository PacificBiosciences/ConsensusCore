* Version 1.0.2
 - Fix a rare crash that could occur during refinement if a read ended up being mapped
   to a zero-length span of template (after lifting of reference alignment onto POA)

* Version 1.0.1
  - Fix for a serious memory leak occurring on alpha/beta mating failures.  Thanks Nigel!
  - Fix for nondeterministic POA consensus

* Version 1.0.0
  - Minor build/deploy system tweaks.  No API changes.

* Version 0.9.1
  - Overhaul of directory structure to be a better-behaved C++ library (separate API headers out)
  - Changes to POA APIs to support "sparse" POA support in laamm

* Version 0.9.0
  - API change to QuiverConfigTable, adding "InsertDefault" methods
