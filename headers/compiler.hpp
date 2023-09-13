#pragma once
/**


PSEUDO:

value manageExpression(Expression expr) {
	if expr.secondExpression is nullptr
		return expr.value

	return calculateExpression(expr.value, expr.opWillUse, manageExpression(expr.secondExpression))
}



*/