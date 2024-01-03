<div align="center">

# GFLanguage

### Grammar

$$

\begin{align*}

    [Prog] &\to [Stmt]^*
    
    \\
    
    [Stmt] &\to
    \begin{cases}

        exit([Expr]);
        \\
        let \space ident = [Expr];
    
    \end{cases}
    
    \\
    
    [Expr] &\to
    \begin{cases}
        
        [BinExpr]
        \\
        Term
    
    \end{cases}

    \\

    [BinExpr] &\to
    \begin{cases}
    
        [Expr] * [Expr] \space &prec = 1
        \\
        [Expr] + [Expr] \space &prec = 0
    
    \end{cases}

    \\

    [Term] &\to
    \begin{cases}

        Integer
        \\
        Identifier

    \end{cases}

\end{align*}

$$

</div>
