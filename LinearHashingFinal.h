#ifndef LINEARHASHING_H
#define LINEARHASHING_H
#include <iostream>
#include "Container.h"

class HashingEmptyException : public ContainerException
{
public:
	virtual const char * what() const noexcept override
	{
		return "Hashing: empty";
	}
};

template<typename E, size_t N >
class Bucket {

public:
	size_t position;
	Bucket* next;
	E *values;
	Bucket()
	{
		values = new E[N]{};
		next = nullptr;
		position = 0;
	}

	~Bucket()
	{
		delete[] values;
		if (next) delete next;
	}

	void addElement(const E& e)
	{
		if (position<N)
		{
			values[position] = e;
			position++;
		}
		else
		{
			if (next == nullptr)
			{
				next = new Bucket<E, N>;
			}
			next->addElement(e);
		}
	}

	std::ostream& print(std::ostream& o) const
	{

		for (size_t i = 0; i < position; i++)
		{
			o << values[i] << "-";
		}

		if (next)
		{
			o << "->";
			next->print(o);
		}
		return o;
	}
};

template <typename E, size_t N = 3>
class LinearHashing : public Container<E>
{
	size_t d;
	size_t nextToSplit;
	size_t deepth;
	Bucket<E, N>* table;
	size_t tableSize;
	size_t  tableMaxSize;

public:

	LinearHashing()
	{
		d = 0;
		nextToSplit = 0;
		deepth = 1;
		table = new Bucket<E, N>[2];
		tableSize = 0;
		tableMaxSize = 0;
	}
	~LinearHashing() {
		delete[] table;
	}

	LinearHashing(std::initializer_list<E> el) : LinearHashing()
	{
		for (auto e : el) add(e);
	}

	size_t hash(size_t d, const E& e) const;
	void quickSort(E* mas, size_t first, size_t last)const;
	void remove(const E& e)
	 {  
	 	size_t index;
	 	
	 	if(hash(d,e)>=nextToSplit)
	 		index=hash(d,e);
	 	else
	 		index=hash(d+1,e);
	 		
	 	Bucket<E,N>* ptr=&table[index];
		
		while(ptr)
		{
			for(int i=0;i<ptr->position;i++)
			{
				if(e==ptr->values[i])
				{
					ptr->values[i]=ptr->values[(ptr->position)-1];
					ptr->position--;
					tableSize--;
					return;	
				}	
			}
			ptr=ptr->next;	
		}			
	 };
	void remove(const E e[], size_t s) 
	{
		for (size_t i = 0; i < s; i++)
		{
				remove(e[i]);
				
		}	
	};
	bool member(const E& e) const
	{
		size_t index;

		if (hash(d, e) >= nextToSplit)
		{
			index = hash(d, e);
		}
		else
			index = hash(d + 1, e);

		Bucket<E, N>* temp = &table[index];

		while (temp)
		{
			for (size_t i = 0; i < temp->position; i++)
			{
				if (temp->values[i] == e)
					return true;
			}
			temp = temp->next;
		}

		return false;
	}

	E min() const
	{
		if (this->empty()) throw HashingEmptyException();
		Bucket<E,N>* ptr=table;
		E* minValue=nullptr;
		Bucket<E,N>* temp;
		
		for(size_t i=0; i<deepth;i++)
		{
			temp=ptr+i;
			
				do
				{		
					for (size_t j = 0; j < temp->position; j++)
						{
							if(minValue==nullptr || *minValue>temp->values[j])
								minValue=&temp->values[j];
						}
					temp = temp->next;
				} while (temp);			
		}			
		return *minValue;
	}
	E max() const
	{
		if (this->empty()) throw HashingEmptyException();
		Bucket<E,N>* ptr=table;
		E* maxValue=nullptr;
		Bucket<E,N>* temp;
		
		for(size_t i=0; i<deepth;i++)
		{
			temp=ptr+i;
			
				do
				{		
					for (size_t j = 0; j < temp->position; j++)
						{
							if(maxValue==nullptr|| temp->values[j]>*maxValue)
								maxValue=&temp->values[j];
						}
					temp = temp->next;
				} while (temp);			
		}			
		return *maxValue;
	}

	bool empty() const
	{
		return size() == 0;
	}

	size_t size() const
	{
		return tableSize;
	}

