#include"BTSolver.hpp"

using namespace std;

// =====================================================================
// Constructors
// =====================================================================

BTSolver::BTSolver ( SudokuBoard input, Trail* _trail,  string val_sh, string var_sh, string cc )
: sudokuGrid( input.get_p(), input.get_q(), input.get_board() ), network( input )
{
	valHeuristics = val_sh;
	varHeuristics = var_sh;
	cChecks =  cc;

	trail = _trail;
}

// =====================================================================
// Consistency Checks
// =====================================================================

// Basic consistency check, no propagation done
bool BTSolver::assignmentsCheck ( void )
{
	for ( Constraint c : network.getConstraints() )
		if ( ! c.isConsistent() )
			return false;

	return true;
}

/**
 * Part 1 TODO: Implement the Forward Checking Heuristic
 *
 * This function will do both Constraint Propagation and check
 * the consistency of the network
 *
 * (1) If a variable is assigned then eliminate that value from
 *     the square's neighbors.
 *
 * Note: remember to trail.push variables before you assign them
 * Return: true if assignment is consistent, false otherwise
 */
bool BTSolver::forwardChecking ( void )
{
	for ( Constraint* c : network.getModifiedConstraints() )
		if ( ! c->isConsistent() )
			return false;
	for( Variable* var :  network.getVariables()){
		if(var->size() == 0)
			return false;
		if(var->getAssignment() != 0){
			for(Variable* n: network.getNeighborsOfVariable(var)){
				if(n->size() == 0)
					return false;
				if(n->getDomain().contains(var->getAssignment())){
					if(n->size() == 1)
						return false;
					trail->push( n );
					n->removeValueFromDomain(var->getAssignment());
				}
			}
		}
	}
	for ( Constraint* c : network.getModifiedConstraints() )
		if ( ! c->isConsistent() )
			return false;
	return true;
}

/**
 * Part 2 TODO: Implement both of Norvig's Heuristics
 *
 * This function will do both Constraint Propagation and check
 * the consistency of the network
 *
 * (1) If a variable is assigned then eliminate that value from
 *     the square's neighbors.
 *
 * (2) If a constraint has only one possible place for a value
 *     then put the value there.
 *
 * Note: remember to trail.push variables before you change their domain
 * Return: true is assignment is consistent, false otherwise
 */
bool BTSolver::norvigCheck ( void )
{
	for ( Constraint* c : network.getModifiedConstraints() )
		if ( ! c->isConsistent() )
			return false;
	for( Variable* var :  network.getVariables()){
		if(var->size() == 0)
			return false;
		if(var->getAssignment() != 0){
			for(Variable* n: network.getNeighborsOfVariable(var)){
				if(n->size() == 0)
					return false;
				if(n->getDomain().contains(var->getAssignment())){
					if(n->size() == 1)
						return false;
					trail->push( n );
					n->removeValueFromDomain(var->getAssignment());
				}
			}
		}
	}
	for ( Constraint* c : network.getModifiedConstraints() )
		if ( ! c->isConsistent() )
			return false;

	vector<int> possible_places;
	for ( Constraint c: network.getConstraints() ) {
		possible_places.resize( sudokuGrid.get_n(), 0 );
		for ( Variable* var: c.vars ) {
			for ( int index: var->getDomain() )
				possible_places[index-1]++;
		}
		
		for ( int i = 0; i < possible_places.size(); i++ ) {
			if ( possible_places[i] == 1 ) {
				for ( Variable* var: c.vars ){
					if ( var->getDomain().contains( i+1 ) ) {
						trail->push( var );
						var->assignValue( i+1 );
					}
				}
			}
		}
		possible_places.clear();
	}
	
	for ( Constraint* c : network.getModifiedConstraints() )
		if ( ! c->isConsistent() )
			return false;
	return true;
}

/**
 * Optional TODO: Implement your own advanced Constraint Propagation
 *
 * Completing the three tourn heuristic will automatically enter
 * your program into a tournament.
 */
bool BTSolver::getTournCC ( void )
{
	for ( Constraint* c : network.getModifiedConstraints() )
		if ( ! c->isConsistent() )
			return false;
	for( Variable* var :  network.getVariables()){
		if(var->size() == 0)
			return false;
		if(var->getAssignment() != 0){
			for(Variable* n: network.getNeighborsOfVariable(var)){
				if(n->size() == 0)
					return false;
				if(n->getDomain().contains(var->getAssignment())){
					if(n->size() == 1)
						return false;
					trail->push( n );
					n->removeValueFromDomain(var->getAssignment());
				}
			}
		}
	}
	for ( Constraint* c : network.getModifiedConstraints() )
		if ( ! c->isConsistent() )
			return false;

	vector<int> possible_places;
	for ( Constraint c: network.getConstraints() ) {
		possible_places.resize( sudokuGrid.get_n(), 0 );
		for ( Variable* var: c.vars ) {
			for ( int index: var->getDomain() )
				possible_places[index-1]++;
		}
		
		for ( int i = 0; i < possible_places.size(); i++ ) {
			if ( possible_places[i] == 1 ) {
				for ( Variable* var: c.vars ){
					if ( var->getDomain().contains( i+1 ) ) {
						trail->push( var );
						var->assignValue( i+1 );
					}
				}
			}
		}
		possible_places.clear();
	}
	
	for ( Constraint* c : network.getModifiedConstraints() )
		if ( ! c->isConsistent() )
			return false;
	return true;
}

