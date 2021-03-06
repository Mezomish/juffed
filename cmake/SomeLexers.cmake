# find TCL lexer

find_path ( QSCI_TCL_LEXER qscilexertcl.h ${QSCINTILLA_INCLUDE_DIR} )
if ( QSCI_TCL_LEXER )
    ADD_DEFINITIONS (
        -DJUFF_TCL_LEXER
    )
endif ( QSCI_TCL_LEXER )

# find Pascal lexer
find_path ( QSCI_PASCAL_LEXER qscilexerpascal.h ${QSCINTILLA_INCLUDE_DIR} )
if ( QSCI_PASCAL_LEXER )
    ADD_DEFINITIONS (
        -DJUFF_PASCAL_LEXER
    )
endif ( QSCI_PASCAL_LEXER )

# find Fortran lexer
find_path ( QSCI_FORTRAN_LEXER qscilexerfortran.h ${QSCINTILLA_INCLUDE_DIR} )
if ( QSCI_FORTRAN_LEXER )
    ADD_DEFINITIONS (
        -DJUFF_FORTRAN_LEXER
    )
endif ( QSCI_FORTRAN_LEXER )

# find Properties lexer
find_path ( QSCI_PROPERTIES_LEXER qscilexerproperties.h ${QSCINTILLA_INCLUDE_DIR} )
if ( QSCI_PROPERTIES_LEXER )
    ADD_DEFINITIONS (
        -DJUFF_PROPERTIES_LEXER
    )
endif ( QSCI_PROPERTIES_LEXER )

# find CSS3Property
find_path ( QSCI_CSS_LEXER qscilexercss.h ${QSCINTILLA_INCLUDE_DIR} )
if ( QSCI_CSS_LEXER )
    EXECUTE_PROCESS (
        COMMAND grep CSS3Property ${QSCI_CSS_LEXER}/qscilexercss.h
        OUTPUT_VARIABLE CSS3_FOUND
    )
    if ( CSS3_FOUND )
#        MESSAGE ( "CSS3 found!" )
        ADD_DEFINITIONS (
            -DCSS3_FOUND
        )
    endif ( CSS3_FOUND )
endif (QSCI_CSS_LEXER )
