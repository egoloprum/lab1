#include <iostream>
#include <iterator>
#include <string>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <map>
#include <vector>

#include "host_main.h"

#define TEST_STUDENT_COUNT 10
#define TEST_HOUSING_APPLICATIONS_COUNT 10
#define STATUS_COUNT 3

char get_rand_char() {
    static std::string charset = "abcdefghijklmnopqrstuvwxyz";
    return charset[rand() % charset.size()];
}

std::string generate_random_name(size_t n) {
    char rbuf[n];
    memset(rbuf, 0, n);
    const size_t name_size = 2 + rand() % (n - 3);
    rbuf[0] = (char) toupper(get_rand_char());
    std::generate(rbuf+1, rbuf+name_size-1, &get_rand_char);
    rbuf[name_size] = ' ';
    rbuf[name_size+1] = (char) toupper(get_rand_char()); 
    std::generate(rbuf+name_size+2, rbuf+n, &get_rand_char);
    return std::string(rbuf, n);
}

std::string generate_random_title(size_t n) {
    char rbuf[n];
    memset(rbuf, 0, n);
    rbuf[0] = (char) toupper(get_rand_char()); 
    std::generate(rbuf+1, rbuf+n, &get_rand_char);
    return std::string(rbuf, n);
}

const std::string target_student = "Денис Фролов";

int main(int argc, char** argv)
{
    srand(time(NULL));

    const uint32_t target_idx = rand() % TEST_STUDENT_COUNT;
    
	std::ofstream log("lab2.log"); //поток вывода сообщений
	gpc *gpc64_inst; //указатель на класс gpc

	//Инициализация gpc
	if (argc<2) {
		log<<"Использование: host_main <путь к файлу rawbinary>"<<std::endl;
		return -1;
	}

	//Захват ядра gpc и запись sw_kernel
	gpc64_inst = new gpc();
	log<<"Открывается доступ к "<<gpc64_inst->gpc_dev_path<<std::endl;
	if (gpc64_inst->load_swk(argv[1])==0) {
		log<<"Программное ядро загружено из файла "<<argv[1]<<std::endl;
	}
	else {
		log<<"Ошибка загрузки sw_kernel файла << argv[1]"<<std::endl;
		return -1;
	}


    std::map<uint32_t, std::string> student_names;
    std::map<uint32_t, std::string> housing_application_titles;
    std::vector<std::string> housing_application_status = {"zbs", "norm", "really bad"};

	//Инициализация таблицы для вложенного запроса
	gpc64_inst->start(__event__(create_students)); //обработчик вставки 

    std::cout << "begin\n";
    
	//1-й вариант: пересылка коротких сообщений
	for (uint32_t idx=0;idx<TEST_STUDENT_COUNT;idx++) {
        std::string name;
        if (idx == target_idx) {
            name = target_student;
        } else {
            name = generate_random_name(10);
        }
        student_names[idx] = name;

        auto student_key = students::key{.idx=idx,.student_id=idx};
        auto student_val = students::val{};

        log << "Create student " << name << std::endl;
        gpc64_inst->mq_send(student_key);
        gpc64_inst->mq_send(student_val);
	}


    gpc64_inst->mq_send(-1ull);
    std::cout << "students filled\n";

    gpc64_inst->start(__event__(create_housing_applications)); //обработчик вставки 

    
	//1-й вариант: пересылка коротких сообщений

    for (uint32_t idx=0; idx<TEST_HOUSING_APPLICATIONS_COUNT; idx++) {
        housing_application_titles[idx] = generate_random_title(10);
        uint32_t ids = rand() % TEST_STUDENT_COUNT;
        uint32_t status = rand() % STATUS_COUNT;

        auto housing_application_key = housing_applications::key{.idx=idx, .housing_application_id=idx};
        auto housing_application_val = housing_applications::val{.student_id=ids, .status=status};

        log << "create housing_application " << housing_application_titles[idx] << " with student " << student_names[ids] << " status = " << housing_application_status[status] <<std::endl;
        gpc64_inst->mq_send(housing_application_key);
        gpc64_inst->mq_send(housing_application_val);
    }
    gpc64_inst->mq_send(-1ull);

    std::cout << "housing_applications filled\n";

    gpc64_inst->start(__event__(select)); //обработчик запроса поиска
    gpc64_inst->mq_send(target_idx);

	while(1) {
        auto housing_application_id = gpc64_inst->mq_receive();
        auto status = gpc64_inst->mq_receive(); // gpc64_inst->mq_receive();
        if (housing_application_id ==  -1ull || status ==  -1ull) {
            break;
        }
        log <<"housing_application for target student " << housing_application_titles[housing_application_id] << " status " << housing_application_status[status] << std::endl;
	}
	log << "Выход!" << std::endl;
	return 0;
}

