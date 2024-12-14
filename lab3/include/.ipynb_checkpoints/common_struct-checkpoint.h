#ifndef COMMON_STRUCT
#define COMMON_STRUCT

#ifdef __riscv64__
#include "map.h"
#endif
#include "compose_keys.hxx"


//Номера структур данных в SPE
enum Structures : uint32_t {
    null            = 0,   	//Нулевая структура не используется
    students_pnum    	= 1,	//Таблица 1 
    housing_applications_pnum  = 2,		//Таблица 2 
    results_pnum = 3
};

#ifdef __riscv64__
//Задание даипазонов и курсоров
template<typename Range>
struct reverse {
        Range r;
        [[gnu::always_inline]] reverse(Range r) : r(r) {}
        [[gnu::always_inline]] auto begin() {return r.rbegin();}
        [[gnu::always_inline]] auto end() {return r.rend();}
};

template<typename K, typename V>
struct Handle {
        bool ret_val;
        K k{get_result_key<K>()};
        V v{get_result_value<V>()};
        [[gnu::always_inline]] Handle(bool ret_val) : ret_val(ret_val) {
        }

        [[gnu::always_inline]] operator bool() const {
                return ret_val;
        }

        [[gnu::always_inline]] K key() const {
                return k;
        }

        [[gnu::always_inline]] V value() const {
                return v;
        }
};
#endif


//////////////////////////////////////
// Описание формата ключа и значения
//////////////////////////////////////

struct students {
	using vertex_t = uint64_t;
    int struct_number;
    constexpr students(int struct_number) : struct_number(struct_number) {}
	static const vertex_t idx_bits = 32;
	static const vertex_t idx_max = (1ull << idx_bits) - 1;
	static const vertex_t idx_min = idx_max; 

	//Запись для формирования ключей (* - наиболее значимые биты поля)
	STRUCT(key)
	{
        uint32_t    filler      :32;
	    uint32_t	student_id	:32;	//Поле 0:
	};

	//Запись для формирования значений
	STRUCT(val)
	{
	    uint32_t    filler      :32;
	    uint32_t	student_id	:32;
	};
	//Обязательная типизация
	#ifdef __riscv64__
    DEFINE_DEFAULT_KEYVAL(key, val)
	#endif
};

constexpr students STUDENTS(Structures::students_pnum);

struct housing_applications {
	using vertex_t = uint32_t;
    int struct_number;
    constexpr housing_applications(int struct_number) : struct_number(struct_number) {}
	static const vertex_t idx_bits = 32;
	static const vertex_t idx_max = (1ull << idx_bits) - 1;
	static const vertex_t idx_min = idx_max; 

	//Запись для формирования ключей (* - наиболее значимые биты поля)
	STRUCT(key)
	{
        time_t    due_date     :32;
        uint32_t  student_id   :32;	//Поле 0:
	};

	//Запись для формирования значений
	STRUCT(val)
	{
        uint32_t  housing_application_id	   :32;	//Поле 0:
        time_t    due_date     :32;
	};
	//Обязательная типизация
	#ifdef __riscv64__
    DEFINE_DEFAULT_KEYVAL(key, val)
	#endif
};

constexpr housing_applications HOUSING_APPLICATIONS(Structures::housing_applications_pnum);

constexpr housing_applications RESULTS(Structures::results_pnum);

#endif //COMMON_STRUCT

