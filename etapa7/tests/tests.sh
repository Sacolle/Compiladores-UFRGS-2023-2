echo iniciando testes

cd ~/compiladores/etapa7

for f in $(ls tests/*.jc) 
do
	echo "testando $f"
	./build/etapa7 $f || (echo "erro buildando $f" && echo && continue)
	gcc -m32 -o build/out build/pre/out.s -g || (echo "erro compilando $f" && continue)
	./build/out
	echo
done