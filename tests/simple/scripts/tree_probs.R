library(ape)
library(TESS)

format_num <- function(x) {
    sprintf("%.10f", x)
}

#########################################
# 1: Yule tree, no hybridization events #
#########################################

## parameters
lambda <- 0.5

## make the tree
tree <- read.tree(text = "((A:0.5,B:0.5):0.25,C:0.75):0.25;")

## write the tree
write.tree(tree, "../data/yule_tree.tre")

## compute the probability
format_num(log(4 * lambda * lambda * exp(-2.25 * lambda)))

################################################
# 2: Yule tree with asymmetrical hybridization #
################################################

## parameters
lambda <- 0.5
eta <- 0.1

## make the tree
tree <- read.evonet(text = "((A:0.5,C#A:0.00):0.25,(B:0.5)C#A:0.25):0.25;")

## write the tree
write.evonet(tree, "../data/yule_asymmetrical_tree.tre")

## compute the probability
format_num(log(2 * 0.5 * eta * lambda * exp(-1.75 * lambda - 0.75 * eta)))

###############################################
# 3: Yule tree with symmetrical hybridization #
###############################################

# parameters
lambda <- 0.5
zeta <- 0.05

# make the tree
tree <- read.evonet(text = "((A:0.5,C#S1:0.00)D#S2:0.25,(B:0.5,D#S2:0.00)C#S1:0.25):0.25;")

# write the tree
write.evonet(tree, "../data/yule_symmetrical_tree.tre")

# compute the probability
format_num(log(2 * zeta * lambda * exp(-1.75 * lambda - 0.75 * zeta)))

#############################################
# 4: Yule tree with hybrid speciation event #
#############################################

# parameters
lambda <- 0.5
nu <- 0.075

# make the tree
tree <- read.evonet(text = "((A:0.5,(B:0.5)D#H:0.0):0.25,(C:0.5,D#H:0.0):0.25):0.25;")

# write the tree
write.evonet(tree, "../data/yule_hybrid_speciation_tree.tre")

# compute the probability
format(log(nu * 2 * lambda * exp(-2.25 * lambda - 1.75 * nu)))

####################################################
# 5: Yule tree with allopolyploid speciation event #
####################################################

lambda <- 0.5
psi <- 0.015

# make the tree
tree <- read.evonet(text = "((A:0.5,(B:0.5)D#P:0.0):0.25,(C:0.5,D#P:0.0):0.25):0.25;")

# write the tree
write.evonet(tree, "../data/yule_allopolyploid_speciation_tree.tre")

# compute the probability
format_num(log(psi * 2 * lambda * exp(-2.25 * lambda - 1.75 * psi)))

#########################################
# 6: Yule tree with incomplete sampling #
#########################################

# parameters
lambda <- 0.5
mu <- 0
rho <- 0.5

# make the tree
tree <- read.tree(text = "((A:0.5,B:0.5):0.25,C:0.75):0.25;")

# compute the probability
p1 <- function(lambda, mu, rho, t) {
    TESS:::tess.equations.p1.constant(lambda, mu, c(), c(), rho, 0, t)
}

format_num(log(4 * lambda * lambda * p1(lambda, mu, rho, 1) * p1(lambda, mu, rho, 0.75) * p1(lambda, mu, rho, 0.5)))

####################################################
# 7: birth-death tree with no hybridization events #
####################################################

# parameters
lambda <- 0.5
mu <- 0.1
rho <- 0.5

# make the tree
tree <- read.tree(text = "((A:0.5,B:0.5):0.25,C:0.75):0.25;")

# compute the probability
p1 <- function(lambda, mu, rho, t) {
    TESS:::tess.equations.p1.constant(lambda, mu, c(), c(), rho, 0, t)
}

format_num(log(lambda * lambda * p1(lambda, mu, rho, 1) * p1(lambda, mu, rho, 0.75) * p1(lambda, mu, rho, 0.5)))


















