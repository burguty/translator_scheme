#include "applier.h"

static constexpr int64_t kMaxValue = 1e18;
static constexpr int64_t kMinValue = -1e18;

std::unordered_map<std::string, Functor> Applier::functors = {
    {"number?", Applier::IntegerOperations::OpIsNumber},
    {"+", Applier::IntegerOperations::OpPlus},
    {"-", Applier::IntegerOperations::OpMinus},
    {"*", Applier::IntegerOperations::OpMultiply},
    {"/", Applier::IntegerOperations::OpDivide},
    {"max", Applier::IntegerOperations::OpMax},
    {"min", Applier::IntegerOperations::OpMin},
    {"abs", Applier::IntegerOperations::OpAbs},
    {"=", Applier::IntegerOperations::OpEqual},
    {"<", Applier::IntegerOperations::OpLess},
    {">", Applier::IntegerOperations::OpGreater},
    {"<=", Applier::IntegerOperations::OpLessEqual},
    {">=", Applier::IntegerOperations::OpGreaterEqual},
    {"boolean?", Applier::BooleanOperations::OpIsBoolean},
    {"not", Applier::BooleanOperations::OpNot},
    {"and", Applier::BooleanOperations::OpAnd},
    {"or", Applier::BooleanOperations::OpOr},
    {"list?", Applier::ListOperations::OpIsList},
    {"null?", Applier::ListOperations::OpIsNull},
    {"pair?", Applier::ListOperations::OpIsPair},
    {"cons", Applier::ListOperations::OpCons},
    {"car", Applier::ListOperations::OpCar},
    {"cdr", Applier::ListOperations::OpCdr},
    {"list", Applier::ListOperations::OpList},
    {"list-ref", Applier::ListOperations::OpListRef},
    {"list-tail", Applier::ListOperations::OpListTail}};

AST Applier::Apply(AST ast) {
    if (ast == nullptr) {
        throw RuntimeError("Runtime error: empty command");
    }
    if (Is<Number>(ast)) {
        return ast;
    } else if (Is<Boolean>(ast)) {
        return ast;
    } else if (Is<Quote>(ast)) {
        return QuoteOperations::OpQuote(As<Quote>(ast));
    } else if (Is<Symbol>(ast)) {
        return ast;
    } else if (Is<Cell>(ast)) {
        auto cell_ast = As<Cell>(ast);
        auto operation_ast = Apply(cell_ast->GetFirst());
        if (!Is<Symbol>(operation_ast)) {
            throw RuntimeError("Runtime Error: incorrect operation");
        }
        std::string operation = As<Symbol>(operation_ast)->GetName();
        if (!functors.contains(operation)) {
            throw RuntimeError("Runtime error: unknown command");
        }
        return functors[operation](cell_ast);
    } else {
        throw RuntimeError("Runtime error: unknown command");
    }
}

Functor Applier::GetFunctor(const std::string& arg) {
    if (!functors.contains(arg)) {
        throw RuntimeError("Runtime error: unknown command");
    }
    return functors[arg];
}

// Quote
AST Applier::QuoteOperations::OpQuote(std::shared_ptr<Quote> ast) {
    return ast->GetCommand();
}

// Integer
AST Applier::IntegerOperations::OpIsNumber(std::shared_ptr<Cell> ast) {
    bool ans = false;
    AST operand = ast->GetSecond();
    if (!operand) {
        throw RuntimeError("Runtime error: number? expects operands");
    }
    if (!Is<Cell>(operand)) {
        throw RuntimeError("Runtime error: expected expression in number?");
    }
    if (As<Cell>(operand)->GetSecond() != nullptr) {
        throw RuntimeError("Runtime error: unary number? expected one argument");
    }
    AST value_ast = Apply(As<Cell>(operand)->GetFirst());
    ans = Is<Number>(value_ast);
    return std::make_shared<Boolean>(ans);
}

