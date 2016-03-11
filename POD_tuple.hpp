#pragma once
#include <cassert>

template<typename...>
struct POD_tuple;

//you need a lot of these in order to have a POD aggregate type
template<typename A>
struct POD_tuple<A>{
	A a;
	using t_a = A;
};
template<typename A>
struct POD_tuple<A> to_pod_tuple(const std::tuple<A> &t){
	return POD_tuple<A>{std::get<0>(t)};
}
template<typename A, typename B>
struct POD_tuple<A,B>{
	A a; B b;
	using t_a = A; using t_b = B; 
};
template<typename A, typename B>
struct POD_tuple<A,B> to_pod_tuple(const std::tuple<A,B> &t){
	return POD_tuple<A,B>{std::get<0>(t),std::get<1>(t)};
}
template<typename A, typename B, typename C>
struct POD_tuple<A,B,C>{
	A a; B b; C c;
	using t_a = A; using t_b = B; using t_c = C;
};
template<typename A, typename B, typename C>
struct POD_tuple<A,B,C> to_pod_tuple(const std::tuple<A,B,C> &t){
	return POD_tuple<A,B,C>{std::get<0>(t),std::get<1>(t),std::get<2>(t)};
}
template<typename A, typename B, typename C, typename D>
struct POD_tuple<A,B,C,D>{
	A a; B b; C c; D d;
	using t_a = A; using t_b = B; using t_c = C; using t_d = D;
};

template<typename...> struct POD_tuple_it;

template<typename A, typename B, typename C>
struct POD_tuple_it<A,B,C> {
	int pos;
	const POD_tuple<A,B,C> &t;

	POD_tuple_it(int pos, const POD_tuple<A,B,C> &t)
		:pos(pos),t(t){}
	
	auto operator++(){
		++pos;
		return *this;
	}
	bool operator==(const POD_tuple_it& o){
		return &o.t == &t && o.pos == pos;
	}
	bool operator!=(const POD_tuple_it& o){
		return !((*this) == o);
	}
	auto operator*(){
		switch(pos){
		case 0: return t.a;
		case 1: return t.b;
		case 2: return t.c;
		};
	}
};

	template<typename... pack>
	auto begin(const POD_tuple<pack...> &p){
		return POD_tuple_it<pack...>(0,p);
	}
	template<typename... pack>
	auto end(const POD_tuple<pack...> &p){
		return POD_tuple_it<pack...>(3,p);
	}
	


template<typename A, typename B, typename C, typename D>
struct POD_tuple<A,B,C,D> to_pod_tuple(const std::tuple<A,B,C,D> &t){
	return POD_tuple<A,B,C,D>{std::get<0>(t),std::get<1>(t),std::get<2>(t),std::get<3>(t)};
}

template<int i, typename A, typename... Rst>
std::enable_if_t<i == 0, A> POD_get(const POD_tuple<A,Rst...>& t){
	return t.a;
}

template<int i, typename A, typename B, typename... Rst>
std::enable_if_t<i == 1, B> POD_get(const POD_tuple<A,B,Rst...> &t){
	return t.b;
}

template<int i, typename A, typename B, typename C, typename... Rst>
std::enable_if_t<i == 2, B> POD_get(const POD_tuple<A,B,C,Rst...> &t){
	return t.c;
}

template<int i, typename A, typename B, typename C, typename D, typename... Rst>
std::enable_if_t<i == 3, B> POD_get(const POD_tuple<A,B,C,D,Rst...> &t){
	return t.d;
}

template<typename A>
void POD_put(POD_tuple<A>& t, int i, const A &elem){
	assert(i == 0);
	t.a = elem;
}

template<typename A, typename B>
void POD_put(POD_tuple<A,B>& t, int i, const B &elem){
	assert(i <= 1);
	if (i == 0) t.a = elem; else t.b = elem;
}

template<typename A, typename B, typename C>
void POD_put(POD_tuple<A,B,C>& t, int i, const C &elem){
	assert(i <= 2);
	if (i == 0) t.a = (A)elem; else if (i == 1) t.b = (B)elem; else t.c = (C)elem;
}

template<typename A, typename B, typename C, typename D>
void POD_put(POD_tuple<A,B,C,D>& t, int i, const D &elem){
	assert(i <= 3);
	if (i == 0) t.a = (A)elem; else if (i == 1) t.b = (B)elem; else if (i == 2) t.c = (C)elem; else t.d = (D) elem;
}