// =====================================================================
// Variable Selectors
// =====================================================================

// Basic variable selector, returns first unassigned variable
Variable* BTSolver::getfirstUnassignedVariable ( void )
{
	for ( Variable* v : network.getVariables() )
		if ( !(v->isAssigned()) )
			return v;

	// Everything is assigned
	return nullptr;
}

/**
 * Part 1 TODO: Implement the Minimum Remaining Value Heuristic
 *
 * Return: The unassigned variable with the smallest domain
 */
Variable* BTSolver::getMRV ( void )
{
	int min = 32767;
	Variable* lowest = nullptr;
	for ( Variable* v : network.getVariables() ){
		if ( v->getAssignment() == 0 ){
			if(v->size() <= min){
				lowest = v;
				min = v->size();
			}
		}
	}
	// cout << "Trail Pushes: " << trail->getPushCount() << endl;
	// cout << "Backtracks: "  << trail->getUndoCount() << endl;
	// if(lowest != nullptr)
		// cout << lowest->toString() << endl;
	return lowest;
}

/**
 * Part 2 TODO: Implement the Degree Heuristic
 *
 * Return: The unassigned variable with the most unassigned neighbors
 */
Variable* BTSolver::getDegree ( void )
{
	vector<Variable*> unassigned;
	unassigned.clear();
	// get minimum size
	for ( Variable* v : network.getVariables() ){
		if ( v->getAssignment() == 0 )
			unassigned.push_back(v);
	}
	if(unassigned.size() == 0)
		return nullptr;
	else if(unassigned.size() == 1)
		return unassigned[0];
	
	int empty_neighbors = 0, current_max = -1;
	Variable* result = nullptr;
	for ( Variable* v : unassigned ){
		for(Variable* n: network.getNeighborsOfVariable(v)){
			if ( n->getAssignment() == 0 )
				empty_neighbors++;
		}
		if(empty_neighbors >= current_max){
			current_max = empty_neighbors;
			result = v;
		}
		empty_neighbors = 0;
	// cout << v->toString() << endl;
	}
	// cout << "Trail Pushes: " << trail->getPushCount() << endl;
	// cout << "Backtracks: "  << trail->getUndoCount() << endl;
	return result;
}

/**
 * Part 2 TODO: Implement the Minimum Remaining Value Heuristic
 *                with Degree Heuristic as a Tie Breaker
 *
 * Return: The unassigned variable with the smallest domain and involved
 *             in the most constraints
 */
Variable* BTSolver::MRVwithTieBreaker ( void )
{
	int min = 32767;
	vector<Variable*> lowest, unassigned;
	// get minimum size
	for ( Variable* v : network.getVariables() ){
		if ( v->getAssignment() == 0 ){
			if(v->size() < min){
				min = v->size();
			}
			unassigned.push_back(v);
		}
	}
	if(unassigned.size() == 0)
		return nullptr;
	
	for ( Variable* v : network.getVariables() ){
		if ( v->getAssignment() == 0 )
			if(v->size() == min)
				lowest.push_back(v);
	}
	if(lowest.size() == 1)
		return lowest[0];
	
	//tie-breaker
	int empty_neighbors = 0, current_max = -1;
	Variable* result = lowest[0];
	for ( Variable* v : lowest ){
		// cout << v->toString() << endl;
		for(Variable* n: network.getNeighborsOfVariable(v)){
			if ( n->getAssignment() == 0 )
				empty_neighbors++;
		}
		// cout << empty_neighbors << endl;
		if(empty_neighbors >= current_max){
			current_max = empty_neighbors;
			result = v;
		}
		// cout << result->toString() << endl;
		empty_neighbors = 0;
	}
	// cout << "Went through" << endl;
	// cout << "Trail Pushes: " << trail->getPushCount() << endl;
	// cout << "Backtracks: "  << trail->getUndoCount() << endl;
	// cout << result->toString() << endl;
	return result;
}

/**
 * Optional TODO: Implement your own advanced Variable Heuristic
 *
 * Completing the three tourn heuristic will automatically enter
 * your program into a tournament.
 */
Variable* BTSolver::getTournVar ( void )
{
	int min = 32767;
	vector<Variable*> lowest, unassigned;
	// get minimum size
	for ( Variable* v : network.getVariables() ){
		if ( v->getAssignment() == 0 ){
			if(v->size() < min){
				min = v->size();
			}
			unassigned.push_back(v);
		}
	}
	if(unassigned.size() == 0)
		return nullptr;
	
	for ( Variable* v : network.getVariables() ){
		if ( v->getAssignment() == 0 )
			if(v->size() == min)
				lowest.push_back(v);
	}
	if(lowest.size() == 1)
		return lowest[0];
	
	//tie-breaker least contrained variable
	int empty_neighbors = 0, current_max = 32767;
	Variable* result = lowest[0];
	for ( Variable* v : lowest ){
		for(Variable* n: network.getNeighborsOfVariable(v)){
			if ( n->getAssignment() == 0 )
				empty_neighbors++;
		}
		if(empty_neighbors <= current_max){
			current_max = empty_neighbors;
			result = v;
		}
		empty_neighbors = 0;
	}
	return result;
}

