
#include "correlationpartial.h"

#include "options/optionfields.h"
#include "options/optionboolean.h"
#include "options/optionlist.h"

CorrelationPartial::CorrelationPartial(int id)
	: Analysis(id, "CorrelationPartial")
{
}

Options *CorrelationPartial::createDefaultOptions()
{
	Options *options = new Options();

	options->add("variables", new OptionFields());
	options->add("controllingFor", new OptionFields());

	options->add("tails", new OptionList(list("twoTailed", "oneTailed")));

	options->add("flagSignificant", new OptionBoolean());

	options->add("meansAndStdDev", new OptionBoolean());
	options->add("crossProducts", new OptionBoolean());

	options->add("missingValues", new OptionList(list("excludePairwise", "excludeListwise")));

	return options;
}
