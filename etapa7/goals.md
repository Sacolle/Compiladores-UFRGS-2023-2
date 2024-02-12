# Metas para essa etapa
Implementar correção de erros no yacc e fazer implementações.


### Passo 1:
Realizar a correção de erros no YACC.

### Passo 2:
Reescrever a hashtable para usar unions para os diferentes tipos de dados.

### Passo 3: 
Otimizações

Otimizações envolvendo loop são difíceis de serem realizadas, pois a linguagem só contém while loops, ou seja, toda a categoria de otimizações envolvendo bounded loops fica muito mais difícil de implementar. Entretanto pode-se realizar [Loop inversion](https://en.wikipedia.org/wiki/Loop_inversion) facilmente na linguagem, habilitando [Loop-invariant code motion](https://en.wikipedia.org/wiki/Loop-invariant_code_motion).

Otimizações bem possíveis são o inline de funções, uso de registradores para variáveis locais e, com isso, a redução no número de instruções. Para isso deve-se checar quais registradores estão sendo usados por default.

Eu gostaria de realizar um debug mode que faz index bounds no acesso a vetores. 

Também deve-se fazer o check se scanf deu certo (falar com o vini)

