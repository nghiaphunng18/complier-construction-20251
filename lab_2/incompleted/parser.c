/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdlib.h>

#include "reader.h"
#include "scanner.h"
#include "parser.h"
#include "error.h"

Token *currentToken;
Token *lookAhead;

void scan(void) {
  Token* tmp = currentToken;
  currentToken = lookAhead;
  lookAhead = getValidToken();
  free(tmp);
}

void eat(TokenType tokenType) {
  if (lookAhead->tokenType == tokenType) {
    printToken(lookAhead);
    scan();
  } else missingToken(tokenType, lookAhead->lineNo, lookAhead->colNo);
}

void compileProgram(void) {
  assert("Parsing a Program ....");
  eat(KW_PROGRAM);
  eat(TK_IDENT);
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_PERIOD);
  assert("Program parsed!");
}

void compileBlock(void) {
  assert("Parsing a Block ....");
  if (lookAhead->tokenType == KW_CONST) {
    eat(KW_CONST);
    compileConstDecl();
    compileConstDecls();
    compileBlock2();
  } 
  else compileBlock2();
  assert("Block parsed!");
}

void compileBlock2(void) {
  if (lookAhead->tokenType == KW_TYPE) {
    eat(KW_TYPE);
    compileTypeDecl();
    compileTypeDecls();
    compileBlock3();
  } 
  else compileBlock3();
}

void compileBlock3(void) {
  if (lookAhead->tokenType == KW_VAR) {
    eat(KW_VAR);
    compileVarDecl();
    compileVarDecls();
    compileBlock4();
  } 
  else compileBlock4();
}

void compileBlock4(void) {
  compileSubDecls();
  compileBlock5();
}

void compileBlock5(void) {
  eat(KW_BEGIN);
  compileStatements();
  eat(KW_END);
}

void compileConstDecls(void) {
  // TODO
  if (lookAhead->tokenType == TK_IDENT) {
    compileConstDecl();
    compileConstDecls();
  }
}

void compileConstDecl(void) {
  // TODO: ConstDecl ::= Ident SB_EQUAL Constant SB_SEMICOLON
  eat(TK_IDENT);
  eat(SB_EQ);
  compileConstant();
  eat(SB_SEMICOLON);
}

void compileTypeDecls(void) {
  // TODO: TypeDecls ::= TypeDecl TypeDecls | epsilon
  if (lookAhead->tokenType == TK_IDENT) {
    compileTypeDecl();
    compileTypeDecls();
  }
}

void compileTypeDecl(void) {
  // TODO: TypeDecl ::= Ident SB_EQUAL Type SB_SEMICOLON
  eat(TK_IDENT);
  eat(SB_EQ);
  compileType();
  eat(SB_SEMICOLON);
}

void compileVarDecls(void) {
  // TODO: VarDecls ::= VarDecl VarDecls | epsilon
  if (lookAhead->tokenType == TK_IDENT) {
    compileVarDecl();
    compileVarDecls();
  }
}

void compileVarDecl(void) {
  // TODO: VarDecl ::= Ident SB_COLON Type SB_SEMICOLON
  eat(TK_IDENT);
  eat(SB_COLON);
  compileType();
  eat(SB_SEMICOLON);
}

void compileSubDecls(void) {
  assert("Parsing subtoutines ....");
  // TODO: SubDecls ::= FunDecl SubDecls | ProcDecl SubDecls | epsilon
  if (lookAhead->tokenType == KW_FUNCTION) {
    compileFuncDecl();
    compileSubDecls();
  } else if (lookAhead->tokenType == KW_PROCEDURE) {
    compileProcDecl();
    compileSubDecls();
  }
  assert("Subtoutines parsed ....");
}

void compileFuncDecl(void) {
  assert("Parsing a function ....");
  // TODO: FunDecl ::= KW_FUNCTION Ident Params SB_COLON BasicType SB_SEMICOLON Block SB_SEMICOLON
  eat(KW_FUNCTION);
  eat(TK_IDENT);
  compileParams();
  eat(SB_COLON);
  compileBasicType();
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_SEMICOLON);
  assert("Function parsed ....");
}

void compileProcDecl(void) {
  assert("Parsing a procedure ....");
  // TODO: ProcDecl ::= KW_PROCEDURE Ident Params SB_SEMICOLON Block SB_SEMICOLON
  eat(KW_PROCEDURE);
  eat(TK_IDENT);
  compileParams();
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_SEMICOLON);
  assert("Procedure parsed ....");
}

