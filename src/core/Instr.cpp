#include "Instr.h"


struct InstrStringifier {
    std::string operator() (const Push& push) const { return std::string { "Push<" } + std::to_string(push.value) + ">"; }
    std::string operator() (const Add&) const { return std::string { "Add" }; }
    std::string operator() (const Add1& add1) const { return std::string { "Add1<" } + std::to_string(add1.value) + ">"; }
    std::string operator() (const Sub&) const { return std::string { "Sub" }; }
    std::string operator() (const SubRev&) const { return std::string { "SubRev" }; }
    std::string operator() (const Mul&) const { return std::string { "Mul" }; }
    std::string operator() (const Mul1& mul1) const { return std::string { "Mul1<" } + std::to_string(mul1.value) + ">"; }
    std::string operator() (const Sqr&) const { return std::string { "Sqr" }; }
    std::string operator() (const Div&) const { return std::string { "Div" }; }
    std::string operator() (const Div1& div1) const { return std::string { "Div1<" + std::to_string(div1.value) + ">" }; }
    std::string operator() (const Mod&) const { return std::string { "Mod" }; }
    std::string operator() (const Mod1& mod1) const { return std::string { "Mod1<" + std::to_string(mod1.value) + ">" }; }
    std::string operator() (const Not&) const { return std::string { "Not" }; }
    std::string operator() (const Comp& comp) const { return std::string { std::string { "Comp<" } + stringify(comp.type) + ">" }; }
    std::string operator() (const Dup&) const { return std::string { "Dup" }; }
    std::string operator() (const Swap&) const { return std::string { "Swap" }; }
    std::string operator() (const Drop&) const { return std::string { "Drop" }; }
    std::string operator() (const Get&) const { return std::string { "Get" }; }
    std::string operator() (const Get2& get2) const { return std::string { "Get2<" } + std::to_string(get2.x) + ", " + std::to_string(get2.y) + ">"; }
    std::string operator() (const Put&) const { return std::string { "Put" }; }
    std::string operator() (const Put2& put2) const { return std::string { "Put2<" } + std::to_string(put2.x) + ", " + std::to_string(put2.y) + ">"; }
    std::string operator() (const ReadInt64&) const { return std::string { "ReadInt64" }; }
    std::string operator() (const ReadChar&) const { return std::string { "ReadChar" }; }
    std::string operator() (const WriteInt64&) const { return std::string { "WriteInt64" }; }
    std::string operator() (const WriteChar&) const { return std::string { "WriteChar" }; }
    std::string operator() (const If&) const { return std::string { "If" }; }
    std::string operator() (const NotIf& notIf) const { return std::string { "NotIf<" } + std::to_string(notIf.dup) + ">"; }
    std::string operator() (const CompIf& compIf) const { return std::string { "CompIf<" } + stringify(compIf.type) + ">"; }
    std::string operator() (const Comp1If& comp1If) const { return std::string { "Comp1If<" } + stringify(comp1If.type) + ", " + std::to_string(comp1If.value) + ", " + std::to_string(comp1If.dup) + ">"; }
    std::string operator() (const Rand&) const { return std::string { "Rand" }; }
    std::string operator() (const End&) const { return std::string { "End" }; }
    std::string operator() (const ChainStart&) const { return std::string { "[" }; }
    std::string operator() (const ChainEnd&) const { return std::string { "]" }; }
};

std::string stringify (const Instr& instr) {
    return std::visit(InstrStringifier {}, instr);
}