
from ConsensusCore import *
import numpy as np


tpl = "GATTACA"*10
read = "GATTACA"*3 + "GATTTTTTACA"*4 + "GATTACA"*3
nTpl = len(tpl)
nObs = len(read)

#
# Way 1: QvSequenceFeatures with empty QV fields.  Only used for
# testing.
#
features = QvSequenceFeatures(read)

#
# Way 2: construct QvSequenceFeatures from arrays constructed in the
# C++ heap.
#
insQv = FloatArray(nObs)
subsQv = FloatArray(nObs)
delQv = FloatArray(nObs)
mergeQv = FloatArray(nObs)

delTag = FloatArray(nObs)
for i in range(nObs):
    delTag[i] = ord('A')

features2 = QvSequenceFeatures(read,
                               insQv.cast(),
                               subsQv.cast(),
                               delQv.cast(),
                               delTag.cast(),
                               mergeQv.cast())


#
# Way 3: construct QvSequenceFeatures from numpy arrays.
#
zeros = FloatFeature(np.zeros(len(read), dtype=np.float32))
insQv = zeros
subsQv = zeros
delQv = zeros
mergeQv = zeros
delTag = FloatFeature(np.array([ord("A")]*len(read), dtype=np.float32))

features3 = QvSequenceFeatures(read,
                               insQv,
                               subsQv,
                               delQv,
                               delTag,
                               mergeQv)



