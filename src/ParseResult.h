MyType := (int, float, string)

DECLARATION
  ATOM
    ID "MyType"
    NIL
    NIL
  TUPLE
    ATOM
      NIL
      NATIVE_TYPE "int"
      NIL
    ATOM
      NIL
      NATIVE_TYPE "float"
      NIL
    ATOM
      NIL
      NATIVE_TYPE "string"
      NIL

    

eight := 8;

DECLARE
  ATOM "eight" : ? = ?
  ATOM ? : ? = 8


five : int := 5;

DECLARE
  ATOM "five" : "int" = ?
  ATOM ? : ? = 5

print(eight);

EVAL
  IDENTIFIER "print"
  TUPLE
    ATOM
      IDENTIFIER "eight"
      NIL
      NIL


main := function(argc : int, argv : List(string)) -> int {
  result = 10;
}

DECLARATION
  ATOM
    IDENTIFIER "main"
    NIL
    NIL
  FUNCTION
    TUPLE
      ATOM
        IDENTIFIER "argc"
        NATIVE_TYPE "int"
        NIL
      ATOM
        IDENTIFIER "argv"
        USER_TYPE "List"
          TUPLE
            ATOM
              IDENTIFIER "string"
              NIL
              NIL
        NIL
    TUPLE
      ATOM "int" : ? = ?
    NIL // no context
    BLOCK
      ASSIGNMENT
        IDENTIFIER "result"
        ATOM ? : ? = 10

      


foobar = a + b + c

ASSIGNMENT
  ATOM "foobar" : ? = ?
  BINOP "+"
    ATOM "a" : ? = ?
    BINOP "+"
      ATOM "b" : ? = ?
      ATOM "c" : ? = ?
