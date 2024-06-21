from include.AST import *

class Parser:
    def __init__(self, tokens : list[Token]) -> None:
        self.tokens : list [Token] = tokens
        self.index : int = 0
    
    def peek(self, offset : int = 0) -> Token:
        tokens : list[Token] = self.tokens
        target_index : int = self.index + offset
        
        if target_index >= len(tokens): return None
        return tokens[target_index]
    
    def consume(self) -> Token:
        tokens : list[Token] = self.tokens

        if self.index >= len(tokens): return None

        self.index += 1
        return tokens[self.index - 1]
    
    def peek_is(self, token_type : TokenType, error : bool = True) -> Token | None:
        token = self.peek()
        if(token == None):
            if(error):
                print_error("EOF", "",
                            f"Expected token '{token_type.name}', but reached end of file.",
                            "Parsing token",
                            "Ensure the code structure is correct and all tokens are present.")
            return None
        if(token.type == token_type):
            return token
        else:
            if(error):
                print_error(token.row, token.col,
                            f"Expected token '{token_type.name}', but found '{token.type.name}'.",
                            f"Parsing token",
                            f"Check the syntax near line {token.row}, {token.col}")
            return None
        
    def try_consume(self, token_type : TokenType, error : bool = True) -> Token | None:
        token = self.peek()
        if(token == None):
            if(error):
                print_error("EOF", "",
                            f"Expected token '{token_type.name}', but reached end of file.",
                            "Parsing token",
                            "Ensure the code structure is correct and all tokens are present.")
            return None
        if(token.type == token_type):
            self.index += 1
            return token
        else:
            if(error):
                print_error(token.row, token.col,
                            f"Expected token '{token_type.name}', but found '{token.type.name}'.",
                            f"Parsing token",
                            f"Check the syntax near line {token.row}, {token.col}")
            return None
        
    # - Parsing - #

    def parse_int_term(self, err : bool = True):
        term = NodeIntTerm()
        if(self.peek_is(TokenType.INT_LIT, False)):
            term.int_term = NodeIntLit()
            term.int_term.int_lit = int(self.consume().value)
            return term
        elif(self.peek_is(TokenType.INT_IDENT, False)):
            term.int_term = NodeIdentLit()
            term.int_term.name = self.consume().value
            return term
        
        if(err):
            print_error(self.peek().row, self.peek().col, "Expected 'INT_TERM'", "Parsing 'INT_TERM'", "Ensure the next token in an 'INT_TERM'")

        return None

    def parse_str_term(self, err : bool = True):
        term = NodeStrTerm()
        if(self.peek_is(TokenType.STR_LIT, False)):
            term.str_term = NodeStrLit()
            term.str_term.str_lit = self.consume().value
            return term
        elif(self.peek_is(TokenType.STR_IDENT, False)):
            term.str_term = NodeIdentLit()
            term.str_term.name = self.consume().value
            return term
    
        if(err):
            print_error(self.peek().row, self.peek().col, "Expected 'STR_TERM'", "Parsing 'STR_TERM'", "Ensure the next token in an 'STR_TERM'")

        return None
    
    def parse_bool_term(self, err : bool = True):
        term = NodeBoolTerm()
        if(self.peek_is(TokenType.BOOL_LIT, False)):
            term.bool_term = NodeBoolLit()
            term.bool_term.bool_lit = True if self.consume().value == "true" else False
            return term
        elif(self.peek_is(TokenType.BOOL_IDENT, False)):
            term.bool_term = NodeIdentLit()
            term.bool_term.name = self.consume().value
            return term

        if(err):
            print_error(self.peek().row, self.peek().col, "Expected 'BOOL_TERM'", "Parsing 'BOOL_TERM'", "Ensure the next token in an 'BOOL_TERM'")

        return None
    
    def parse_int_expr(self, err : bool = True):
        expr = NodeIntExpr()
        term = self.parse_int_term(False)
        if(term != None):
            expr.int_expr = term
            return expr
        
        if(err):
            print_error(self.peek().row, self.peek().col, "Expected 'INT_EXPR'", "Parsing 'INT_EXPR'", "Ensure the next token in an 'INT_EXPR'")

        return None
    
    def parse_str_expr(self, err : bool = True):
        expr = NodeStrExpr()
        term = self.parse_str_term(False)
        if(term != None):
            expr.str_expr = term
            return expr
        
        if(err):
            print_error(self.peek().row, self.peek().col, "Expected 'STR_EXPR'", "Parsing 'STR_EXPR'", "Ensure the next token in an 'STR_EXPR'")

        return None

    def parse_bool_expr(self, err : bool = True):
        expr = NodeBoolExpr()
        term = self.parse_bool_term(False)
        if(term != None):
            expr.bool_expr = term
            return expr
        
        if(err):
            print_error(self.peek().row, self.peek().col, "Expected 'BOOL_EXPR'", "Parsing 'BOOL_EXPR'", "Ensure the next token in an 'BOOL_EXPR'")

        return None

    def parse_expr(self, err : bool = True):
        expr = NodeExpr()
        int_expr = self.parse_int_expr(False)
        if(int_expr != None):
            expr.expr = int_expr
            return expr
        
        bool_expr = self.parse_bool_expr(False)
        if(bool_expr != None):
            expr.expr = bool_expr
            return expr
    
        str_expr = self.parse_str_expr(False)
        if(str_expr != None):
            expr.expr = str_expr
            return expr
        
        if(err):
            print_error(self.peek().row, self.peek().col, "Expected 'EXPR'", "Parsing 'EXPR'", "Ensure the next token in an 'EXPR'")

        return None

    def parse_scope(self):
        scope = NodeStmtScope()
        self.try_consume(TokenType.L_CURLY)
        while(not self.peek_is(TokenType.R_CURLY, False))and(self.peek() != None):
            stmt = self.parse_stmt()
            if(stmt != None):
                scope.stmts.append(stmt)
            else:
                break

        self.try_consume(TokenType.R_CURLY)

        return scope

    def parse_stmt(self):
        stmt = NodeStmt()

        if(self.try_consume(TokenType.EXIT, False)):
            self.try_consume(TokenType.L_PAREN)
            stmt.stmt = NodeStmtExit()
            stmt.stmt.int_expr = self.parse_int_expr()
            self.try_consume(TokenType.R_PAREN)
            return stmt
        
        elif(self.peek_is(TokenType.DEF, False))or(self.peek_is(TokenType.SET, False)):
            is_set : bool = False
            if(self.try_consume(TokenType.SET, False)):
                is_set = True
            else:
                self.consume()

            if(self.peek_is(TokenType.INT_IDENT, False)):
                name = self.consume().value
                self.try_consume(TokenType.EQ)
                expr = self.parse_int_expr()
            elif(self.peek_is(TokenType.STR_IDENT, False)):
                name = self.consume().value
                self.try_consume(TokenType.EQ)
                expr = self.parse_str_expr()
            elif(self.peek_is(TokenType.BOOL_IDENT, False)):
                name = self.consume().value
                self.try_consume(TokenType.EQ)
                expr = self.parse_bool_expr()
            else:
                print("INVALID VAR TYPE")
                # TODO : Error Message

            if(is_set):
                stmt.stmt = NodeStmtSet()                    
            else:
                stmt.stmt = NodeStmtDef()
            
            stmt.stmt.name = name
            stmt.stmt.expr = NodeExpr()
            stmt.stmt.expr.expr = expr

            return stmt

        elif(self.try_consume(TokenType.PRINT, False)):
            stmt.stmt = NodeStmtPrint()
            self.try_consume(TokenType.L_PAREN)
            stmt.stmt.str_expr = self.parse_str_expr()
            self.try_consume(TokenType.R_PAREN)
            return stmt
    
        elif(self.try_consume(TokenType.PRINT_INT, False)):
            stmt.stmt = NodeStmtPrintInt()
            self.try_consume(TokenType.L_PAREN)
            stmt.stmt.int_expr = self.parse_int_expr()
            self.try_consume(TokenType.R_PAREN)
            return stmt
        
        elif(self.try_consume(TokenType.MKDIR, False)):
            stmt.stmt = NodeStmtMkDir()
            self.try_consume(TokenType.L_PAREN)
            stmt.stmt.path = self.parse_str_expr()
            self.try_consume(TokenType.COL)
            stmt.stmt.mode = self.parse_int_expr()
            self.try_consume(TokenType.R_PAREN)
            return stmt
        
        elif(self.try_consume(TokenType.MKFL, False)):
            stmt.stmt = NodeStmtMkFl()
            self.try_consume(TokenType.L_PAREN)
            stmt.stmt.path = self.parse_str_expr()
            self.try_consume(TokenType.COL)
            stmt.stmt.mode = self.parse_int_expr()
            self.try_consume(TokenType.COL)
            stmt.stmt.flags = self.parse_int_expr()
            self.try_consume(TokenType.R_PAREN)
            return stmt

        elif(self.peek_is(TokenType.L_CURLY, False)):
            stmt.stmt = self.parse_scope()
            return stmt
        
        elif(self.try_consume(TokenType.IF, False)):
            stmt.stmt = NodeStmtIf()
            stmt.stmt.bool_expr = self.parse_bool_expr()
            stmt.stmt.scope = self.parse_scope()
            if(self.try_consume(TokenType.ELSE, False)):
                stmt.stmt.else_scope = self.parse_scope()
            return stmt
        
        elif(self.try_consume(TokenType.WHILE, False)):
            stmt.stmt = NodeStmtWhile()
            stmt.stmt.bool_expr = self.parse_bool_expr()
            stmt.stmt.scope = self.parse_scope()
            return stmt

        return None

    def parse_program(self):
        program = NodeProgram()

        while(self.peek() != None):
            stmt = self.parse_stmt()
            if(stmt != None):
                program.stmts.append(stmt)
            else:
                pass
                # TODO : Error Message

        return program