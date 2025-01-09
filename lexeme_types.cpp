
class LexemeTypes {
public:
    static bool IsSkip(const char& a) {
        return a == ' ' || a == '\n' || a == '\t';
    }

    static bool IsLetter(const char& a) {
        return ('a' <= a && a <= 'z') || ('A' <= a && a <= 'Z');
    }

    static bool IsDigit(const char& a) {
        return '0' <= a && a <= '9';
    }

    static bool IsStartSymbol(const char& a) {
        return IsLetter(a) || a == '<' || a == '=' || a == '>' || a == '*' || a == '/' || a == '#';
    }

    static bool IsInnerSymbol(const char& a) {
        return IsStartSymbol(a) || IsDigit(a) || a == '?' || a == '!' || a == '-';
    }

    static bool IsPlus(const char& a) {
        return a == '+';
    }

    static bool IsMinus(const char& a) {
        return a == '-';
    }

    static bool IsBracket(const char& a) {
        return a == '(' || a == ')';
    }

    static bool IsQuote(const char& a) {
        return a == '\'';
    }

    static bool IsDot(const char& a) {
        return a == '.';
    }
};
