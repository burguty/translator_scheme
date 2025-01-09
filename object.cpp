#include "object.h"

Number::Number(int64_t value) : value_(value) {
}

int64_t Number::GetValue() const {
    return value_;
}

Symbol::Symbol(std::string symbol) : symbol_(symbol) {
}

const std::string& Symbol::GetName() const {
    return symbol_;
}

Boolean::Boolean(bool value) : value_(value) {
}

bool Boolean::GetValue() const {
    return value_;
}

Quote::Quote() : cmd_(nullptr) {
}

Quote::Quote(std::shared_ptr<Object> cmd) : cmd_(cmd) {
}

std::shared_ptr<Object> Quote::GetCommand() const {
    return cmd_;
}

Cell::Cell() {
    first_ = nullptr;
    second_ = nullptr;
}

Cell::Cell(const std::shared_ptr<Object>& lhs, const std::shared_ptr<Object>& rhs)
    : first_(lhs), second_(rhs) {
}

void Cell::SetFirst(std::shared_ptr<Object>&& other) {
    first_ = other;
}

void Cell::SetSecond(std::shared_ptr<Object>&& other) {
    second_ = other;
}

std::shared_ptr<Object> Cell::GetFirst() const {
    return first_;
}

std::shared_ptr<Object> Cell::GetSecond() const {
    return second_;
}