AST Applier::IntegerOperations::OpPlus(std::shared_ptr<Cell> ast) {
    int64_t sum = 0;
    AST operand = ast->GetSecond();
    while (operand) {
        if (!Is<Cell>(operand)) {
            throw RuntimeError("Runtime error: expected expression in +");
        }
        AST value_ast = Apply(As<Cell>(operand)->GetFirst());
        if (!Is<Number>(value_ast)) {
            throw RuntimeError("Runtime error: expected number in +");
        }
        sum += As<Number>(value_ast)->GetValue();
        operand = As<Cell>(operand)->GetSecond();
    }
    return std::make_shared<Number>(sum);
}

AST Applier::IntegerOperations::OpMinus(std::shared_ptr<Cell> ast) {
    int64_t sum = 0;
    AST operand = ast->GetSecond();
    if (!operand) {
        throw RuntimeError("Runtime error: substraction expects operands");
    }
    if (!Is<Cell>(operand)) {
        throw RuntimeError("Runtime error: expected expression in -");
    }
    AST value_ast = Apply(As<Cell>(operand)->GetFirst());
    if (!Is<Number>(value_ast)) {
        throw RuntimeError("Runtime error: expected number in -");
    }
    sum = As<Number>(value_ast)->GetValue();
    operand = As<Cell>(operand)->GetSecond();
    while (operand) {
        if (!Is<Cell>(operand)) {
            throw RuntimeError("Runtime error: expected expression in -");
        }
        AST value_ast = Apply(As<Cell>(operand)->GetFirst());
        if (!Is<Number>(value_ast)) {
            throw RuntimeError("Runtime error: expected number in -");
        }
        sum -= As<Number>(value_ast)->GetValue();
        operand = As<Cell>(operand)->GetSecond();
    }
    return std::make_shared<Number>(sum);
}

AST Applier::IntegerOperations::OpMultiply(std::shared_ptr<Cell> ast) {
    int64_t mult = 1;
    std::shared_ptr<Object> operand = ast->GetSecond();
    while (operand) {
        if (!Is<Cell>(operand)) {
            throw RuntimeError("Runtime error: expected expression in *");
        }
        AST value_ast = Apply(As<Cell>(operand)->GetFirst());
        if (!Is<Number>(value_ast)) {
            throw RuntimeError("Runtime error: expected number in *");
        }
        mult *= As<Number>(value_ast)->GetValue();
        operand = As<Cell>(operand)->GetSecond();
    }
    return std::make_shared<Number>(mult);
}

AST Applier::IntegerOperations::OpDivide(std::shared_ptr<Cell> ast) {
    int64_t div = 0;
    AST operand = ast->GetSecond();
    if (!operand) {
        throw RuntimeError("Runtime error: divide expects operands");
    }
    if (!Is<Cell>(operand)) {
        throw RuntimeError("Runtime error: expected expression in /");
    }
    AST value_ast = Apply(As<Cell>(operand)->GetFirst());
    if (!Is<Number>(value_ast)) {
        throw RuntimeError("Runtime error: expected number in /");
    }
    div = As<Number>(value_ast)->GetValue();
    operand = As<Cell>(operand)->GetSecond();

    if (!operand) {
        throw RuntimeError("Runtime error: divide expects operands");
    }
    if (!Is<Cell>(operand)) {
        throw RuntimeError("Runtime error: expected expression in /");
    }
    value_ast = Apply(As<Cell>(operand)->GetFirst());
    if (!Is<Number>(value_ast)) {
        throw RuntimeError("Runtime error: expected number in /");
    }
    if (As<Number>(value_ast)->GetValue() == 0) {
        throw RuntimeError("Runtime error: catched 0 in /");
    }
    div /= As<Number>(value_ast)->GetValue();
    operand = As<Cell>(operand)->GetSecond();

    while (operand) {
        if (!Is<Cell>(operand)) {
            throw RuntimeError("Runtime error: expected expression in /");
        }
        AST value_ast = Apply(As<Cell>(operand)->GetFirst());
        if (!Is<Number>(value_ast)) {
            throw RuntimeError("Runtime error: expected number in /");
        }
        if (As<Number>(value_ast)->GetValue() == 0) {
            throw RuntimeError("Runtime error: catched 0 in /");
        }
        div /= As<Number>(value_ast)->GetValue();
        operand = As<Cell>(operand)->GetSecond();
    }
    return std::make_shared<Number>(div);
}

