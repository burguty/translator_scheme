#include "scheme.h"
#include "parser.h"
#include "applier.h"
#include <sstream>
#include <vector>

std::string AsString(AST ast) {
    if (ast == nullptr) {
        return "()";
    }
    if (Is<Number>(ast)) {
        auto number = As<Number>(ast)->GetValue();
        return std::to_string(number);
    } else if (Is<Boolean>(ast)) {
        auto boolean = As<Boolean>(ast)->GetValue();
        return boolean ? "#t" : "#f";
    } else if (Is<Quote>(ast)) {
        std::string ans = "(quote ";
        ans += AsString(As<Quote>(ast)->GetCommand());
        ans += ")";
        return ans;
    } else if (Is<Symbol>(ast)) {
        return As<Symbol>(ast)->GetName();
    } else if (Is<Cell>(ast)) {
        std::vector<std::string> all;
        auto operand = ast;
        while (operand) {
            if (!Is<Cell>(operand)) {
                all.push_back(". " + AsString(operand));
                break;
            } else {
                if (As<Cell>(operand)->GetFirst() == nullptr) {
                    all.push_back("()");
                } else {
                    all.push_back(AsString(As<Cell>(operand)->GetFirst()));
                }
                operand = As<Cell>(operand)->GetSecond();
            }
        }
        std::string ans;
        ans = "(";
        for (size_t i = 0; i < all.size(); ++i) {
            if (i > 0) {
                ans += " ";
            }
            ans += all[i];
        }
        ans += ")";
        return ans;
    } else {
        throw RuntimeError("Runtime error: unknown literal");
    }
}

std::string Interpreter::Run(const std::string& expr) {
    std::stringstream ss{expr};
    Tokenizer tokenizer{&ss};

    AST ast = Read(&tokenizer);
    if (!tokenizer.IsEnd()) {
        throw SyntaxError("Syntax error: extra expressions");
    }

    AST result = Applier::Apply(ast);

    return AsString(result);
}
