# GFLanguage

## Documentation

Coming soon...

## Grammar

Guarda la grammatica in 'Grammar.md'.

## Bug

Il compiler a volte non genererà alcun token, ma non preoccupatevi ve ne accorgerete dal seguente messaggio:

`The file is blank or there is some problem with the compiler. Please retry some times...`

Se il file non è vuoto basterà riprovare finchè il compiler non darà questo problema.

## Debug

Uno strumento che può aiutare il debugging è il parametro -dt che serve a stampare a schermo i token che ha analizzato il compiler, in questa maniera potrete verificare che il compiler abbia fatto il suo lavoro

## Args

Oltre al parametro illustrato precedentemente ce ne sono altri 2:

| Args | Descrizione | Facoltativo |
| --- | --- | --- |
| `-i` | Nome del file da compilare (con estenzione) | $$No$$ |
| `-op` | Percorso del file out.asm creato dal compiler | $$Si$$ |
| `-dt` | Print Tokens | $$Si$$ |