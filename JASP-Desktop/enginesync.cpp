#include "enginesync.h"

#include <QApplication>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

#ifdef __WIN32__
#include <windows.h>
#include <winbase.h>
#endif

#include <boost/foreach.hpp>

#include "lib_json/json.h"

using namespace boost::interprocess;
using namespace std;

EngineSync::EngineSync(Analyses *analyses, QObject *parent = 0)
	: QObject(parent)
{
	_analyses = analyses;

	_analyses->analysisAdded.connect(boost::bind(&EngineSync::sendMessages, this));
	_analyses->analysisOptionsChanged.connect(boost::bind(&EngineSync::sendMessages, this));

	_timer = new QTimer(this);
	connect(_timer, SIGNAL(timeout()), this, SLOT(process()));
	_timer->start(50);

    try {

		shared_memory_object::remove("JASP_IPC");
		_channels.push_back(new IPCChannel("JASP_IPC", 0));
		//_channels.push_back(new IPCChannel("JASP_IPC", 1));
		//_channels.push_back(new IPCChannel("JASP_IPC", 2));
		//_channels.push_back(new IPCChannel("JASP_IPC", 3));

    }
    catch (interprocess_exception e)
    {
        qDebug() << "interprocess exception! " << e.what() << "\n";
        throw e;
    }

	for (int i = 0; i < _channels.size(); i++)
	{
		_analysesInProgress.push_back(NULL);
		startSlaveProcess(i);
	}

}

EngineSync::~EngineSync()
{
	for (int i = 0; i < _slaveProcesses.size(); i++)
	{
		_slaveProcesses[i]->terminate();
		_slaveProcesses[i]->kill();
	}
}

void EngineSync::sendToProcess(int processNo, Analysis *analysis)
{
	std::cout << "send " << analysis->id() << " to process " << processNo << "\n";
	std::cout.flush();

	bool init;

	if (analysis->status() == Analysis::Empty)
	{
		init = true;
		analysis->setStatus(Analysis::Initing);
	}
	else
	{
		init = false;
		analysis->setStatus(Analysis::Running);
	}

	_analysesInProgress[processNo] = analysis;

	Json::Value json = Json::Value(Json::objectValue);

	json["id"] = analysis->id();
	json["name"] = analysis->name();
	json["options"] = analysis->options()->asJSON();
	json["perform"] = (init ? "init" : "run");

	string str = json.toStyledString();

	_channels[processNo]->send(str);

	cout << str << "\n";
	cout.flush();

}

void EngineSync::process()
{
	for (int i = 0; i < _channels.size(); i++)
	{
		Analysis *analysis = _analysesInProgress[i];

		if (analysis == NULL)
			continue;

		IPCChannel *channel = _channels[i];
		string data;

		if (channel->receive(data))
		{
			std::cout << "message received\n";
            std::cout << data << "\n";
			std::cout.flush();

			Json::Reader reader;
			Json::Value json;
			reader.parse(data, json);

			int id = json.get("id", -1).asInt();
			//bool init = json.get("perform", "init").asString() == "init";
			Json::Value results = json.get("results", Json::nullValue);
			bool complete = json.get("complete", false).asBool();

			analysis->setResults(results);

			if (analysis->status() == Analysis::Initing)
			{
				analysis->setStatus(Analysis::Inited);
				_analysesInProgress[i] = NULL;
				sendMessages();
			}
			else if (complete)
			{
				analysis->setStatus(Analysis::Complete);
				_analysesInProgress[i] = NULL;
				sendMessages();
			}
		}

	}
}

void EngineSync::sendMessages()
{
	std::cout << "send messages\n";
	std::cout.flush();

	for (int i = 0; i < _analysesInProgress.size(); i++)
	{
		Analysis *analysis = _analysesInProgress[i];
		if (analysis != NULL && analysis->status() == Analysis::Empty)
			sendToProcess(i, analysis);
	}

	for (Analyses::iterator itr = _analyses->begin(); itr != _analyses->end(); itr++)
	{
		Analysis *analysis = *itr;

		if (analysis->status() == Analysis::Empty)
		{
			bool sent = false;

			for (int i = 0; i < _analysesInProgress.size(); i++)
			{
				if (_analysesInProgress[i] == NULL)
				{
					sendToProcess(i, analysis);
					sent = true;
					break;
				}
			}

			if (sent == false)  // no free processes left
				return;
		}
		else if (analysis->status() == Analysis::Inited)
		{
            for (int i = 0; i < _analysesInProgress.size(); i++) // don't perform 'runs' on process 0, only inits.
			{
				if (_analysesInProgress[i] == NULL)
				{
					sendToProcess(i, analysis);
					break;
				}
			}
		}
	}

}

void EngineSync::startSlaveProcess(int no)
{
	QDir programDir = QFileInfo( QCoreApplication::applicationFilePath() ).absoluteDir();
	QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
	QString engineExe = QFileInfo( QCoreApplication::applicationFilePath() ).absoluteDir().absoluteFilePath("JASPEngine");

	QStringList args;
	args << QString::number(no);

#ifdef __WIN32__
    env.insert("PATH", programDir.absoluteFilePath("R-3.0.0\\library\\RInside\\libs\\i386") + ";" + programDir.absoluteFilePath("R-3.0.0\\library\\Rcpp\\libs\\i386") + ";" + programDir.absoluteFilePath("R-3.0.0\\bin\\i386"));

    DWORD processId = GetCurrentProcessId();
    args << QString::number(processId);
#elif __APPLE__
    env.insert("DYLD_LIBRARY_PATH", programDir.absoluteFilePath("R-3.0.0/lib"));
#else
    env.insert("LD_LIBRARY_PATH", programDir.absoluteFilePath("R-3.0.0/lib") + ";" + programDir.absoluteFilePath("R-3.0.0/library/RInside/lib") + ";" + programDir.absoluteFilePath("R-3.0.0/library/Rcpp/lib"));
#endif

	env.insert("R_HOME", programDir.absoluteFilePath("R-3.0.0"));

	QProcess *slave = new QProcess(this);
	slave->setProcessEnvironment(env);
	slave->start(engineExe, args);

	_slaveProcesses.push_back(slave);

	connect(slave, SIGNAL(readyReadStandardOutput()), this, SLOT(subProcessStandardOutput()));
	connect(slave, SIGNAL(readyReadStandardError()), this, SLOT(subProcessStandardError()));
	connect(slave, SIGNAL(error(QProcess::ProcessError)), this, SLOT(subProcessError(QProcess::ProcessError)));
	connect(slave, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(subprocessFinished(int,QProcess::ExitStatus)));
	connect(slave, SIGNAL(started()), this, SLOT(subProcessStarted()));

}

void EngineSync::subProcessStandardOutput()
{
	QProcess *process = qobject_cast<QProcess *>(this->sender());
	QByteArray data = process->readAllStandardOutput();
	qDebug() << QString(data);
}

void EngineSync::subProcessStandardError()
{
	QProcess *process = qobject_cast<QProcess *>(this->sender());
	qDebug() << process->readAllStandardError();
}

void EngineSync::subProcessStarted()
{
	qDebug() << "subprocess started";
}

void EngineSync::subProcessError(QProcess::ProcessError error)
{
	qDebug() << "subprocess error" << error;
}

void EngineSync::subprocessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	qDebug() << "subprocess finished" << exitCode;
}

