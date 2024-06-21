from include.Parser import *

class Variable:
    def __init__(self, name : str, pos : int) -> None:
        self.name : str = name
        self.pos : int = pos

class Generator:
    def __init__(self, program : NodeProgram) -> None:
        self.program : NodeProgram = program

        self.output_init : str = ""
        self.output_hidden_func : str = ""
        self.output_text : str = ""
        self.output_data : str = ""
        self.output_bss : str = ""

        self.pos : int = 0
        self.label_count : int = 0

        self.variables : list[Variable] = []
        self.scopes : list[int] = []

    def push(self, reg : str):
        self.output_text += f"    push {reg}\n"
        self.pos += 1

    def pop(self, reg : str):
        self.output_text += f"    pop {reg}\n"
        self.pos -= 1

    def create_label(self):
        self.label_count += 1
        return f"label{self.label_count}"

    # - Generating - #        

    def gen_ident_lit(self, ident_lit : NodeIdentLit):
        found : bool = False
        for var in self.variables:
            if(var.name == ident_lit.name):
                found = True
                self.push(f"QWORD [rsp + {str((self.pos - var.pos - 1) * 8)}]")
                break

        if(not found):
            pass
            print(ident_lit.name + " NOT FOUND")
            # TODO : Error Message

    def gen_int_term(self, int_term : NodeIntTerm):
        if(type(int_term.int_term) == NodeIntLit):
            self.push(int_term.int_term.int_lit)
        elif(type(int_term.int_term) == NodeIdentLit):
            self.gen_ident_lit(int_term.int_term)

    def gen_str_term(self, str_term : NodeStrTerm):
        if(type(str_term.str_term) == NodeStrLit):
            label = self.create_label()
            self.output_data += f"{label} : db `{str_term.str_term.str_lit}`, 0\n"
            self.output_text += "    mov rdi, " + label + "\n"
            self.push("rdi")
            
        elif(type(str_term.str_term) == NodeIdentLit):
            self.gen_ident_lit(str_term.str_term)
    
    def gen_bool_term(self, bool_term : NodeBoolTerm):
        if(type(bool_term.bool_term) == NodeBoolLit):
            self.push("GF_TRUE" if bool_term.bool_term.bool_lit == True else "GF_FALSE")
        elif(type(bool_term.bool_term) == NodeIdentLit):
            self.gen_ident_lit(bool_term.bool_term)

    def gen_int_expr(self, int_expr : NodeIntExpr):
        if(type(int_expr.int_expr) == NodeIntTerm):
            self.gen_int_term(int_expr.int_expr)

    def gen_str_expr(self, str_expr : NodeStrExpr):
        if(type(str_expr.str_expr) == NodeStrTerm):
            self.gen_str_term(str_expr.str_expr)
    
    def gen_bool_expr(self, bool_expr : NodeBoolExpr):
        if(type(bool_expr.bool_expr) == NodeBoolTerm):
            self.gen_bool_term(bool_expr.bool_expr)

    def gen_expr(self, expr : NodeExpr):
        if(type(expr.expr) == NodeIntExpr):
            self.gen_int_expr(expr.expr)
        elif(type(expr.expr) == NodeBoolExpr):
            self.gen_bool_expr(expr.expr)
        elif(type(expr.expr) == NodeStrExpr):
            self.gen_str_expr(expr.expr)

    def gen_scope(self, scope : NodeStmtScope):
        if(len(scope.stmts) > 0):
            self.scopes.append(len(self.variables))

            for i in range(len(scope.stmts)):
                self.gen_stmt(scope.stmts[i])

            pop_count = len(self.variables) - self.scopes[-1]
            self.output_text += f"    add rsp, {pop_count * 8}\n"
            self.pos -= pop_count

            for _ in range(pop_count):
                self.variables.pop()

            self.scopes.pop()

    def gen_stmt(self, stmt : NodeStmt):
        if(type(stmt.stmt) == NodeStmtExit):
            self.gen_int_expr(stmt.stmt.int_expr)

            self.pop("rdi")
            self.output_text += "    call _exit\n"

        elif(type(stmt.stmt) == NodeStmtDef):
            for var in self.variables:
                if(var.name == stmt.stmt.name):
                    print("VARIABILE GIA' DICHIARATA")
                    # TODO : Error Message
                    break

            self.variables.append(Variable(stmt.stmt.name, self.pos))
            self.gen_expr(stmt.stmt.expr)

        elif(type(stmt.stmt) == NodeStmtSet):
            found : bool = False
            for var in self.variables:
                if(var.name == stmt.stmt.name):
                    found = True
                    self.gen_expr(stmt.stmt.expr)
                    self.pop("rax")
                    self.output_text += f"    mov QWORD [rsp + {str((self.pos - var.pos - 1) * 8)}], rax\n"
                    break

            if(not found):
                print("VARIABILE NON ESISTENTE" + str(stmt.stmt.expr))
                # TODO : Error Message

        elif(type(stmt.stmt) == NodeStmtPrint):
            self.gen_str_expr(stmt.stmt.str_expr)
            self.pop("rdi")
            self.output_text += "    call strlen\n"
            self.output_text += "    mov rsi, rdi\n"
            self.output_text += "    mov rdx, rax\n"
            self.output_text += "    mov rdi, 1\n"
            self.output_text += "    mov rax, 1\n"
            self.output_text += "    syscall\n"
        
        elif(type(stmt.stmt) == NodeStmtPrintInt):
            self.gen_int_expr(stmt.stmt.int_expr)
            self.pop("ax")
            self.output_text += "    call print_number\n"

        elif(type(stmt.stmt) == NodeStmtMkDir):
            self.gen_int_expr(stmt.stmt.mode)
            self.gen_str_expr(stmt.stmt.path)
            self.output_text += "    mov rax, 83\n"
            self.pop("rdi")
            self.pop("rsi")
            self.output_text += "    syscall\n"
            self.output_text += "    test rax, rax\n    js exit_with_error\n"

        elif(type(stmt.stmt) == NodeStmtMkFl):
            self.gen_int_expr(stmt.stmt.mode)
            self.gen_int_expr(stmt.stmt.flags)
            self.gen_str_expr(stmt.stmt.path)
            self.output_text += "    mov rax, 2\n"
            self.pop("rdi")
            self.pop("rsi")
            self.pop("rdx")
            self.output_text += "    syscall\n"
            self.output_text += "    test rax, rax\n    js exit_with_error\n"

        elif(type(stmt.stmt) == NodeStmtScope):
            self.gen_scope(stmt.stmt)
        
        elif(type(stmt.stmt) == NodeStmtIf):
            true_label = self.create_label()
            false_label = self.create_label()
            end_label = self.create_label()
            
            self.gen_bool_expr(stmt.stmt.bool_expr)
            self.pop("rax")
            self.output_text += "    cmp rax, GF_TRUE\n"
            self.output_text += "    je " + true_label + "\n"
            self.output_text += "    jne " + false_label + "\n"
            self.output_text += true_label + ":\n"
            self.gen_scope(stmt.stmt.scope)
            if(stmt.stmt.else_scope != None):
                self.output_text += "    jmp " + end_label + "\n"
            self.output_text += false_label + ":\n"
            if(stmt.stmt.else_scope != None):
                self.gen_scope(stmt.stmt.else_scope)
                self.output_text += end_label + ":\n"

        elif(type(stmt.stmt) == NodeStmtWhile):
            init_label = self.create_label()
            true_label = self.create_label()
            false_label = self.create_label()

            self.gen_bool_expr(stmt.stmt.bool_expr)
            self.pop("rax")
            self.output_text += init_label + ":\n"
            self.output_text += "    cmp rax, GF_TRUE\n"
            self.output_text += "    je " + true_label + "\n"
            self.output_text += "    jne " + false_label + "\n"
            self.output_text += true_label + ":\n"
            self.gen_scope(stmt.stmt.scope)
            self.gen_bool_expr(stmt.stmt.bool_expr)
            self.pop("rax")
            self.output_text += "    jmp " + init_label + "\n"
            self.output_text += false_label + ":\n"

    def gen_program(self) -> str:
        self.output_init += "section .text\n\n"
        self.output_init += "default rel\n"
        self.output_init += "global _start\n"
        self.output_init += "extern _exit, strlen\n"

        self.output_data = "section .data\n\n"
        self.output_data += "GF_TRUE : db 255\nGF_FALSE : db 0\n"

        self.output_bss = "section .bss\n\n"
        self.output_bss += "printNumberBuffer resb 25\n"
        self.output_bss += "printNumberBufferPos resb 8\n"
        
        # Print Int Number #
        # Input  -> AX
        # Output -> PrintString
        self.output_hidden_func = """
            print_number:
                mov rcx, printNumberBuffer
                mov rbx, 0
                mov [rcx], rbx
                inc rcx
                mov [printNumberBufferPos], rcx
            print_number_loop:
                mov rdx, 0
                mov rbx, 10
                div rbx 
                push rax
                add rdx, 48
                mov rcx, [printNumberBufferPos]
                mov [rcx], dl
                inc rcx
                mov [printNumberBufferPos], rcx
                pop rax
                cmp rax, 0
                jne print_number_loop
            print_number_loop_2:
                mov rcx, [printNumberBufferPos]
                mov rax, 1
                mov rdi, 1
                mov rsi, rcx
                mov rdx, 1
                syscall
                mov rcx, [printNumberBufferPos]
                dec rcx
                mov [printNumberBufferPos], rcx
                cmp rcx, printNumberBuffer
                jge print_number_loop_2
                ret\n
        """

        self.output_text += "_start:\n"

        for stmt in self.program.stmts:
            self.gen_stmt(stmt)

        self.output_text += "    xor rdi, rdi\n"
        self.output_text += "    call _exit\n\n"

        self.output_text += "exit_with_error:\n"
        self.output_text += "    mov rax, 60\n    mov rdi, 1\n    syscall"

        self.output_text = self.output_text.replace("    push rdi\n    pop rdi\n", "")

        return f"{self.output_init}\n{self.output_hidden_func}\n{self.output_text}\n{self.output_data}\n{self.output_bss}"