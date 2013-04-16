
from __future__ import print_function

import os, sys
from numpy import *
from matplotlib.pyplot import *
from matplotlib.cm import binary

from ConsensusCore import *
from example_data import *

def main(hasDisplay):
    # Poa Consensus stuff
    seqv = ["GATTACA",
            "GATTTACA"]

    pc = PoaConsensus.FindConsensus(seqv)  # default config, uses global alignment
    pc = PoaConsensus.FindConsensus(seqv, PoaConfig.GLOBAL_ALIGNMENT)  # more explicit...

    print(pc.Sequence())
    assert pc.Sequence() == "GATTACA"

    # or use a custom config
    custom_config = PoaConfig()
    custom_config.UseGlobalAlignment = False
    custom_config.Params.Mismatch= -2   # give mismatch a higher penalty
    pc = PoaConsensus.FindConsensus(seqv, custom_config)
    print(pc.Sequence())
    g =  pc.Graph()
    print(g.ToGraphViz())
    g.WriteGraphVizFile("/tmp/foo.dot")

    I = features.Length()
    J = len(tpl)
    a = DenseMatrix(I+1, J+1)
    b = DenseMatrix(I+1, J+1)
    ext = DenseMatrix(I+1, 2)
    extSSE = DenseMatrix(I+1, 2)

    spec = QvModelParams.Default()
    e = QvEvaluator(features, tpl, spec)

    r = SimpleQvRecursor(ALL_MOVES, BandingOptions(4, 200.))
    r.FillAlphaBeta(e, a, b)

    print(a.Get(0,0))
    print(a.ToHostMatrix())

    alignment = r.Alignment(e, a)
    print(alignment.Target())
    print(alignment.Query())

    if hasDisplay:
        figure(1)
        subplot(121)
        imshow(a.ToHostMatrix())
        title("Alpha")
        subplot(122)
        imshow(b.ToHostMatrix())
        title("Beta")
        show()


if __name__ == "__main__":
    print("PID: ", os.getpid())
    try:
        hasDisplay = (not "NODISPLAY" in os.environ and
                      ("DISPLAY" in os.environ or os.uname()[0] == "Darwin"))
        main(hasDisplay)
    except Exception as inst:
        # IPython intercepts sys.exit, a policy i disagree
        # with.  Use the raw C os._exit call to force the
        # issue.
        #sys.exit(1)
        print("Exception: ", type(inst), inst)
        import os
        os._exit(1)