// =====================================================================
// Value Selectors
// =====================================================================

// Default Value Ordering
vector<int> BTSolver::getValuesInOrder ( Variable* v )
{
	vector<int> values = v->getDomain().getValues();
	sort( values.begin(), values.end() );
	return values;
}

/**
 * Part 1 TODO: Implement the Least Constraining Value Heuristic
 *
 * The Least constraining value is the one that will knock the least
 * values out of it's neighbors domain.
 *
 * Return: A list of v's domain sorted by the LCV heuristic
 *         The LCV is first and the MCV is last
 */
 
bool pair_sort_helper(const pair<int,int> &a, const pair<int,int> &b)
{
    return (a.second < b.second);
}

vector<int> BTSolver::getValuesLCVOrder ( Variable* v )
{
	vector<int> values = v->getDomain().getValues();
	int constraints = 0;
	vector<pair<int, int>> val_const;
	vector<int> result;
	// cout << v->toString() << endl;
	for(auto const& i : values){
		for( Variable* var :  network.getNeighborsOfVariable(v)){
			if(var->getDomain().contains(i))
				constraints++;
		}
		val_const.push_back(make_pair(i,constraints));
		constraints = 0;
	}
	sort(val_const.begin(), val_const.end(), pair_sort_helper);
	for(auto v : val_const){
		result.push_back(v.first);
		// cout << v.first << " " << v.second << endl;
	}
	
	return result;
}

bool tourn_pair_sort_helper(const pair<int,int> &a, const pair<int,int> &b)
{
    return (a.second > b.second);
}

/**
 * Optional TODO: Implement your own advanced Value Heuristic
 *
 * Completing the three tourn heuristic will automatically enter
 * your program into a tournament.
 */
vector<int> BTSolver::getTournVal ( Variable* v )
{
	vector<int> values = v->getDomain().getValues();
	int constraints = 0;
	vector<pair<int, int>> val_const;
	vector<int> result;
	// cout << v->toString() << endl;
	for(auto const& i : values){
		for( Variable* var :  network.getNeighborsOfVariable(v)){
			if(var->getDomain().contains(i))
				constraints++;
		}
		val_const.push_back(make_pair(i,constraints));
		constraints = 0;
	}
	sort(val_const.begin(), val_const.end(), tourn_pair_sort_helper);
	for(auto v : val_const){
		result.push_back(v.first);
		// cout << v.first << " " << v.second << endl;
	}
	
	return result;
}

// =====================================================================
// Engine Functions
// =====================================================================

void BTSolver::solve ( void )
{
	if ( hasSolution )
		return;

	// Variable Selection
	Variable* v = selectNextVariable();

	if ( v == nullptr )
	{
		for ( Variable* var : network.getVariables() )
		{
			// If all variables haven't been assigned
			if ( ! ( var->isAssigned() ) )
			{
				cout << "Error" << endl;
				return;
			}
		}

		// Success
		hasSolution = true;
		return;
	}

	// Attempt to assign a value
	for ( int i : getNextValues( v ) )
	{
		// Store place in trail and push variable's state on trail
		trail->placeTrailMarker();
		trail->push( v );

		// Assign the value
		v->assignValue( i );

		// Propagate constraints, check consistency, recurse
		if ( checkConsistency() )
			solve();

		// If this assignment succeeded, return
		if ( hasSolution )
			return;

		// Otherwise backtrack
		trail->undo();
	}
}

bool BTSolver::checkConsistency ()
{
	if ( cChecks == "forwardChecking" )
		return forwardChecking();

	if ( cChecks == "norvigCheck" )
		return norvigCheck();

	if ( cChecks == "tournCC" )
		return getTournCC();

	return assignmentsCheck();
}

Variable* BTSolver::selectNextVariable ( void )
{
	if ( varHeuristics == "MinimumRemainingValue" )
		return getMRV();

	if ( varHeuristics == "Degree" )
		return getDegree();

	if ( varHeuristics == "MRVwithTieBreaker" )
		return MRVwithTieBreaker();

	if ( varHeuristics == "tournVar" )
		return getTournVar();

	return getfirstUnassignedVariable();
}

vector<int> BTSolver::getNextValues ( Variable* v )
{
	if ( valHeuristics == "LeastConstrainingValue" )
		return getValuesLCVOrder( v );

	if ( valHeuristics == "tournVal" )
		return getTournVal( v );

	return getValuesInOrder( v );
}

bool BTSolver::haveSolution ( void )
{
	return hasSolution;
}

SudokuBoard BTSolver::getSolution ( void )
{
	return network.toSudokuBoard ( sudokuGrid.get_p(), sudokuGrid.get_q() );
}

ConstraintNetwork BTSolver::getNetwork ( void )
{
	return network;
}
