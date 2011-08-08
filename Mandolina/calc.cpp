#include "calc.h"

//baseop보다 우선순위가 높거나 같은 것(즉, 코드값이 더 작은 것)을 모두 빼낸다.
//단, baseop가 right-to-left이면 안 빼냄.
int CCalc::PopStack(TOKEN **tok_stk, int& tok_pos, int *opc_stk, int& opc_pos, int baseop)
{
	int x,y,z=GetPrecedence(baseop); TOKEN *ps;
	for(;opc_pos>0;opc_pos--) {
		x=opc_stk[opc_pos-1]; y=GetPrecedence(x);
		//스택의 연산자, 즉 식에서 왼쪽에 있는 연산자가 우리 기준 연산자보다 우선순위가 낮거나,
		//우선순위가 같아도 방향이 반대면, 나중에 처리되도록 연산자를 더 빼내지 않는다.
		if(x==CONDITIONAL2 && (z>y || baseop==CONDITIONAL2)) {
			//?:를 빼낼 조건은? baseop 자체가 :이거나, baseop가 ?:보다도 우선 순위가 낮을 때만
			//x 앞에는 CONDITIONAL이 있어야 한다. 이걸 빼내도록.
			if(opc_pos<2) return CEvaluator::invalidMatch;
			else {
				if(opc_stk[opc_pos-2]!=CONDITIONAL) return CEvaluator::invalidMatch;
				//try again with ? operator
				opc_pos--; x=opc_stk[opc_pos-1]; y=GetPrecedence(x);
			}
		}
		else if(y>z || (y==z && IsRightToLeft(x) && IsRightToLeft(baseop)) ) break;

		//닫는 괄호가 드디어 열린 괄호를 만났으므로 임무 완수
		if(x==OPENINGBR && baseop==CLOSINGBR) { opc_pos--; return CEvaluator::noError; }

		y=GetDegree(x);
		if(tok_pos-y<0) return CEvaluator::invalidMatch; //stack underflow.. 피연산자가 부족하다
		tok_stk[tok_pos-y]=ps=new TOKEN(x, tok_stk[tok_pos-y],
			y>=2 ? tok_stk[tok_pos-y+1]: NULL, y==3 ? tok_stk[tok_pos-y+2]: NULL);
		if(y>=2) { //clear the buffer that has become a child
			tok_stk[tok_pos-y+1]=NULL; if(y==3) tok_stk[tok_pos-y+2]=NULL;
		}
		tok_pos-=y-1;

		//문법적으로 명백한 연산 오류를 컴파일 타임에 미리 가려낸다
		if(ps->nOpCode==ASSIGN && ps->pOper[0]->nOpCode!=VARIABLE)
			return CEvaluator::invalidAssign;
		if((ps->nOpCode==DIVIDE || ps->nOpCode==MODULOUS) &&
			(ps->pOper[1]->nOpCode==CONSTANT && ps->pOper[1]->nValue==0) )
			return CEvaluator::divisionZero;
	}
	//여는 괄호가 부족하다
	return baseop==CLOSINGBR ? CEvaluator::invalidMatch: CEvaluator::noError;
}

