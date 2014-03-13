#include <QString>

class CalcException {
	QString message;
public:
	CalcException();
	CalcException(QString);
	QString get_message();
};