AST Applier::IntegerOperations::OpMin(std::shared_ptr<Cell> ast) {
    int64_t ans = kMaxValue;
    AST operand = ast->GetSecond();
    if (!operand) {
        throw RuntimeError("Runtime error: min() expects operands");
    }
    if (!Is<Cell>(operand)) {
        throw RuntimeError("Runtime error: expected expression in min()");
    }
    AST value_ast = Apply(As<Cell>(operand)->GetFirst());
    if (!Is<Number>(value_ast)) {
        throw RuntimeError("Runtime error: expected number in min()");
    }
    ans = std::min(ans, As<Number>(value_ast)->GetValue());
    operand = As<Cell>(operand)->GetSecond();
    while (operand) {
        if (!Is<Cell>(operand)) {
            throw RuntimeError("Runtime error: expected expression in min()");
        }
        AST value_ast = Apply(As<Cell>(operand)->GetFirst());
        if (!Is<Number>(value_ast)) {
            throw RuntimeError("Runtime error: expected number in min()");
        }
        ans = std::min(ans, As<Number>(value_ast)->GetValue());
        operand = As<Cell>(operand)->GetSecond();
    }
    return std::make_shared<Number>(ans);
}

AST Applier::IntegerOperations::OpMax(std::shared_ptr<Cell> ast) {
    int64_t ans = kMinValue;
    AST operand = ast->GetSecond();
    if (!operand) {
        throw RuntimeError("Runtime error: max() expects operands");
    }
    if (!Is<Cell>(operand)) {
        throw RuntimeError("Runtime error: expected expression in max()");
    }
    AST value_ast = Apply(As<Cell>(operand)->GetFirst());
    if (!Is<Number>(value_ast)) {
        throw RuntimeError("Runtime error: expected number in max()");
    }
    ans = std::max(ans, As<Number>(value_ast)->GetValue());
    operand = As<Cell>(operand)->GetSecond();
    while (operand) {
        if (!Is<Cell>(operand)) {
            throw RuntimeError("Runtime error: expected expression in max()");
        }
        AST value_ast = Apply(As<Cell>(operand)->GetFirst());
        if (!Is<Number>(value_ast)) {
            throw RuntimeError("Runtime error: expected number in max()");
        }
        ans = std::max(ans, As<Number>(value_ast)->GetValue());
        operand = As<Cell>(operand)->GetSecond();
    }
    return std::make_shared<Number>(ans);
}

AST Applier::IntegerOperations::OpAbs(std::shared_ptr<Cell> ast) {
    int64_t ans = 0;
    AST operand = ast->GetSecond();
    if (!operand) {
        throw RuntimeError("Runtime error: abs() expects operands");
    }
    if (!Is<Cell>(operand)) {
        throw RuntimeError("Runtime error: expected expression in abs()");
    }
    if (As<Cell>(operand)->GetSecond() != nullptr) {
        throw RuntimeError("Runtime error: unary abs() expected one argument");
    }
    AST value_ast = Apply(As<Cell>(operand)->GetFirst());
    if (!Is<Number>(value_ast)) {
        throw RuntimeError("Runtime error: expected number in abs()");
    }
    ans = std::abs(As<Number>(value_ast)->GetValue());
    return std::make_shared<Number>(ans);
}

AST Applier::IntegerOperations::OpEqual(std::shared_ptr<Cell> ast) {
    bool ans = true;
    int64_t value = 0;
    bool started = false;
    AST operand = ast->GetSecond();
    while (operand) {
        if (!Is<Cell>(operand)) {
            throw RuntimeError("Runtime error: expected expression in =");
        }
        AST value_ast = Apply(As<Cell>(operand)->GetFirst());
        if (!Is<Number>(value_ast)) {
            throw RuntimeError("Runtime error: expected number in max()");
        }
        if (started) {
            ans = (value == As<Number>(value_ast)->GetValue());
            if (!ans) {
                break;
            }
        } else {
            value = As<Number>(value_ast)->GetValue();
            started = true;
        }
        operand = As<Cell>(operand)->GetSecond();
    }
    return std::make_shared<Boolean>(ans);
}

