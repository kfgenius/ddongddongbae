//C-style Integer Expression Calculator
//created by Kim Yongmook ( http://moogi.new21.org )
//Originically created on October 10, 2004
//Modified on April 22, 2005

#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#define CALCARRAYSIZE(x) (sizeof(x)/sizeof(x[0]))
struct TOKEN;
typedef __int64 EVALTYPE;
class CEvaluator {
protected:
	TOKEN *m_pRoot;
	void CopyObject(const CEvaluator& obj); //recursive
public:
	enum ErrorCode {
		noError=0, tooComplex=-1, invalidMatch=-2, invalidChar=-3,
		invalidCode=-4, divisionZero=-5, invalidAssign=-6, noToken=-7
	};
	CEvaluator(): m_pRoot(NULL) { }
	CEvaluator(EVALTYPE x); //constructs a constant expression
	CEvaluator(const CEvaluator& obj): m_pRoot(NULL) { CopyObject(obj); }
	const CEvaluator& operator =(const CEvaluator& obj) { CopyObject(obj); return *this; }
	~CEvaluator() { Clear(); }

	//데이터 처리
	void Clear();
	BOOL IsEmpty() const { return !m_pRoot; }

	//수식 처리, 평가 관련
	int Interpret(PCTSTR pszExp);
	int Evaluate(EVALTYPE& res, EVALTYPE *pVar=NULL, UINT *pvcg=NULL) const;
	void Simplify();
	BOOL Restore(PTSTR pszStr, BOOL bHex=FALSE) const; //recursive
	static EVALTYPE QuickEvaluate(PCTSTR pszExp, EVALTYPE *pVar=NULL, int *pErr=NULL);
};

struct TOKEN {
	TOKEN(int cod, EVALTYPE pp);
	TOKEN(int cod, TOKEN *p1, TOKEN *p2, TOKEN *p3=NULL);
	int nOpCode;
	union {
		EVALTYPE nValue;
		int nVar;
		TOKEN *pOper[3];
	};
};

class CCalc {
public:
	class CIterator { //for recursive calculation
	public:
		PTSTR m_pszTo; BOOL m_bHex; //string restoration
		void Infix(const TOKEN& rt, BOOL bParen=FALSE);
	};
	struct SIMPLESTACK { //for non-recursive calculation
		int pos; TOKEN *pt;
	};
	enum { SIZE=32 };
	//operator descriptor
	static PCTSTR m_szOpc[];
	static inline BOOL IsOperator(int cod) { return cod>=0; }
	static int GetPrecedence(int cod);
	static int IsRightToLeft(int cod);
	static int GetDegree(int cod);

	//operation helper
	static void FreeNode(TOKEN *rt);
	static TOKEN *GetClone(TOKEN *rt); //recursive
	static int PopStack(TOKEN **tok_stk, int& tok_pos, int *opc_stk, int& opc_pos, int baseop=999);
};

enum ETokenType { //상수, 변수, 여는괄호와 닫는괄호
	CONSTANT=-3, VARIABLE=-2, OPENINGBR=100, CLOSINGBR
};
enum EOperatorCode { //operator code
	//unary: ~ ! - + (0~3)
	ONECOMPL, LOGICALNOT, UNARYMINUS, UNARYPLUS,
	//binary * / % + - << >> < > (4~12)
	TIMES, DIVIDE, MODULOUS, PLUS, MINUS, LEFTSHIFT, RIGHTSHIFT, LESSTHAN, GREATERTHAN,
	//<= >= == != & ^ | && || (13~21)
	LESSTHANEQ, GREATERTHANEQ, EQUALTO, INEQUAL, BITAND, BITXOR, BITOR, AND, OR,
	//? : = , (22~25)
	CONDITIONAL, CONDITIONAL2, ASSIGN, COMMA, __endopc
};