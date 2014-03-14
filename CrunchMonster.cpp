#include <iostream>
#include <cmath>
#include "CalcException.h"
#include <QMap>
#include <QString>
#include <QTextStream>
using namespace std;

QMap<QString,double> variables;
QMap<QString,double> constants;
bool radians;

double eval_expression(QString& expr, qint32& pos);

double eval_literal(QString& expr, qint32& pos)
{
	double result = 0.0;
	bool negative = false;
	bool frac = false;
	bool literal_is_num = false;
	qint32 frac_counter = 1;
	QString function_buffer = "";
	if (expr[pos] == '-')
	{
		negative = true;
		pos++;
	}
	if (expr[pos] == 0)
	{
		throw new CalcException("Invalid input.");
	}
	while (expr[pos] != 0)
	{
		if (expr[pos] >= 0x30 && expr[pos] <= 0x39 && function_buffer == "")
		{
			literal_is_num = true;
			if (!frac)
			{
				result *= 10;
				result += (double)expr[pos].unicode() - 48;
			}
			else{
				double digit_add = (double)expr[pos].unicode() - 48;
				for (qint32 i = 0; i < frac_counter; i++)
				{
					digit_add /= 10;
				}
				frac_counter++;
				result += digit_add;
			}
		}
		else if (!literal_is_num && ((expr[pos] >= 0x61 && expr[pos] <= 0x7a) || (expr[pos] >= 0x41 && expr[pos] <= 0x5a) || (function_buffer != "" && expr[pos] >= 0x30 && expr[pos] <= 0x39)))
		{
			function_buffer += expr[pos];
		}
		else if (expr[pos] == '.')
		{
			frac = true;
		}
		else if (expr[pos] == '(')
		{
			result = eval_expression(expr,++pos);
			if (function_buffer == "sin")
			{
				if (!radians)
				{
					result *= M_PI/180;
				}
				result = sin(result);
			}
			else if (function_buffer == "cos")
			{
				if (!radians)
				{
					result *= M_PI/180;
				}
				result = cos(result);
			}
			else if (function_buffer == "tan")
			{
				if (!radians)
				{
					result *= M_PI/180;
				}
				result = tan(result);
			}
			else if (function_buffer == "asin")
			{
				result = asin(result);
				if (!radians)
				{
					result *= M_PI/180;
				}
			}
			else if (function_buffer == "acos")
			{
				result = acos(result);
				if (!radians)
				{
					result *= M_PI/180;
				}
			}
			else if (function_buffer == "atan")
			{
				result = atan(result);
				if (!radians)
				{
					result *= M_PI/180;
				}
			}
			else if (function_buffer == "sinh")
			{
				result = sinh(result);
			}
			else if (function_buffer == "cosh")
			{
				result = cosh(result);
			}
			else if (function_buffer == "tanh")
			{
				result = tanh(result);
			}
			else if (function_buffer == "asinh")
			{
				result = asinh(result);
			}
			else if (function_buffer == "acosh")
			{
				result = acosh(result);
			}
			else if (function_buffer == "atanh")
			{
				result = atanh(result);
			}
			else if (function_buffer == "sqrt")
			{
				result = sqrt(result);
			}
			else if (function_buffer == "cbrt")
			{
				result = cbrt(result);
			}
			else if (function_buffer == "abs")
			{
				result = abs(result);
			}
			else if (function_buffer == "ceil")
			{
				result = ceil(result);
			}
			else if (function_buffer == "floor")
			{
				result = floor(result);
			}
			else if (function_buffer == "round")
			{
				result = round(result);
			}
			else if (function_buffer == "ln")
			{
				result = log(result);
			}
			else if (function_buffer == "log")
			{
				result = log10(result);
			}
			else if (function_buffer == "log2")
			{
				result = log2(result);
			}
			else if (function_buffer != "")
			{
				throw new CalcException("Unknown function \"" + function_buffer + ".\"");
			}
			function_buffer += '(';
		}
		else{
			break;
		}
		pos++;
	}
	if (function_buffer != "" && function_buffer[function_buffer.length()-1] != '(')
	{
		if (variables.find(function_buffer) != variables.end())
		{
			result = variables[function_buffer];
		}
		else if (constants.find(function_buffer) != constants.end())
		{
			result = constants[function_buffer];
		}
		else{
			throw new CalcException("Unknown variable \"" + function_buffer + ".\"");
		}
	}
	if (negative)
	{
		result = -result;
	}
	return result;
}