AST Applier::IntegerOperations::OpLess(std::shared_ptr<Cell> ast) {
    bool ans = true;
    int64_t last = kMinValue;
    AST operand = ast->GetSecond();
    while (operand) {
        if (!Is<Cell>(operand)) {
            throw RuntimeError("Runtime error: expected expression in <");
        }
        AST value_ast = Apply(As<Cell>(operand)->GetFirst());
        if (!Is<Number>(value_ast)) {
            throw RuntimeError("Runtime error: expected number in <");
        }
        ans = (last < As<Number>(value_ast)->GetValue());
        last = As<Number>(value_ast)->GetValue();
        if (!ans) {
            break;
        }
        operand = As<Cell>(operand)->GetSecond();
    }
    return std::make_shared<Boolean>(ans);
}

AST Applier::IntegerOperations::OpGreater(std::shared_ptr<Cell> ast) {
    bool ans = true;
    int64_t last = kMaxValue;
    AST operand = ast->GetSecond();
    while (operand) {
        if (!Is<Cell>(operand)) {
            throw RuntimeError("Runtime error: expected expression in >");
        }
        AST value_ast = Apply(As<Cell>(operand)->GetFirst());
        if (!Is<Number>(value_ast)) {
            throw RuntimeError("Runtime error: expected number in >");
        }
        ans = (last > As<Number>(value_ast)->GetValue());
        last = As<Number>(value_ast)->GetValue();
        if (!ans) {
            break;
        }
        operand = As<Cell>(operand)->GetSecond();
    }
    return std::make_shared<Boolean>(ans);
}

AST Applier::IntegerOperations::OpLessEqual(std::shared_ptr<Cell> ast) {
    bool ans = true;
    int64_t last = kMinValue;
    AST operand = ast->GetSecond();
    while (operand) {
        if (!Is<Cell>(operand)) {
            throw RuntimeError("Runtime error: expected expression in <=");
        }
        AST value_ast = Apply(As<Cell>(operand)->GetFirst());
        if (!Is<Number>(value_ast)) {
            throw RuntimeError("Runtime error: expected number in <=");
        }
        ans = (last <= As<Number>(value_ast)->GetValue());
        last = As<Number>(value_ast)->GetValue();
        if (!ans) {
            break;
        }
        operand = As<Cell>(operand)->GetSecond();
    }
    return std::make_shared<Boolean>(ans);
}

AST Applier::IntegerOperations::OpGreaterEqual(std::shared_ptr<Cell> ast) {
    bool ans = true;
    int64_t last = kMaxValue;
    AST operand = ast->GetSecond();
    while (operand) {
        if (!Is<Cell>(operand)) {
            throw RuntimeError("Runtime error: expected expression in >=");
        }
        AST value_ast = Apply(As<Cell>(operand)->GetFirst());
        if (!Is<Number>(value_ast)) {
            throw RuntimeError("Runtime error: expected number in >=");
        }
        ans = (last >= As<Number>(value_ast)->GetValue());
        last = As<Number>(value_ast)->GetValue();
        if (!ans) {
            break;
        }
        operand = As<Cell>(operand)->GetSecond();
    }
    return std::make_shared<Boolean>(ans);
}

// Boolean
AST Applier::BooleanOperations::OpIsBoolean(std::shared_ptr<Cell> ast) {
    bool ans = false;
    AST operand = ast->GetSecond();
    if (!operand) {
        throw RuntimeError("Runtime error: boolean? expects operands");
    }
    if (!Is<Cell>(operand)) {
        throw RuntimeError("Runtime error: expected expression in boolean?");
    }
    if (As<Cell>(operand)->GetSecond() != nullptr) {
        throw RuntimeError("Runtime error: unary boolean? expected one argument");
    }
    AST value_ast = Apply(As<Cell>(operand)->GetFirst());
    ans = Is<Boolean>(value_ast);
    return std::make_shared<Boolean>(ans);
}

