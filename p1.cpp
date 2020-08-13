#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <streambuf>
#include <stack>

using namespace std;

/*

Notes________________________________________________

- C++ class declarations are private by default

- what is the '+' in types grammar
- add checks to token generator
- Everything is marked as ID besides begin and end (keywords)
- OutExp checks for string and then assumes an expression

Project______________________________________________

- Lexical Analyzer (Tokenize) -> Recursive Descent Parser => Abstract Syntax Tree
- Given Lexical Rules & grammar for language (SUBC)

- What data structure is optimal for a Recursive Descent Parser?
- AKA how should tokens be stored?

*/

// Utility Functions

enum TokenType { IDENTIFIER, CHAR, INT, STRING, OP, KEYWORD };
string enumString[6] = { "IDENTIFIER", "CHAR", "INT", "STRING", "OP", "KEYWORD" };

class Node {

public:

	string token;

	//Node* parent;
	vector<Node> children;
	int numChildren;

	Node() {
		token = "";
		numChildren = 0;
	}
	Node(string token) {
		this->token = token;
		numChildren = 0;
	}
	/*	Node(Node& node) {
	token = node.token;
	parent = node.parent;
	for (int i = 0; i < node.numChildren; i++) {
	addChild(node.children.at(i));
	}
	}
	*/
	void addChild(Node child) {
		children.push_back(child);
		numChildren++;
	}

	string printNode(int depth) {
		string out = "";
		for (int i = 0; i < depth; i++)
		{
			out += ". ";
		}
		out += token + "(" + to_string(numChildren) + ")" + "\n";
		for (int i = numChildren; i > 0; i--) {
			out += children[i - 1].printNode(depth + 1);
		}

		return out;
	}
};

class Tree {

	Node rootNode;
	int numNodes;

public:

	Tree() {
		numNodes = 0;
	}
	Tree(Node root) {
		rootNode = root;
		numNodes = 1;
	}
	Tree(string token) {
		rootNode = Node(token);
		numNodes = 1;
	}

	string printTree() {
		return rootNode.printNode(0);
	}

	Node getRoot() {
		return rootNode;
	}
	string getRootToken() {
		return rootNode.token;
	}

};

class stringIterator {

private:
	string elements;
	int index;

public:
	// Constructor
	stringIterator(string elements) {
		this->elements = elements;
		index = 0;
	}
	// Function Definitions
	void begin() {
		index = 0;
	}
	char next() {
		if (hasNext()) {
			index++;
			return elements[index];
		}
	}
	char pop() {
		if (hasNext()) {
			index++;
			return elements[index - 1];
		}
	}
	bool hasNext() {
		return (index + 1) < elements.size();
	}
	char currentItem() {
		return elements[index];
	}
	// Operator Overloading
	char operator++() {
		if (hasNext()) {
			index++;
			return elements[index];
		}
	}
	char operator*() {
		return elements[index];

	}
};

class vectorIterator {

private:

	vector<pair<string, TokenType>> elements;
	int index;

public:
	// Constructor
	vectorIterator() {}
	vectorIterator(vector<pair<string, TokenType>>elements) {
		this->elements = elements;
		index = 0;
	}
	// Function Definitions
	void begin() {
		index = 0;
	}
	pair<string, TokenType> next() {
		if (hasNext()) {
			index++;
			return elements[index];
		}
	}
	pair<string, TokenType> prev() {
		if (index > 0) {
			index--;
			return elements[index];
		}
	}
	pair<string, TokenType> pop() {
		if (hasNext()) {
			index++;
			return elements[index - 1];
		}
	}
	bool hasNext() {
		return (index + 1) < elements.size();
	}
	pair<string, TokenType> currentItem() {
		return elements[index];
	}
	// Operator Overloading
	pair<string, TokenType> operator++() {
		return next();
	}
	pair<string, TokenType> operator*() {
		return currentItem();
	}
	// getter
	int getIndex() {
		return index;
	}
};

