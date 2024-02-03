echo iniciando testes

cd ..

# echo 
# echo teste dos condicionais
# make run in=tests/cond.jc && gcc -m32 -o out out.s -g 2> /dev/null && ./out

echo 
echo teste dos whiles
make run in=tests/whiles.jc && gcc -m32 -o out out.s -g 2> /dev/null && ./out


# echo 
# echo teste da matemática
# make run in=tests/math.jc && gcc -m32 -o out out.s -g 2> /dev/null && ./out

# echo
# echo teste de laços
# make run in=tests/factorial.jc && gcc -m32 -o out out.s -g 2> /dev/null && ./out

echo
echo teste de complexo
make run in=tests/backpack.jc && gcc -m32 -o out out.s -g 2> /dev/null && ./out