         


“ADVCALC” DOCUMENTATION
                             
Summary:
       This program is an arithmetic expression evaluator. The program reads
stdin inputs line by line and evaluating them after parsing it according to the
designed BNF-Grammar. Result is printed right afterwards meaning that the
program acts like an interpreter. Program also contains various features such as
variable assignments, comments and error detecting. To add a comment ‘%’
symbol should be used. If an expression contains invalid characters or written in
an invalid format, program detects it and warns the user. Variable assignment
feature works with 2 arrays that are responsible for holding the variable names
and the respective values which resembles the HashMap data structure.


Important Notes:
       Compilation can be done via the Makefile which is located in the source
folder. Makefile compiles the c file and creates an executable called “advcalc”.
To end the program user needs to press Control+D and it terminates
automatically.


Implementation Details:
    Parsing of the given expression is done according to the following BNF –
Grammar:


<expression > -> <variable> “=” <orstatement> | <orstatement>
<orstatement > -> <andstatement><moreor>
<moreor> -> “|” <orstatement> | ε

<andstatement> -> <incrementOperations> <moreand>
<moreand> -> “&” <andstatement> | ε
<incrementoperations> -> <terms><moreincrement>
<moreincrement> -> “+” <incrementoperations> | “-“ <incrementoperations> | ε
<terms> -> <factors><moreterms>
<moreterms> -> “*” <terms> | ε
<factors> -> <constant> | <variable> | <function> | “(“<orstatement> “)”
<function> -> “xor(“ <orstatement> “,” <orstatement> “)” | “ls(“ <orstatement>
“,” <orstatement> “)” | “rs(“ <orstatement> “,” <orstatement> “)” | “lr(“
<orstatement> “,” <orstatement> “)” | “rr(“ <orstatement> “,” <orstatement>
“)” | “not(“ <orstatement> “)”
(Constants are 64-bit integers and Variables are alphabetic words)


      Program contains a function for each level in the BNF. When an
expression is entered initially it checks whether the expression contains and
assignment or not. Then, several error checks take place to ensure that the given
expression is valid. Finally, it calls the functions corresponding to BNF levels
recursively. Thus, the program parses and evaluates the expression
simultaneously while also checking for an error in the expression. If an error is
detected, evaluation process terminates and program prints “Error!” message. If
the expression is an assignment, program finds the variable in the array and
adjusts the value.


      One of the most crucial functions in this implementation is the gettoken()
function. Since the program does the parsing and the evaluation processes
simultaneously, it is essential to check what the upcoming token is. With the
help of the gettoken() function, program detects the next token in line and
decides whether the input is valid. If not, calling the appropriate function also
depends on this function.

Examples:

% ./advcalc

>x=1

> y = 3 + x * (5 + 7)

>y

15

>x

1

>x=x+1

>x

2

> xor(350, 65)

287

> xor(350,65

Error!

>%

% ./advcalc

> (3 + 5))

Error!

> (((1+3))) * (7 - 2) %comment

20

>a=3

>ab=4

Error!

>b=aa

Error!

>b=a*a

>b

9
Difficulties Encountered / Possible Improvements:
       One of the major difficulties that was encountered during this project was
the lack of the modern data structures in C language. A HashMap could have
been used for storing the variables and their values. Furthermore, Stack and
PriorityQueue data structures could have possibly made the evaluation process
easier. Another drawback to mention is that the program does not use the
memory efficiently since the used parts are not freed afterwards in the current
implementation.
