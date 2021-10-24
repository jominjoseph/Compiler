# Compiler
Basic Programming Language Compiler
Prog := Slist
Slist := Stmt SC { Slist }
Stmt := IfStmt | PrintStmt | Expr
IfStmt := IF Expr THEN Stmt
PrintStmt := PRINT Expr
Expr := LogicExpr { ASSIGN LogicExpr }
LogicExpr := CompareExpr { (LOGICAND | LOGICOR) CompareExpr } CompareExpr := AddExpr { (EQ | NEQ | GT | GEQ | LT | LEQ) AddExpr } AddExpr := MulExpr { (PLUS | MINUS) MulExpr }
MulExpr := Factor { (STAR | SLASH) Factor }
Factor := MINUS Primary | Primary
Primary := IDENT | ICONST | SCONST | TRUE | FALSE | LPAREN Expr RPAREN
This language will be used for the remainder of the semester. The following items describe the language.
1. The language contains three types: integer, string, and boolean.
2. All operators are left associative except for ASSIGN and unary MINUS, which are both
right associative.
3. An IfStmt evaluates the Expr. The expr must evaluate to a boolean. If the boolean is
true, then the Stmt is executed.
4. A PrintStmt evaluates the Expr and prints its value. A newline is not appended.
5. In an Expr, the ASSIGN operator copies the value from the operand on the right side of
the operator to the operand on the left side of the operator. The left side of the operator must be an IDENT. If the IDENT does not exist, it is created. If the ident already exists, its value is replaced. The value of the Expr is the value of the identifier, and the type of the Expr is the type associated with the value.
6. The type of an IDENT is the type of the value assigned to it.
7. The LogicExpr performs short-circuited evaluation of logical and and or.
8. The CompareExpr performs a comparison between its operands.
9. The PLUS and MINUS operators in AddExpr represent addition and subtraction.
10. The STAR and SLASH operators in MulExpr represents multiplication and division.
11. The MINUS operator in Factor is a unary minus.
12. The type of TRUE is boolean true. The type of FALSE is boolean false.
13. It is an error if a variable is used before a value is assigned to it.
14. Addition is defined between two integers (the result being the sum) or two strings (the
result being the concatenation).
15. Subtraction is defined between two integers (the result being the difference).
16. Multiplication is defined between two integers (the result being the product) or for an
integer and a string (the result being the string repeated integer times).
17. Division is defined between two integers
18. Logical and and logical or are defined between two booleans. The result is the boolean
value of the logical operation.
19. Comparisons for equality and inequality is defined between pairs of integers, pairs of
strings, and pairs of booleans. The result is the boolean result of the test.
20. Comparisons for other than equality and inequality are defined between pairs of integers
and pairs of strings.
21. Unary minus is defined for an integer (which is defined as -1 * the integer) and for
boolean (which is defined as a logical not).
22. Performing an operation with incorrect types or type combinations is an error.
23. Multiplying a string by a negative integer is an error.
24. Assigning to anything other than an identifier is an error.
25. An IF statement whose Expr is not boolean typed is an error.
