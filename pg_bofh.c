#include "postgres.h"
#include "optimizer/planner.h"
#include "nodes/nodeFuncs.h"
#include "fmgr.h"

PG_MODULE_MAGIC;

void		_PG_init(void);
void		_PG_fini(void);

static PlannedStmt *bofh_plannerhook(Query *query, int cursorOptions, ParamListInfo boundParams);

static planner_hook_type prev_planner_hook = NULL;

static bool bofh_query_walker(Node *node, bool* hasWhere);

void
_PG_init(void)
{
	prev_planner_hook = planner_hook;
	planner_hook = bofh_plannerhook;
}


void
_PG_fini(void)
{
	/* Uninstall hooks. */
	planner_hook = prev_planner_hook;
}


static PlannedStmt *
bofh_plannerhook(Query *parse,
			 int cursorOptions,
			 ParamListInfo boundParams)
{
	PlannedStmt *plannedstmt = NULL;
	bool hasWhere = false;

	if (prev_planner_hook)
		plannedstmt = prev_planner_hook(parse, cursorOptions, boundParams);
	else
		plannedstmt = standard_planner(parse, cursorOptions, boundParams);
	query_or_expression_tree_walker((Node *) parse, bofh_query_walker, &hasWhere, 0);
	if(!hasWhere){
		elog(ERROR, "Thou shall not write queries without WHERE clauses!");
	}
	return plannedstmt;
}


static bool
bofh_query_walker(Node *node, bool* hasWhere)
{
	if (node == NULL)
	{
		return false;
	}
	switch (node->type)
	{
		case T_FromExpr:
			expression_tree_walker((Node *) ((FromExpr *) node)->quals, bofh_query_walker, hasWhere);
			return false;
		case T_OpExpr:
			*hasWhere = true;
			return true;
		default:
			return false;
	}
	return false;
}
