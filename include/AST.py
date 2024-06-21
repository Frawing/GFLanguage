from include.Tokenizer import *

def stmt(s : str):
    return f"{Fore.MAGENTA}Stmt{s}{Fore.WHITE}"

def val(s):
    return f"{Fore.YELLOW}{s}{Fore.WHITE}"

def dec(s : str):
    return f"{Fore.CYAN}{s}{Fore.WHITE}"

def prog():
    return Fore.GREEN + "NodeProgram"

# Literals

class NodeIdentLit:
    def __init__(self) -> None:
        self.name : str = ""
    def __str__(self) -> str:
        return val(f"Ident {dec('->')} {val(self.name)}")

class NodeIntLit:
    def __init__(self) -> None:
        self.int_lit : int = 0
    def __str__(self) -> str:
        return val(self.int_lit)

class NodeStrLit:
    def __init__(self) -> None:
        self.str_lit : str = ""
    def __str__(self) -> str:
        return val(self.str_lit)

class NodeBoolLit:
    def __init__(self) -> None:
        self.bool_lit : bool = False
    def __str__(self) -> str:
        return f"{val(self.bool_lit)}"

# Terms

class NodeIntTerm:
    def __init__(self) -> None:
        self.int_term : NodeIntLit | NodeIdentLit = NodeIntLit()
    def __str__(self) -> str:
        return f"{self.int_term}"

class NodeStrTerm:
    def __init__(self) -> None:
        self.str_term : NodeStrLit | NodeIdentLit = NodeStrLit()
    def __str__(self) -> str:
        return f"{self.str_term}"

class NodeBoolTerm:
    def __init__(self) -> None:
        self.bool_term : NodeBoolLit | NodeIdentLit = NodeBoolLit()
    def __str__(self) -> str:
        return f"{self.bool_term}"
    
# Expressions

class NodeIntExpr:
    def __init__(self) -> None:
        self.int_expr : NodeIntTerm = NodeIntTerm()
    def __str__(self) -> str:
        return f"{self.int_expr}"

class NodeStrExpr:
    def __init__(self) -> None:
        self.str_expr : NodeStrTerm = NodeStrTerm()
    def __str__(self) -> str:
        return f"{self.str_expr}"

class NodeBoolExpr:
    def __init__(self) -> None:
        self.bool_expr : NodeBoolTerm = NodeBoolTerm()
    def __str__(self) -> str:
        return f"{self.bool_expr}"
    
class NodeExpr:
    def __init__(self) -> None:
        self.expr : NodeIntExpr | NodeStrExpr | NodeBoolExpr = NodeIntExpr()
    def __str__(self) -> str:
        return f"{self.expr}"

# Statments

class NodeStmtDef:
    def __init__(self) -> None:
        self.name : str
        self.expr : NodeExpr
    def __str__(self) -> str:
        return f"{stmt('Def')} {dec('--Name->')} {val(self.name)} {dec('--Value->')} {self.expr}"

class NodeStmtSet:
    def __init__(self) -> None:
        self.name : str
        self.expr : NodeExpr
    def __str__(self) -> str:
        return f"{stmt('Set')} {dec('--Name->')} {val(self.name)} {dec('--Value->')} {self.expr}"

class NodeStmtExit:
    def __init__(self) -> None:
        self.int_expr : NodeIntExpr
    def __str__(self) -> str:
        return f"{stmt('Exit')} {dec('->')} {self.int_expr}"
    
class NodeStmtPrint:
    def __init__(self) -> None:
        self.str_expr : NodeStrExpr
    def __str__(self) -> str:
        return f"{stmt('Print')} {dec('->')} {self.str_expr}"
    
class NodeStmtPrintInt:
    def __init__(self) -> None:
        self.int_expr : NodeIntExpr
    def __str__(self) -> str:
        return f"{stmt('PrintInt')} {dec('->')} {self.int_expr}"

class NodeStmtMkDir:
    def __init__(self) -> None:
        self.path : NodeStrExpr
        self.mode : NodeIntExpr
    def __str__(self) -> str:
        return f"{stmt('MkDir')} {dec('--Path->')} {val(self.path)} {dec('--Mode->')} {val(self.mode)}"

class NodeStmtMkFl:
    def __init__(self) -> None:
        self.path : NodeStrExpr
        self.mode : NodeIntExpr
        self.flags : NodeIntExpr
    def __str__(self) -> str:
        return f"{stmt('MkFl')} {dec('--Path->')} {val(self.path)} {dec('--Mode->')} {val(self.mode)} {dec('--Flags->')} {val(self.flags)}"


class NodeStmtScope:
    def __init__(self) -> None:
        self.stmts : list[NodeStmt] = []
    def __str__(self) -> str:
        return stmt('Scope') + dec(' -> ') + "[\n\n" + "\n".join(map(str, self.stmts)) + "\n\n]"
    
class NodeStmtIf:
    def __init__(self) -> None:
        self.bool_expr : NodeBoolExpr
        self.scope : NodeStmtScope
        self.else_scope : NodeStmtScope = None
    def __str__(self) -> str:
        return f"{stmt('If')} {dec('--BoolExpr->')} {self.bool_expr} {dec('--Scope->')} {self.scope}"

class NodeStmtWhile:
    def __init__(self) -> None:
        self.bool_expr : NodeBoolExpr
        self.scope : NodeStmtScope
    def __str__(self) -> str:
        return f"{stmt('While')} {dec('--BoolExpr->')} {self.bool_expr} {dec('--Scope->')} {self.scope}"

# Node Statment / Program

class NodeStmt:
    def __init__(self) -> None:
        self.stmt : (NodeStmtExit | NodeStmtPrint | NodeStmtPrintInt |
                     NodeStmtMkDir | NodeStmtMkFl |
                     NodeStmtDef | NodeStmtSet |
                     NodeStmtScope | NodeStmtIf | NodeStmtWhile)
    def __str__(self) -> str:
        return f"{self.stmt}"

class NodeProgram:
    def __init__(self) -> None:
        self.stmts : list[NodeStmt] = []
    def __str__(self) -> str:
        return prog() + dec(' -> ') + "[\n\n" + "\n\n".join(map(str, self.stmts)) + Fore.WHITE +"\n\n]"