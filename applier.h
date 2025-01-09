#include "parser.h"
#include <unordered_map>

typedef AST (*Functor)(std::shared_ptr<Cell>);

class Applier {
public:
    Applier() = delete;
    ~Applier() = delete;

    static Functor GetFunctor(const std::string& arg);

    static AST Apply(AST ast);

private:
    class QuoteOperations {
    public:
        static AST OpQuote(std::shared_ptr<Quote> ast);
    };

    class IntegerOperations {
    public:
        static AST OpIsNumber(std::shared_ptr<Cell> ast);

        static AST OpPlus(std::shared_ptr<Cell> ast);
        static AST OpMinus(std::shared_ptr<Cell> ast);
        static AST OpMultiply(std::shared_ptr<Cell> ast);
        static AST OpDivide(std::shared_ptr<Cell> ast);

        static AST OpEqual(std::shared_ptr<Cell> ast);
        static AST OpLess(std::shared_ptr<Cell> ast);
        static AST OpGreater(std::shared_ptr<Cell> ast);
        static AST OpLessEqual(std::shared_ptr<Cell> ast);
        static AST OpGreaterEqual(std::shared_ptr<Cell> ast);

        static AST OpMin(std::shared_ptr<Cell> ast);
        static AST OpMax(std::shared_ptr<Cell> ast);

        static AST OpAbs(std::shared_ptr<Cell> ast);
    };

    class BooleanOperations {
    public:
        static AST OpIsBoolean(std::shared_ptr<Cell> ast);
        static AST OpNot(std::shared_ptr<Cell> ast);
        static AST OpAnd(std::shared_ptr<Cell> ast);
        static AST OpOr(std::shared_ptr<Cell> ast);
    };

    class ListOperations {
    public:
        static AST OpIsList(std::shared_ptr<Cell> ast);
        static AST OpIsPair(std::shared_ptr<Cell> ast);
        static AST OpIsNull(std::shared_ptr<Cell> ast);

        static AST OpList(std::shared_ptr<Cell> ast);
        static AST OpListRef(std::shared_ptr<Cell> ast);
        static AST OpListTail(std::shared_ptr<Cell> ast);

        static AST OpCons(std::shared_ptr<Cell> ast);
        static AST OpCdr(std::shared_ptr<Cell> ast);
        static AST OpCar(std::shared_ptr<Cell> ast);
    };

    static std::unordered_map<std::string, Functor> functors;
};
