//문라이브에서 쓰는 스트링을 해쉬값으로 바꾸는 방법을 담은 객체

class CHash
{
private:
	CHash() {};
	CHash(const CHash& other);
	~CHash() {};

public:
	static CHash* GetInstance();
	int GetHash(char* sz);
};
