name=$1
if [ -z ${1+x} ]; then echo "insira um nome de arquivo" && exit 1; fi

source .env || exit 1

cd $name || exit 1

tar czf $name.tgz -X ../.tarignore . || exit 1

sended_file=1

sftp -o HostKeyAlgorithms=+ssh-dss $host:public_html/$key/ <<EOF || sended_file=0
put $name.tgz
exit
EOF

rm $name.tgz

if [ $sended_file != 1 ]; then
	echo "Falha no envio do arquivo"
	exit 1
else
	username=(${host//@/ })
	echo "Envio da $name feito com sucesso! Acesse em:"
	echo "https://www.inf.ufrgs.br/~${username[0]}/$key/${name}.tgz"
fi
	