string fileToString(string file) {

	std::ifstream t(file);
	std::string str;

	t.seekg(0, std::ios::end);
	str.reserve(t.tellg());
	t.seekg(0, std::ios::beg);

	str.assign((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());

	return str;
}

// Lexical Analyzer
vector<pair<string, TokenType>> generateTokens(string input) {

	vector<pair<string, TokenType>> tokens;
	stringIterator iter = stringIterator(input);
	string buffer = "";

	int n = 0;
	while (iter.hasNext())
	{
		buffer += *iter;
		//	cout << n++ << ": " + buffer << endl;

		if ((*iter > 64 && *iter < 91) || (*iter > 96 && *iter < 123)) // Keyword or Identifiers
		{
			// Consume characters until operator or whitespace is struck, create token, increment index
			iter.next();
			while ((*iter > 64 && *iter < 91) || (*iter > 96 && *iter < 123) || (*iter > 47 && *iter < 58) || *iter == '_') // Alphabets, digits, or underscore
			{
				buffer += iter.pop(); // pop = return current item and increment index
			}
			if (buffer == "program" || buffer == "var" || buffer == "const" || buffer == "type" || buffer == "function"
				|| buffer == "return" || buffer == "begin" || buffer == "end" || buffer == "output" || buffer == "if"
				|| buffer == "then" || buffer == "else" || buffer == "while" || buffer == "do" || buffer == "case" || buffer == "of"
				|| buffer == "otherwise" || buffer == "repeat" || buffer == "for" || buffer == "return" || buffer == "until" || buffer == "loop"
				|| buffer == "pool" || buffer == "exit" || buffer == "mod" || buffer == "and" || buffer == "or" || buffer == "not" || buffer == "read"
				|| buffer == "succ" || buffer == "pred" || buffer == "chr" || buffer == "ord" || buffer == "eof")
			{
				tokens.push_back(make_pair(buffer, KEYWORD));
			}
			else
			{
				tokens.push_back(make_pair(buffer, IDENTIFIER));
			}
			buffer = "";
		}
		else if (*iter == ';' || *iter == '+' || *iter == '-' || *iter == '*' || *iter == '/' || *iter == ',' || *iter == '(' || *iter == ')' || *iter == '=') // Operators (Unstackable)
		{
			// Consume character (*iter), create token, increment index
			tokens.push_back(make_pair(buffer, OP));
			buffer = "";
			iter.next();
		}
		else if (*iter == ':' || *iter == '.' || *iter == '<' || *iter == '>') // Operator (Stackable)
		{
			// Conesume characters until operator is complete, create token, increment index
			iter.next();
			while (*iter == '=' || *iter == '.' || *iter == '>' || *iter == ':')  // Possible Stacked Operators ( >= <= :=: := .. <> )
			{
				buffer += iter.pop();
			}
			tokens.push_back(make_pair(buffer, OP));
			buffer = "";
		}
		else if (*iter > 47 && *iter < 58)  // Integer
		{
			// Consume characters until integer ends, create token, increment index
			iter.next();
			while (*iter > 47 && *iter < 58)  // Integers
			{
				buffer += iter.pop();
			}
			tokens.push_back(make_pair(buffer, INT));
			buffer = "";
		}
		else if (*iter == '_') // Identifier
		{
			// Consume characters until operator or whitespace is struck, create token, increment index
			iter.next();
			while ((*iter > 64 && *iter < 91) || (*iter > 96 && *iter < 123) || (*iter > 47 && *iter < 58) || *iter == '_') // Alphabets, digits, or underscore
			{
				buffer += iter.pop();
			}
			tokens.push_back(make_pair(buffer, IDENTIFIER));
			buffer = "";

		}
		else if (*iter == ' ') // Whitespace
		{
			// Consume contiguous whitespace, increment index
			iter.next();
			while (*iter == ' ') // Whitespace
			{
				iter.next();
			}
			buffer = "";

		}
		else if (*iter == 39) // Char  (' = 39) 
		{
			// Consume ' the designated char and next ', create token, increment index
			buffer += iter.next();
			buffer += iter.next();
			tokens.push_back(make_pair(buffer, CHAR));
			buffer = "";
			iter.next();
		}
		else if (*iter == '{') // Comment
		{
			// Consume { and everything until next }, create token, increment index
			iter.next();
			while (*iter != '}') {
				buffer += iter.pop();
			}
			buffer += iter.pop();
			// tokens.push_back(buffer);
			buffer = "";
		}
		else if (*iter == '"')	// String
		{
			// Consume " and everything until next ", create token, increment index
			iter.next();
			while (*iter != '"') {
				buffer += iter.pop();
			}
			buffer += iter.pop();
			tokens.push_back(make_pair(buffer, STRING));
			buffer = "";
		}
		else if (*iter == '\t') {
			iter.next();
			buffer = "";
		}
		else if (*iter == '#') // Random Symbol Handling
		{
			// Consume character, create token, increment index
			//tokens.push_back(buffer);
			while (*iter != '\n') {
				iter.next();
			}
			buffer = "";
			iter.next();
		}
		else if (*iter == '\n') // Newline == 10
		{
			// Consume character, create token, increment index
			//tokens.push_back(buffer);
			buffer = "";
			iter.next();
		}
	}
	tokens.push_back(make_pair("", CHAR));
	return tokens;
}

void printTokens(vector<pair<string, TokenType>> tokens) {
	for (int i = 0; i < tokens.size(); i++)
	{
		if (tokens[i].first == "\n") {
			cout << '[' << "/n" << ']' << endl;
		}
		else {
			cout << '[' << tokens[i].first << ']';
		}
	}
	cout << endl;
}


// Abstract Syntax Tree Functions
class derivationTree {

	stack<Tree> treeStack;
	vector<pair<string, TokenType>> tokens;
	vectorIterator iter;

	bool debug = false;
	bool error = false;

public:

	derivationTree(vector<pair<string, TokenType>> tokens) {
		this->tokens = tokens;
		iter = vectorIterator(tokens);
	}

	void start() {
		// Tiny       -> 'program' Name ':' Consts Types Dclns
		//				  SubProgs Body Name '.'						= > "program";
		if (debug) { print("Start"); }

		consume("program");
		Name();
		consume(":");
		Consts();
		Types();
		Dclns();
		SubProgs();
		Body();
		Name();
		consume(".");

		buildTree("program", 7);
	}

	void printStack() {
		string s = "";
		stack<Tree> temp = stack<Tree>(treeStack);

		cout << "Stack___________" << endl;
		while (!temp.empty()) {
			s += temp.top().getRootToken() + "\n";
			temp.pop();
		}
		cout << s;
		cout << "printAST()____________" << endl; printAST();
		cout << endl;
	}

	string printAST() {
		string out = "";

		stack<Tree> temp = stack<Tree>(treeStack);
		vector<Tree> reverseTemp;
		while (!temp.empty()) {
			reverseTemp.push_back(temp.top());
			temp.pop();
		}

		for (int i = reverseTemp.size(); i > 0; i--) {
			out += reverseTemp[i - 1].printTree();
		}

		return out;
	}

	void setDebug(bool b) {
		debug = b;
	}

	bool getError() {
		return error;
	}

private:

	// Utility Functions

	void buildTree(string token, int n) {
		// 1) Pop n trees
		// 2) Build 'token' parent node
		// 3) Push new tree

		if (debug) {
			cout << "buildTree(" << token << ", " << n << ")" << endl;
			//	printStack(); 
		}

		Node newNode = Node(token);
		for (int i = 0; i < n; i++)
		{
			Tree temp = treeStack.top();
			newNode.addChild(treeStack.top().getRoot());
			treeStack.pop();
		}
		treeStack.push(Tree(newNode));
	}

	int List() {
		Name();
		int n = 1;
		while ((*iter).first == ",") {	// iteration to consume list
			consume(",");
			Name();
			n++;
		}
		return n;
	}

	void print(string caller)
	{
		int index = iter.getIndex();
		cout << caller << ": ";

		for (int i = 1; i > 0; i--) {
			if (index - i >= 0) {
				cout << "[" << tokens.at(index - i).first << "] ";
			}
		}

		cout << "<" << tokens.at(index).first << "> ";

		for (int i = 1; i < 2; i++) {
			if (index + i < tokens.size()) {
				cout << "[" << tokens.at(index + i).first << "] ";
			}
		}
		cout << endl;

	}

	// Non-Terminal Functions

	void Consts() {
		//	Consts     -> 'const' Const list ',' ';' = > "consts"
		//			   ->							 = > "consts";
		if (debug) { print("Consts"); }

		if ((*iter).first == "const") {
			consume("const");
			Const();
			int n = 1;
			while ((*iter).first == ",") {	// iteration to consume list
				consume(",");
				Const();
				n++;
			}
			consume(";");

			buildTree("consts", n);
		}
		else {
			// Build 
			buildTree("consts", 0);
		}
	}

	void Const() {
		//	Const	->	Name '=' ConstValue			= > "const";
		if (debug) { print("Const"); }

		if (iter.currentItem().second == IDENTIFIER) {	// add other consts values?
			Name();
			consume("=");
			ConstValue();
			buildTree("const", 2);
		}
		else {
			cout << "__Error: Invalid Token. " << (*iter).first << " @ Name" << endl;
			error = true;
		}

	}

	void ConstValue() {
		//	ConstValue  -> '<integer>'
		//				-> '<char>'
		//			    ->  Name;
		if (debug) { print("ConstValue"); }

		if ((*iter).second == INT) {
			consume(INT);
			buildTree("<integer>", 1);
		}
		else if ((*iter).second == CHAR) {
			consume(CHAR);
			buildTree("char", 1);
		}
		else if ((*iter).second == IDENTIFIER) {
			Name();
		}
		else {
			cout << "__Error: Invalid Token. " << (*iter).first << " @ INT, CHAR, NAME" << endl;
			error = true;

		}
	}

	void Types() {
		//	Types	-> 'type' (Type ';') +		= > "types"
		//			->							= > "types";
		if (debug) { print("Types"); }

		if ((*iter).first == "type") {
			consume("type");
			Type();
			consume(";");
			int n = 1;
			while ((*iter).second == IDENTIFIER) {	// iteration to consume additional types
				Type();
				consume(";");
				n++;
			}
			buildTree("types", n);
		}
		else {
			buildTree("types", 0);
		}
	}

	void Type() {
		//	Type	->	Name '=' LitList		= > "type";
		if (debug) { print("Type"); }

		if (iter.currentItem().second == IDENTIFIER) {
			Name();
			consume("=");
			LitList();
			buildTree("type", 2);
		}
		else {
			cout << "__Error: Invalid Token. " << (*iter).first << " @ Name " << endl;
			error = true;

		}
	}

	void LitList() {
		//	LitList	  -> '(' Name list ',' ')'		= > "lit";
		if (debug) { print("LitList"); }

		if ((*iter).first == "(") {
			consume("(");
			int n = List();
			consume(")");
			buildTree("lit", n);
		}
		else {
			cout << "__Error: Invalid Token. " << (*iter).first << " @ ( " << endl;
			error = true;
		}
	}

	void SubProgs() {
		// SubProgs   -> Fcn*			=> "subprogs";
		if (debug) { print("SubProgs"); }

		int n = 0;
		while ((*iter).first == "function") {
			Fcn();
			n++;
		}
		buildTree("subprogs", n);
	}

	void Fcn() {
		//	Fcn   -> 'function' Name '(' Params ')' ':' Name ';'
		//		Consts Types Dclns Body Name ';' = > "fcn";
		if (debug) { print("Fcn"); }

		if ((*iter).first == "function") {
			consume("function");
			Name();
			consume("(");
			Params();
			consume(")");
			consume(":");
			Name();
			consume(";");
			Consts();
			Types();
			Dclns();
			Body();
			Name();
			consume(";");

			buildTree("fcn", 8);
		}
		else {
			cout << "__Error: Invalid Token. " << (*iter).first << " @ function" << endl;
			error = true;
		}

	}

	void Params() {
		//	Params -> Dcln list ';'				= > "params";
		if (debug) { print("Params"); }

		if ((*iter).second == IDENTIFIER) {
			Dcln();
			int n = 1;
			while ((*iter).first == ";") {
				consume(";");
				Dcln();
				n++;
			}
			buildTree("params", n);
		}
		else {
			cout << "__Error: Invalid Token. " << (*iter).first << " @ Dcln" << endl;
			error = true;
		}
	}

	void Dclns() {
		//	Dclns  -> 'var' (Dcln ';') +		= > "dclns"
		if (debug) { print("Dclns"); }

		if ((*iter).first == "var") {
			consume("var");
			Dcln();
			consume(";");
			int n = 1;
			while ((*iter).second == IDENTIFIER) {
				Dcln();
				consume(";");
				n++;
			}
			buildTree("dclns", n);
		}
		else {
			buildTree("dclns", 0);
		}
	}

	void Dcln() {
		// Dcln  -> Name list ',' ':' Name			=> "var";
		if (debug) { print("Dcln"); }

		if ((*iter).second == IDENTIFIER) {
			int n = List();
			consume(":");
			Name();

			buildTree("var", n + 1);
		}
		else {
			cout << "__Error: Invalid Token. " << (*iter).first << " @ Name" << endl;
			error = true;
		}
	}

	void Body() {
		// Body  -> 'begin' Statement list ';' 'end'      	=> "block";
		if (debug) { print("Body"); }

		if ((*iter).first == "begin") {
			consume("begin");
			Statement();
			int n = 1;
			while ((*iter).first == ";") {	// iteration to consume list
				consume(";");
				Statement();
				n++;
			}
			consume("end");
			buildTree("block", n);

		}
		else {
			cout << "__Error: Invalid Token. " << (*iter).first << " @ begin" << endl;
			error = true;
		}
	}

	void Statement() {
		if (debug) { print("Statement"); }

		if ((*iter).second == IDENTIFIER) {
			//	Statement->Assignment
			Assignment();
		}
		else if ((*iter).second == KEYWORD) {
			if ((*iter).first == "output") {
				//	-> 'output' '(' OutExp list ',' ')'		= > "output"
				consume("output");
				consume("(");
				OutExp();
				int n = 1;
				while ((*iter).first == ",") {	// iteration to consume list
					consume(",");
					OutExp();
					n++;
				}
				consume(")");
				buildTree("output", n);

			}
			else if ((*iter).first == "if") {
				// -> 'if' Expression 'then' Statement ('else' Statement) ?		= > "if"
				consume("if");
				Expression();
				consume("then");
				Statement();
				if ((*iter).first == "else") {
					consume("else");
					Statement();
					buildTree("if", 3);
				}
				else {
					buildTree("if", 2);
				}

			}
			else if ((*iter).first == "while") {
				//	-> 'while' Expression 'do' Statement			=> "while"
				consume("while");
				Expression();
				consume("do");
				Statement();
				buildTree("while", 2);
			}
			else if ((*iter).first == "repeat") {
				//	-> 'repeat' Statement list ';' 'until' Expression		=> "repeat"	
				consume("repeat");
				Statement();
				int n = 1;
				while ((*iter).first == ";") {
					consume(";");
					Statement();
					n++;
				}
				consume("until");
				Expression();
				buildTree("repeat", n + 1);
			}
			else if ((*iter).first == "for") {
				//	-> 'for' '(' ForStat ';' ForExp ';' ForStat ')'	Statement = > "for"	
				consume("for");
				consume("(");
				ForStat();
				consume(";");
				ForExp();
				consume(";");
				ForStat();
				consume(")");
				Statement();

				buildTree("for", 4);
			}
			else if ((*iter).first == "loop") {
				//	-> 'loop' Statement list ';' 'pool' = > "loop"
				consume("loop");
				Statement();
				int n = 1;
				while ((*iter).first == ";") {	// iteration to consume list
					consume(";");
					Statement();
					n++;
				}
				consume("pool");

				buildTree("loop", n);
			}
			else if ((*iter).first == "case") {
				//	 -> 'case' Expression 'of' Caseclauses OtherwiseClause 'end'	= > "case"	
				consume("case");
				Expression();
				consume("of");
				int n = Caseclauses();
				int m = OtherwiseClause();
				consume("end");

				buildTree("case", n + m + 1);
			}
			else if ((*iter).first == "read") {
				// 	-> 'read' '(' Name list ',' ')'			= > "read"
				consume("read");
				consume("(");
				int n = List();
				consume(")");

				buildTree("read", n);
			}
			else if ((*iter).first == "exit") {
				// 	-> 'exit' = > "exit"
				consume("exit");
				buildTree("exit", 0);
			}
			else if ((*iter).first == "return") {
				// 	-> 'return' Expression = > "return"
				consume("return");
				Expression();

				buildTree("return", 1);
			}
			else if ((*iter).first == "begin") {
				// -> Body
				Body();
			}
			else {
				// ->			=> '<null>'
				buildTree("<null>", 0);
			}
		}
	}

	void OutExp() {
		// OutExp     -> Expression				=> "integer"
		//			  -> StringNode				=> "string";
		if (debug) { print("OutExp"); }

		if ((*iter).second == STRING) {
			StringNode();
			buildTree("string", 1);
		}
		else
		{
			Expression();
			buildTree("integer", 1);
			// cout << "__Error: Invalid Token. " << (*iter).first << " @ Expression, StringNode" << endl;
		}
	}

	void StringNode() {
		// StringNode -> '<string>';
		if (debug) { print("StringNode"); }
		consume(STRING);
		buildTree("<string>", 1);
	}

	int Caseclauses() {
		// Caseclauses	-> (Caseclause ';')+;
		// Irregular return structure (need to return num clasues for case n)
		if (debug) { print("Caseclauses"); }
		int n = 0;
		while ((*iter).second == IDENTIFIER || (*iter).second == INT) {
			Caseclause();
			consume(";");
			n++;
		}
		return n;
	}

	void Caseclause() {
		// Caseclause -> CaseExpression list ',' ':' Statement     => "case_clause";
		if (debug) { print("Caseclause"); }

		CaseExpression();
		int n = 1;
		while ((*iter).first == ",") {	// iteration to consume list
			consume(",");
			CaseExpression();
			n++;
		}
		consume(":");
		Statement();

		buildTree("case_clause", n + 1);
	}

	void CaseExpression() {
		// CaseExpression -> ConstValue
		//			      -> ConstValue '..' ConstValue		=> "..";
		if (debug) { print("CaseExpression"); }

		ConstValue();

		if ((*iter).first == "..") {
			consume("..");
			ConstValue();
			buildTree("..", 2);
		}
	}

	int OtherwiseClause() {
		// OtherwiseClause -> 'otherwise' Statement = > "otherwise"
		//				   -> ;
		if (debug) { print("OtherwiseClause"); }

		if ((*iter).first == "otherwise") {
			consume("otherwise");
			Statement();
			buildTree("otherwise", 1);
			return 1;
		}
		else if ((*iter).first == ";") {
			consume(";");
		}
		return 0;
	}

	void Assignment() {
		// Assignment	 -> Name ':=' Expression  	        	=> "assign"
		//				 -> Name ':=:' Name						= > "swap";
		if (debug) { print("Assignment"); }

		Name();
		if ((*iter).first == ":=") {
			consume(":=");
			Expression();
			buildTree("assign", 2);
		}
		else if ((*iter).first == ":=:") {
			consume(":=:");
			Name();
			buildTree("swap", 2);
		}
		else {
			cout << "__Error: Invalid Token. " << (*iter).first << " @ :=, :=:" << endl;
			error = true;
		}
	}

	void ForStat() {
		// ForStat		  -> Assignment
		//				  ->					=> "<null>";
		if (debug) { print("ForStat"); }

		if ((*iter).second == IDENTIFIER) {
			Assignment();
		}
		else {
			// Build <null>
			buildTree("<null>", 0);
		}
	}

	void ForExp() {
		// ForExp       -> Expression
		//				->					=> "true";
		if (debug) { print("ForExp"); }

		Expression();
		// Build true	
	}

	void Expression() {
		// Expression -> Term	                         		
		//			-> Term '<=' Term = > "<="
		//			->Term '<' Term = > "<"
		//			->Term '>=' Term = > ">="
		//			->Term '>' Term = > ">"
		//			->Term '=' Term = > "="
		//			->Term '<>' Term = > "<>";
		if (debug) { print("Expression"); }

		Term();

		if ((*iter).first == "<=") {
			consume("<=");
			Term();
			buildTree("<=", 2);
		}
		else if ((*iter).first == "<") {
			consume("<");
			Term();
			buildTree("<", 2);
		}
		else if ((*iter).first == ">=") {
			consume(">=");
			Term();
			buildTree(">=", 2);
		}
		else if ((*iter).first == ">") {
			consume(">");
			Term();
			buildTree(">", 2);
		}
		else if ((*iter).first == "=") {
			consume("=");
			Term();
			buildTree("=", 2);
		}
		else if ((*iter).first == "<>") {
			consume("<>");
			Term();
			buildTree("<>", 2);
		}
	}

	void Term() {
		// Terms are separated by addition or subtraction.

		// Term       -> Factor 
		//			  -> Term '+' Factor = > "+"
		//			  -> Term '-' Factor = > "-"
		//			  -> Term 'or' Factor = > "or";
		if (debug) { print("Term"); }

		Factor();
		while ((*iter).first == "+" || (*iter).first == "-" || (*iter).first == "or")
		{
			if ((*iter).first == "+") {
				consume("+");
				Term();
				buildTree("+", 2);
			}
			else if ((*iter).first == "-") {
				consume("-");
				Term();
				buildTree("-", 2);
			}
			else if ((*iter).first == "or") {
				consume("or");
				Term();
				buildTree("or", 2);
			}
		}
	}

	void Factor() {
		// Factors are separated by multiplication or division.

		// Factor     -> Factor '*' Primary			=> "*"
		//			  -> Factor '/' Primary			=> "/"
		//			  -> Factor 'and' Primary		=> "and"
		//			  -> Factor 'mod' Primary		=> "mod"
		//			  -> Primary;
		if (debug) { print("Factor"); }

		Primary();

		while ((*iter).first == "*" || (*iter).first == "/" || (*iter).first == "and" || (*iter).first == "mod")
		{
			if ((*iter).first == "*") {
				consume("*");
				Factor();
				buildTree("*", 2);
			}
			else if ((*iter).first == "/") {
				consume("/");
				Factor();
				buildTree("/", 2);
			}
			else if ((*iter).first == "and") {
				consume("and");
				Factor();
				buildTree("and", 2);

			}
			else if ((*iter).first == "mod") {
				consume("mod");
				Factor();
				buildTree("mod", 2);
			}
		}
	}

	void Primary() {
		if (debug) { print("Primary"); }

		if ((*iter).first == "-") {
			// Primary		-> '-' Primary         => "-"
			consume("-");
			Primary();
			buildTree("-", 1);
		}
		else if ((*iter).first == "+") {
			//	-> '+' Primary
			consume("+");
			Primary();
		}
		else if ((*iter).first == "not") {
			//	-> 'not' Primary		= > "not"
			consume("not");
			Primary();
			buildTree("not", 1);
		}
		else if ((*iter).first == "eof") {
			//	-> 'eof'				= > "eof"
			consume("eof");
			buildTree("eof", 0);
		}
		else if ((*iter).first == "(") {
			//		-> '(' Expression ')'
			consume("(");
			Expression();
			consume(")");
		}
		else if ((*iter).first == "succ") {
			//		-> 'succ' '(' Expression ')'			= > "succ"
			consume("succ");
			consume("(");
			Expression();
			consume(")");
			buildTree("succ", 1);
		}
		else if ((*iter).first == "pred") {
			//		-> 'pred' '(' Expression ')'			= > "pred"
			consume("pred");
			consume("(");
			Expression();
			consume(")");
			buildTree("pred", 1);
		}
		else if ((*iter).first == "chr") {
			//		-> 'chr' '(' Expression ')'				= > "chr"
			consume("chr");
			consume("(");
			Expression();
			consume(")");
			buildTree("chr", 1);
		}
		else if ((*iter).first == "ord") {
			//		-> 'ord' '(' Expression ')'				= > "ord";

			consume("ord ");
			consume("(");
			Expression();
			consume(")");
			buildTree("ord", 1);
		}
		else if (iter.currentItem().second == IDENTIFIER) {
			//		-> Name
			//		-> Name '(' Expression list ',' ')'		= > "call"
			Name();
			if ((*iter).first == "(") {
				consume("(");
				Expression();
				int n = 1;
				while ((*iter).first == ",") {	// iteration to consume list
					consume(",");
					Expression();
					n++;
				}
				consume(")");
				buildTree("call", n + 1);
			}
		}
		else if (iter.currentItem().second == INT) {
			//		-> '<integer>'
			consume(INT);
			buildTree("<integer>", 1);
		}
		else if (iter.currentItem().second == CHAR) {
			//		-> '<char>'
			consume(CHAR);
			buildTree("<char>", 1);
		}
		else {
			cout << "__Error: Invalid Token. " << (*iter).first << " @ etc" << endl;
			error = true;
		}
	}

	void Name()
	{
		//	-> '<identifier>'
		if (debug) { print("Name"); }
		consume(IDENTIFIER);
		buildTree("<identifier>", 1);
	}

	// Consume Terminals
	bool consume(string token) {
		string s = "Consume: '" + token + "'";

		if (debug) { print(s); }

		if ((*iter).first == token) {
			++iter;
			return true;
		}
		else {
			cout << "__Error: " << (*iter).first << " != " << token << endl;
			error = true;
			return false;
		}
	}

	bool consume(TokenType t) {
		string s = "Consume: '" + enumString[(int)t] + "' ";

		if (debug) { print(s); }

		if ((*iter).second == t) {

			treeStack.push(Tree((*iter).first));

			++iter;
			return true;
		}
		else {
			cout << "__Error: Invalid token type." << (*iter).first << " @ etc" << endl;
			error = true;
			return false;
		}
	}

};

void compare(string a, string b) {

	cout << endl;
	cout << "Length A:" << a.size() << endl;
	cout  << "Length B:" << b.size() << endl;
	
	stringIterator aiter = stringIterator(a);
	stringIterator biter = stringIterator(b);

	int i = 0;
	while (aiter.hasNext() && biter.hasNext()) {
		bool b = (aiter.currentItem()) == (biter.currentItem());
		cout << i << ": A (" << *aiter << ") == B (" << *biter << ") = " << b << endl;

		i++;
		aiter.next();
		biter.next();
	}
}

int main(int argc, char* argv[]) {

	vector<pair<string, TokenType>> tokens;
	bool consumeTestBools[25];
	bool buildTestBools[25];

	//cout << argc << endl;
	//for(int i = 0; i < argc; i++) {
	//	cout << i << ": '" << argv[i] << "'" << endl;
	//}

	if (argc == 1) {

		string in;
		cin >> in;

		while (in != "end")
		{
			if (in == "test")
			{
				string testnumber;
				cin >> testnumber;

				if (testnumber == "all")
				{
					for (int i = 1; i < 26; i++)
					{
						string treeFile;
						string file;
						if (i < 10)
						{
							//cout << i << endl;
							file = fileToString("/cise/homes/schaefer/Desktop/projecty/tests/tiny_0" + to_string(i));
							treeFile = fileToString("/cise/homes/schaefer/Desktop/projecty/tests/tiny_0" + to_string(i) + ".tree");
						}
						else
						{
							file = fileToString("/cise/homes/schaefer/Desktop/projecty/tests/tiny_" + to_string(i));
							treeFile = fileToString("/cise/homes/schaefer/Desktop/projecty/tests/tiny_" + to_string(i) + ".tree");
						}

						tokens = generateTokens(file);
						derivationTree ast = derivationTree(tokens);
						//ast.setDebug(true);
						ast.start();

						cout << "tiny_" << i << ": " << (treeFile == ast.printAST()) << endl;
						//buildTestBools[i - 1] = (treeFile == ast.printAST());

					}
				}
				else {
					string file = fileToString("/cise/homes/schaefer/Desktop/projecty/tests/tiny_" + testnumber);
					string treeFile = fileToString("/cise/homes/schaefer/Desktop/projecty/tests/tiny_" + testnumber + ".tree");

					cout << file << endl << endl;

					tokens = generateTokens(file);
					//	printTokens(tokens);
					derivationTree ast = derivationTree(tokens);
					//	ast.setDebug(true);
					ast.start();
					string treeString = ast.printAST();
					cout << treeString << endl << (treeFile == treeString) << endl;
					//compare(treeString, treeFile);
				}
			}
			cin >> in;
		}
	}
	else if (argc > 2)
	{
		// cout << (argv[1] == "-ast") << endl; OUTPUTS FALSE WHEN IT SHOULDNT?
		// if(argv[1] == "-ast") {
		//	cout << "ast found";
		string file = fileToString(argv[2]);
		tokens = generateTokens(file);
		derivationTree ast = derivationTree(tokens);
		ast.start();
		cout << ast.printAST();
		//}
	}


	return 0;

}
