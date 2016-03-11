#include <type_traits>
#include <tuple>
#include <functional>
#include "POD_tuple.hpp"

struct Arg {};


template<typename Enum, Enum Name, typename Ret>
struct Observer {
	static_assert(std::is_pod<Ret>::value, "Error: only POD return types");
	Ret (*fun) (Arg*);
	using t = Ret;
};

template<typename Enum, Enum Name, typename F>
auto build_observer(F fun){
	using R = std::result_of_t<F(Arg*)>;
	return Observer<Enum,Name,R>{fun};
}

#define observify(name,fun...) build_observer<decltype(name),name>(fun)

template<typename Enum, typename... Rets>
class ObserversDemo {
	//this assumes names will be in same order as functions.
	POD_tuple<Rets (*) (Arg*)...> funs;
	POD_tuple<Rets...> extra_params;

	template<int index> auto DemoBuilder_helper_funs(){ return std::tuple<>{};}
	template<int index, Enum Name, typename Ret, typename... Rest>
	auto DemoBuilder_helper_funs(Observer<Enum,Name,Ret> o1, Rest... rst){
		using namespace std;
		static_assert(static_cast<int>(Name) == index, "Error: non-enum name, or name used out-of-order.");
		return tuple_cat(make_tuple(o1.fun), DemoBuilder_helper_funs<index + 1>(rst...));
	}

	template<int index> auto DemoBuilder_helper_data(){ return std::tuple<>{};}
	template<int index, Enum Name, typename Ret, typename... Rest>
	auto DemoBuilder_helper_data(Observer<Enum,Name,Ret> o1, Rest... rst){
		using namespace std;
		static_assert(static_cast<int>(Name) == index, "Error: non-enum name, or name used out-of-order.");
		return tuple_cat(std::tuple<Ret>{}, DemoBuilder_helper_data<index + 1>(rst...));
	}

public:
	void updateLoop(){
		int i = 0;
		for (const auto& f : funs){
			Arg arg;
			POD_put(extra_params, i,f(&arg));
			++i;
		}
	}

	template<Enum Name>
	auto observe() const {
		return POD_get<static_cast<int>(Name)>(extra_params);
	}

	template<Enum Name>
	std::function<bool (Arg*)> K1() const {
		return [&](Arg*){
			//does some row scan made possible by its arguments,
			//and aggregates them here.
			return observe<Name>();
		};
	}
	
	template<Enum Name, typename Ret, typename... Args>
	ObserversDemo(Observer<Enum,Name,Ret> obs, Args... args)
		:funs(to_pod_tuple(DemoBuilder_helper_funs<0>(obs,args...))),
		 extra_params(to_pod_tuple(DemoBuilder_helper_data<0>(obs,args...))){}
};

//for convenience
template<typename Enum, Enum Name, typename Ret, typename... Args>
auto DemoBuilder(Observer<Enum,Name,Ret> obs, Args... args){
	return ObserversDemo<Enum,Ret,typename Args::t...>{obs,args...};
}

int main(){
	enum class FooNames {Bar, Baz, Bop};
	auto test = DemoBuilder(
		observify(FooNames::Bar,[](Arg*) -> int{return 5;}),
		observify(FooNames::Baz,[](Arg*) -> int{return 6;}),
		observify(FooNames::Bop,[](Arg*) -> int{return 7;})
		);
	test.updateLoop();
	assert(test. K1<FooNames::Baz>()(nullptr));
	return test.observe<FooNames::Bar>();
}
