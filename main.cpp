#include <pqxx/pqxx>
#include <iostream>
#include <Windows.h>
#include <string>

pqxx::connection c(
	"host=localhost "
	"port=5432 "
	"dbname=postgres "
	"user=postgres "
	"password=postgrespassword");
pqxx::work t{ c };
class BD {

public:
	BD() {; }
	//ћетод, создающий структуру Ѕƒ (таблицы)
	void create_bd(std::string table_name, std::string attributes) {
		t.exec("CREATE TABLE IF NOT EXISTS " + t.esc(table_name) + " ( " +
			t.esc(attributes) + ");");
		t.commit();
	}
	//ћетод, позвол€ющий добавить нового клиента
	void add_client(std::string first_name, std::string second_name, std::string email) {
		t.exec("INSERT INTO PersonalInfo (first_name, second_name, email) "
			"VALUES ('" + t.esc(first_name) + "', '" + t.esc(second_name) + "', '" + t.esc(email) + "')");
		t.commit();
	}
	//ћетод, позвол€ющий добавить телефон дл€ существующего клиента
	void add_number(int id1, int num) { 
		t.exec("INSERT INTO PhoneNumber (number) "
			"VALUES (" + std::to_string(num) + ")");
		int id2 = t.query_value<int>("SELECT id FROM PhoneNumber WHERE number=" + std::to_string(num));
		t.exec("INSERT INTO PhoneInfo (id_pers, id_phone) "
			"VALUES (" + std::to_string(id1) + ", " + std::to_string(id2) + ")");
		t.commit();
	}
	//ћетод, позвол€ющий изменить данные о клиенте
	void change_info(int id1, std::string first_name, std::string second_name, std::string email) {
		if (first_name != "-") t.exec("UPDATE PersonalInfo SET first_name='" + t.esc(first_name) + "' WHERE id =" + std::to_string(id1) + ";");
		if (second_name != "-") t.exec("UPDATE PersonalInfo SET second_name='" + t.esc(second_name) + "' WHERE id =" + std::to_string(id1) + ";");
		if (email != "-") t.exec("UPDATE PersonalInfo SET email='" + t.esc(email) + "' WHERE id =" + std::to_string(id1) + ";");
		t.commit();
	}
	//ћетод, позвол€ющий удалить телефон дл€ существующего клиента
	void delete_number(int id2) {
		t.exec("DELETE FROM PhoneInfo WHERE id_phone=" + std::to_string(id2));
		t.exec("DELETE FROM PhoneNumber WHERE id=" + std::to_string(id2));
		t.commit();
	}
	//ћетод, позвол€ющий удалить существующего клиента
	void delete_info(int id1) {
		int id2 = t.query_value<int>("SELECT id_phone FROM PhoneInfo WHERE id_pers=" + std::to_string(id1) + ";");
		t.exec("DELETE FROM PhoneInfo WHERE id_phone=" + std::to_string(id2) + ";");
		t.exec("DELETE FROM PhoneNumber WHERE id=" + std::to_string(id2) + ";");
		t.exec("DELETE FROM PersonalInfo WHERE id=" + std::to_string(id1) + ";");
		t.commit();
	}
	//ћетод, позвол€ющий найти клиента по его данным (имени, фамилии, email-у или телефону)
	int get_id1(std::string info) {
		int id1 = t.query_value<int>("SELECT id FROM PersonalInfo WHERE first_name='" + t.esc(info) + "' or second_name='" +
			t.esc(info) + "' or email='" + t.esc(info) + "';");
		return id1;
	}
	int get_id1(int info) {
		int id2 = t.query_value<int>("SELECT id FROM PhoneNumber WHERE number =" + std::to_string(info));
		int id1 = t.query_value<int>("SELECT id_pers FROM PhoneInfo WHERE id_phone =" + std::to_string(id2));
		return id1;
	}
	int get_id2(int info) {
		int id2 = t.query_value<int>("SELECT id FROM PhoneNumber WHERE number =" + std::to_string(info));
		return id2;
	}
	int get_id2(std::string info) {
		int id1 = t.query_value<int>("SELECT id FROM PersonalInfo WHERE first_name='" + t.esc(info) + "' or second_name='" +
			t.esc(info) + "' or email='" + t.esc(info) + "';");
		int id2 = t.query_value<int>("SELECT id_phone FROM PhoneInfo WHERE id_pers =" + std::to_string(id1));
		return id2;
	}
};
int main() {
	try {
		SetConsoleCP(CP_UTF8);
		SetConsoleOutputCP(CP_UTF8);
		BD bd;
		bd.create_bd("PersonalInfo", "id SERIAL PRIMARY KEY NOT NULL, first_name varchar(50) not null, second_name varchar(50) not null, email varchar(50) not null unique");
		//bd.create_bd("PhoneNumber", "id SERIAL PRIMARY KEY NOT NULL, number integer unique");
		//bd.create_bd("PhoneInfo", "id_pers integer references PersonalInfo(id), id_phone integer references PhoneNumber(id), constraint pk primary key(id_pers, id_phone)");
		//bd.add_client("Ivan", "Ivanov", "testemail@.com");
		//std::cout << bd.get_id2("testemail@.com");
		//bd.add_number(bd.get_id1("Ivanov"), 123456789);
		//bd.change_info(bd.get_id1("testemail@.com"), "Alexandr", "-", "-");
		//std::cout << "id_pers = " << bd.get_id1(123456789) << " id_phone = " << bd.get_id2(123456789) << std::endl;
		//bd.delete_info(bd.get_id1("Ivanov"));
	}
	catch (pqxx::sql_error e)
	{
		std::cout << e.what() << std::endl;
	}
}