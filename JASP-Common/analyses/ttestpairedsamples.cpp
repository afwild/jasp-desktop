#include "ttestpairedsamples.h"

#include "options/options.h"
#include "options/option.h"
#include "options/optionfields.h"
#include "options/optionboolean.h"
#include "options/optioninteger.h"
#include "options/optionintegerarray.h"
#include "options/optionlist.h"
#include "options/optionnumber.h"
#include "options/optionfieldpairs.h"

using namespace std;

TTestPairedSamples::TTestPairedSamples(int id)
	: Analysis(id, "TTestPairedSamples")
{
}

Options *TTestPairedSamples::createDefaultOptions()
{
	Options *options = new Options();

	options->add("pairs", new OptionFieldPairs());

	options->add("meanDifference", new OptionBoolean());
	options->add("confidenceInterval", new OptionBoolean());
	options->add("confidenceIntervalInterval", new OptionNumber(.95, 0, 1, "%"));
	options->add("descriptives", new OptionBoolean());
	options->add("effectSize", new OptionBoolean());

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
