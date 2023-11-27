#  UFRGS - Compiladores
Este é um repositório com as etapas do trabalhos da cadeira de compiladores do professor Marcelo Johann no semestre de 2023/2. Essa cadeira se utiliza de *yacc* e *lex* para gerar o parser e lexer respectivamente. A partir da 3ª etapa a implementação passa a desviar do que é demonstrado em aula, sendo um tanto _overkill_. 


Este repo possui dois scripts shell *send.sh* e *test.sh*. Eles servem para automatizar o processo de envio dos trabalhos e validação destes. Para que eles funionem é necessário um arquivo *.env* na segunte forma:
```ini
key=<chave única dada pelo professor>
host=<nome do inf>@html.inf.ufrgs.br
```
O script *test.sh* gera uma pasta chamada testing em que ele baixa, descompacta e compila o arquivo para checar se o envio foi correto.


