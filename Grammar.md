$$
\begin{align}

    [ \text{Grammar} ] &\to

        \begin{cases}

            [ \text{Prog} ] &\to [ \text{Stmt} ]* \\

            \\

            [ \text{Stmt} ] &\to 
                \begin{cases}
                    \text{exit}    ( \space [ \text{IntExpr} ] \space ) \\
                    \text{def} \space \text{ident} = [ \text{Expr} ] \\
                    \text{set} \space \text{ident} = [ \text{Expr} ] \\
                    \text{print} ( \space [ \text{StrExpr} ] \space ) \\
                    \text{print\_int} ( \space [ \text{IntExpr} ] \space ) \\
                    [ \text{Scope} ] \\
                    \text{if} [ \text{BoolExpr} ] [ \text{Scope} ] [ \text{Else} ]+ \\
                    \text{while} [ \text{BoolExpr} ] [ \text{Scope} ]
                \end{cases} \\

            \\

            [ \text{Else} ] &\to \text{else} [ \text{Scope} ] \\

            \\

            [ \text{Scope} ] &\to { \space [ \text{Stmt} ]* \space } \\

            \\

            [ \text{Expr} ] &\to
                \begin{cases}
                    [ \text{IntExpr} ]
                \end{cases} \\

            \\

            [ \text{IntExpr} ] &\to
                \begin{cases}
                    [ \text{IntTerm} ]
                \end{cases} \\

            \\

            [ \text{StrExpr} ] &\to
                \begin{cases}
                    [ \text{StrTerm} ]
                \end{cases} \\

            \\

            [ \text{BoolExpr} ] &\to
                \begin{cases}
                    [ \text{BoolTerm} ]
                \end{cases} \\

            \\

            [ \text{IntTerm} ] &\to
                \begin{cases}
                    \text{IntLit} \\
                    \text{IdentLit}            
                \end{cases} \\
            
            \\

            [ \text{StrTerm} ] &\to
                \begin{cases}
                    \text{StrLit} \\
                    \text{IdentLit}            
                \end{cases} \\

            \\

            [ \text{BoolTerm} ] &\to
                \begin{cases}
                    \text{BoolLit} \\
                    \text{IdentLit}            
                \end{cases}

        \end{cases}
        
\end{align}
$$