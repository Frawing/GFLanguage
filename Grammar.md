$$
\begin{align}

    [Prog] &\to [Stmt]^*

    \\

    [Scope] &\to {[Stmt]^*}
    
    \\
    
    [Stmt] &\to
    \begin{cases}

        exit( [IntExpr] );
        \\
        println( [TextExpr] )
        \\
        print( [TextExpr] );
        \\
        input();
        \\
        input( [TextIdent] );
        \\
        let \space ident = [Expr];
        \\
        ident = [Expr];
        \\
        [Scope]
        \\
        if ( [IntExpr] [CompSimb] [IntExpr] ) [Scope] [IfPred]^*
        \\
        while ( [IntExpr] [CompSimb] [IntExpr] ) [Scope]
    
    \end{cases}

    \\

    [Ident] &\to
    \begin{cases}
    
        [TextIdent]
        \\
        [IntIdent]
    
    \end{cases}

    \\

    [TextIdent] &\to let \space ident = [TextExpr]

    \\

    [IntIdent] &\to let \space ident = [IntExpr]
    
    \\

    [IfPred] &\to
    \begin{cases}
    
        elif ( [IntExpr] [CompSimb] [IntExpr] ) [Scope] [IfPred]
        \\
        else [Scope]
        \\
        \epsilon

    \end{cases}

    \\
    
    [Expr] &\to
    \begin{cases}

        [IntExpr]
        \\
        [TextExpr]

    \end{cases}

    \\
    
    [IntExpr] &\to
    \begin{cases}

        [BinExpr]
        \\
        [Term]

    \end{cases}

    \\
    
    [TextExpr] &\to
    \begin{cases}

        String
        \\
        Ident

    \end{cases}
    
    \\
    
    [BinExpr] &\to
    \begin{cases}

        [IntExpr] * [IntExpr] &\text{prec} = 1
        \\
        [IntExpr] \space / \space [Expr] &\text{prec} = 1
        \\
        [IntExpr] + [IntExpr] &\text{prec} = 0
        \\
        [IntExpr] - [IntExpr] &\text{prec} = 0
    
    \end{cases}
    
    \\
    
    [Term] &\to
    \begin{cases}

        Integer
        \\
        Identifier
        \\
        ( [IntExpr] )

    \end{cases}

    \\

    [CompSimbs] &\to
    \begin{cases}
    
        >
        \\
        <
        \\
        ==
        \\
        !=
    
    \end{cases}

\end{align}
$$