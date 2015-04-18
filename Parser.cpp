

#include "StdAfx.h"
#include <wchar.h>
#include "Parser.h"
#include "Scanner.h"
#include "helpers.h"
#include <locale>
#include <codecvt>



static std::string Convert(const std::wstring& text)
{
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  return converter.to_bytes(text);
}

void Parser::DefineFunction()
{
  compiler.DefineFunction();
}

void Parser::DefineIdentifier()
{
  compiler.DefineIdentifier(Convert(t->val));
}

void Parser::DefineProcedure()
{
  compiler.DefineProcedure();
}

void Parser::EnterLocalScope()
{
  compiler.EnterLocalScope();
}

void Parser::LeaveLocalScope()
{
  compiler.LeaveLocalScope();
}

void Parser::Merge()
{
  compiler.Merge(); 
}

void Parser::PushNew(const std::string& tag)
{
  compiler.PushNew(tag);
}

void Parser::PushNewBinary(const std::string& tag)
{
  compiler.PushNewBinary(tag);
}

void Parser::PushNewNumber()
{
  compiler.PushNewNumber(tagValue, std::stod(t->val));
}

void Parser::PushNewString()
{
  compiler.PushNewString(tagValue, Convert(t->val));
}

void Parser::SynErr(int n) {
	if (errDist >= minErrDist) errors->SynErr(la->line, la->col, n);
	errDist = 0;
}

void Parser::SemErr(const wchar_t* msg) {
	if (errDist >= minErrDist) errors->Error(t->line, t->col, msg);
	errDist = 0;
}

void Parser::Get() {
	for (;;) {
		t = la;
		la = scanner->Scan();
		if (la->kind <= maxT) { ++errDist; break; }

		if (dummyToken != t) {
			dummyToken->kind = t->kind;
			dummyToken->pos = t->pos;
			dummyToken->col = t->col;
			dummyToken->line = t->line;
			dummyToken->next = NULL;
			coco_string_delete(dummyToken->val);
			dummyToken->val = coco_string_create(t->val);
			t = dummyToken;
		}
		la = t;
	}
}

void Parser::Expect(int n) {
	if (la->kind==n) Get(); else { SynErr(n); }
}

void Parser::ExpectWeak(int n, int follow) {
	if (la->kind == n) Get();
	else {
		SynErr(n);
		while (!StartOf(follow)) Get();
	}
}

bool Parser::WeakSeparator(int n, int syFol, int repFol) {
	if (la->kind == n) {Get(); return true;}
	else if (StartOf(repFol)) {return false;}
	else {
		SynErr(n);
		while (!(StartOf(syFol) || StartOf(repFol) || StartOf(0))) {
			Get();
		}
		return StartOf(syFol);
	}
}

void Parser::expressions() {
		expression();
		Merge();
		while (la->kind == 4 /* "," */) {
			Get();
			expression();
			Merge();
		}
}

void Parser::expression() {
		value();
		while (StartOf(1)) {
			switch (la->kind) {
			case 11 /* "=" */: {
				equal();
				break;
			}
			case 12 /* "<>" */: {
				unequal();
				break;
			}
			case 13 /* "<" */: {
				smaller();
				break;
			}
			case 14 /* ">" */: {
				greater();
				break;
			}
			case 15 /* "<=" */: {
				smallerEqual();
				break;
			}
			case 16 /* ">=" */: {
				greaterEqual();
				break;
			}
			}
		}
}

void Parser::invokation() {
		Expect(_identifier);
		DefineIdentifier();
		if (la->kind == 5 /* "(" */) {
			Get();
			expressions();
			Expect(6 /* ")" */);
		}
}

void Parser::factor() {
		if (la->kind == _number) {
			Get();
			PushNewNumber();
		} else if (la->kind == _string) {
			Get();
			PushNewString();
		} else if (la->kind == _identifier) {
			invokation();
		} else if (la->kind == 5 /* "(" */) {
			Get();
			expression();
			Expect(6 /* ")" */);
		} else SynErr(32);
}

void Parser::term() {
		factor();
		while (la->kind == 7 /* "*" */ || la->kind == 8 /* "/" */) {
			if (la->kind == 7 /* "*" */) {
				Get();
				factor();
				PushNewBinary(tagMultiply);
			} else {
				Get();
				factor();
				PushNewBinary(tagDivide);
			}
		}
}

void Parser::value() {
		term();
		while (la->kind == 9 /* "+" */ || la->kind == 10 /* "-" */) {
			if (la->kind == 9 /* "+" */) {
				Get();
				term();
				PushNewBinary(tagAdd);
			} else {
				Get();
				term();
				PushNewBinary(tagSubtract);
			}
		}
}

void Parser::equal() {
		Expect(11 /* "=" */);
		value();
		PushNewBinary(tagEqual);
}

void Parser::unequal() {
		Expect(12 /* "<>" */);
		value();
		PushNewBinary(tagNotEqual);
}

void Parser::smaller() {
		Expect(13 /* "<" */);
		value();
		PushNewBinary(tagLess);
}

