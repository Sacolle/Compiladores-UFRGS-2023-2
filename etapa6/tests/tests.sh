echo iniciando testes

cd ~/compiladores/etapa6

for f in $(ls tests/*.jc) 
do
	echo "testando $f"
	make run in=$f || (echo "erro buildando $f" && echo && continue)
	gcc -m32 -o out out.s -g 2> /dev/null || (echo "erro compilando $f" && continue)
	./out
	echo
done