#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>

class RAMSLOT
{
public:
	long int _val;
};
RAMSLOT* RAM = new RAMSLOT[1048576];
RAMSLOT registers[9];
class VARIABLE
{
public:
	unsigned int value = 0;
	std::string name;
	VARIABLE(unsigned int value, std::string name)
	{
		this->value = value;
		this->name = name;
	}
	bool operator==(const VARIABLE rhs)
	{
		return rhs.name == this->name;
	}
};

std::vector<VARIABLE> variables;
unsigned int cur_memory = 0;
RAMSLOT* ptr_parse(std::string value)
{

	std::string cmp_string = value;
	if (cmp_string[0] == '*') cmp_string.erase(cmp_string.begin());
	auto it = std::find(variables.begin(), variables.end(), VARIABLE(0, cmp_string));
	if (value[0] == 'r' && it == variables.end())
	{
		int register_idx = value[1] - 48;
		return &registers[register_idx];
	}
	if (value[0] == '*')
	{
		value.erase(value.begin());
		return &RAM[RAM[variables[std::distance(variables.begin(), it)].value]._val];
	}
	RAMSLOT* slot = &RAM[variables[std::distance(variables.begin(), it)].value];

	return slot;
}
RAMSLOT parse(std::string value, bool init)
{

	std::string cmp_string = value;
	if (cmp_string[0] == '*') cmp_string.erase(cmp_string.begin());
	auto it = std::find(variables.begin(), variables.end(), VARIABLE(0, cmp_string));
	if (it != variables.end())
	{

		if (value[0] == '*')
		{

			value.erase(value.begin());
			return RAM[RAM[variables[std::distance(variables.begin(), it)].value]._val];
		}
	}
	else if (value[0] == 'r')
	{

		int register_idx = value[1] - 48;
		return registers[register_idx];
	}
	RAMSLOT slot;
	if (it != variables.end())
	{
		if (!init)
		{
			slot = RAM[variables[std::distance(variables.begin(), it)].value];
		}
		else
		{
			slot._val = std::distance(variables.begin(), it);
		}
	}
	else
	{
		slot._val = std::stoi(value, nullptr, 16);
	}
	return slot;
}

void increment(RAMSLOT* slot)
{
	long int i = slot->_val;
	long int mask = 1;
	while (i & mask)
	{
		i &= ~mask;
		mask <<= 1;
	}
	i |= mask;
	slot->_val = i;
}
void add(RAMSLOT slot1, RAMSLOT slot2, RAMSLOT* destination)
{
	destination->_val = slot1._val + slot2._val;
}
void subtract(RAMSLOT slot1, RAMSLOT slot2, RAMSLOT* destination)
{
	destination->_val = slot1._val - slot2._val;
}
void multiply(RAMSLOT slot1, RAMSLOT slot2, RAMSLOT* destination)
{
	destination->_val = slot1._val * slot2._val;
}
void divide(RAMSLOT slot1, RAMSLOT slot2, RAMSLOT* destination)
{
	destination->_val = slot1._val / slot2._val;
}