int CEvaluator::Interpret(PCTSTR st)
{
	int retc=noError, lastopr=0; Clear();

	TOKEN *tok_stk[CCalc::SIZE]={NULL,}; int opc_stk[CCalc::SIZE]; //피연산자와 연산자의 스택
	int tok_pos=0, opc_pos=0;
	while(*st) {
		//skip whitespaces
		while(*st==' ' || *st=='\t') st++; if(!*st) break;

		if(tok_pos==CALCARRAYSIZE(tok_stk)) { retc=tooComplex; break; }
		//피연산자를 찾는다
		if(*st>='0' && *st<='9') { //피연산자
			if(lastopr==CONSTANT) { retc=invalidMatch; break; }
			EVALTYPE x;
			if(*st=='0' && (st[1]|32)=='x') { //16진수
				for(st+=2, x=0; ; st++)
					if(*st>='0' && *st<='9') x=(x<<4)+(*st-'0');
					else if( (*st|32)>='a' && (*st|32)<='f') x=(x<<4)+(*st|32)-'a'+10;
					else break;
			}
			else for(x=0; *st>='0' && *st<='9'; st++) x= x*10+(*st-'0');
			tok_stk[tok_pos]=new TOKEN(CONSTANT, x); tok_pos++;
			lastopr=CONSTANT;
		}
		else if((*st|32)>='a' && (*st|32)<='z') { 
			if(lastopr==CONSTANT) { retc=invalidMatch; break; } //연달아 상수??
			tok_stk[tok_pos]=new TOKEN(VARIABLE, (*st|32)-'a'); tok_pos++;
			while((*st|32)>='a' && (*st|32)<='z') st++;
			lastopr=CONSTANT;
		}
		else {
			int x,y;
			for(y=2;y>=1;y--)
				for(x=0;CCalc::m_szOpc[x];x++)
					if((int)_tcslen(CCalc::m_szOpc[x])==y && _tcsncmp(CCalc::m_szOpc[x], st, y)==0)
						goto Exf;
Exf:
			if(*st=='(') x=OPENINGBR, y=1; else if(*st==')') x=CLOSINGBR, y=1;
			else if(!CCalc::m_szOpc[x]) { retc=invalidChar; break; } //unrecognized code
			else if(CCalc::IsOperator(lastopr)) { //+, -가 단항인지 이항인지 판단
				if(x==PLUS) x=UNARYPLUS; else if(x==MINUS) x=UNARYMINUS;
			}
			else {
				if(x==UNARYPLUS) x=PLUS; else if(x==UNARYMINUS) x=MINUS;
			}

			//앞에 있는 연산자 중, 우리 연산자보다 우선순위가 높은 것을 빼낸다 (여는 괄호는 예외)
			if(x!=OPENINGBR) {
				retc=CCalc::PopStack(tok_stk, tok_pos, opc_stk, opc_pos, x); if(retc) break;
			}
			//우리 연산자를 스택에다 넣는다. (닫는 괄호는 예외)
			if(x!=CLOSINGBR) {
				if(opc_pos==CALCARRAYSIZE(opc_stk)) { retc=tooComplex; break; }
				opc_stk[opc_pos++]=x; lastopr=x;
			}
			else lastopr=CONSTANT; //닫는 괄호의 등급은 상수이다!!
			st+=y;
		}
	}

	if(!retc) { //괄호고 뭐고 할 것 없이 스택에 남아 있는 피연산자와 연산자들을 최종 정리한다
		retc=CCalc::PopStack(tok_stk, tok_pos, opc_stk, opc_pos);
		//피연산자 스택엔 원소가 딱 하나만 있어야 한다
		if(tok_pos!=1 && !opc_pos) retc= tok_pos ? invalidMatch: noToken;
	}

	if(retc)
		for(int x=0;x<CALCARRAYSIZE(tok_stk); x++) //free all other remaining nodes
			CCalc::FreeNode(tok_stk[x]);
	else m_pRoot=tok_stk[0]; //수식 해석에 실패했다면 루트 노드도 지워 버린다
	return retc;
}

TOKEN *CCalc::GetClone(TOKEN *rt)
{
	if(rt) {
		switch(rt->nOpCode) {
		case CONSTANT: return new TOKEN(rt->nOpCode, rt->nValue);
		case VARIABLE: return new TOKEN(rt->nOpCode, rt->nVar);
		default:
			return new TOKEN(rt->nOpCode, GetClone(rt->pOper[0]), GetClone(rt->pOper[1]),
				GetClone(rt->pOper[2]));
		}
	}
	else return NULL;
}

void CCalc::FreeNode(TOKEN *rt)
{
	CCalc::SIMPLESTACK stk[CCalc::SIZE], *cs=stk, *st_top=stk+CALCARRAYSIZE(stk)-1;
	stk[0].pos=0; stk[0].pt=rt;
	while(cs>=stk) {
		if(cs==st_top) return;
		if(cs->pt && CCalc::IsOperator(cs->pt->nOpCode))
			if(cs->pos<3) {cs[1].pt=cs->pt->pOper[cs->pos]; cs->pos++, cs++, cs->pos=0; } //push
			else cs--;
		else { delete cs->pt; cs--; } //delete leaf node
	}
}