	size_t apply(std::function<void(const E&)> f, Order order = dontcare) const
	{		
	
		if(empty())return 0;
		Bucket<E,N>* temp=table;
		Bucket<E,N>* ptr;
		size_t posInArr=0;
		size_t rc=0;
		
		if (order == dontcare) {
     	try {
		     	
       	for(int i=0;i<deepth;i++)
       	{
       		ptr=temp+i;	
			do
			{
				for(int j=0;j<ptr->position;j++)
				{
					f(ptr->values[j]);
					rc++;
				}	
				ptr=ptr->next;	
			}while(ptr);		
		}
     } catch (...) {}
   } else {
     E * val = new E[tableSize];
     
		for(size_t i=0; i<deepth;i++)
		{
			ptr=temp+i;			
				do
				{	
					for (size_t j = 0; j < ptr->position; j++)
						{
							val[posInArr]=ptr->values[j];
							posInArr++;
						}
					ptr = ptr->next;
				} while (ptr);			
		}		
		quickSort(val,0,tableSize-1);

     try {
       if (order == descending) {
         for (size_t i = tableSize; i;) {
           f(val[--i]);
           rc++;
         }
       } else {
         for (size_t i = 0; i < tableSize; ++i) {
           f(val[i]);
           rc++;
         }
       }
     } catch (...) {}
     delete[] val;
   }
   return rc;
}

	void add(const E& e)
	{
		size_t index;

		if (hash(d, e) >= nextToSplit)
		{
			index = hash(d, e);
		}
		else
			index = hash(d + 1, e);

		if (!member(e))
		{

			table[index].addElement(e);
			tableSize++;
			
			if (tableSize >= deepth*N)
			{

				deepth++;
				nextToSplit++;
				rehashing();
		
				if (nextToSplit == (1 << d))
					{
						d++;
						nextToSplit = 0;
						
					Bucket<E, N>* temp = table;
					table = new Bucket<E, N>[1<<(d+1)];
					Bucket<E, N>* bucketPtr;
					
				for (size_t i = 0; i < deepth; i++)
				{
					bucketPtr = temp + i;
					do {
						for (size_t j = 0; j < bucketPtr->position; j++)
						{
							table[i].addElement(bucketPtr->values[j]);
						}
						bucketPtr = bucketPtr->next;
					} while (bucketPtr);
				}
				delete[] temp;
					}
				
			}
		}
	}
	void add(const E e[], size_t s)
	{
		for (size_t i = 0; i < s; i++)
		{
			if (!member(e[i]))
				add(e[i]);
		}
	}

	void rehashing()
	{

		Bucket<E, N>* temp = &table[nextToSplit - 1];
		while (temp)
		{
			for (size_t i = 0; i <(temp->position); i++)
			{
				size_t a = hash(d + 1, temp->values[i]);
				if ((deepth-1) == a)
				{
					table[deepth-1].addElement(temp->values[i]);
					temp->values[i] = temp->values[(temp->position) - 1];
					temp->position--;
					i--;
				}
			}
			temp = temp->next;
		}
	}

	std::ostream& print(std::ostream& o) const
	{
		o << "table size: " << tableSize << " nmax: " << deepth*N << " nextto split: " << nextToSplit << " d = " << d << '\n';
		for (size_t i = 0; i < deepth; i++)
		{
			table[i].print(o);
			o << "\n";
		}
		return o;
	}
};

template<typename E, size_t N>
size_t LinearHashing<E, N>::hash(size_t d, const E& e) const {
	size_t myHashValue = hashValue(e);
	if (d == 0)
		return 0;
	return myHashValue&((1 << (d)) - 1);
}

template<typename E, size_t N> 
void LinearHashing<E, N>::quickSort(E *mas, size_t first, size_t last)const 
{ 
	E mid; 
	size_t f = first, l = last; 
	mid = mas[(f + l) / 2]; 
	do 
	{	 
	while (mid>mas[f]) f++; 
	while (mas[l]>mid) l--; 
	if (l>=f) 
	{ 
	std::swap(mas[f],mas[l]); 
	f++; 
	if(l!=0)
	l--; 
	} 
	} while (l>f); 
	if (l>first) quickSort(mas, first, l); 
	if (last>f) quickSort(mas, f, last); 
} 


#endif

