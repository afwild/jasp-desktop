#include "correlation.h"

#include "options/optionfields.h"
#include "options/optionboolean.h"
#include "options/optionlist.h"

Correlation::Correlation(int id)
	: Analysis(id, "Correlation")
{
}

Options *Correlation::createDefaultOptions()
{
	Options *options = new Options();

	options->add("variables", new OptionFields());

	options->add("pearson", new OptionBoolean(true));
	options->add("kendallsTauB", new OptionBoolean());
	options->add("spearman", new OptionBoolean());

	options->add("tails", new OptionList(list("twoTailed", "oneTailed")));

	options->add("flagSignificant", new OptionBoolean());

	options->add("meansAndStdDev", new OptionBoolean());
	options->add("crossProducts", new OptionBoolean());

	options->add("missingValues", new OptionList(list("excludePairwise", "excludeListwise")));

	return options;
}