AST Applier::BooleanOperations::OpNot(std::shared_ptr<Cell> ast) {
    bool ans = false;
    AST operand = ast->GetSecond();
    if (!operand) {
        throw RuntimeError("Runtime error: not expects operands");
    }
    if (!Is<Cell>(operand)) {
        throw RuntimeError("Runtime error: expected expression in not");
    }
    if (As<Cell>(operand)->GetSecond() != nullptr) {
        throw RuntimeError("Runtime error: unary not expected one argument");
    }
    AST value_ast = Apply(As<Cell>(operand)->GetFirst());
    if (Is<Boolean>(value_ast)) {
        ans = !As<Boolean>(value_ast)->GetValue();
    }
    return std::make_shared<Boolean>(ans);
}

AST Applier::BooleanOperations::OpAnd(std::shared_ptr<Cell> ast) {
    AST operand = ast->GetSecond();
    AST last_expr = std::make_shared<Boolean>(true);
    while (operand) {
        if (!Is<Cell>(operand)) {
            throw RuntimeError("Runtime error: and expects expression");
        }
        last_expr = Apply(As<Cell>(operand)->GetFirst());
        bool current = true;
        if (Is<Boolean>(last_expr)) {
            current = As<Boolean>(last_expr)->GetValue();
        }
        if (!current) {
            return last_expr;
        }
        operand = As<Cell>(operand)->GetSecond();
    }
    return last_expr;
}

AST Applier::BooleanOperations::OpOr(std::shared_ptr<Cell> ast) {
    AST operand = ast->GetSecond();
    AST last_expr = std::make_shared<Boolean>(false);
    while (operand) {
        if (!Is<Cell>(operand)) {
            throw RuntimeError("Runtime error: and expects expression");
        }
        last_expr = Apply(As<Cell>(operand)->GetFirst());
        bool current = true;
        if (Is<Boolean>(last_expr)) {
            current = As<Boolean>(last_expr)->GetValue();
        }
        if (current) {
            return last_expr;
        }
        operand = As<Cell>(operand)->GetSecond();
    }
    return last_expr;
}

// List
AST Applier::ListOperations::OpIsList(std::shared_ptr<Cell> ast) {
    AST operand = ast->GetSecond();
    if (!operand) {
        throw RuntimeError("Runtime error: list? expects operands");
    }
    if (!Is<Cell>(operand)) {
        throw RuntimeError("Runtime error: expected expression in list?");
    }
    if (As<Cell>(operand)->GetSecond() != nullptr) {
        throw RuntimeError("Runtime error: list? expected only 1 argument");
    }
    AST value_ast = Apply(As<Cell>(operand)->GetFirst());
    if (value_ast == nullptr) {
        return std::make_shared<Boolean>(true);
    }
    if (!Is<Cell>(value_ast)) {
        return std::make_shared<Boolean>(false);
    }
    operand = As<Cell>(value_ast)->GetSecond();
    while (operand) {
        if (!Is<Cell>(operand)) {
            return std::make_shared<Boolean>(false);
        }
        if (As<Cell>(operand)->GetSecond() != nullptr) {
            std::make_shared<Boolean>(false);
        }
        operand = As<Cell>(operand)->GetSecond();
    }
    return std::make_shared<Boolean>(true);
}

AST Applier::ListOperations::OpIsNull(std::shared_ptr<Cell> ast) {
    AST operand = ast->GetSecond();
    if (!operand) {
        throw RuntimeError("Runtime error: null? expects operand");
    }
    if (!Is<Cell>(operand)) {
        throw RuntimeError("Runtime error: expected expression in null?");
    }
    if (As<Cell>(operand)->GetSecond() != nullptr) {
        throw RuntimeError("Runtime error: null? expected only 1 argument");
    }
    AST value_ast = Apply(As<Cell>(operand)->GetFirst());
    return std::make_shared<Boolean>(value_ast == nullptr);
}

