/////////////////////////////////////////
// ����Ʈ Ŭ����
////////////////////////////////////////

template <class T> class CDList
{
protected:
	//�Ӹ��� ���� ������
	T* head;
	T* tail;
	T* current;
	int count;	//��� �ִ� ����Ʈ �� ��

public:
	//�����ڿ� �Ҹ���
	CDList();
	~CDList();

	void AddList(T value);			//����Ʈ �߰�
	void InsertList(T valuse);		//current �����Ϳ� ����
	void EditList(T* ptr, T value);	//�������̵� �� �� �� �κи� �������̵� ���ָ� ��

	void Delete(T* ptr);			//�����Ͱ� ����Ű�� ����Ʈ�� ����
	void DeleteCurrent();			//Current �����Ͱ� ����Ű�� ����Ʈ ����
	void DeleteHead();				//����Ʈ�� ���� ���� ����
	void DeleteTail();				//����Ʈ�� �������� ����
	void DeleteDetail(T* ptr);		//�������̵� �� �� �� �κи� �������̵� ���ָ� ��

	int GetCount();					//����Ʈ ���� �˾ƿ���

	T* GetHead();					//�Ӹ� �ּҰ� ��������
	T* GetTail();					//���� �ּҰ� ��������
	T* GetPrev();					//���� ����Ʈ ���ϱ�
	T* GetNext();					//���� ����Ʈ ���ϱ�
	T* GetCurrent();				//���� �۾� ���� ����Ʈ �ּҰ� ��������

	void Remove();				//����Ʈ ��� ����
};


/////////////////////////////////////////
// �迭 Ŭ����
////////////////////////////////////////

template <class T> class CDArray
{
protected:
	//�迭�� ũ��
	int array_max;

public:
	//������
	T* data;

	//�����ڿ� �Ҹ���
	CDArray(int size);
	~CDArray();
};


/////////////////////////////////////////
// ����Ʈ �޼ҵ�
////////////////////////////////////////

//�����ڿ� �Ҹ���
template <class T> CDList<T>::CDList()
{
	head=tail=current=NULL;
	count=0;
}

template <class T> CDList<T>::~CDList()
{
	Remove();
}

//�Է� ����
template <class T> void CDList<T>::AddList(T value)
{
	//���ο� �� �ֱ�
	T* new_list = new T;
	memcpy(&new_list, &value, sizeof(T));
	
	//����
	if(tail!=NULL)tail->next=new_list;
	new_list->prev=tail;
	new_list->next=NULL;

	//�Ӹ����� ������
	if(head==NULL)head=new_list;
	current=tail=new_list;

	count++;
}

template <class T> void CDList<T>::InsertList(T value)
{
	if(current==NULL)AddList(valuse);

	//���ο� �� �ֱ�
	T* new_list = new T;
	memcpy(new_list, value, sizeof(T));
	
	//���� ����
	if(current->prev!=NULL)
	{
		current->prev->next=new_list;
		new_list->prev=current->prev;
	}
	else
	{
		head=new_list;
		new_list->prev=NULL;
	}
	//���� ����
	current->prev=new_list;
	new_list->next=current;

	current=new_list;
	count++;
}

template <class T> void CDList<T>::EditList(T* ptr, T value)
{
	//���ο� �� �ֱ�
	memcpy(ptr, value, sizeof(T));
}

//���� ����
template <class T> void CDList<T>::Remove()
{
	T* ptr=head;
	while(ptr!=NULL)
	{
		T* old_ptr=ptr->next;	//���� �ּ� ���
		DeleteDetail(ptr);
		ptr=old_ptr;
	}

	//�ʱ�ȭ
	head=NULL;
	tail=NULL;
	current=NULL;
	count=0;
}

template <class T> void CDList<T>::Delete(T* ptr)
{
	if(ptr!=NULL)
	{
		//�յ��� ����Ʈ ���� ����
		if(ptr->prev!=NULL)ptr->prev->next=ptr->next;
			else head=ptr->next;
		if(ptr->next!=NULL)ptr->next->prev=ptr->prev;
			else tail=ptr->prev;

		delete[] ptr;
		ptr=NULL;

		count--;
	}
}

template <class T> void CDList<T>::DeleteHead()
{
	Delete(head);
}

template <class T> void CDList<T>::DeleteTail()
{
	Delete(tail);
}

//���� ���� �޸� �Ҵ����� delete [] ������ ������ ���� �� ���� ���
//�� �κ��� �������̵��ؼ� �ڵ带 �Է��� �ָ� ��
template <class T> void CDList<T>::DeleteDetail(T* ptr)
{
	delete [] ptr;
	ptr=NULL;
}

//ã�� ����
template <class T> int CDList<T>::GetCount()
{
	return count;
}

template <class T> T* CDList<T>::GetHead()
{
	current=head;
	return head;
}

template <class T> T* CDList<T>::GetTail()
{
	current=tail;
	return tail;
}

template <class T> T* CDList<T>::GetPrev()
{
	if((current->prev)==NULL)return NULL;

	curren=current->prev;
	return current;
}

template <class T> T* CDList<T>::GetNext()
{
	if((current->next)==NULL)return NULL;

	curren=current->next;
	return current;
}

template <class T> T* CDList<T>::GetCurrent()
{
	return current;
}


//////////////////////////////
// �迭 �޼ҵ�
//////////////////////////////

template <class T> CDArray<T>::CDArray(int size)
{
	array_max=size;

	data=new T[array_max];
}

template <class T> CDArray<T>::~CDArray()
{
	delete [] data;
	data=NULL;
}