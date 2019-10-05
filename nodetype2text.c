static const char* nodetype2text( int nodetype ) {
    switch ( nodetype ) {
        case _NT_GENERIC: return "_NT_GENERIC";
        case NT_INT_LIT: return "NT_INT_LIT";
        case NT_TERMINAL_1: return "NT_TERMINAL_1";
        case NT_REAL_LIT: return "NT_REAL_LIT";
        case NT_TERMINAL_DOT: return "NT_TERMINAL_DOT";
        case NT_TERMINAL_7: return "NT_TERMINAL_7";
        case NT_TERMINAL_MINUS: return "NT_TERMINAL_MINUS";
        case NT_STR_LIT: return "NT_STR_LIT";
        case NT_TERMINAL_11: return "NT_TERMINAL_11";
        case NT_BOOL_LIT: return "NT_BOOL_LIT";
        case NT_TERMINAL_TRUE: return "NT_TERMINAL_TRUE";
        case NT_TERMINAL_FALSE: return "NT_TERMINAL_FALSE";
        case NT_PTR_LIT: return "NT_PTR_LIT";
        case NT_TERMINAL_NIL: return "NT_TERMINAL_NIL";
        case NT_IDENTIFIER: return "NT_IDENTIFIER";
        case NT_TERMINAL_19: return "NT_TERMINAL_19";
        case NT_BASE_TYPE: return "NT_BASE_TYPE";
        case NT_TERMINAL_INT: return "NT_TERMINAL_INT";
        case NT_TERMINAL_REAL: return "NT_TERMINAL_REAL";
        case NT_TERMINAL_STRING: return "NT_TERMINAL_STRING";
        case NT_TERMINAL_BYTE: return "NT_TERMINAL_BYTE";
        case NT_TERMINAL_BOOL: return "NT_TERMINAL_BOOL";
        case NT_OBJECT_POINTER: return "NT_OBJECT_POINTER";
        case NT_TERMINAL_XOR: return "NT_TERMINAL_XOR";
        case NT_CELL_TYPE: return "NT_CELL_TYPE";
        case NT_CELL_POINTER: return "NT_CELL_POINTER";
        case NT_TERMINAL_STAR: return "NT_TERMINAL_STAR";
        case NT_TYPE: return "NT_TYPE";
        case NT_TYPE_LIST: return "NT_TYPE_LIST";
        case NT_TERMINAL_COMMA: return "NT_TERMINAL_COMMA";
        case NT_NEG_OP: return "NT_NEG_OP";
        case NT_TERMINAL_NOT: return "NT_TERMINAL_NOT";
        case NT_MULT_OP: return "NT_MULT_OP";
        case NT_TERMINAL_SLASH: return "NT_TERMINAL_SLASH";
        case NT_TERMINAL_MOD: return "NT_TERMINAL_MOD";
        case NT_ADD_OP: return "NT_ADD_OP";
        case NT_TERMINAL_PLUS: return "NT_TERMINAL_PLUS";
        case NT_SHF_OP: return "NT_SHF_OP";
        case NT_TERMINAL_SHL: return "NT_TERMINAL_SHL";
        case NT_TERMINAL_SHR: return "NT_TERMINAL_SHR";
        case NT_CMP_OP: return "NT_CMP_OP";
        case NT_TERMINAL_NE: return "NT_TERMINAL_NE";
        case NT_TERMINAL_LE: return "NT_TERMINAL_LE";
        case NT_TERMINAL_GE: return "NT_TERMINAL_GE";
        case NT_TERMINAL_EQ: return "NT_TERMINAL_EQ";
        case NT_TERMINAL_LT: return "NT_TERMINAL_LT";
        case NT_TERMINAL_GT: return "NT_TERMINAL_GT";
        case NT_AND_OP: return "NT_AND_OP";
        case NT_TERMINAL_AND: return "NT_TERMINAL_AND";
        case NT_OR_OP: return "NT_OR_OP";
        case NT_TERMINAL_OR: return "NT_TERMINAL_OR";
        case NT_C_NUM_BASE_E: return "NT_C_NUM_BASE_E";
        case NT_TERMINAL_LPAREN: return "NT_TERMINAL_LPAREN";
        case NT_TERMINAL_RPAREN: return "NT_TERMINAL_RPAREN";
        case NT_C_NUM_NEG_E: return "NT_C_NUM_NEG_E";
        case NT_C_NUM_MULT_E: return "NT_C_NUM_MULT_E";
        case NT_C_NUM_ADD_E: return "NT_C_NUM_ADD_E";
        case NT_C_NUM_SHF_E: return "NT_C_NUM_SHF_E";
        case NT_C_NUM_CMP_E: return "NT_C_NUM_CMP_E";
        case NT_C_NUM_AND_E: return "NT_C_NUM_AND_E";
        case NT_C_NUM_OR_E: return "NT_C_NUM_OR_E";
        case NT_C_NUM_E: return "NT_C_NUM_E";
        case NT_C_STR_BASE_E: return "NT_C_STR_BASE_E";
        case NT_C_STR_ADD_E: return "NT_C_STR_ADD_E";
        case NT_C_STR_CMP_E: return "NT_C_STR_CMP_E";
        case NT_C_STR_AND_E: return "NT_C_STR_AND_E";
        case NT_C_STR_OR_E: return "NT_C_STR_OR_E";
        case NT_C_STR_E: return "NT_C_STR_E";
        case NT_C_BOOL_BASE_E: return "NT_C_BOOL_BASE_E";
        case NT_C_BOOL_NOT_E: return "NT_C_BOOL_NOT_E";
        case NT_C_BOOL_AND_E: return "NT_C_BOOL_AND_E";
        case NT_C_BOOL_OR_E: return "NT_C_BOOL_OR_E";
        case NT_C_BOOL_E: return "NT_C_BOOL_E";
        case NT_C_PTR_E: return "NT_C_PTR_E";
        case NT_CONST_EXPR: return "NT_CONST_EXPR";
        case NT_C_EXPR_LIST: return "NT_C_EXPR_LIST";
        case NT_ACCESS_OP: return "NT_ACCESS_OP";
        case NT_TERMINAL_150: return "NT_TERMINAL_150";
        case NT_TERMINAL_LBRACK: return "NT_TERMINAL_LBRACK";
        case NT_TERMINAL_RBRACK: return "NT_TERMINAL_RBRACK";
        case NT_IDENT_ACCESS: return "NT_IDENT_ACCESS";
        case NT_VALOF_EXPR: return "NT_VALOF_EXPR";
        case NT_TERMINAL_VALOF: return "NT_TERMINAL_VALOF";
        case NT_NUM_BASE_E: return "NT_NUM_BASE_E";
        case NT_NUM_NEG_E: return "NT_NUM_NEG_E";
        case NT_NUM_MULT_E: return "NT_NUM_MULT_E";
        case NT_NUM_ADD_E: return "NT_NUM_ADD_E";
        case NT_NUM_SHF_E: return "NT_NUM_SHF_E";
        case NT_NUM_CMP_E: return "NT_NUM_CMP_E";
        case NT_NUM_AND_E: return "NT_NUM_AND_E";
        case NT_NUM_OR_E: return "NT_NUM_OR_E";
        case NT_NUM_E: return "NT_NUM_E";
        case NT_STR_BASE_E: return "NT_STR_BASE_E";
        case NT_STR_ADD_E: return "NT_STR_ADD_E";
        case NT_STR_CMP_E: return "NT_STR_CMP_E";
        case NT_STR_AND_E: return "NT_STR_AND_E";
        case NT_STR_OR_E: return "NT_STR_OR_E";
        case NT_STR_E: return "NT_STR_E";
        case NT_BOOL_BASE_E: return "NT_BOOL_BASE_E";
        case NT_BOOL_NOT_E: return "NT_BOOL_NOT_E";
        case NT_BOOL_AND_E: return "NT_BOOL_AND_E";
        case NT_BOOL_OR_E: return "NT_BOOL_OR_E";
        case NT_BOOL_E: return "NT_BOOL_E";
        case NT_ARRAY_INST: return "NT_ARRAY_INST";
        case NT_TERMINAL_DYNAMIC: return "NT_TERMINAL_DYNAMIC";
        case NT_TERMINAL_ASSOC: return "NT_TERMINAL_ASSOC";
        case NT_OBJECT_INST: return "NT_OBJECT_INST";
        case NT_PTR_E: return "NT_PTR_E";
        case NT_TERMINAL_NEW: return "NT_TERMINAL_NEW";
        case NT_EXPR: return "NT_EXPR";
        case NT_EXPR_LIST: return "NT_EXPR_LIST";
        case NT_CALL_ARG: return "NT_CALL_ARG";
        case NT_TERMINAL_253: return "NT_TERMINAL_253";
        case NT_TERMINAL_DEFAULT: return "NT_TERMINAL_DEFAULT";
        case NT_CALL_ARG_LIST: return "NT_CALL_ARG_LIST";
        case NT_BLOCK: return "NT_BLOCK";
        case NT_TERMINAL_LBRACE: return "NT_TERMINAL_LBRACE";
        case NT_TERMINAL_RBRACE: return "NT_TERMINAL_RBRACE";
        case NT_OPTIONAL_BLOCK: return "NT_OPTIONAL_BLOCK";
        case NT_TERMINAL_SEMIC: return "NT_TERMINAL_SEMIC";
        case NT_ARGUMENT_DECL: return "NT_ARGUMENT_DECL";
        case NT_TERMINAL_COLON: return "NT_TERMINAL_COLON";
        case NT_TERMINAL_ELLIPSIS: return "NT_TERMINAL_ELLIPSIS";
        case NT_ARGUMENT_LIST: return "NT_ARGUMENT_LIST";
        case NT_ARG_DECLARATOR: return "NT_ARG_DECLARATOR";
        case NT_RETURN_DECL: return "NT_RETURN_DECL";
        case NT_TERMINAL_RETURNING: return "NT_TERMINAL_RETURNING";
        case NT_IDENT_LIST: return "NT_IDENT_LIST";
        case NT_EXTENDS_CLAUSE: return "NT_EXTENDS_CLAUSE";
        case NT_TERMINAL_EXTENDS: return "NT_TERMINAL_EXTENDS";
        case NT_IMPL_CLAUSE: return "NT_IMPL_CLAUSE";
        case NT_TERMINAL_IMPLEMENTS: return "NT_TERMINAL_IMPLEMENTS";
        case NT_VISIBILITY: return "NT_VISIBILITY";
        case NT_TERMINAL_PRIVATE: return "NT_TERMINAL_PRIVATE";
        case NT_TERMINAL_PROTECTED: return "NT_TERMINAL_PROTECTED";
        case NT_TERMINAL_PUBLIC: return "NT_TERMINAL_PUBLIC";
        case NT_ACCESS_TYPE: return "NT_ACCESS_TYPE";
        case NT_TERMINAL_READONLY: return "NT_TERMINAL_READONLY";
        case NT_TERMINAL_READWRITE: return "NT_TERMINAL_READWRITE";
        case NT_TERMINAL_WRITEONLY: return "NT_TERMINAL_WRITEONLY";
        case NT_STORAGE_TYPE: return "NT_STORAGE_TYPE";
        case NT_TERMINAL_STATIC: return "NT_TERMINAL_STATIC";
        case NT_DERIVATION: return "NT_DERIVATION";
        case NT_TERMINAL_OVERRIDE: return "NT_TERMINAL_OVERRIDE";
        case NT_TERMINAL_FINAL: return "NT_TERMINAL_FINAL";
        case NT_IMPLEMENTATION: return "NT_IMPLEMENTATION";
        case NT_TERMINAL_IMPLEMENTATION: return "NT_TERMINAL_IMPLEMENTATION";
        case NT_ATTRIB_FLAG: return "NT_ATTRIB_FLAG";
        case NT_ATTRIB_FLAGS: return "NT_ATTRIB_FLAGS";
        case NT_ATTRIB_DECL: return "NT_ATTRIB_DECL";
        case NT_TERMINAL_ATTRIBUTE: return "NT_TERMINAL_ATTRIBUTE";
        case NT_METHOD_FLAG: return "NT_METHOD_FLAG";
        case NT_METHOD_FLAGS: return "NT_METHOD_FLAGS";
        case NT_METHOD_DECL: return "NT_METHOD_DECL";
        case NT_TERMINAL_METHOD: return "NT_TERMINAL_METHOD";
        case NT_FIELD: return "NT_FIELD";
        case NT_FIELD_LIST: return "NT_FIELD_LIST";
        case NT_CLASS_DECL: return "NT_CLASS_DECL";
        case NT_TERMINAL_CLASS: return "NT_TERMINAL_CLASS";
        case NT_IATTRIB_DECL: return "NT_IATTRIB_DECL";
        case NT_IMETHOD_DECL: return "NT_IMETHOD_DECL";
        case NT_IFIELD: return "NT_IFIELD";
        case NT_IFIELD_LIST: return "NT_IFIELD_LIST";
        case NT_INTERFACE_DECL: return "NT_INTERFACE_DECL";
        case NT_TERMINAL_INTERFACE: return "NT_TERMINAL_INTERFACE";
        case NT_FUNC_DECL: return "NT_FUNC_DECL";
        case NT_TERMINAL_FUNCTION: return "NT_TERMINAL_FUNCTION";
        case NT_PROC_DECL: return "NT_PROC_DECL";
        case NT_TERMINAL_PROCEDURE: return "NT_TERMINAL_PROCEDURE";
        case NT_VAR_DECL: return "NT_VAR_DECL";
        case NT_TERMINAL_VAR: return "NT_TERMINAL_VAR";
        case NT_CONST_DECL: return "NT_CONST_DECL";
        case NT_TERMINAL_CONST: return "NT_TERMINAL_CONST";
        case NT_ENUM_ITEM: return "NT_ENUM_ITEM";
        case NT_ENUM_ITEM_LIST: return "NT_ENUM_ITEM_LIST";
        case NT_ENUM_DECL: return "NT_ENUM_DECL";
        case NT_TERMINAL_ENUM: return "NT_TERMINAL_ENUM";
        case NT_LET_STMT: return "NT_LET_STMT";
        case NT_TERMINAL_LET: return "NT_TERMINAL_LET";
        case NT_DECL_STMT: return "NT_DECL_STMT";
        case NT_BLOCK_DECL: return "NT_BLOCK_DECL";
        case NT_IMPORT_PATH: return "NT_IMPORT_PATH";
        case NT_IMPORT_LIST: return "NT_IMPORT_LIST";
        case NT_IMPORT_STMT: return "NT_IMPORT_STMT";
        case NT_TERMINAL_IMPORT: return "NT_TERMINAL_IMPORT";
        case NT_XREF_TYPE: return "NT_XREF_TYPE";
        case NT_TERMINAL_MODULE: return "NT_TERMINAL_MODULE";
        case NT_TERMINAL_PACKAGE: return "NT_TERMINAL_PACKAGE";
        case NT_TERMINAL_LIBRARY: return "NT_TERMINAL_LIBRARY";
        case NT_STR_LIT_LIST: return "NT_STR_LIT_LIST";
        case NT_PLATFORM_SPEC: return "NT_PLATFORM_SPEC";
        case NT_TERMINAL_PLATFORM: return "NT_TERMINAL_PLATFORM";
        case NT_LANGUAGE_SPEC: return "NT_LANGUAGE_SPEC";
        case NT_TERMINAL_LANGUAGE: return "NT_TERMINAL_LANGUAGE";
        case NT_HEADER_SPEC: return "NT_HEADER_SPEC";
        case NT_TERMINAL_HEADER: return "NT_TERMINAL_HEADER";
        case NT_FUNCTION_SPEC: return "NT_FUNCTION_SPEC";
        case NT_PROCEDURE_SPEC: return "NT_PROCEDURE_SPEC";
        case NT_CLASS_SPEC: return "NT_CLASS_SPEC";
        case NT_ALIEN_SPEC: return "NT_ALIEN_SPEC";
        case NT_ALIEN_SPECS: return "NT_ALIEN_SPECS";
        case NT_ALIEN_STMT: return "NT_ALIEN_STMT";
        case NT_TERMINAL_ALIEN: return "NT_TERMINAL_ALIEN";
        case NT_GLOBAL_STMT: return "NT_GLOBAL_STMT";
        case NT_GLOBAL_STMTS: return "NT_GLOBAL_STMTS";
        case NT_GLOBAL_INTRO: return "NT_GLOBAL_INTRO";
        case NT_GLOBAL_SPEC: return "NT_GLOBAL_SPEC";
        case NT_TERMINAL_PROGRAM: return "NT_TERMINAL_PROGRAM";
        case NT_GLOBAL_SCOPE: return "NT_GLOBAL_SCOPE";
        case NT_LABEL_DECL: return "NT_LABEL_DECL";
        case NT_CALL_STMT: return "NT_CALL_STMT";
        case NT_TERMINAL_CALL: return "NT_TERMINAL_CALL";
        case NT_BLOCK_OR_STMT: return "NT_BLOCK_OR_STMT";
        case NT_IF_STMT: return "NT_IF_STMT";
        case NT_TERMINAL_IF: return "NT_TERMINAL_IF";
        case NT_TERMINAL_UNLESS: return "NT_TERMINAL_UNLESS";
        case NT_TERMINAL_ELSE: return "NT_TERMINAL_ELSE";
        case NT_WHILE_STMT: return "NT_WHILE_STMT";
        case NT_TERMINAL_WHILE: return "NT_TERMINAL_WHILE";
        case NT_TERMINAL_UNTIL: return "NT_TERMINAL_UNTIL";
        case NT_REPEAT_STMT: return "NT_REPEAT_STMT";
        case NT_TERMINAL_REPEAT: return "NT_TERMINAL_REPEAT";
        case NT_TERMINAL_DO: return "NT_TERMINAL_DO";
        case NT_LOOP_STMT: return "NT_LOOP_STMT";
        case NT_TERMINAL_LOOP: return "NT_TERMINAL_LOOP";
        case NT_TERMINAL_FOREVER: return "NT_TERMINAL_FOREVER";
        case NT_FOR_STMT: return "NT_FOR_STMT";
        case NT_TERMINAL_FOR: return "NT_TERMINAL_FOR";
        case NT_TERMINAL_TO: return "NT_TERMINAL_TO";
        case NT_TERMINAL_DOWNTO: return "NT_TERMINAL_DOWNTO";
        case NT_LOOP_CONTROL: return "NT_LOOP_CONTROL";
        case NT_TERMINAL_BREAK: return "NT_TERMINAL_BREAK";
        case NT_TERMINAL_CONTINUE: return "NT_TERMINAL_CONTINUE";
        case NT_TERMINAL_AGAIN: return "NT_TERMINAL_AGAIN";
        case NT_LOOP_TYPE_STMT: return "NT_LOOP_TYPE_STMT";
        case NT_SWITCH_CASE: return "NT_SWITCH_CASE";
        case NT_TERMINAL_CASE: return "NT_TERMINAL_CASE";
        case NT_DEFAULT_CASE: return "NT_DEFAULT_CASE";
        case NT_ANY_CASE: return "NT_ANY_CASE";
        case NT_CASE_LIST: return "NT_CASE_LIST";
        case NT_SWITCH_STMT: return "NT_SWITCH_STMT";
        case NT_TERMINAL_SWITCH: return "NT_TERMINAL_SWITCH";
        case NT_GOTO_STMT: return "NT_GOTO_STMT";
        case NT_TERMINAL_GOTO: return "NT_TERMINAL_GOTO";
        case NT_TERMINAL_GOSUB: return "NT_TERMINAL_GOSUB";
        case NT_RETURN_STMT: return "NT_RETURN_STMT";
        case NT_TERMINAL_RETURN: return "NT_TERMINAL_RETURN";
        case NT_RESULTIS_STMT: return "NT_RESULTIS_STMT";
        case NT_TERMINAL_RESULTIS: return "NT_TERMINAL_RESULTIS";
        case NT_DELETE_STMT: return "NT_DELETE_STMT";
        case NT_TERMINAL_DELETE: return "NT_TERMINAL_DELETE";
        case NT_FLOW_CTRL_STMT: return "NT_FLOW_CTRL_STMT";
        case NT_STMT: return "NT_STMT";
        case NT_STATEMENT: return "NT_STATEMENT";
        case NT_STATEMENT_LIST: return "NT_STATEMENT_LIST";
        default: break;
    }
    return "???";
}