int main()
{
	std::ifstream if_file;
	std::string open_this;
	std::cout << "Enter *.fasm file directory: ";
	std::cin >> open_this;
	if_file.open(open_this);
	std::vector<std::string> program_memory;
	std::string cur_string;
	while (std::getline(if_file, cur_string))
	{
		program_memory.push_back(cur_string);
	}

	bool reading_vars = false;
	long int cmp_result = 0;
	std::vector<RAMSLOT> stack;

	int debug_mode;
	printf("Debug? (0/1) ");
	std::cin >> debug_mode;
	while (true)
	{
		unsigned long int line_idx = registers[8]._val;

		std::string line = program_memory[line_idx];
		line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
		if (line.empty())
		{
			increment(&registers[8]);
			continue;
		}
		std::stringstream stream(line);
		std::string arg;

		std::vector<std::string> args;
		while (getline(stream, arg, ' '))
		{

			args.push_back(arg);
		}
		if (args[0] == ".vars:")
		{
			reading_vars = true;
			increment(&registers[8]);
			continue;
		}
		else if (args[0] == ".inst:")
		{
			reading_vars = false;
			increment(&registers[8]);
			continue;
		}
		if (reading_vars)
		{
			variables.push_back(VARIABLE(cur_memory, args[0]));

			RAM[cur_memory]._val = parse(args[2], true)._val;
			cur_memory += std::stoi(args[1], nullptr, 16);
		}
		else
		{
			if (args[0] == "int")
			{

				long int val = parse(args[1], false)._val;
				if (val == 0) printf("%i", registers[0]._val);
				else if (val == 1) return 0;
				else if (val == 2) printf("%c", registers[0]._val);
				else if (val == 3)
				{
					std::string c;
					std::cin >> c;
					std::string c2;
					std::getline(std::cin, c2);
					c.append(c2);
					std::reverse(c.begin(), c.end());
					for (char c_char : c)
					{
						RAMSLOT slot;
						slot._val = c_char;
						stack.push_back(slot);
					}
				}
			}
			else if (args[0] == "mov")
			{
				RAMSLOT* slot = ptr_parse(args[1]);
				slot->_val = parse(args[2], false)._val;
			}
			else if (args[0] == "pmov")
			{
				RAMSLOT slot = parse(args[1], false);
				RAM[slot._val]._val = parse(args[2], false)._val;
			}
			else if (args[0] == "add")
			{
				if (args[1][0] != 'r' || args[2][0] != 'r' || args[3][0] != 'r') return -1;
				add(registers[args[2][1] - 48], registers[args[3][1] - 48], ptr_parse(args[1]));
			}
			else if (args[0] == "sub")
			{
				if (args[1][0] != 'r' || args[2][0] != 'r' || args[3][0] != 'r') return -1;
				subtract(registers[args[2][1] - 48], registers[args[3][1] - 48], ptr_parse(args[1]));
			}
			else if (args[0] == "mul")
			{
				if (args[1][0] != 'r' || args[2][0] != 'r' || args[3][0] != 'r') return -1;
				multiply(registers[args[2][1] - 48], registers[args[3][1] - 48], ptr_parse(args[1]));
			}
			else if (args[0] == "div")
			{
				if (args[1][0] != 'r' || args[2][0] != 'r' || args[3][0] != 'r') return -1;
				divide(registers[args[2][1] - 48], registers[args[3][1] - 48], ptr_parse(args[1]));
			}
			else if (args[0] == "inc")
			{
				increment(ptr_parse(args[1]));
			}
			else if (args[0] == "cmp")
			{
				cmp_result = parse(args[1], false)._val - parse(args[2], false)._val;

			}
			else if (args[0] == "jmp")
			{
				if (args[1] == "eq")
				{

					if (cmp_result == 0)
					{

						registers[8]._val = parse(args[2], false)._val;
					}
				}
				if (args[1] == "lt")
				{

					if (cmp_result < 0)
					{
						registers[8]._val = parse(args[2], false)._val;
					}
				}
				if (args[1] == "gt")
				{
					if (cmp_result > 0)
					{
						registers[8]._val = parse(args[2], false)._val;
					}
				}


			}
			else if (args[0] == "push")
			{
				stack.push_back(parse(args[1], false));
			}
			else if (args[0] == "pop")
			{
				ptr_parse(args[1])->_val = stack[stack.size() - 1]._val;
				stack.pop_back();
			}
		}

		if (debug_mode)
		{
			std::cout << "==------------------------------------==" << std::endl;
			std::cout << line << std::endl;

			for (VARIABLE variable : variables)
			{
				std::cout << variable.name << ": " << RAM[variable.value]._val << std::endl;
			}
			for (int register_idx = 0;register_idx < 9;register_idx++)
			{
				RAMSLOT reg = registers[register_idx];
				std::cout << "r" << register_idx << ": " << reg._val << std::endl;
			}
			std::cout << "cmp result: " << cmp_result << std::endl;
			std::cout << "------------------------------------" << std::endl;
			printf("Continue debugging? ");
			std::cin >> debug_mode;
		}



		increment(&registers[8]);

	}



	delete RAM;
	return 0;
}