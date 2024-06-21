from include.Tokens import *

class Tokenizer:
    def __init__(self, fc : str) -> None:
        self.file_content : str = fc
        self.index : int = 0
        self.curr_row : int = 1
        self.curr_col : int = 1

    def peek(self, offset : int = 0) -> str:
        file_content : str = self.file_content
        target_index : int = self.index + offset
        
        if target_index >= len(file_content): return None
        return file_content[target_index]
    
    def consume(self) -> str:
        file_content : str = self.file_content

        if self.index >= len(file_content): return None

        r : str = self.file_content[self.index]
        self.index += 1
        if(r == "\n"):
            self.curr_row += 1
            self.curr_col = 1
        else:
            self.curr_col += 1
        return r

    def tokenize(self) -> list[Token]:
        tokens : list[Token] = []
        buf : str = ""

        while(self.peek() != None):
            if(self.peek() == "\n"): self.consume()
            
            elif(self.peek().isalpha()):
                buf = self.consume()

                while(self.peek() != None)and((self.peek().isalpha())or(self.peek() == "_")or(self.peek().isdecimal())):
                    buf += self.consume()

                if(buf == "exit"):
                    tokens.append(Token(TokenType.EXIT, self.curr_row, self.curr_col - len(buf), ""))
                elif(buf == "print"):
                    tokens.append(Token(TokenType.PRINT, self.curr_row, self.curr_col - len(buf), ""))
                elif(buf == "print_int"):
                    tokens.append(Token(TokenType.PRINT_INT, self.curr_row, self.curr_col - len(buf), ""))
                elif(buf == "def"):
                    tokens.append(Token(TokenType.DEF, self.curr_row, self.curr_col - len(buf), ""))
                elif(buf == "set"):
                    tokens.append(Token(TokenType.SET, self.curr_row, self.curr_col - len(buf), ""))
                elif(buf == "true"):
                    tokens.append(Token(TokenType.BOOL_LIT, self.curr_row, self.curr_col - len(buf), buf))
                elif(buf == "false"):
                    tokens.append(Token(TokenType.BOOL_LIT, self.curr_row, self.curr_col - len(buf), buf))
                elif(buf == "if"):
                    tokens.append(Token(TokenType.IF, self.curr_row, self.curr_col - len(buf), ""))
                elif(buf == "else"):
                    tokens.append(Token(TokenType.ELSE, self.curr_row, self.curr_col - len(buf), ""))
                elif(buf == "while"):
                    tokens.append(Token(TokenType.WHILE, self.curr_row, self.curr_col - len(buf), ""))
                elif(buf == "mkdir"):
                    tokens.append(Token(TokenType.MKDIR, self.curr_row, self.curr_col - len(buf), ""))
                elif(buf == "mkfl"):
                    tokens.append(Token(TokenType.MKFL, self.curr_row, self.curr_col - len(buf), ""))
                else:
                    if(buf.startswith("i_")):
                        tokens.append(Token(TokenType.INT_IDENT, self.curr_row, self.curr_col - len(buf), buf))
                    elif(buf.startswith("b_")):
                        tokens.append(Token(TokenType.BOOL_IDENT, self.curr_row, self.curr_col - len(buf), buf))
                    elif(buf.startswith("s_")):
                        tokens.append(Token(TokenType.STR_IDENT, self.curr_row, self.curr_col - len(buf), buf))
                    else:
                        pass
                        # TODO : Error Message

                buf = ""

            elif(self.peek().isdecimal()):
                buf = self.consume()

                while(self.peek() != None)and(self.peek().isdecimal()):
                    buf += self.consume()
                
                tokens.append(Token(TokenType.INT_LIT, self.curr_row, self.curr_col - len(buf), buf))
                buf = ""
            
            elif(self.peek() == "\""):
                self.consume()
                while(self.peek() != None)and(self.peek() != "\""):
                    buf += self.consume()
                if(self.peek() == "\""):
                    self.consume()
                    tokens.append(Token(TokenType.STR_LIT, self.curr_row, self.curr_col, buf))
                    buf = ""

            elif(self.peek().isspace()):
                self.consume()
                
            elif(self.peek() == "/"):
                self.consume()

                if(self.peek() == "/"):
                    while(self.peek() != None)and(self.peek() != "\n"):
                        self.consume()

                elif(self.peek() == "&"):
                    self.consume()
                    while(self.peek() != None)and(self.peek() != "&")and(self.peek(1) != "/"):                        
                        self.consume()

                    self.consume()
                    self.consume()

                else:
                    pass
                    # TODO : '/' Token

            else:
                match self.peek():
                    case "(":
                        tokens.append(Token(TokenType.L_PAREN, self.curr_row, self.curr_col, ""))
                        self.consume()
                    case ")":
                        tokens.append(Token(TokenType.R_PAREN, self.curr_row, self.curr_col, ""))
                        self.consume()

                    case "{":
                        tokens.append(Token(TokenType.L_CURLY, self.curr_row, self.curr_col, ""))
                        self.consume()
                    case "}":
                        tokens.append(Token(TokenType.R_CURLY, self.curr_row, self.curr_col, ""))
                        self.consume()

                    case "=":
                        tokens.append(Token(TokenType.EQ, self.curr_row, self.curr_col, ""))
                        self.consume()
                    case ",":
                        tokens.append(Token(TokenType.COL, self.curr_row, self.curr_col, ""))
                        self.consume()

                    case _:
                        pass
                        # TODO : Error Message

        return tokens