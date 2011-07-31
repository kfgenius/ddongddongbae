#include "calc.h"

//baseop���� �켱������ ���ų� ���� ��(��, �ڵ尪�� �� ���� ��)�� ��� ������.
//��, baseop�� right-to-left�̸� �� ����.
int CCalc::PopStack(TOKEN **tok_stk, int& tok_pos, int *opc_stk, int& opc_pos, int baseop)
{
	int x,y,z=GetPrecedence(baseop); TOKEN *ps;
	for(;opc_pos>0;opc_pos--) {
		x=opc_stk[opc_pos-1]; y=GetPrecedence(x);
		//������ ������, �� �Ŀ��� ���ʿ� �ִ� �����ڰ� �츮 ���� �����ں��� �켱������ ���ų�,
		//�켱������ ���Ƶ� ������ �ݴ��, ���߿� ó���ǵ��� �����ڸ� �� ������ �ʴ´�.
		if(x==CONDITIONAL2 && (z>y || baseop==CONDITIONAL2)) {
			//?:�� ���� ������? baseop ��ü�� :�̰ų�, baseop�� ?:���ٵ� �켱 ������ ���� ����
			//x �տ��� CONDITIONAL�� �־�� �Ѵ�. �̰� ��������.
			if(opc_pos<2) return CEvaluator::invalidMatch;
			else {
				if(opc_stk[opc_pos-2]!=CONDITIONAL) return CEvaluator::invalidMatch;
				//try again with ? operator
				opc_pos--; x=opc_stk[opc_pos-1]; y=GetPrecedence(x);
			}
		}
		else if(y>z || (y==z && IsRightToLeft(x) && IsRightToLeft(baseop)) ) break;

		//�ݴ� ��ȣ�� ���� ���� ��ȣ�� �������Ƿ� �ӹ� �ϼ�
		if(x==OPENINGBR && baseop==CLOSINGBR) { opc_pos--; return CEvaluator::noError; }

		y=GetDegree(x);
		if(tok_pos-y<0) return CEvaluator::invalidMatch; //stack underflow.. �ǿ����ڰ� �����ϴ�
		tok_stk[tok_pos-y]=ps=new TOKEN(x, tok_stk[tok_pos-y],
			y>=2 ? tok_stk[tok_pos-y+1]: NULL, y==3 ? tok_stk[tok_pos-y+2]: NULL);
		if(y>=2) { //clear the buffer that has become a child
			tok_stk[tok_pos-y+1]=NULL; if(y==3) tok_stk[tok_pos-y+2]=NULL;
		}
		tok_pos-=y-1;

		//���������� ����� ���� ������ ������ Ÿ�ӿ� �̸� ��������
		if(ps->nOpCode==ASSIGN && ps->pOper[0]->nOpCode!=VARIABLE)
			return CEvaluator::invalidAssign;
		if((ps->nOpCode==DIVIDE || ps->nOpCode==MODULOUS) &&
			(ps->pOper[1]->nOpCode==CONSTANT && ps->pOper[1]->nValue==0) )
			return CEvaluator::divisionZero;
	}
	//���� ��ȣ�� �����ϴ�
	return baseop==CLOSINGBR ? CEvaluator::invalidMatch: CEvaluator::noError;
}

