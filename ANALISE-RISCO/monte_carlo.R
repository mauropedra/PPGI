set.seed(1)
n = 10000
gera_pi <- function(n){
x <- runif(n,-1,1)
y <- runif(n,-1,1)
cont <- 0
for (i in 1:n){
  if((x[i]^2 + y[i]^2) <= 1 ){
    cont <- cont + 1 }
}
(cont/n*4)
}

gera_m_pi <-function(m){
vector=c()
for(j in 1:m) vector <- c(vector,gera_pi(n))
mean(vector)}

gera_m_pi(100)
hist(vector)

