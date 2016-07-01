#pragma once

#include "types.h"

typedef struct newLLType
{  
  int first_line;  
  int first_column;  
  int last_line;  
  int last_column;  
  QString line;
} newLLType;

#define YYLTYPE newLLType
#define YYLTYPE_IS_DECLARED 1

