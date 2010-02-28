/////////////////////////////////////////
// 리스트 클래스
////////////////////////////////////////

template <class T> class CDList
{
protected:
	//머리와 꼬리 포인터
	T* head;
	T* tail;
	T* current;
	int count;	//들어 있는 리스트 총 수

public:
	//생성자와 소멸자
	CDList();
	~CDList();

	void AddList(T value);			//리스트 추가
	void InsertList(T valuse);		//current 포인터에 넣음
	void EditList(T* ptr, T value);	//오버라이들 할 때 이 부분만 오버라이드 해주면 됨

	void Delete(T* ptr);			//포인터가 가리키는 리스트를 지움
	void DeleteCurrent();			//Current 포인터가 가리키는 리스트 지움
	void DeleteHead();				//리스트의 제일 앞을 지움
	void DeleteTail();				//리스트의 마지막을 지움
	void DeleteDetail(T* ptr);		//오버라이드 할 때 이 부분만 오버라이드 해주면 됨

	int GetCount();					//리스트 수를 알아오기

	T* GetHead();					//머리 주소값 가져오기
	T* GetTail();					//꼬리 주소값 가져오기
	T* GetPrev();					//앞의 리스트 구하기
	T* GetNext();					//다음 리스트 구하기
	T* GetCurrent();				//지금 작업 중인 리스트 주소값 가져오기

	void Remove();				//리스트 모두 제거
};


/////////////////////////////////////////
// 배열 클래스
////////////////////////////////////////

template <class T> class CDArray
{
protected:
	//배열의 크기
	int array_max;

public:
	//데이터
	T* data;

	//생성자와 소멸자
	CDArray(int size);
	~CDArray();
};


/////////////////////////////////////////
// 리스트 메소드
////////////////////////////////////////

//생성자와 소멸자
template <class T> CDList<T>::CDList()
{
	head=tail=current=NULL;
	count=0;
}

template <class T> CDList<T>::~CDList()
{
	Remove();
}

//입력 종류
template <class T> void CDList<T>::AddList(T value)
{
	//새로운 값 넣기
	T* new_list = new T;
	memcpy(&new_list, &value, sizeof(T));
	
	//연결
	if(tail!=NULL)tail->next=new_list;
	new_list->prev=tail;
	new_list->next=NULL;

	//머리값과 꼬리값
	if(head==NULL)head=new_list;
	current=tail=new_list;

	count++;
}

template <class T> void CDList<T>::InsertList(T value)
{
	if(current==NULL)AddList(valuse);

	//새로운 값 넣기
	T* new_list = new T;
	memcpy(new_list, value, sizeof(T));
	
	//앞쪽 연결
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
	//뒤쪽 연결
	current->prev=new_list;
	new_list->next=current;

	current=new_list;
	count++;
}

template <class T> void CDList<T>::EditList(T* ptr, T value)
{
	//새로운 값 넣기
	memcpy(ptr, value, sizeof(T));
}

//삭제 종류
template <class T> void CDList<T>::Remove()
{
	T* ptr=head;
	while(ptr!=NULL)
	{
		T* old_ptr=ptr->next;	//다음 주소 백업
		DeleteDetail(ptr);
		ptr=old_ptr;
	}

	//초기화
	head=NULL;
	tail=NULL;
	current=NULL;
	count=0;
}

template <class T> void CDList<T>::Delete(T* ptr)
{
	if(ptr!=NULL)
	{
		//앞뒤의 리스트 정보 변경
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

//만약 동적 메모리 할당으로 delete [] 만으로 깨끗이 지울 수 없는 경우
//이 부분을 오버라이드해서 코드를 입력해 주면 됨
template <class T> void CDList<T>::DeleteDetail(T* ptr)
{
	delete [] ptr;
	ptr=NULL;
}

//찾기 종류
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
// 배열 메소드
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