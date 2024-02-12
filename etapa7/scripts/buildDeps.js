const fs  = require('node:fs/promises')
const path = require('node:path')

//const rules = file.toString().split('%%')[1]

//const defines = rules.match(/(AST\_[^,)]*)/g)
//const defines = rules.match(/(AST\_[^)]*)/g)


const walk = async (dirPath) => Promise.all(
	await fs.readdir(dirPath, { withFileTypes: true }).then((entries) => entries.map((entry) => {
		const childPath = path.join(dirPath, entry.name)
		return entry.isDirectory() ? walk(childPath) : childPath
	})),
)
function getFileName(name){
	return name.replace(/\..*/, '').split('/').slice(-1)
}

async function main(){
	const files = (await walk('./src/')).flat(100)

	const name2file = Object.fromEntries(files.map(f => [f.split('/').slice(-1), f]))
	
	const dict = Object.fromEntries(files
		.filter(f => !f.endsWith('.h'))
		.map(f => [getFileName(f), [f]])
	)
	
	for(const f of files){
		const contents = await fs.readFile(f)
		const includes = contents.toString().match(/#include ".*"/g)
		if(!includes) continue;
		
		const paths = includes
			.map(n => n.split("\"")[1].split('/').slice(-1) )
			.map(p => name2file[p] ? name2file[p] : `build/pre/${p}`)
		dict[getFileName(f)].push(...paths)
	}
	await fs.writeFile("./depends.mk", stringfy(dict) + "\nlex.yy = build/pre/lex.yy.c\ny.tab = build/pre/y.tab.c")
}

function stringfy(obj){
	return Object.entries(obj).map(([k, v]) => `${k} = ${v.join(' ')}`).join('\n')
}


main();