#ifndef LISTMODELVARIABLESASSIGNED_H
#define LISTMODELVARIABLESASSIGNED_H

#include "listmodelvariables.h"

#include "listmodelvariablesavailable.h"
#include "options/optionfields.h"

#include "column.h"

class ListModelVariablesAssigned : public ListModelVariables, public BoundModel
{
	Q_OBJECT
public:
	explicit ListModelVariablesAssigned(QObject *parent = 0);
	virtual void bindTo(Option *option) OVERRIDE;

	virtual bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const OVERRIDE;
	virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) OVERRIDE;

	void setSource(ListModelVariablesAvailable *source);

	const QList<ColumnInfo> &assigned() const;

	virtual void mimeDataMoved(const QModelIndexList &indexes) OVERRIDE;
signals:
	void assignmentsChanged();

private slots:
	void assignToBoundOption();
	void sourceVariablesChanged();

private:

	OptionFields *_boundTo;
	bool _onlyOne;

	ListModelVariablesAvailable *_source;

	QList<ColumnInfo> _toEject;

};

#endif // LISTMODELVARIABLESASSIGNED_H
