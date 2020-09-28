Name: Dung Le
Netid: dle8
Student ID: 30677753

Partner: Loc Bui (lbui3)

A complete list of implementations that we have done includes error-recovery for program, stmt_list, stmt, cond, and
expr.

To be more clear, the division of workloads is as follows:
- Loc Bui:
    - Translate the code into C++. Replace C libraries call to equivalences in C++.
    - Implement exception-based syntax error recovery, stmt_list, stmt
- Dung Le:
    - Extend the language for if and while statements
    - Implement cond, expr

Instructions to run the project is as follow:
    - Compile with Makefile: make parse
    - Run the executable file: ./parse

Note: 
    - To notify the end of program, include EOF character instead of $$ like in the language.
    - The best way to run program is write the input in input.txt and run ./parse < input.txt

Example Input:
x := a + b

Example Output:
(program
  [
    (:=
      (id 'x')
      (+
        (id 'a')
        (id 'b')
      )
    )
  ]
)