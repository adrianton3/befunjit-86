#include <format>

#include "Instr.h"


struct InstrStringifier {
    std::string operator() (const Push& push) const { return std::format("Push<{}>", push.value); }
    std::string operator() (const Add&) const { return std::format("Add"); }
    std::string operator() (const Add1& add1) const { return std::format("Add1<{}>", add1.value); }
    std::string operator() (const Sub&) const { return std::format("Sub"); }
    std::string operator() (const SubRev&) const { return std::format("SubRev"); }
    std::string operator() (const Mul&) const { return std::format("Mul"); }
    std::string operator() (const Mul1& mul1) const { return std::format("Mul1<{}>", mul1.value); }
    std::string operator() (const Sqr&) const { return std::format("Sqr"); }
    std::string operator() (const Div&) const { return std::format("Div"); }
    std::string operator() (const Div1& div1) const { return std::format("Div1<{}>", div1.value); }
    std::string operator() (const Mod&) const { return std::format("Mod"); }
    std::string operator() (const Mod1& mod1) const { return std::format("Mod1<{}>", mod1.value); }
    std::string operator() (const Not&) const { return std::format("Not"); }
    std::string operator() (const Comp& comp) const { return std::format("Comp<{}>", stringify(comp.type)); }
    std::string operator() (const Dup&) const { return std::format("Dup"); }
    std::string operator() (const Swap&) const { return std::format("Swap"); }
    std::string operator() (const Drop&) const { return std::format("Drop"); }
    std::string operator() (const Get&) const { return std::format("Get"); }
    std::string operator() (const Get2& get2) const { return std::format("Get2<{}, {}>", get2.x, get2.y); }
    std::string operator() (const Put&) const { return std::format("Put"); }
    std::string operator() (const Put2& put2) const { return std::format("Put2<{}, {}>", put2.x, put2.y); }
    std::string operator() (const ReadInt64&) const { return std::format("ReadInt64"); }
    std::string operator() (const ReadChar&) const { return std::format("ReadChar"); }
    std::string operator() (const WriteInt64&) const { return std::format("WriteInt64"); }
    std::string operator() (const WriteChar&) const { return std::format("WriteChar"); }
    std::string operator() (const If&) const { return std::format("If"); }
    std::string operator() (const NotIf& notIf) const { return std::format("NotIf<{}>", notIf.dup); }
    std::string operator() (const CompIf& compIf) const { return std::format("CompIf<{}>", stringify(compIf.type)); }
    std::string operator() (const Comp1If& comp1If) const { return std::format("Comp1If<{}, {}, {}>", stringify(comp1If.type), comp1If.value, comp1If.dup); }
    std::string operator() (const Rand&) const { return std::format("Rand"); }
    std::string operator() (const End&) const { return std::format("End"); }
    std::string operator() (const ChainStart&) const { return std::format("["); }
    std::string operator() (const ChainEnd&) const { return std::format("]"); }
};

std::string stringify (const Instr& instr) {
    return std::visit(InstrStringifier {}, instr);
}