module DiversinetInterface

export computeLogLikelihood, jitterNewick, readNewick, setConditionalProbabilityType, setEta, setKMaxInt, setLambda
export setMu, setNu, setPsi, setRho, setZeta, simulate

using CxxWrap
import Libdl
@wrapmodule(()->"$(@__DIR__)/../deps/libjlDiversinetInterface." * Libdl.dlext)

function __init__()
    @initcxx
end

end #module
