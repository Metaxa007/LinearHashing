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
	size_t pushindex;

public:

	LinearHashing()
	{
		d = 0;
		nextToSplit = 0;
		deepth = 1;
		table = new Bucket<E, N>[deepth];
		tableSize = 0;
		tableMaxSize = 0;
		pushindex = N;
	}
	~LinearHashing() {
		delete[] table;
	}

	LinearHashing(std::initializer_list<E> el) : LinearHashing()
	{
		for (auto e : el) add(e);
	}

	size_t hash(size_t d, const E& e) const;
	void remove(const E& e)
	 {  
	 	size_t index;
	 	
	 	if(hash(d,e)>=nextToSplit)
	 		index=hash(d,e);
	 	else
	 		index=hash(d,e+1);
	 		
	 	Bucket<E,N>* ptr=table[index];
		
		while(ptr)
		{
			for(int i=0;i<ptr->position;i++)
			{
				if(e==ptr->values[i])
				{
					ptr->values[i]=ptr->values[(ptr->position)-1];
					ptr->position--;
					return;	
				}	
			}
			ptr=ptr->next;	
		}			
	 };
	void remove(const E e[], size_t s) 
	{
			
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
		return table[0].values[0];
	}
	E max() const
	{
		return table[0].values[0];
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
		return 0;
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
			
			if (tableSize >= deepth*N*0.9)
			{

				deepth += pushindex;

				Bucket<E, N>* temp = table;
				table = new Bucket<E, N>[deepth];
				Bucket<E, N>* bucketPtr;
				for (size_t i = 0; i < deepth - pushindex; i++)
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
				for (size_t t=0; t < pushindex; t++) {					
					nextToSplit++;
					rehashing();

					if (nextToSplit == (1 << d))
					{
						d++;
						nextToSplit = 0;
					}
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
				if (nextToSplit-1 + (1<<(d)) == a)
				{
					table[nextToSplit-1 + (1 << (d))].addElement(temp->values[i]);
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

#endif
