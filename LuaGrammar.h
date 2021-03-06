chunk : (stat (';')?)* (laststat (';')?)?;

block : chunk;

stat :  varlist1 '=' explist1 | 
	functioncall | 
	'do' block 'end' | 
	'while' exp 'do' block 'end' | 
	'repeat' block 'until' exp | 
	'if' exp 'then' block ('elseif' exp 'then' block)* ('else' block)? 'end' | 
	'for' NAME '=' exp ',' exp (',' exp)? 'do' block 'end' | 
	'for' namelist 'in' explist1 'do' block 'end' | 
	'function' funcname funcbody | 
	'local' 'function' NAME funcbody | 
	'local' namelist ('=' explist1)? ;

laststat : 'return' (explist1)? | 'break';

funcname : NAME ('.' NAME)* (':' NAME)? ;

varlist1 : var (',' var)*;


namelist : NAME (',' NAME)*;

explist1 : (exp ',')* exp;

exp :  ('nil'|
        'false' |
         'true' |
          number |
           string |
             function |
              prefixexp |
               tableconstructor |
                unop exp)
               
               (binop exp)* ;

var: (NAME | '(' exp ')' varSuffix) varSuffix*;

prefixexp: varOrExp nameAndArgs*;

functioncall: varOrExp nameAndArgs+;

varOrExp: var | '(' exp ')';

nameAndArgs: (':' NAME)? args;

varSuffix: nameAndArgs* ('[' exp ']' | '.' NAME);

args :  '(' (explist1)? ')' | tableconstructor | string ;

function : 'function' funcbody;

funcbody : '(' (parlist1)? ')' block 'end';

parlist1 : namelist (',' '...')? | '...';

tableconstructor : '{' (fieldlist)? '}';

fieldlist : field (fieldsep field)* (fieldsep)?;

field : '[' exp ']' '=' exp | NAME '=' exp | exp;

fieldsep : ',' | ';';

binop : '+' | '-' | '*' | '/' | '^' | '%' | '..' | 
		 '<' | '<=' | '>' | '>=' | '==' | '~=' | 
		 'and' | 'or';

unop : '-' | 'not' | '#';