void Parser::greater() {
		Expect(14 /* ">" */);
		value();
		PushNewBinary(tagGreater);
}

void Parser::smallerEqual() {
		Expect(15 /* "<=" */);
		value();
		PushNewBinary(tagLessOrEqual);
}

void Parser::greaterEqual() {
		Expect(16 /* ">=" */);
		value();
		PushNewBinary(tagGreaterOrEqual);
}

void Parser::run() {
		Expect(17 /* "run" */);
		PushNew(tagRun);
		expression();
		Merge();
}

void Parser::assignement() {
		invokation();
		Expect(11 /* "=" */);
		expression();
		PushNewBinary(tagAssign);
}

void Parser::print() {
		Expect(18 /* "print" */);
		PushNew(tagPrint);
		expression();
		Merge();
}

void Parser::returnment() {
		Expect(19 /* "return" */);
		PushNew(tagReturn);
		expression();
		Merge();
}

void Parser::selection() {
		Expect(20 /* "if" */);
		PushNew(tagSelection);
		expression();
		Merge();
		Expect(21 /* "then" */);
		statements();
		Merge();
		Expect(22 /* "endif" */);
}

void Parser::statements() {
		PushNew(tagSequence);
		statement();
		Merge();
		while (StartOf(2)) {
			statement();
			Merge();
		}
}

void Parser::iteration() {
		Expect(23 /* "while" */);
		PushNew(tagIteration);
		expression();
		Merge();
		Expect(24 /* "do" */);
		statements();
		Merge();
		Expect(25 /* "end" */);
}

void Parser::routine() {
		if (la->kind == 26 /* "function" */) {
			Get();
			DefineFunction();
		} else if (la->kind == 27 /* "procedure" */) {
			Get();
			DefineProcedure();
		} else SynErr(33);
		invokation();
		Merge();EnterLocalScope();
		statements();
		Merge();
		Expect(25 /* "end" */);
		LeaveLocalScope();
}

void Parser::definitions() {
		Expect(28 /* "define" */);
		if (la->kind == 29 /* "variable" */) {
			Get();
			PushNew(tagDefine);
		} else if (la->kind == 30 /* "parameter" */) {
			Get();
			PushNew(tagParameters);
		} else SynErr(34);
		expressions();
}

void Parser::statement() {
		switch (la->kind) {
		case 17 /* "run" */: {
			run();
			break;
		}
		case _identifier: {
			assignement();
			break;
		}
		case 18 /* "print" */: {
			print();
			break;
		}
		case 19 /* "return" */: {
			returnment();
			break;
		}
		case 20 /* "if" */: {
			selection();
			break;
		}
		case 23 /* "while" */: {
			iteration();
			break;
		}
		case 26 /* "function" */: case 27 /* "procedure" */: {
			routine();
			break;
		}
		case 28 /* "define" */: {
			definitions();
			break;
		}
		default: SynErr(35); break;
		}
}

void Parser::makro() {
		statements();
}




// If the user declared a method Init and a mehtod Destroy they should
// be called in the contructur and the destructor respctively.
//
// The following templates are used to recognize if the user declared
// the methods Init and Destroy.

template<typename T>
struct ParserInitExistsRecognizer {
	template<typename U, void (U::*)() = &U::Init>
	struct ExistsIfInitIsDefinedMarker{};

	struct InitIsMissingType {
		char dummy1;
	};
	
	struct InitExistsType {
		char dummy1; char dummy2;
	};

	// exists always
	template<typename U>
	static InitIsMissingType is_here(...);

	// exist only if ExistsIfInitIsDefinedMarker is defined
	template<typename U>
	static InitExistsType is_here(ExistsIfInitIsDefinedMarker<U>*);

	enum { InitExists = (sizeof(is_here<T>(NULL)) == sizeof(InitExistsType)) };
};

template<typename T>
struct ParserDestroyExistsRecognizer {
	template<typename U, void (U::*)() = &U::Destroy>
	struct ExistsIfDestroyIsDefinedMarker{};

	struct DestroyIsMissingType {
		char dummy1;
	};
	
	struct DestroyExistsType {
		char dummy1; char dummy2;
	};

	// exists always
	template<typename U>
	static DestroyIsMissingType is_here(...);

	// exist only if ExistsIfDestroyIsDefinedMarker is defined
	template<typename U>
	static DestroyExistsType is_here(ExistsIfDestroyIsDefinedMarker<U>*);

	enum { DestroyExists = (sizeof(is_here<T>(NULL)) == sizeof(DestroyExistsType)) };
};

// The folloing templates are used to call the Init and Destroy methods if they exist.

// Generic case of the ParserInitCaller, gets used if the Init method is missing
template<typename T, bool = ParserInitExistsRecognizer<T>::InitExists>
struct ParserInitCaller {
	static void CallInit(T *t) {
		// nothing to do
	}
};

// True case of the ParserInitCaller, gets used if the Init method exists
template<typename T>
struct ParserInitCaller<T, true> {
	static void CallInit(T *t) {
		t->Init();
	}
};

