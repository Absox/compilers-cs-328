//
// Created by ran on 4/16/16.
//

#ifndef COMPILERS_HWK5_REGISTERSTACK_H
#define COMPILERS_HWK5_REGISTERSTACK_H


class RegisterStack {
public:
    RegisterStack(const unsigned int& num_registers, const unsigned int& start_register);

    unsigned int push();
    unsigned int pop();

    unsigned int get_stack_size();

private:
    bool lower_empty;
    bool upper_empty;
    unsigned int stack_size;

    const unsigned int num_registers;
    const unsigned int start_register;

};


#endif //COMPILERS_HWK5_REGISTERSTACK_H
