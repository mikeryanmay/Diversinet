module DiversinetInterface

export computeLogLikelihood, readNewick, setEta, setKMaxInt, setLambda, setMu, setNu, setPsi, setRho, setZeta, simulate

using CxxWrap
import Libdl
@wrapmodule(()->"$(@__DIR__)/../deps/libjlDiversinetInterface." * Libdl.dlext)

function __init__()
    @initcxx
end

end #module