double eval_exponential(QString& expr, qint32& pos)
{
	double result = eval_literal(expr,pos);
	while (expr[pos] != 0)
	{
		if (expr[pos] != '^')
		{
			return result;
		}
		if (expr[pos] == '^')
		{
			double exponent = eval_literal(expr,++pos);
			result = pow(result,exponent);
		}
	}
	return result;
}

double eval_term(QString& expr, qint32& pos)
{
	double result = eval_exponential(expr,pos);
	while (expr[pos] != 0)
	{
		if (expr[pos] != '*' && expr[pos] != '/' && expr[pos] != '%')
		{
			return result;
		}
		if (expr[pos] == '*')
		{
			result *= eval_exponential(expr,++pos);
			pos--;
		}
		else if (expr[pos] == '/')
		{
			double factor = eval_exponential(expr,++pos);
			if (factor != 0.0)
			{
				result /= factor;
				pos--;
			}
			else{
				throw new CalcException("Attempt at division by zero.");
			}
		}
		else if (expr[pos] == '%')
		{
			double factor = eval_exponential(expr,++pos);
			if (factor == floor(factor) && result == floor(result))
			{
				if (factor != 0.0)
				{
					result = (int)result % (int)factor;
					pos--;
				}
				else{
					throw new CalcException("Attempt at division by zero.");
				}
			}
			else{
				throw new CalcException("Modulus on decimals is impossible.");
			}
		}
		pos++;
	}
	return result;
}

double eval_expression(QString& expr, qint32& pos)
{
	double result = eval_term(expr,pos);
	while (expr[pos] != 0)
	{
		if (expr[pos] == ')')
		{
			return result;
		}
		if (expr[pos] == '+')
		{
			result += eval_term(expr, ++pos);
			pos--;
		}
		else if (expr[pos] == '-')
		{
			result -= eval_term(expr, ++pos);
			pos--;
		}
		else{
			throw new CalcException("Invalid expression.");
		}
		pos++;
	}
	return result;
}

QString eval_variable(QString& expr, qint32& pos)
{
	QString var_name = "";
	while (expr[pos] != '=')
	{
		if (var_name == "")
		{
			if ((expr[pos] >= 0x61 && expr[pos] <= 0x7a) || (expr[pos] >= 0x41 && expr[pos] <= 0x5a))
			{
				var_name += expr[pos];
			}
			else{
				throw new CalcException("Variables must start with a letter.");
			}
		}
		else{
			if ((expr[pos] >= 0x61 && expr[pos] <= 0x7a) || (expr[pos] >= 0x30 && expr[pos] <= 0x39) || (expr[pos] >= 0x41 && expr[pos] <= 0x5a))
			{
				var_name += expr[pos];
			}
			else{
				throw new CalcException("Variables can only contain letters and numbers.");
			}
		}
		pos++;
	}
	return var_name;
}

int main()
{
	constants.insert("e",M_E);
	constants.insert("pi",M_PI);
	QTextStream std_in(stdin);
	QString command = "";
	qint32 start_pos = 0;
	radians = true;
	while (command != "exit" && command != "quit")
	{
		if (command.contains("="))
		{
			try{
				QString var_name = eval_variable(command,start_pos);
				if (constants.find(var_name) == constants.end())
				{
					variables.insert(var_name,eval_expression(command,++start_pos));
				}
				else{
					cout << "Can't use the name of a constant for a variable name.\n";
				}
				start_pos = 0;
			}
			catch (CalcException* ce)
			{
				cout << ce->get_message().toStdString() << endl;
				delete ce;
			}
		}
		else if (command == "degrees")
		{
			radians = false;
			cout << "Degree mode activated.\n";
		}
		else if (command == "radians")
		{
			radians = true;
			cout << "Radian mode activated.\n";
		}
		else if (command != "")
		{
			try{
				double result = eval_expression(command,start_pos);
				cout << result << endl;
				variables.insert("ans",result);
			}
			catch (CalcException* ce)
			{
				cout << ce->get_message().toStdString() << endl;
				delete ce;
			}
			start_pos = 0;
		}
		cout << "> ";
		command = std_in.readLine();
		command.replace(" ","");
	}
	return 0;
}