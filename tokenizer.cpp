#include "tokenizer.h"
#include "error.h"
#include "lexeme_types.cpp"

#include <sstream>

bool SymbolToken::operator==(const SymbolToken& other) const {
    return name == other.name;
}

bool QuoteToken::operator==(const QuoteToken&) const {
    return true;
}

bool DotToken::operator==(const DotToken&) const {
    return true;
}

bool ConstantToken::operator==(const ConstantToken& other) const {
    return value == other.value;
}

bool BooleanToken::operator==(const BooleanToken& other) const {
    return value == other.value;
}

Tokenizer::Tokenizer(std::istream* is) : is_(is) {
    Next();
}

bool Tokenizer::IsEnd() {
    char sym;

    auto get_symbol = [&]() -> bool {
        sym = is_->get();
        return sym != EOF;
    };

    auto peek_symbol = [&]() -> bool {
        sym = is_->peek();
        return sym != EOF;
    };

    while (peek_symbol()) {
        if (LexemeTypes::IsSkip(sym)) {
            get_symbol();
            continue;
        }
        break;
    }

    return !current_token_.has_value();
}

void Tokenizer::Next() {
    current_token_.reset();

    char sym;

    auto get_symbol = [&]() -> bool {
        sym = is_->get();
        return sym != EOF;
    };

    auto peek_symbol = [&]() -> bool {
        sym = is_->peek();
        return sym != EOF;
    };

    while (get_symbol()) {
        if (LexemeTypes::IsSkip(sym)) {
            continue;
        }
        if (LexemeTypes::IsPlus(sym) || LexemeTypes::IsMinus(sym)) {
            std::string str;
            str += sym;
            while (peek_symbol() && LexemeTypes::IsDigit(sym)) {
                get_symbol();
                str += sym;
            }
            if (str.size() == 1) {
                current_token_ = Token{SymbolToken{str}};
            } else {
                current_token_ = Token{ConstantToken{std::stoi(str)}};
            }
            break;
        }
        if (LexemeTypes::IsDigit(sym)) {
            std::string str;
            str += sym;
            while (peek_symbol() && LexemeTypes::IsDigit(sym)) {
                get_symbol();
                str += sym;
            }
            current_token_ = Token{ConstantToken{std::stoi(str)}};
            break;
        }
        if (LexemeTypes::IsStartSymbol(sym)) {
            std::string str;
            str += sym;
            while (peek_symbol() && LexemeTypes::IsInnerSymbol(sym)) {
                get_symbol();
                str += sym;
            }
            if (str == "#t") {
                current_token_ = Token{BooleanToken{true}};
            } else if (str == "#f") {
                current_token_ = Token{BooleanToken{false}};
            } else {
                current_token_ = Token{SymbolToken{str}};
            }
            return;
        }
        if (LexemeTypes::IsQuote(sym)) {
            current_token_ = Token{QuoteToken{}};
            return;
        }
        if (LexemeTypes::IsBracket(sym)) {
            current_token_ = (sym == '(') ? Token{BracketToken::OPEN} : Token{BracketToken::CLOSE};
            return;
        }
        if (LexemeTypes::IsDot(sym)) {
            current_token_ = Token{DotToken{}};
            return;
        }
    }
}

Token Tokenizer::GetToken() {
    if (!current_token_.has_value()) {
        throw SyntaxError("Syntax error: token not found");
    }
    return current_token_.value();
}

std::vector<Token> Read(const std::string& stream) {
    std::stringstream is(stream);
    Tokenizer tokenizer(&is);

    std::vector<Token> output;
    while (!tokenizer.IsEnd()) {
        output.push_back(tokenizer.GetToken());
        tokenizer.Next();
    }
    return output;
}
