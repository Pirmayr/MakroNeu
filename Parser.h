

#if !defined(COCO_PARSER_H__)
#define COCO_PARSER_H__



#include "Scanner.h"
#include "compiler.h"



class Errors {
public:
	int count;			// number of errors detected

	Errors();
	void SynErr(int line, int col, int n);
	void Error(int line, int col, const wchar_t *s);
	void Warning(int line, int col, const wchar_t *s);
	void Warning(const wchar_t *s);
	void Exception(const wchar_t *s);

}; // Errors

class Parser {
private:
	enum {
		_EOF=0,
		_string=1,
		_identifier=2,
		_number=3
	};
	int maxT;

	Token *dummyToken;
	int errDist;
	int minErrDist;

  void DefineFunction();
  void DefineIdentifier();
  void DefineProcedure();
  void EnterLocalScope();
  void LeaveLocalScope();
  void Merge();
  void PushNew(const std::string& tag);
  void PushNewBinary(const std::string& tag);
  void PushNewNumber();
  void PushNewString();

	void SynErr(int n);
	void Get();
	void Expect(int n);
	bool StartOf(int s);
	void ExpectWeak(int n, int follow);
	bool WeakSeparator(int n, int syFol, int repFol);

public:
	Scanner *scanner;
	Compiler::ICompiler& compiler;
	Errors  *errors;

	Token *t;			// last recognized token
	Token *la;			// lookahead token



	Parser(Scanner *scanner, Compiler::ICompiler& compiler);
	~Parser();
	void SemErr(const wchar_t* msg);

	void expressions();
	void expression();
	void invokation();
	void factor();
	void term();
	void value();
	void equal();
	void unequal();
	void smaller();
	void greater();
	void smallerEqual();
	void greaterEqual();
	void run();
	void assignement();
	void print();
	void returnment();
	void selection();
	void statements();
	void iteration();
	void routine();
	void definitions();
	void statement();
	void makro();

	void Parse();

}; // end Parser



#endif