// Generic case of the ParserDestroyCaller, gets used if the Destroy method is missing
template<typename T, bool = ParserDestroyExistsRecognizer<T>::DestroyExists>
struct ParserDestroyCaller {
	static void CallDestroy(T *t) {
		// nothing to do
	}
};

// True case of the ParserDestroyCaller, gets used if the Destroy method exists
template<typename T>
struct ParserDestroyCaller<T, true> {
	static void CallDestroy(T *t) {
		t->Destroy();
	}
};

void Parser::Parse() {
	t = NULL;
	la = dummyToken = new Token();
	la->val = coco_string_create(L"Dummy Token");
	Get();
	makro();
	Expect(0);
}

Parser::Parser(Scanner *scanner, Compiler::ICompiler& aCompiler): compiler(aCompiler) {
	maxT = 31;

	ParserInitCaller<Parser>::CallInit(this);
	dummyToken = NULL;
	t = la = NULL;
	minErrDist = 2;
	errDist = minErrDist;
	this->scanner = scanner;
	errors = new Errors();
}

bool Parser::StartOf(int s) {
	const bool T = true;
	const bool x = false;

	static bool set[3][33] = {
		{T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x},
		{x,x,x,x, x,x,x,x, x,x,x,T, T,T,T,T, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x},
		{x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,T,T, T,x,x,T, x,x,T,T, T,x,x,x, x}
	};



	return set[s][la->kind];
}

Parser::~Parser() {
	ParserDestroyCaller<Parser>::CallDestroy(this);
	delete errors;
	delete dummyToken;
}

Errors::Errors() {
	count = 0;
}

void Errors::SynErr(int line, int col, int n) {
	wchar_t* s;
	switch (n) {
			case 0: s = coco_string_create(L"EOF expected"); break;
			case 1: s = coco_string_create(L"string expected"); break;
			case 2: s = coco_string_create(L"identifier expected"); break;
			case 3: s = coco_string_create(L"number expected"); break;
			case 4: s = coco_string_create(L"\",\" expected"); break;
			case 5: s = coco_string_create(L"\"(\" expected"); break;
			case 6: s = coco_string_create(L"\")\" expected"); break;
			case 7: s = coco_string_create(L"\"*\" expected"); break;
			case 8: s = coco_string_create(L"\"/\" expected"); break;
			case 9: s = coco_string_create(L"\"+\" expected"); break;
			case 10: s = coco_string_create(L"\"-\" expected"); break;
			case 11: s = coco_string_create(L"\"=\" expected"); break;
			case 12: s = coco_string_create(L"\"<>\" expected"); break;
			case 13: s = coco_string_create(L"\"<\" expected"); break;
			case 14: s = coco_string_create(L"\">\" expected"); break;
			case 15: s = coco_string_create(L"\"<=\" expected"); break;
			case 16: s = coco_string_create(L"\">=\" expected"); break;
			case 17: s = coco_string_create(L"\"run\" expected"); break;
			case 18: s = coco_string_create(L"\"print\" expected"); break;
			case 19: s = coco_string_create(L"\"return\" expected"); break;
			case 20: s = coco_string_create(L"\"if\" expected"); break;
			case 21: s = coco_string_create(L"\"then\" expected"); break;
			case 22: s = coco_string_create(L"\"endif\" expected"); break;
			case 23: s = coco_string_create(L"\"while\" expected"); break;
			case 24: s = coco_string_create(L"\"do\" expected"); break;
			case 25: s = coco_string_create(L"\"end\" expected"); break;
			case 26: s = coco_string_create(L"\"function\" expected"); break;
			case 27: s = coco_string_create(L"\"procedure\" expected"); break;
			case 28: s = coco_string_create(L"\"define\" expected"); break;
			case 29: s = coco_string_create(L"\"variable\" expected"); break;
			case 30: s = coco_string_create(L"\"parameter\" expected"); break;
			case 31: s = coco_string_create(L"??? expected"); break;
			case 32: s = coco_string_create(L"invalid factor"); break;
			case 33: s = coco_string_create(L"invalid routine"); break;
			case 34: s = coco_string_create(L"invalid definitions"); break;
			case 35: s = coco_string_create(L"invalid statement"); break;

		default:
		{
			wchar_t format[20];
			coco_swprintf(format, 20, L"error %d", n);
			s = coco_string_create(format);
		}
		break;
	}
	wprintf(L"-- line %d col %d: %ls\n", line, col, s);
	coco_string_delete(s);
	count++;
}

void Errors::Error(int line, int col, const wchar_t *s) {
	wprintf(L"-- line %d col %d: %ls\n", line, col, s);
	count++;
}

void Errors::Warning(int line, int col, const wchar_t *s) {
	wprintf(L"-- line %d col %d: %ls\n", line, col, s);
}

void Errors::Warning(const wchar_t *s) {
	wprintf(L"%ls\n", s);
}

void Errors::Exception(const wchar_t* s) {
	wprintf(L"%ls", s); 
	exit(1);
}


