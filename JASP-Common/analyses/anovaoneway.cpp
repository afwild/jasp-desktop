
#include "anovaoneway.h"

#include "options/options.h"
#include "options/option.h"
#include "options/optionfields.h"
#include "options/optionboolean.h"
#include "options/optioninteger.h"
#include "options/optionintegerarray.h"
#include "options/optionlist.h"
#include "options/optionnumber.h"
#include "options/optionfield.h"

using namespace std;

AnovaOneWay::AnovaOneWay(int id)
	: Analysis(id, "AnovaOneWay")
{
}

Options *AnovaOneWay::createDefaultOptions()
{
	Options *options = new Options();

	options->add("variables", new OptionFields());
	options->add("groupingVariable", new OptionField());

	vector<string> equalityOfVariances;
	equalityOfVariances.push_back("assumeEqual");
	equalityOfVariances.push_back("assumeUnequal");
	equalityOfVariances.push_back("both");

	options->add("equalityOfVariances", new OptionList(equalityOfVariances));

	options->add("testUnequalVariances", new OptionBoolean());

	options->add("meanDifference", new OptionBoolean());
	options->add("confidenceInterval", new OptionBoolean());
	options->add("confidenceIntervalInterval", new OptionNumber(.95, 0, 1, "%"));
	options->add("descriptives", new OptionBoolean());

	vector<string> missingValues;
	missingValues.push_back("excludeAnalysisByAnalysis");
	missingValues.push_back("excludeListwise");

	options->add("missingValues", new OptionList(missingValues));

	vector<string> tails;
	tails.push_back("twoTailed");
	tails.push_back("oneTailedGreaterThan");
	tails.push_back("oneTailedLessThan");

	return options;
}
