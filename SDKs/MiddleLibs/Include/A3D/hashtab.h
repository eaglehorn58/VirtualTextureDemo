 /*
 * FILE: hashtab.h
 *
 * DESCRIPTION: hash table
 *
 * CREATED BY: Cui Ming 2002-1-21
 *
 * HISTORY: abase::hashtab的插入和删除，查找效率不如AHashTble,但提供遍历功能,使用时需要注意
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
*/

#ifndef __ABASE_HASH_TABLE_H__
#define __ABASE_HASH_TABLE_H__

#include "ATypes.h"
#include "vector.h"

#pragma warning (disable: 4786)
#pragma warning (disable: 4284)

namespace abase
{

enum { __abase_num_primes = 28};	//prime list from stl :)
static const auint32 __abase_prime_list[__abase_num_primes] =
{
  53ul,         97ul,         193ul,       389ul,       769ul,
  1543ul,       3079ul,       6151ul,      12289ul,     24593ul,
  49157ul,      98317ul,      196613ul,    393241ul,    786433ul,
  1572869ul,    3145739ul,    6291469ul,   12582917ul,  25165843ul,
  50331653ul,   100663319ul,  201326611ul, 402653189ul, 805306457ul, 
  1610612741ul, 3221225473ul, 4294967291ul
};
inline size_t __abase_next_prime(size_t __n)
{
	for(int i = 0;i< __abase_num_primes; i++)
	{
		if(__abase_prime_list[i] > __n)
		{
			return __abase_prime_list[i];
		}
	}
	return size_t(-1);
}


template<class T, class U>
struct pair
{
	typedef T first_type;
	typedef U second_type;
	T first;
	U second;

	pair() {};
	pair(const T& x, const U& y) : first(x), second(y) {}

	template<class V, class W> pair(const pair<V, W>& pr) 
	{
		first = pr.first;
		second = pr.second;
	}
};

template <class _Val,class _Key>
struct Hashtable_node
{
	_Val _val;
	_Key _key;

	Hashtable_node* _next;		//	point to next node in same bucket row
	Hashtable_node*	_prev_ele;	//	point to previos node in element list
	Hashtable_node* _next_ele;	//	point to next node in element list
	auint32 _bucket_idx;		//	index in bucket

	inline Hashtable_node(const _Key& key, const _Val& val, Hashtable_node* next,
				Hashtable_node* prev_ele, Hashtable_node* next_ele, auint32 bucket_idx) :
	_val(val),
	_key(key),
	_next(next),
	_bucket_idx(bucket_idx)
	{
		_prev_ele = prev_ele;
		_next_ele = next_ele;
		if (prev_ele) prev_ele->_next_ele = this;
		if (next_ele) next_ele->_prev_ele = this;
	}

	void Release()
	{
		this->~Hashtable_node();
	}
};

struct _hash_function
{
	_hash_function() {}
	_hash_function(const _hash_function & hf) {}

	inline auint32 operator()(char data) const {return data;}
	inline auint32 operator()(short data) const {return data;}
	//inline auint32 operator()(int data) const {return data;}
	inline auint32 operator()(aint32 data) const {return data;}
	inline auint32 operator()(unsigned char data) const {return data;}
	inline auint32 operator()(unsigned short data) const {return data;}
	//NOTE!! by liyi 增加unsigned long是为了兼容老引擎,跨平台有错
	inline auint32 operator()(unsigned long data) const {return data;}
	inline auint32 operator()(auint32 data) const {return data;}
	inline auptrint operator()(void *data) const {return (auptrint)(data);}

	inline auint32 operator()(aint64 data) const
	{
		return (auint32)((data >> 32) + (data & 0xffffffff));
	}

	inline auint32 operator()( auint64 data) const
	{
		return (auint32)((data >> 32) + (data & 0xffffffff));
	}

