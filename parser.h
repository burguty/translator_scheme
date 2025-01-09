#pragma once

#include "error.h"
#include "object.h"

using AST = std::shared_ptr<Object>;

AST Read(Tokenizer* tokenizer);
