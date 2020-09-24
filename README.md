# Parser

A parser that parse the below language and output an AST.

Language:
```
P	→ 	SL $$
SL	→ 	S SL  |  ε
S	→ 	id := E  |  read id  |  write E  |  if C SL end  |  while C SL end
C	→ 	E ro E
E	→ 	T TT
T	→ 	F FT
F	→ 	( E )  |  id  |  lit
TT	→ 	ao T TT  |  ε
FT	→ 	mo F FT  |  ε
ro	→ 	=  |  <>  |  <  |  >  |  <=  |  >=
ao	→ 	+  |  -
mo	→ 	*  |  /
```

### Run

Compile program with Makefile: `make parse`

### Example Input