make
   
echo "testando cenario Mini forno..."
INE5410_INFO=1./program 2 10 40 40 40 40 1
echo "testando cenario Greve de Pizzaiolos..." 
INE5410_INFO=1 ./program 4 2 40 40 40 40 1
echo "testando cenario Inflacao moveleira..."
INE5410_INFO=1 ./program 10 10 10 10 40 40 1
echo "testando cenario Greve de garcons..."
INE5410_INFO=1 ./program 10 10 40 2 40 40 1
echo "testando cenario Escassez de fichas ..."
INE5410_INFO=1 ./program 10 10 40 40 3 40 1
