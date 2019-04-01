#include <rttr/registration>
//using namespace rttr;

#include <iostream>

#include "../../core/matadata.h"

enum class E_Alignment { AlignLeft = 0x0001, AlignRight = 0x0002, AlignHCenter = 0x0004, AlignJustify = 0x0008 };

struct MyStruct {
    MyStruct(){};

    void method() {
        std::cout << "method called" << std::endl;
    }
    void methodWithParams(const std::string& first, bool second, double third, int last) {
        std::cout << "methodWithParams called" << std::endl;
        std::cout << "first " << first << std::endl;
        std::cout << "second " << second << std::endl;
        std::cout << "third " << third << std::endl;
        std::cout << "last " << last << std::endl;
    }
    std::string methodWithParamsAndReturnValue(const std::string& name) {
        std::cout << "methodWithParamsAndReturnValue called" << std::endl;
        std::cout << "name " << name << std::endl;
        return "Hello " + name;
    }
    void methodWithDefaultParameter(const std::string& first, const std::string& second = "string") {
        std::cout << "methodWithDefaultParameter called" << std::endl;
        std::cout << "first " << first << std::endl;
        std::cout << "second " << second << std::endl;
    }

    std::string methodWithEnumParame(E_Alignment al) {
        std::cout << "methodWithEnumParame called" << std::endl;
        std::cout << "al " << int(al) << std::endl;

        std::string res;
        switch(al) {
        case E_Alignment::AlignLeft:
            res = "AlignLeft";
            break;
        case E_Alignment::AlignRight:
            res = "AlignRight";
            break;
        case E_Alignment::AlignHCenter:
            res = "AlignHCenter";
            break;
        case E_Alignment::AlignJustify:
            res = "AlignJustify";
            break;
        default:
            break;
        }
        std::cout << res << std::endl;
        return res;
    }

    int data;
};
