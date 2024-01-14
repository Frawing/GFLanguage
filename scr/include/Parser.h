#pragma once

#include <variant>
#include "Error.h"
#include "Arena.h"
#include "Token.h"

struct NodeExpr;
struct NodeIntExpr;
struct NodeStmt;
struct NodeIfPred;

/* Term */

struct NodeTermInt{
    Token token_integer;
};

struct NodeTermIdent{
    Token token_ident;
};

struct NodeTermParen{
    NodeIntExpr* int_expr;
};

struct NodeTerm
{
    std::variant<NodeTermInt*,
                 NodeTermIdent*,
                 NodeTermParen*> value;
};

/* Compare Simbols */

struct NodeCompSimb{
    Token token_simb;
};

/* Text */

struct NodeTextString{
    Token token_string;
};

struct NodeTextIdent
{
    Token token_ident;
};


/* Bin Expr */

struct NodeBinExprMul{
    NodeIntExpr* lhs;
    NodeIntExpr* rhs;
};

struct NodeBinExprDiv{
    NodeIntExpr* lhs;
    NodeIntExpr* rhs;
};

struct NodeBinExprAdd{
    NodeIntExpr* lhs;
    NodeIntExpr* rhs;
};

struct NodeBinExprSub{
    NodeIntExpr* lhs;
    NodeIntExpr* rhs;
};

struct NodeBinExpr{
    std::variant<NodeBinExprMul*, NodeBinExprDiv*,
                 NodeBinExprAdd*, NodeBinExprSub*> value;
};

/* Expr */

struct NodeIntExpr{
    std::variant<NodeTerm*,
                 NodeBinExpr*> value;
};

struct NodeTextExpr{
    std::variant<NodeTextString*,
                 NodeTextIdent*> value;
};

struct NodeExpr{
    std::variant<NodeTextExpr*, NodeIntExpr*> value;
};

/* Stmt */

struct NodeStmtExit{
    NodeIntExpr* int_expr;
};

struct NodeStmtPrint
{
    NodeTextExpr* text_expr;
    bool new_line = false;
};

struct NodeStmtLet{
    Token token_ident;
    NodeExpr* expr;
};

struct NodeStmtAssign{
    Token token_ident;
    NodeExpr* expr;
};

/* Conditions */

struct NodeScope{
    std::vector<NodeStmt*> stmts;
};

struct NodeIfPredElif{
    NodeIntExpr* int_expr_1;
    NodeCompSimb* comp_simb;
    NodeIntExpr* int_expr_2;
    NodeScope* scope;
    std::optional<NodeIfPred*> pred;
};

struct NodeIfPredElse{
    NodeScope* scope;
};

struct NodeIfPred{
    std::variant<NodeIfPredElif*, NodeIfPredElse*> value;
};

struct NodeStmtIf
{
    NodeIntExpr* int_expr_1;
    NodeCompSimb* comp_simb;
    NodeIntExpr* int_expr_2;
    NodeScope* scope;
    std::optional<NodeIfPred*> pred;
};

/* Other */

struct NodeStmt
{
    std::variant<NodeStmtLet*, NodeStmtAssign*,
                 NodeStmtIf*, NodeScope*,
                 NodeStmtExit*, NodeStmtPrint*> value;
};

struct NodeProg{
    std::vector<NodeStmt*> stmts;
};


/* Parser Class */

class Parser
{
    public:

        explicit inline Parser(const std::vector<Token> p_tokens) 
            : tokens(p_tokens), allocator(1024 * 1024 * 4) {}

        std::optional<NodeTerm*> parse_term();
        std::optional<NodeExpr*> parse_expr();
        std::optional<NodeTextExpr*> parse_text_expr();
        std::optional<NodeIntExpr*> parse_int_expr(int min_prec = 0);
        std::optional<NodeScope*> parse_scope();
        std::optional<NodeIfPred*> parse_if_pred();
        std::optional<NodeStmt*> parse_stmt();
        inline std::optional<NodeProg> parse_program()
        {
            NodeProg prog;
            while(peek().has_value())
            {
                if(auto stmt = parse_stmt()){
                    prog.stmts.push_back(stmt.value());
                }else{
                    error_invalid("statment");
                }
            }
            return prog;
        }

    private:
        
        const inline std::optional<Token> peek(int offset = 0)
        {
            if((index + offset) >= tokens.size()){
                return {};
            }else{
                return tokens.at(index + offset);
            }
        }
        inline Token consume(){
            return tokens.at(index++);
        }

        inline std::optional<Token> try_consume(const TokenType type)
        {
            if (peek().has_value() && peek().value().type == type){
                return consume();
            }
            return {};
        }

        inline std::optional<Token> try_consume(const TokenType type, const std::string msg, const int line = 0)
        {
            if (peek().has_value() && peek().value().type == type){
                return consume();
            }

            error_expected(msg, line);
            return {};
        }

        const std::vector<Token> tokens;
        size_t index = 0;
        ArenaAllocator allocator;

};