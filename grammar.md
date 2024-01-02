$$

\begin{align}

    [prog] &\to [stmt]^*

    \\

    [expr] &\to INTEGER

    \\

    [stmt] &\to
    \begin{cases} 

        exit([expr]); \\
        let \space [indent] \space = \space [expr];

    \end{cases}

\end{align}

$$