int CEvaluator::Interpret(PCTSTR st)
{
	int retc=noError, lastopr=0; Clear();

	TOKEN *tok_stk[CCalc::SIZE]={NULL,}; int opc_stk[CCalc::SIZE]; //�ǿ����ڿ� �������� ����
	int tok_pos=0, opc_pos=0;
	while(*st) {
		//skip whitespaces
		while(*st==' ' || *st=='\t') st++; if(!*st) break;

		if(tok_pos==CALCARRAYSIZE(tok_stk)) { retc=tooComplex; break; }
		//�ǿ����ڸ� ã�´�
		if(*st>='0' && *st<='9') { //�ǿ�����
			if(lastopr==CONSTANT) { retc=invalidMatch; break; }
			EVALTYPE x;
			if(*st=='0' && (st[1]|32)=='x') { //16����
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
			if(lastopr==CONSTANT) { retc=invalidMatch; break; } //���޾� ���??
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
			else if(CCalc::IsOperator(lastopr)) { //+, -�� �������� �������� �Ǵ�
				if(x==PLUS) x=UNARYPLUS; else if(x==MINUS) x=UNARYMINUS;
			}
			else {
				if(x==UNARYPLUS) x=PLUS; else if(x==UNARYMINUS) x=MINUS;
			}

			//�տ� �ִ� ������ ��, �츮 �����ں��� �켱������ ���� ���� ������ (���� ��ȣ�� ����)
			if(x!=OPENINGBR) {
				retc=CCalc::PopStack(tok_stk, tok_pos, opc_stk, opc_pos, x); if(retc) break;
			}
			//�츮 �����ڸ� ���ÿ��� �ִ´�. (�ݴ� ��ȣ�� ����)
			if(x!=CLOSINGBR) {
				if(opc_pos==CALCARRAYSIZE(opc_stk)) { retc=tooComplex; break; }
				opc_stk[opc_pos++]=x; lastopr=x;
			}
			else lastopr=CONSTANT; //�ݴ� ��ȣ�� ����� ����̴�!!
			st+=y;
		}
	}

	if(!retc) { //��ȣ�� ���� �� �� ���� ���ÿ� ���� �ִ� �ǿ����ڿ� �����ڵ��� ���� �����Ѵ�
		retc=CCalc::PopStack(tok_stk, tok_pos, opc_stk, opc_pos);
		//�ǿ����� ���ÿ� ���Ұ� �� �ϳ��� �־�� �Ѵ�
		if(tok_pos!=1 && !opc_pos) retc= tok_pos ? invalidMatch: noToken;
	}

	if(retc)
		for(int x=0;x<CALCARRAYSIZE(tok_stk); x++) //free all other remaining nodes
			CCalc::FreeNode(tok_stk[x]);
	else m_pRoot=tok_stk[0]; //���� �ؼ��� �����ߴٸ� ��Ʈ ��嵵 ���� ������
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
		case UNARYMINUS: case UNARYPLUS: case ONECOMPL: case LOGICALNOT: //prefix ����
			_tcscpy(m_pszTo, m_szOpc[rt.nOpCode]); while(*m_pszTo) m_pszTo++;
			Infix(*rt.pOper[0], GetPrecedence(rt.pOper[0]->nOpCode)>x);
			break;
		case ASSIGN: //�����ʿ��� �������� �����ϴ� ���� ������
			Infix(*rt.pOper[0], GetPrecedence(rt.pOper[0]->nOpCode)>=x);
			_tcscpy(m_pszTo, m_szOpc[rt.nOpCode]); while(*m_pszTo) m_pszTo++;
			Infix(*rt.pOper[1], GetPrecedence(rt.pOper[1]->nOpCode)>x);
			break;
		default: //���ʿ��� ���������� �����ϴ� ���� ������
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

//a? b: c? d: e�� a? b: (c ? d: e)�� ����. ���� �����ʿ��� �������� ����
//�켱������ ���� �����ڵ��� ���� ������ ����.
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
	//���� ������, ���� ������, ? : ������.
	//1? 0?2:3 :4 �̷� ������ ���� ? �����ڴ� �����ʿ��� �������� �򰡵ȴ�
	return cod==ASSIGN || (cod>=ONECOMPL && cod<=UNARYPLUS) || cod==CONDITIONAL || cod==CONDITIONAL2;
}

int CCalc::GetDegree(int cod)
{
	if(cod>=ONECOMPL && cod<=UNARYPLUS) return 1; //���� ������
	else if(cod==CONDITIONAL || cod==CONDITIONAL2) return 3; //���� ������
	else return 2; //�������� ���� ������
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
