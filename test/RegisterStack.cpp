//
// Created by ran on 4/16/16.
//

#include "RegisterStack.h"

#include <iostream>
using std::cout;
using std::endl;

RegisterStack::RegisterStack(const unsigned int &num_registers,
                             const unsigned int &start_register)
        : num_registers(num_registers), start_register(start_register) {
    stack_size = 0;
}


unsigned int RegisterStack::push() {
    auto result = stack_size++ % num_registers + start_register;
    if (stack_size % num_registers == num_registers / 2
        && (stack_size / num_registers > 0)) {
        cout << "Pushing upper half" << endl;
        cout << "push {r" << num_registers / 2 + start_register << "-r" << num_registers + start_register - 1 << "}" << endl;
        upper_empty = true;
    } else if (stack_size % num_registers == 0 && stack_size > 0) {
        cout << "Pushing lower half" << endl;
        cout << "push {r" << start_register << "-r" << num_registers / 2 + start_register - 1 << "}" << endl;
    }
    cout << "Pushed into r" << result << endl;
    return result;
}

unsigned int RegisterStack::pop() {
    if (stack_size % num_registers == num_registers / 2
        && stack_size / num_registers > 0) {
        cout << "Popping upper half" << endl;
        cout << "pop {r" << num_registers / 2 + start_register << "-r" << num_registers + start_register - 1 << "}" << endl;

    } else if (stack_size % num_registers == 0 && stack_size > 0) {
        cout << "Popping lower half" << endl;
        cout << "pop {r" << start_register << "-r" << num_registers / 2 + start_register - 1 << "}" << endl;
    }

    auto result = --stack_size % num_registers + start_register;
    cout << "Popped value is in r" << result << endl;
    return result;
}


unsigned int RegisterStack::get_stack_size() {
    return stack_size;
}