	inline auint32 operator()(char *s) const
	{
		auint32 h = 0;
		for(;*s; s++)
		{
			h = h * 31 + *(unsigned char *)s;
		}
		return h;
	}
	inline auint32 operator()(const char *s) const
	{
		auint32 h = 0;
		for(;*s; s++)
		{
			h = h * 31 + *(unsigned char *)s;
		}
		return h;
	}
	inline auint32 operator()(const wchar_t *s) const
	{
		auint32 h = 0;
		for(;*s; s++)
		{
			h = h * 31 + *s;
		}
		return h;
	}
};

struct _hash_string
{
	typedef const char * LPCSTR;
	const char * __str;
	_hash_string(const char * str):__str(str){}
	operator LPCSTR() const { return __str;}
	bool operator ==(const char * str) const { return strcmp(str,__str) == 0;}
	bool operator ==(const _hash_string & rhs) const { return strcmp(rhs,__str) == 0;}
};


/*class definition*/
template <class _Value, class _Key, class _HashFunc,class _Allocator=default_alloc>
class hashtab
{
public:

	typedef _Key	key_type;
	typedef _Value	value_type;
	typedef pair<value_type *, bool> pair_type;
	typedef Hashtable_node<_Value,_Key> _Node;
	typedef _Value& reference;
	typedef _Value* pointer;

	template <class _ItValue>
	class iterator_template
	{
	private:

		const hashtab<_Value,_Key,_HashFunc,_Allocator>* _tab;
		_Node* _node;
	//	iterator_template<_ItValue> & operator =(const iterator_template<_ItValue> &);
	private:

		iterator_template(const hashtab<_Value, _Key,_HashFunc, _Allocator> * __this) : _tab(__this)
		{
			_node = _tab->_elements;
		}

		iterator_template(_Node* __node, const hashtab<_Value, _Key, _HashFunc, _Allocator> * __this) : _tab(__this), _node(__node)
		{}

		iterator_template(const hashtab<_Value,_Key, _HashFunc, _Allocator> * __this, int) : _tab(__this), _node(0)
		{}

	public:

		iterator_template() : _tab(0), _node(0) {}

	public:

		bool operator == (const iterator_template<_ItValue> & rhs) const
		{
			if (_tab && rhs._tab)
			{
				ASSERT(_tab == rhs._tab);
				return _tab == rhs._tab && _node == rhs._node;
			}
			else
				return _node == rhs._node;
		}

		bool operator != (const iterator_template<_ItValue> & rhs) const
		{
			return !operator == (rhs);
		}

		_ItValue* value() const
		{
			return _node ? &(_node->_val) : NULL;
		}
		
		const _Key* key() const
		{
			return _node ? &(_node->_key) : NULL;
		}
		
		_ItValue* operator->()const
		{
			return _node ? &(_node->_val) : NULL;
		}

		iterator_template<_ItValue> & operator ++()
		{
			if (_node != NULL)
				_node = _node->_next_ele;

			return *this;
		}

		bool is_eof() const
		{
			return !_node;
		}

		friend class hashtab<_Value, _Key, _HashFunc, _Allocator>;
	};

	typedef iterator_template<_Value> iterator;
	typedef iterator_template<const _Value> const_iterator;
	typedef size_t size_type;

	//friend hashtab::iterator;
	//friend const_iterator;
	friend class abase::hashtab<_Value, _Key,_HashFunc,_Allocator>::iterator_template<_Value>;
	friend class abase::hashtab<_Value, _Key,_HashFunc,_Allocator>::iterator_template<const _Value>;

private:

	_HashFunc	_hash;
	size_t		_num_elements;
	vector<_Node*, _Allocator> _buckets;
	_Node*		_elements;		//	element list
	
	size_t get_hash(const key_type & key) const 
	{
		return _hash(key) % _buckets.size();
	}

	void release_node(_Node* pnode)
	{
		//	Remove from elements list
		if (pnode->_prev_ele)
			pnode->_prev_ele->_next_ele = pnode->_next_ele;
		else
			_elements = pnode->_next_ele;

		if (pnode->_next_ele)
			pnode->_next_ele->_prev_ele = pnode->_prev_ele;

		//	Release node
		pnode->Release();
		_Allocator::deallocate(pnode, sizeof (_Node));
		--_num_elements;

	#ifdef _DEBUG
		if (_num_elements == 0)
		{
			ASSERT(!_elements);
		}
	#endif
	}

	const hashtab<_Value,_Key,_HashFunc,_Allocator> & operator=(const hashtab<_Value,_Key,_HashFunc,_Allocator> & rhs);
	hashtab(const hashtab<_Value,_Key,_HashFunc,_Allocator>& rhs);

public:

