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

function defines_tprint(defs){
	defs.forEach(e => console.log(`TPRINT(${e.toUpperCase().replace(/,.*/, '')});`)) 
}

function defines_rewrite(defs){
	defs.forEach(e => console.log(
	`case ${e.toUpperCase().replace(/,.*/, '')}:
    ${e.split(',').slice(1).join('\n    ')}
    break;`
	)) 
}

defines_rewrite(defines)
//defines_declarations(defines)




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