#include <iostream>
#include <fstream>
#include <vector>
#include <any>
#include <tuple>
#include <string>
#include <sstream>
#include <map>

// TODO: simplify
std::tuple<std::string, std::string, std::any> parse_field_line(const std::string& line) {
    std::string delimiter = " ";
    size_t delim_len = delimiter.length();
    size_t pos_start = 0;
    size_t pos_end;

    std::string name;
    std::string type;
    std::any value;

    {
        pos_end = line.find(delimiter, pos_start);
        type = line.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
    }
    {
        pos_end = line.find(delimiter, pos_start);
        name = line.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
    }
    {
        std::string _value = line.substr(pos_start);
        if (type == "int") {
            value = std::stoi(_value);
        } else if (type == "string") {
            value = _value;
        } else if (type == "float") {
            value = std::stof(_value);
        }
    }

    return std::make_tuple(type, name, value);
}

class Field {
public:
    Field(const std::string& name, const std::string& type, std::any value)
        : name_(name), type_(type), value_(value) {
    }

    Field() {
    }

    static Field create_from_string(const std::string& str) {
        auto [type, name, value] = parse_field_line(str);

        return { name, type, value };
    }

    std::string get_name() const {
        return name_;
    }

    std::string get_type() const {
        return type_;
    }

    std::any get_value() const {
        return value_;
    }

private:
    std::string name_;
    std::string type_;
    std::any value_;
};

class Serializable {
public:
    void add_field(const Field& field) {
        fields_[field.get_name()] = field;
    }

    void clear_fields() {
        fields_.clear();
    }

    std::map<std::string, Field> get_fields() const {
        return fields_;
    }

private:
    std::map<std::string, Field> fields_;
};

class Serializator {
public:
    template <typename T>
    void serialize(Serializable* obj) {
        std::ofstream stream(typeid(T).name());
        std::stringstream ss;

        auto fields = obj->get_fields();
        for (auto field : fields) {
            std::string field_type = field.second.get_type();
            std::string field_name = field.second.get_name();
            std::any field_value = field.second.get_value();

            ss << field_type << " " << field_name << " ";
            if (field_type == "int") {
                ss << std::any_cast<int>(field_value);
            } else if (field_type == "string") {
                ss << std::any_cast<std::string>(field_value);
            } else if (field_type == "float") {
                ss << std::any_cast<float>(field_value);
            }
            ss << "\n";
        }

        stream << ss.str();
        stream.flush();
        stream.close();
    }

    template <typename T>
    T deserialize() {
        std::ifstream stream(typeid(T).name());

        Serializable s;
        std::string line;
        while (std::getline(stream, line)) {
            Field field = Field::create_from_string(line);
            s.add_field(field);
        }

        return { s };
    }
};

class Person : public Serializable {
public:
    Person(const Serializable& obj) {
        auto fields = obj.get_fields();
        name_ = std::any_cast<std::string>(fields["name"].get_value());
        age_ = std::any_cast<int>(fields["age"].get_value());
        balance_ = std::any_cast<float>(fields["balance"].get_value());
    }

    Person(const std::string& name, int age, float balance)
        : name_(name), age_(age), balance_(balance) {
        update_fields();
    }

    void update_fields() {
        clear_fields();
        add_field(Field{ "name", "string", name_ });
        add_field(Field{ "age", "int", age_ });
        add_field(Field{ "balance", "float", balance_ });
    }

    void print() {
        std::cout << name_ << " " << age_ << " " << balance_ << "\n";
    }

    void set_name(const std::string& name) {
        name_ = name;
    }

    void set_age(int age) {
        age_ = age;
    }

    void set_balance(float balance) {
        balance_ = balance;
    }

private:
    std::string name_;
    int age_;
    float balance_;
};

int main() {
    Serializator s;

    Person p{"Bob", 20, 15.51};
    s.serialize<Person>(&p);
    /*
    * in file 
    * 
    * int age 20
    * float balance 15.51
    * string name Bob
    */

    s.deserialize<Person>().print(); // OUTPUT: Bob 20 15.51
}
