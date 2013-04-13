//최대값
static int Max(int x, int y)
{
	if(x>y)return x;
		else return y;
}

//최소값
static int Min(int x, int y)
{
	if(x>y)return y;
		else return x;
}

//최대 최소값
static int MaxMin(int x, int min, int max)
{
	if(x<min)return min;
		else if(x>max) return max;
		else return x;
}

//WORD(16bit)크기의 RGB
static WORD RGB2(WORD r, WORD g, WORD b)
{
	return ((r>>3)<<11) | ((g>>2)<<5) | (b>>3);
}

static bool Between(int value, int min, int max)
{
	if(value>=min && value<=max)return true;
		else return false;
}