#include <iostream>
#include <string>
#include <exception>
#define lmb '\\'

class l_term {
public:
    explicit l_term(char x) : l_type(SYMBOL), l_val(x) {}

    l_term(char x, l_term *term) : l_type(ASSOC), l_val(x), arg2(term) {}

    l_term(l_term *term1, l_term *term2) : l_type(APP), arg1(term1), arg2(term2) {}

    l_term *getarg1() const {
        switch (l_type) {
            case SYMBOL:
            case ASSOC:
                return new l_term(l_val);
            case APP:
                return arg1;
        }
    }

    l_term *getarg2() const {
        switch (l_type) {
            case SYMBOL:
                return nullptr;
            case ASSOC:
            case APP:
                return arg2;
        }
    }

    enum type {
        SYMBOL,
        ASSOC,
        APP,
    };

    enum type gettype() const {
        return l_type;
    }

    friend std::ostream &operator<<(std::ostream &stream, l_term &term);

    friend l_term *lapp_leftassociative(l_term *term);

    void alpha_reduction(char newval) {
        if (gettype() != ASSOC)
            throw std::logic_error("given term is not associative type");
        char oldvar = l_val;
        l_val = newval;

        if (arg1)
            alpha_reduct(arg1, oldvar, newval);
        if (arg2)
            alpha_reduct(arg2, oldvar, newval);
    }

private:
    void alpha_reduct(l_term *term, char oldvar, char newval) {
        if (term->l_val) {
            if (term->l_val == newval)
                throw std::logic_error("given reductive value already present in expression");
            else if (term->l_val == oldvar)
                term->l_val = newval;
        }
        if (term->arg1)
            alpha_reduct(term->arg1, oldvar, newval);
        if (term->arg2)
            alpha_reduct(term->arg2, oldvar, newval);
    }

    enum type l_type;
    char l_val = '\0';
    l_term *arg1 = nullptr; // only used in APP
    l_term *arg2 = nullptr;
};

l_term *lapp_leftassociative(l_term *term) {
    while (term->arg2->arg1 != nullptr) {
        if (term->l_type != l_term::APP) {
            return lapp_leftassociative(term->arg2);
        }
        l_term *rightkid = term->arg2;
        term->arg2 = term->arg2->arg1;
        rightkid->arg1 = term;
        term = rightkid;
    }
    return term;
}

l_term *lparse(std::string &expression, size_t i = 0, bool appl = true) {
    if (expression[i] == lmb) {
        char vr = expression[i + 1];
        return new l_term(vr, lparse(expression, i + 3));
    } else if (expression[i] == '(') {
        size_t a = i;
        size_t b = i;
        for (; b < expression.size(); b++) {
            if (expression[b] == ')')
                break;
        }
        std::string slice = expression.substr(a + 1, b - a - 1);
        return new l_term(lparse(slice), lparse(expression, b + 1, false));
    } else {
        char vr = expression[i];
        if (i < expression.size() - 1) {
            if (appl) {
                l_term *appres = new l_term(new l_term(vr), lparse(expression, i + 1, false));
                return lapp_leftassociative(appres);
            } else
                return new l_term(new l_term(vr), lparse(expression, i + 1, false));
        } else
            return new l_term(vr);
    }
}


std::ostream &operator<<(std::ostream &stream, l_term &term) {
    switch (term.l_type) {
        case l_term::SYMBOL:
            if (term.l_val)
                stream << term.l_val;
            break;
        case l_term::ASSOC:
            stream << lmb << term.l_val << '.' << '(' << *term.arg2 << ')';
            break;
        case l_term::APP:
            stream << '(' << *term.arg1 << *term.arg2 << ')';
            break;
    }
    return stream;
}

int main() {
    std::string lexp;
    std::cin >> lexp;
    l_term res = *lparse(lexp);
    std::cout << res << std::endl;

    char areduction_ask;
    std::cin >> areduction_ask;

    res.alpha_reduction(areduction_ask);
    std::cout << res << std::endl;
    return 0;
}
