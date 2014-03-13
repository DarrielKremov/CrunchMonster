#include "CalcException.h"

CalcException::CalcException ()
{
	message = "";
}
CalcException::CalcException (QString message)
{
	this->message = message;
}
QString CalcException::get_message()
{
	return message;
}