AST Applier::ListOperations::OpIsPair(std::shared_ptr<Cell> ast) {
    AST operand = ast->GetSecond();
    if (!operand) {
        throw RuntimeError("Runtime error: pair? expects operand");
    }
    if (!Is<Cell>(operand)) {
        throw RuntimeError("Runtime error: expected expression in pair?");
    }
    if (As<Cell>(operand)->GetSecond() != nullptr) {
        throw RuntimeError("Runtime error: pair? expected only 1 argument");
    }
    AST value_ast = Apply(As<Cell>(operand)->GetFirst());
    if (!Is<Cell>(value_ast)) {
        return std::make_shared<Boolean>(false);
    }
    auto cell_ast = As<Cell>(value_ast);
    bool ans = (cell_ast->GetFirst() != nullptr) && (cell_ast->GetSecond() != nullptr);
    return std::make_shared<Boolean>(ans);
}

AST Applier::ListOperations::OpList(std::shared_ptr<Cell> ast) {
    return ast->GetSecond();
}

AST Applier::ListOperations::OpCons(std::shared_ptr<Cell> ast) {
    AST operand = ast->GetSecond();
    if (!operand) {
        throw RuntimeError("Runtime error: cons expects 1st operand");
    }
    if (!Is<Cell>(operand)) {
        throw RuntimeError("Runtime error: expected expression in cons");
    }
    AST first_value = Apply(As<Cell>(operand)->GetFirst());

    operand = As<Cell>(operand)->GetSecond();
    if (!operand) {
        throw RuntimeError("Runtime error: cons expects 2nd operand");
    }
    if (!Is<Cell>(operand)) {
        throw RuntimeError("Runtime error: expected expression in cons");
    }
    if (As<Cell>(operand)->GetSecond() != nullptr) {
        throw RuntimeError("Runtime error: cons expected only 2 arguments");
    }
    AST second_value = Apply(As<Cell>(operand)->GetFirst());

    return std::make_shared<Cell>(first_value, second_value);
}

AST Applier::ListOperations::OpCar(std::shared_ptr<Cell> ast) {
    AST operand = ast->GetSecond();
    if (!operand) {
        throw RuntimeError("Runtime error: car expects operand");
    }
    if (!Is<Cell>(operand)) {
        throw RuntimeError("Runtime error: expected expression in car");
    }
    if (As<Cell>(operand)->GetSecond() != nullptr) {
        throw RuntimeError("Runtime error: car expected only 1 argument");
    }
    AST value_ast = Apply(As<Cell>(operand)->GetFirst());
    if (value_ast == nullptr || !Is<Cell>(value_ast)) {
        throw RuntimeError("Runtime error: car expected not empty list");
    }
    return As<Cell>(value_ast)->GetFirst();
}

AST Applier::ListOperations::OpCdr(std::shared_ptr<Cell> ast) {
    AST operand = ast->GetSecond();
    if (!operand) {
        throw RuntimeError("Runtime error: cdr expects operand");
    }
    if (!Is<Cell>(operand)) {
        throw RuntimeError("Runtime error: expected expression in cdr");
    }
    if (As<Cell>(operand)->GetSecond() != nullptr) {
        throw RuntimeError("Runtime error: cdr expected only 1 argument");
    }
    AST value_ast = Apply(As<Cell>(operand)->GetFirst());
    if (value_ast == nullptr || !Is<Cell>(value_ast)) {
        throw RuntimeError("Runtime error: cdr expected not empty list");
    }
    return As<Cell>(value_ast)->GetSecond();
}