void CCalc::CIterator::Infix(const TOKEN& rt, BOOL bParen)
{
	int x;
	switch(rt.nOpCode) {
	case CONSTANT:
		if(m_bHex) _stprintf(m_pszTo, _T("0x%I64x"), rt.nValue);
		else _stprintf(m_pszTo, _T("%I64d"), rt.nValue);
		while(*m_pszTo) m_pszTo++; break;
	case VARIABLE:
		_stprintf(m_pszTo, _T("%c"), rt.nVar+'a'); while(*m_pszTo) m_pszTo++; break;
	default://1==a ? b==c+2 ? a+5:f: 3==2+3
		x=GetPrecedence(rt.nOpCode);
		if(bParen) _tcscpy(m_pszTo, _T("(")), m_pszTo++;
		switch(rt.nOpCode) {
		case CONDITIONAL:
			Infix(*rt.pOper[0], GetPrecedence(rt.pOper[0]->nOpCode)>=x);
			_tcscpy(m_pszTo, _T(" ? ")); m_pszTo+=3;
			Infix(*rt.pOper[1], GetPrecedence(rt.pOper[1]->nOpCode)>x);
			_tcscpy(m_pszTo, _T(" : ")); m_pszTo+=3;
			Infix(*rt.pOper[2], GetPrecedence(rt.pOper[2]->nOpCode)>x);
			break;
		case UNARYMINUS: case UNARYPLUS: case ONECOMPL: case LOGICALNOT: //prefix 단항
			_tcscpy(m_pszTo, m_szOpc[rt.nOpCode]); while(*m_pszTo) m_pszTo++;
			Infix(*rt.pOper[0], GetPrecedence(rt.pOper[0]->nOpCode)>x);
			break;
		case ASSIGN: //오른쪽에서 왼쪽으로 결합하는 이항 연산자
			Infix(*rt.pOper[0], GetPrecedence(rt.pOper[0]->nOpCode)>=x);
			_tcscpy(m_pszTo, m_szOpc[rt.nOpCode]); while(*m_pszTo) m_pszTo++;
			Infix(*rt.pOper[1], GetPrecedence(rt.pOper[1]->nOpCode)>x);
			break;
		default: //왼쪽에서 오른쪽으로 결합하는 이항 연산자
			Infix(*rt.pOper[0], GetPrecedence(rt.pOper[0]->nOpCode)>x);
			_tcscpy(m_pszTo, m_szOpc[rt.nOpCode]); while(*m_pszTo) m_pszTo++;
			Infix(*rt.pOper[1], GetPrecedence(rt.pOper[1]->nOpCode)>=x);
		}
		if(bParen) _tcscpy(m_pszTo, _T(")")), m_pszTo++;
		break;
	}
}

CEvaluator::CEvaluator(EVALTYPE x): m_pRoot(new TOKEN(CONSTANT, x)) {}

void CEvaluator::Clear()
{
	CCalc::FreeNode(m_pRoot); m_pRoot=NULL;
}

//a? b: c? d: e는 a? b: (c ? d: e)와 같다. 따라서 오른쪽에서 왼쪽으로 결합
//우선순위가 같은 연산자들은 결합 순서도 같다.
int CCalc::GetPrecedence(int cod)
{
#define SAMEORDER(a,b) if(cod>=a && cod<=b) return a
	SAMEORDER(ONECOMPL, UNARYPLUS);
	else SAMEORDER(TIMES, MODULOUS);
	else SAMEORDER(PLUS, MINUS);
	else SAMEORDER(LEFTSHIFT, RIGHTSHIFT);
	else SAMEORDER(LESSTHAN, GREATERTHANEQ);
	else SAMEORDER(EQUALTO, INEQUAL);
	else SAMEORDER(CONDITIONAL, CONDITIONAL2);
	else return cod;
#undef SAMEORDER
}

int CCalc::IsRightToLeft(int cod)
{
	//단항 연산자, 대입 연산자, ? : 연산자.
	//1? 0?2:3 :4 이런 문장을 보면 ? 연산자는 오른쪽에서 왼쪽으로 평가된다
	return cod==ASSIGN || (cod>=ONECOMPL && cod<=UNARYPLUS) || cod==CONDITIONAL || cod==CONDITIONAL2;
}

int CCalc::GetDegree(int cod)
{
	if(cod>=ONECOMPL && cod<=UNARYPLUS) return 1; //단항 연산자
	else if(cod==CONDITIONAL || cod==CONDITIONAL2) return 3; //삼항 연산자
	else return 2; //나머지는 이항 연산자
}

PCTSTR CCalc::m_szOpc[]={
	_T("~"), _T("!"), _T("-"), _T("+"), _T("*"), _T("/"), _T("%"), _T("+"), _T("-"), _T("<<"),
	_T(">>"), _T("<"), _T(">"), _T("<="), _T(">="), _T("=="), _T("!="), _T("&"), _T("^"),
	_T("|"), _T("&&"), _T("||"), _T("?"), _T(":"), _T("="), _T(","), NULL
};

