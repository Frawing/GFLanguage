from include.Global import *

TokenType = Enum('TokenType', [

    'EXIT', 'PRINT', 'PRINT_INT',
    'MKDIR', 'MKFL',

    'DEF', 'SET',
    'IF', 'ELSE', 'WHILE',

    'INT_LIT', 'BOOL_LIT', 'STR_LIT',
    'INT_IDENT', 'BOOL_IDENT', 'STR_IDENT',

    'L_PAREN', 'R_PAREN', 'L_CURLY', 'R_CURLY',
    'EQ', 'COL'

])

class Token:
    def __init__(self, type : TokenType, row : int, col : int, value : str) -> None:
        self.type : TokenType = type
        self.row : int = row
        self.col : int = col
        self.value : str = value

    def __str__(self) -> str:
        return f"{self.type.name}{f' : {self.value}' if self.value != '' else ''} - ({self.row}, {self.col})"
