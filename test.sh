name=$1
if [ -z ${1+x} ]; then echo "insira um nome de arquivo" && exit 1; fi

source .env || exit 1

if [ -d testing ]; then 
	rm -r testing/*;
else 
	mkdir testing
fi;

cd testing || exit 1

username=(${host//@/ })	
wget https://www.inf.ufrgs.br/~${username[0]}/$key/${name}.tgz -q || (echo "Falha em baixar o arquivo"; exit 1)

tar xzf "$name".tgz || (echo "falha em descompactar o arquivo"; exit 1)
make
if [ -f "$name" ]; then
    echo "Compilação do arquivo enviado OK!"
else
    echo "Falha na Compilação" 
fi

echo "Resultado da compilação se encontra na pasta testing"
