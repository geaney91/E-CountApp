#include "writetologfile.h"
#include <QTextStream>
#include <QFile>


WriteToLogFile::WriteToLogFile()
{

}

void WriteToLogFile::write_to_file(QString text)
{
    QFile file(".\eCountLog.txt");
    file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text);
    QTextStream stream( &file );
    //for (int i = 0; i < final.size(); i++)
    //{
        stream << text << endl;
    //}
    file.close();
}