	hashtab(size_t __n, const _HashFunc & __hf) :
	_hash(__hf),
	_num_elements(0),
	_buckets(_M_next_size(__n),(_Node *)0),
	_elements(0)
	{
	}

	explicit hashtab(size_t __n) : 
	_hash(),
	_num_elements(0),
	_buckets(_M_next_size(__n), (_Node*)0),
	_elements(0)
	{
	}

	~hashtab()
	{
		clear();
	}

	size_t size() const { return _num_elements; }
	size_t max_size() const { return size_t(-1); }
	bool empty() const { return size() == 0; }
	void clear();
	void resize(size_t __num_elements_hint);

	inline bool put(const key_type & __key , const value_type & __val)
	{
		resize(_num_elements + 1);
		return put_noresize(__key,__val);
	}

	iterator begin() { return iterator(this); }
	iterator end() { return iterator(this, 0); }
	const_iterator begin() const { return const_iterator(this); } //$$$ 这里并没有返回const迭代器
	const_iterator end() const{ return const_iterator(this,0); }
	bool put_noresize(const key_type & __key , const value_type & __val);
	pair<value_type *, bool> get(const key_type &__key) const;
	value_type * nGet(const key_type &__key) const;	//特殊的get:)
	bool erase(const key_type &__key);
	iterator erase(const iterator& __it);
	size_type bucket_count() const { return _buckets.size(); }	
//	size_type max_bucket_count() const {return __stl_prime_list[(int)__stl_num_primes - 1]; } 

	iterator find(const key_type& __key) 
	{
		size_type __n = get_hash(__key);
		_Node* __first;
		for ( __first = _buckets[__n];
		__first && !(__first->_key == __key);
		__first = __first->_next)
		{}
		return iterator(__first, this);
	} 
	
	const_iterator find(const key_type& __key) const
	{
		size_type __n = get_hash(__key);
		_Node* __first;
		for ( __first = _buckets[__n];
		__first && !(__first->_key == __key);
		__first = __first->_next)
		{}
		return const_iterator(__first, this);
	} 
	reference find_or_insert(const key_type & __key,const value_type& __obj);

	size_type elems_in_bucket(size_type __bucket) const
	{
		size_type __result = 0;
		for (_Node* __cur = _buckets[__bucket]; __cur; __cur = __cur->_next)
			__result += 1;
		return __result;
	}

	template<class _EnumFunc>
	void enum_element(_EnumFunc & __func)
	{
		_Node* __cur = _elements;
		while (__cur)
		{
			__func(__cur->_key, __cur->_val);
			__cur = __cur->_next_ele;
		}
	}

