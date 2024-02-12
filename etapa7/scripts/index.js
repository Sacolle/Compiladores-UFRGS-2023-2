const fs = require('fs')

const file = fs.readFileSync('./parser.y')
const rules = file.toString().split('%%')[1]

//const defines = rules.match(/(AST\_[^,)]*)/g)
const defines = rules.match(/(AST\_[^)]*)/g)

const args = (str) => {let j = 0; for(ch of str) if(ch == '$') j++; return j }

function defines_declarations(defs){
	let i = 1;
	defs.forEach(e => console.log(
		`#define ${e.toUpperCase().replace(/,.*/, '')} ((${i++} << LPAD) | LAYOUT(0, 0, 0, 0) | ${args(e)})`
	))
}

function case_for_defines(defs){
	defs.forEach(e => console.log(
		`case ${e.toUpperCase().replace(/,.*/, '')}: return ;`
	))
}

function defines_tprint(defs){
	defs.forEach(e => console.log(`TPRINT(${e.toUpperCase().replace(/,.*/, '')});`)) 
}

function defines_rewrite(defs){
	defs.forEach(e => console.log(
	`case ${e.toUpperCase().replace(/,.*/, '')}:
    /*${e.split(',').slice(1).join('\n    ')}*/
    break;`
	)) 
}

function parses_rewrite(defs){
	defs.forEach(e => console.log(
	`PARSE(${e.toUpperCase().replace(/,.*/, '')}); break;`
	)) 
}

function parses_functions(defs){
	defs.forEach(e => console.log(
	`void parse_${e.toUpperCase().replace(/,.*/, '')}(AstNode* ast){

}`
	)) 
}
//parses_functions(defines)
//defines_rewrite(defines)
//defines_declarations(defines)
case_for_defines(defines);

const defs = `
TAC_SYMBOL, 
TAC_MOVE,
TAC_ADD,
TAC_SUB, 
TAC_MUL,
TAC_DIV,
TAC_LESS,
TAC_GREAT,
TAC_LE,
TAC_GE,
TAC_EQ,
TAC_DIF,
TAC_AND,
TAC_OR,
TAC_NEG,
TAC_LABEL,
TAC_BEGINFUN,
TAC_ENDFUN,
TAC_IFZ, 
TAC_JUMP, 
TAC_CALL, 
TAC_ARG, 
TAC_RET, 
TAC_PRINT,
TAC_READ`
let global = 1
//defs.split(',').forEach(e => console.log(`case ${e.trim()}: `))


/*

%type <ast> inicio
%type <ast> def
%type <ast> fun
%type <ast> tipo
%type <ast> literal
%type <ast> declaracao
%type <ast> valorLista
%type <ast> paramLista
%type <ast> paramListaContinua
%type <ast> exp
%type <ast> funParam
%type <ast> code
%type <ast> startCmd
%type <ast> cmd
%type <ast> flow
%type <ast> action


*/