int CEvaluator::Evaluate(EVALTYPE& res, EVALTYPE *pVar, UINT *pvcg) const
{
	if(!m_pRoot) return noToken;
	struct {
		EVALTYPE calval[3]; TOKEN *pt;
		int pos, loc; //NOTE: loc is very likely to be elmiated
	} stk[CCalc::SIZE], *cs=stk, *st_top=stk+CALCARRAYSIZE(stk)-1;
	stk[0].pos=stk[0].loc=0; stk[0].pt=m_pRoot;

	int op;
	while(cs>=stk) {
		if(cs==st_top) return tooComplex; op=cs->pt->nOpCode;
		switch(op) {
		case CONSTANT: //for non-operators
			cs->calval[cs->loc]=cs->pt->nValue; break;
		case VARIABLE:
			cs->calval[cs->loc]=pVar ? pVar[cs->pt->nVar]: 0; break;
		default: {
			EVALTYPE &ea=cs[1].calval[0], &eb=cs[1].calval[1], &ev=cs->calval[cs->loc];
			switch(cs->pos) { //if sufficient number of operands are provided
			case 1:
				switch(op) {
				case ONECOMPL: ev=~ea; goto Q; case LOGICALNOT: ev=!ea; goto Q;
				case UNARYMINUS: ev=-ea; goto Q; case UNARYPLUS: ev=ea; goto Q;
				default: if(op>=__endopc || op<0) return invalidCode;
				}
				break;
			case 2:
				switch(op) {
				//arithemetic
				case TIMES: ev=ea*eb; goto Q;
				case DIVIDE: if(eb) { ev=ea/eb; goto Q; } else return divisionZero;
				case MODULOUS: if(eb) { ev=ea%eb; goto Q; } else return divisionZero;
				case PLUS: ev=ea+eb; goto Q; case MINUS: ev=ea-eb; goto Q;
				case LEFTSHIFT: ev=ea<<eb; goto Q; case RIGHTSHIFT: ev=ea>>eb; goto Q;

				//logical
				case LESSTHAN: ev=ea<eb; goto Q; case GREATERTHAN: ev=ea>eb; goto Q;
				case LESSTHANEQ: ev=ea<=eb; goto Q; case GREATERTHANEQ: ev=ea>=eb; goto Q;
				case EQUALTO: ev=ea==eb; goto Q; case INEQUAL: ev=ea!=eb; goto Q;

				//bit
				case BITAND: ev=ea&eb; goto Q;
				case BITXOR: ev=ea^eb; goto Q; case BITOR: ev=ea|eb; goto Q;

				//logic
				case AND: case OR: ev=!!eb; goto Q; //lazy evaluation

				//et cetra
				case CONDITIONAL: ev=eb; goto Q; //it's already handled in a special way
				case ASSIGN:
					if(cs->pt->pOper[0]->nOpCode!=VARIABLE) return invalidAssign;
					ev=eb; if(pVar) pVar[cs->pt->pOper[0]->nVar]=eb; goto Q;
				case COMMA:	ev=eb; goto Q;
				default: if(op>=__endopc || op<0) return invalidCode;
				}
				break;
			}

			//preprae to push stack
			if(cs->pos==1) { //first operand is determined
				EVALTYPE &ea=cs[1].calval[0], &ev=cs->calval[cs->loc];
				switch(op) {
				case AND: if(!ea) { ev=0; goto Q; } break; //unconditionally FALSE
				case OR: if(ea) { ev=1; goto Q; } break; //unconditionally TRUE
				case CONDITIONAL: //decide which token to evaluate
					cs[1].pt=cs->pt->pOper[ea ? 1:2], cs[1].loc=cs->pos; goto P;
				}
			}
			cs[1].pt=cs->pt->pOper[cs->pos], cs[1].loc=cs->pos;
		P:
			cs->pos++, cs++, cs->pos=0; continue;
		}
		}
	Q:
		cs--; //pop
	}
	res=stk[0].calval[0]; return noError;
}

TOKEN::TOKEN(int cod, EVALTYPE pp): nOpCode(cod)
{
	if(cod==CONSTANT) nValue=pp; else nVar=(int)pp;
}

TOKEN::TOKEN(int cod, TOKEN *p1, TOKEN *p2, TOKEN *p3): nOpCode(cod)
{
	pOper[0]=p1, pOper[1]=p2, pOper[2]=p3;
}

EVALTYPE CEvaluator::QuickEvaluate(PCTSTR pszExp, EVALTYPE *pVar, int *pErr)
{
	CEvaluator ev; EVALTYPE vp; int ec=ev.Interpret(pszExp);
	if(ec) { if(pErr) *pErr=ec; return 0; }
	ec=ev.Evaluate(vp, pVar); if(pErr) *pErr=ec; return ec ? 0: vp;
}

BOOL CEvaluator::Restore(PTSTR pszStr, BOOL bHex) const
{
	if(m_pRoot) {
		CCalc::CIterator is; is.m_pszTo=pszStr; is.m_bHex=bHex;
		is.Infix(*m_pRoot); return TRUE;
	}
	else { *pszStr=0; return FALSE; }
}

void CEvaluator::CopyObject(const CEvaluator& obj)
{
	if(&obj!=this) m_pRoot=CCalc::GetClone(obj.m_pRoot);
}

/*int main()
{
	TCHAR buf[100]; CEvaluator ev; EVALTYPE vals[26]={0,};
	EVALTYPE vr; int px;
	while(1) {
		printf("? "); gets(buf); if(*buf==0) break;
		px=ev.Interpret(buf);
		if(px) printf("Interpretion error: %d\n", px);
		else {
			px=ev.Evaluate(vr, vals);
			if(px) printf("Evaluation error: %d\n", px);
			else printf("%I64d\n", vr);
		}
	}
	return 0;
}*/
