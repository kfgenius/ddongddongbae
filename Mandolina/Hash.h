//�����̺꿡�� ���� ��Ʈ���� �ؽ������� �ٲٴ� ����� ���� ��ü

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
