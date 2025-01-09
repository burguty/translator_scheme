#include "parser.h"

AST ReadList(Tokenizer* tokenizer);

AST Read(Tokenizer* tokenizer) {
    Token token = tokenizer->GetToken();
    if (std::get_if<QuoteToken>(&token)) {
        tokenizer->Next();
        return std::make_shared<Quote>(Read(tokenizer));
    } else if (auto bracket_token = std::get_if<BracketToken>(&token)) {
        if (*bracket_token == BracketToken::OPEN) {
            tokenizer->Next();
            return ReadList(tokenizer);
        } else {
            throw SyntaxError("Syntax error: got: ')' , expected: '(' ");
        }
    } else if (auto constant_token = std::get_if<ConstantToken>(&token)) {
        tokenizer->Next();
        return std::make_shared<Number>(constant_token->value);
    } else if (auto boolean_token = std::get_if<BooleanToken>(&token)) {
        tokenizer->Next();
        return std::make_shared<Boolean>(boolean_token->value);
    } else if (auto symbol_token = std::get_if<SymbolToken>(&token)) {
        if (symbol_token->name == "quote") {
            throw SyntaxError("Syntax error: incorrect form 'quote'");
        } else {
            tokenizer->Next();
            return std::make_shared<Symbol>(symbol_token->name);
        }
    } else {
        throw SyntaxError("Syntax error: unexpected token in expression");
    }
}

AST ReadList(Tokenizer* tokenizer) {

    Token token = tokenizer->GetToken();
    if (auto bracket_token = std::get_if<BracketToken>(&token)) {
        if (*bracket_token == BracketToken::CLOSE) {
            tokenizer->Next();
            return nullptr;
        }
    }

    if (auto symbol_token = std::get_if<SymbolToken>(&token)) {
        if (symbol_token->name == "quote") {
            tokenizer->Next();
            auto output = std::make_shared<Quote>(Read(tokenizer));
            token = tokenizer->GetToken();
            if (auto bracket_token = std::get_if<BracketToken>(&token)) {
                if (*bracket_token == BracketToken::CLOSE) {
                    tokenizer->Next();
                    return output;
                } else {
                    throw SyntaxError("Syntax error: expected ')' in form 'quote'");
                }
            } else {
                throw SyntaxError("Syntax error: expected ')' in form 'quote'");
            }
        }
    }

    std::shared_ptr<Cell> output = std::make_shared<Cell>();
    std::shared_ptr<Cell> last = output;
    for (token = tokenizer->GetToken();; token = tokenizer->GetToken()) {

        if (std::get_if<DotToken>(&token)) {
            throw SyntaxError("Syntax error: first element of pair is skipped");
        }

        last->first_ = Read(tokenizer);
        token = tokenizer->GetToken();
        if (std::get_if<DotToken>(&token)) {
            tokenizer->Next();
            last->second_ = Read(tokenizer);
            token = tokenizer->GetToken();
            if (auto bracket_token = std::get_if<BracketToken>(&token)) {
                if (*bracket_token == BracketToken::CLOSE) {
                    tokenizer->Next();
                    break;
                } else {
                    throw SyntaxError("Syntax error: expected ')', got '('");
                }
            } else {
                throw SyntaxError("Syntax error: expected ')'");
            }
        } else if (auto bracket_token = std::get_if<BracketToken>(&token)) {
            if (*bracket_token == BracketToken::CLOSE) {
                last->second_ = nullptr;
                tokenizer->Next();
                break;
            } else {
                last->second_ = std::make_shared<Cell>();
                last = As<Cell>(last->second_);
            }
        } else {
            last->second_ = std::make_shared<Cell>();
            last = As<Cell>(last->second_);
        }
    }
    return output;
}