	template<class _EnumFunc>
	void enum_buckets(_EnumFunc & __func)
	{	
		for (_Node **fp = _buckets.begin();fp != _buckets.end(); fp ++)
		{
			__func(*fp);
		}
	}
private:
	size_t _M_next_size(size_t __n) const 
	{
		return __abase_next_prime(__n);
	}

};

template <class _Value, class _Key, class _HashFunc,class _Allocator>
typename hashtab<_Value,_Key,_HashFunc,_Allocator>::reference 
hashtab<_Value,_Key,_HashFunc,_Allocator>::find_or_insert(const key_type & __key,const value_type& __val)
{
	resize(_num_elements + 1);
	
	size_t __n = get_hash(__key);
	_Node* __first = _buckets[__n];
	
	for (_Node* __cur = __first; __cur; __cur = __cur->_next)
	{
		if (__cur->_key == __key)
			return __cur->_val;
	}

	_Node* prev_ele = NULL;
	_Node* next_ele = _elements;
	_Node* __tmp = new (_Allocator::allocate(sizeof(_Node))) _Node(__key, __val, __first, prev_ele, next_ele, (auint32)__n);
	_elements = __tmp;
	_buckets[__n] = __tmp;
	++_num_elements;

	return __tmp->_val;
}

template <class _Value, class _Key, class _HashFunc,class _Allocator>
bool hashtab<_Value,_Key,_HashFunc,_Allocator>::put_noresize(const key_type & __key , const value_type & __val)
{
	size_t __n = get_hash(__key);
	_Node* __first = _buckets[__n];

	for (_Node* __cur = __first; __cur; __cur = __cur->_next) 
	{
		if (__cur->_key == __key)
			return false;
	}

	_Node* prev_ele = NULL;
	_Node* next_ele = _elements;
	_Node* __tmp = new (_Allocator::allocate(sizeof(_Node))) _Node(__key, __val, __first, prev_ele, next_ele, (auint32)__n);
	_elements = __tmp;
	_buckets[__n] = __tmp;
	++_num_elements;

	return true;
}

template <class _Value, class _Key, class _HashFunc,class _Allocator>
void hashtab<_Value,_Key,_HashFunc,_Allocator>::resize(size_t __num_elements_hint)
{
	const size_t __old_n = _buckets.size();
	if (__num_elements_hint > __old_n)
	{
		const size_t __n = _M_next_size(__num_elements_hint);
		if (__n > __old_n)
		{
			vector<_Node*, _Allocator> __tmp(__n, (_Node*)(0));
			
			_Node* __first = _elements;
			while (__first)
			{
				size_t __new_bucket = _hash(__first->_key) % __n;

				__first->_next = __tmp[__new_bucket];
				__first->_bucket_idx = (auint32)__new_bucket;
				__tmp[__new_bucket] = __first;

				__first = __first->_next_ele;
			}

			_buckets.swap(__tmp);
		}
	}
}

template <class _Value, class _Key, class _HashFunc,class _Allocator>
void hashtab<_Value,_Key,_HashFunc,_Allocator>::clear()
{
	for (size_t __i = 0; __i < _buckets.size(); ++__i)
	{
		_Node* __cur = _buckets[__i];

		while (__cur != NULL)
		{
			_Node * __next = __cur->_next;
			__cur->Release();
			_Allocator::deallocate(__cur,sizeof(_Node));
			__cur = __next;
		}

		_buckets[__i] = 0;
	}

	_elements = 0;
	_num_elements = 0;
}

template <class _Value, class _Key, class _HashFunc,class _Allocator>
pair<_Value *, bool> hashtab<_Value,_Key,_HashFunc,_Allocator>::get(const key_type &__key) const
{
	size_t __n = get_hash(__key);
	_Node* __first = _buckets[__n];

	for (_Node* __cur = __first; __cur; __cur = __cur->_next) 
	{
		if (__cur->_key == __key)
			return pair<_Value *,bool>(&(__cur->_val),true);
	}

	return pair<_Value *,bool>((_Value *)NULL,false);
}

template <class _Value, class _Key, class _HashFunc,class _Allocator>
_Value* hashtab<_Value,_Key,_HashFunc,_Allocator>::nGet(const key_type &__key) const
{
	size_t __n = get_hash(__key);
	_Node* __first = _buckets[__n];

	for (_Node* __cur = __first; __cur; __cur = __cur->_next) 
	{
		if (__cur->_key == __key)
			return &(__cur->_val);
	}

	return NULL;
}

template <class _Value, class _Key, class _HashFunc,class _Allocator>
bool hashtab<_Value,_Key,_HashFunc,_Allocator>::erase(const key_type &__key)
{
	size_t __n = get_hash(__key);
	_Node* __first = _buckets[__n];
	_Node *__prev=NULL;

	for (_Node* __cur = __first; __cur; __prev = __cur,__cur = __cur->_next) 
	{
		if (__cur->_key == __key)
		{			
			if(__cur == __first)
			{
				_buckets[__n] = __cur->_next;
			}
			else
			{
				__prev->_next = __cur->_next;
			}

			release_node(__cur);
			return true;
		}
	}

	return false;
}

template <class _Value, class _Key, class _HashFunc,class _Allocator>
typename hashtab<_Value,_Key,_HashFunc,_Allocator>::iterator hashtab<_Value,_Key,_HashFunc,_Allocator>::erase(const iterator &__it)
{
	iterator it(__it);
	++it;
	_Node* __p = __it._node;

	if (__p)
	{
		auint32 bucket_idx = __p->_bucket_idx;
		_Node* __first = _buckets[bucket_idx];

		if (__first == __p)
		{
			_buckets[bucket_idx] = __first->_next;
			release_node(__first);
		}
		else
		{
			_Node* __next = __first->_next;
			while (__next)
			{
				if (__next == __p)
				{
					__first->_next = __next->_next;
					release_node(__next);
					break;
				}
				else
				{
					__first = __next;
					__next = __first->_next;
				}
			}
		}
	}

	return it;
}

}	//	abase

#endif