void compileUnsignedConstant(void) {
  // TODO: UnsignedConstant ::= Number | ConstIdent | ConstChar
  switch (lookAhead->tokenType) {
  case TK_NUMBER:
    eat(TK_NUMBER);
    break;
  case TK_IDENT:
    eat(TK_IDENT);
    break;
  case TK_CHAR:
    eat(TK_CHAR);
    break;
  default:
    error(ERR_INVALIDCONSTANT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

void compileConstant(void) {
  // TODO: Constant ::= SB_PLUS Constant2 | SB_MINUS Constant2 | Constant2 | ConstChar
  switch (lookAhead->tokenType) {
  case SB_PLUS:
    eat(SB_PLUS);
    compileConstant2();
    break;
  case SB_MINUS:
    eat(SB_MINUS);
    compileConstant2();
    break;
  case TK_CHAR:
    eat(TK_CHAR);
    break;
  default:
    compileConstant2();
    break;
  }
}

void compileConstant2(void) {
  // TODO: Constant2 ::= ConstIdent | Number
  switch (lookAhead->tokenType) {
  case TK_IDENT:
    eat(TK_IDENT);
    break;
  case TK_NUMBER:
    eat(TK_NUMBER);
    break;
  default:
    error(ERR_INVALIDCONSTANT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

void compileType(void) {
  // TODO: Type ::= KW_INTEGER | KW_CHAR | TypeIdent | KW_ARRAY ...
  switch (lookAhead->tokenType) {
  case KW_INTEGER:
    eat(KW_INTEGER);
    break;
  case KW_CHAR:
    eat(KW_CHAR);
    break;
  case TK_IDENT:
    eat(TK_IDENT);
    break;
  case KW_ARRAY:
    eat(KW_ARRAY);
    eat(SB_LSEL); // [.
    eat(TK_NUMBER);
    eat(SB_RSEL); // .]
    eat(KW_OF);
    compileType();
    break;
  default:
    error(ERR_INVALIDTYPE, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

void compileBasicType(void) {
  // TODO: BasicType ::= KW_INTEGER | KW_CHAR
  if (lookAhead->tokenType == KW_INTEGER) {
    eat(KW_INTEGER);
  } else if (lookAhead->tokenType == KW_CHAR) {
    eat(KW_CHAR);
  } else {
    error(ERR_INVALIDBASICTYPE, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileParams(void) {
  // TODO: Params ::= SB_LPAR Param Params2 SB_RPAR | epsilon
  if (lookAhead->tokenType == SB_LPAR) {
    eat(SB_LPAR);
    compileParam();
    compileParams2();
    eat(SB_RPAR);
  }
}

void compileParams2(void) {
  // TODO: Params2 ::= SB_SEMICOLON Param Params2 | epsilon
  if (lookAhead->tokenType == SB_SEMICOLON) {
    eat(SB_SEMICOLON);
    compileParam();
    compileParams2();
  }
}

void compileParam(void) {
  // TODO: Param ::= Ident SB_COLON BasicType | KW_VAR Ident SB_COLON BasicType
  if (lookAhead->tokenType == TK_IDENT) {
    eat(TK_IDENT);
    eat(SB_COLON);
    compileBasicType();
  } else if (lookAhead->tokenType == KW_VAR) {
    eat(KW_VAR);
    eat(TK_IDENT);
    eat(SB_COLON);
    compileBasicType();
  } else {
    error(ERR_INVALIDPARAM, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileStatements(void) {
  // TODO: Statements ::= Statement Statements2
  compileStatement();
  compileStatements2();
}

void compileStatements2(void) {
  // TODO: Statements2 ::= SB_SEMICOLON Statement Statements2 | epsilon
  if (lookAhead->tokenType == SB_SEMICOLON) {
    eat(SB_SEMICOLON);
    compileStatement();
    compileStatements2();
  } else if (lookAhead->tokenType == TK_IDENT || 
             lookAhead->tokenType == KW_CALL ||
             lookAhead->tokenType == KW_IF ||
             lookAhead->tokenType == KW_WHILE ||
             lookAhead->tokenType == KW_FOR ||
             lookAhead->tokenType == KW_BEGIN) {
    missingToken(SB_SEMICOLON, lookAhead->lineNo, lookAhead->colNo);
    compileStatement(); 
    compileStatements2();
  }
}

void compileStatement(void) {
  switch (lookAhead->tokenType) {
  case TK_IDENT:
    compileAssignSt();
    break;
  case KW_CALL:
    compileCallSt();
    break;
  case KW_BEGIN:
    compileGroupSt();
    break;
  case KW_IF:
    compileIfSt();
    break;
  case KW_WHILE:
    compileWhileSt();
    break;
  case KW_FOR:
    compileForSt();
    break;
    // EmptySt needs to check FOLLOW tokens
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
    break;
    // Error occurs
  default:
    error(ERR_INVALIDSTATEMENT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

void compileAssignSt(void) {
  assert("Parsing an assign statement ....");
  // TODO AssignSt ::= Ident Indexes SB_ASSIGN Expression
  eat(TK_IDENT);
  compileIndexes();
  eat(SB_ASSIGN);
  compileExpression();
  assert("Assign statement parsed ....");
}

void compileCallSt(void) {
  assert("Parsing a call statement ....");
  // TODO CallSt ::= KW_CALL Ident Arguments
  eat(KW_CALL);
  eat(TK_IDENT);
  compileArguments();
  assert("Call statement parsed ....");
}

void compileGroupSt(void) {
  assert("Parsing a group statement ....");
  // TODO GroupSt ::= KW_BEGIN Statements KW_END
  eat(KW_BEGIN);
  compileStatements();
  eat(KW_END);
  assert("Group statement parsed ....");
}

void compileIfSt(void) {
  assert("Parsing an if statement ....");
  eat(KW_IF);
  compileCondition();
  eat(KW_THEN);
  compileStatement();
  if (lookAhead->tokenType == KW_ELSE) 
    compileElseSt();
  assert("If statement parsed ....");
}

void compileElseSt(void) {
  eat(KW_ELSE);
  compileStatement();
}

void compileWhileSt(void) {
  assert("Parsing a while statement ....");
  // TODO WhileSt ::= KW_WHILE Condition KW_DO Statement
  eat(KW_WHILE);
  compileCondition();
  eat(KW_DO);
  compileStatement();
  assert("While statement parsed ....");
}

void compileForSt(void) {
  assert("Parsing a for statement ....");
  // TODO ForSt ::= KW_FOR Ident SB_ASSIGN Expression KW_TO Expression KW_DO Statement
  eat(KW_FOR);
  eat(TK_IDENT);
  eat(SB_ASSIGN);
  compileExpression();
  eat(KW_TO);
  compileExpression();
  eat(KW_DO);
  compileStatement();
  assert("For statement parsed ....");
}

void compileArguments(void) {
  // TODO Arguments ::= SB_LPAR Expression Arguments2 SB_RPAR | epsilon
  if (lookAhead->tokenType == SB_LPAR) {
    eat(SB_LPAR);
    compileExpression();
    compileArguments2();
    eat(SB_RPAR);
  }
}

void compileArguments2(void) {
  // TODO Arguments2 ::= SB_COMMA Expression Arguments2 | epsilon
  if (lookAhead->tokenType == SB_COMMA) {
    eat(SB_COMMA);
    compileExpression();
    compileArguments2();
  }
}

void compileCondition(void) {
  // TODO Condition ::= Expression Comparator Expression
  compileExpression();
  switch (lookAhead->tokenType) {
  case SB_EQ:
  case SB_NEQ:
  case SB_LT:
  case SB_LE:
  case SB_GT:
  case SB_GE:
    eat(lookAhead->tokenType);
    break;
  default:
    error(ERR_INVALIDCOMPARATOR, lookAhead->lineNo, lookAhead->colNo);
  }
  compileExpression();
  compileCondition2();
}

void compileCondition2(void) {
  // TODO
}

void compileExpression(void) {
  assert("Parsing an expression");
  // TODO Expression ::= SB_PLUS Term Expression2 | SB_MINUS Term Expression2 | Term Expression2
  if (lookAhead->tokenType == SB_PLUS || lookAhead->tokenType == SB_MINUS) {
    eat(lookAhead->tokenType);
  }
  compileTerm();
  compileExpression2();
  assert("Expression parsed");
}

void compileExpression2(void) {
  // TODO Expression2 ::= (SB_PLUS | SB_MINUS) Term Expression2 | epsilon
  if (lookAhead->tokenType == SB_PLUS || lookAhead->tokenType == SB_MINUS) {
    eat(lookAhead->tokenType);
    compileTerm();
    compileExpression2();
  }
}


void compileExpression3(void) {
  // TODO 
  compileExpression();
}

void compileTerm(void) {
  // TODO Term ::= Factor Term2
  compileFactor();
  compileTerm2();
}

void compileTerm2(void) {
  // TODO
  if (lookAhead->tokenType == SB_TIMES || lookAhead->tokenType == SB_SLASH) {
    eat(lookAhead->tokenType);
    compileFactor();
    compileTerm2();
  }
}

void compileFactor(void) {
  // TODO Factor ::= UnsignedConstant | Ident Indexes | Ident Arguments | SB_LPAR Expression SB_RPAR
  switch (lookAhead->tokenType) {
  case TK_NUMBER:
  case TK_CHAR:
    compileUnsignedConstant();
    break;
  case TK_IDENT:
    eat(TK_IDENT);
    switch (lookAhead->tokenType) {
    case SB_LPAR: // Function Call
      compileArguments();
      break;
    case SB_LSEL: // Array Access
      compileIndexes();
      break;
      // else: Simple variable or constant
    default:
      break;
    }
    break;
  case SB_LPAR:
    eat(SB_LPAR);
    compileExpression();
    eat(SB_RPAR);
    break;
  default:
    error(ERR_INVALIDFACTOR, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

void compileIndexes(void) {
  // TODO Indexes ::= SB_LSEL Expression SB_RSEL Indexes | epsilon
  if (lookAhead->tokenType == SB_LSEL) {
    eat(SB_LSEL); // (.
    compileExpression();
    eat(SB_RSEL); // .)
    compileIndexes();
  }
}

int compile(char *fileName) {
  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  currentToken = NULL;
  lookAhead = getValidToken();

  compileProgram();

  free(currentToken);
  free(lookAhead);
  closeInputStream();
  return IO_SUCCESS;

}