AST Applier::ListOperations::OpListRef(std::shared_ptr<Cell> ast) {
    AST operand = ast->GetSecond();
    if (!operand) {
        throw RuntimeError("Runtime error: list-ref expects 1st operand");
    }
    if (!Is<Cell>(operand)) {
        throw RuntimeError("Runtime error: expected expression in list-ref");
    }
    AST copy_second_son = std::make_shared<Cell>(As<Cell>(operand)->GetFirst(), nullptr);
    AST first_arg = As<Cell>(operand)->GetFirst();

    operand = As<Cell>(operand)->GetSecond();
    if (!operand) {
        throw RuntimeError("Runtime error: list-ref expects 2st operand");
    }
    if (!Is<Cell>(operand)) {
        throw RuntimeError("Runtime error: expected expression in list-ref");
    }
    if (As<Cell>(operand)->GetSecond() != nullptr) {
        throw RuntimeError("Runtime error: list-ref expected only 2 arguments");
    }
    AST second_arg = As<Cell>(operand)->GetFirst();

    auto to_check_list = std::make_shared<Cell>(std::make_shared<Number>(0), copy_second_son);
    if (!As<Boolean>(OpIsList(As<Cell>(to_check_list)))->GetValue()) {
        throw RuntimeError("Runtime error: list-ref catched invalid list");
    }

    AST list_ast = Apply(first_arg);
    if (list_ast == nullptr || !Is<Cell>(list_ast)) {
        throw RuntimeError("Runtime error: list-ref catched invalid list");
    }

    AST index_ast = Apply(second_arg);
    if (!Is<Number>(index_ast)) {
        throw RuntimeError("Runtime error: invalid index in list-ref");
    }
    int index = As<Number>(index_ast)->GetValue();
    if (index < 0) {
        throw RuntimeError("Runtime error: invalid index in list-ref");
    }

    operand = list_ast;
    int current = 0;
    while (operand) {
        if (current == index) {
            return As<Cell>(operand)->GetFirst();
        }
        ++current;
        operand = As<Cell>(operand)->GetSecond();
    }
    throw RuntimeError("Runtime error: index out of range in list-ref");
}

AST Applier::ListOperations::OpListTail(std::shared_ptr<Cell> ast) {
    AST operand = ast->GetSecond();
    if (!operand) {
        throw RuntimeError("Runtime error: list-tail expects 1st operand");
    }
    if (!Is<Cell>(operand)) {
        throw RuntimeError("Runtime error: expected expression in list-tail");
    }
    AST copy_second_son = std::make_shared<Cell>(As<Cell>(operand)->GetFirst(), nullptr);
    AST first_arg = As<Cell>(operand)->GetFirst();

    operand = As<Cell>(operand)->GetSecond();
    if (!operand) {
        throw RuntimeError("Runtime error: list-tail expects 2st operand");
    }
    if (!Is<Cell>(operand)) {
        throw RuntimeError("Runtime error: expected expression in list-tail");
    }
    if (As<Cell>(operand)->GetSecond() != nullptr) {
        throw RuntimeError("Runtime error: list-tail expected only 2 arguments");
    }
    AST second_arg = As<Cell>(operand)->GetFirst();

    auto to_check_list = std::make_shared<Cell>(std::make_shared<Number>(0), copy_second_son);
    if (!As<Boolean>(OpIsList(to_check_list))->GetValue()) {
        throw RuntimeError("Runtime error: list-tail catched invalid list");
    }

    AST list_ast = Apply(first_arg);
    if (list_ast == nullptr || !Is<Cell>(list_ast)) {
        throw RuntimeError("Runtime error: list-tail catched invalid list");
    }

    AST index_ast = Apply(second_arg);
    if (!Is<Number>(index_ast)) {
        throw RuntimeError("Runtime error: invalid index in list-tail");
    }
    int index = As<Number>(index_ast)->GetValue();
    if (index < 0) {
        throw RuntimeError("Runtime error: invalid index in list-tail");
    }

    operand = list_ast;
    int current = 0;
    while (operand) {
        if (current == index) {
            return operand;
        }
        ++current;
        operand = As<Cell>(operand)->GetSecond();
    }
    if (current == index) {
        return nullptr;
    }
    throw RuntimeError("Runtime error: index out of range in list-ref");
}
